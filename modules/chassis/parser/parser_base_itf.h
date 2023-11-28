#pragma once

#include "cyber/common/log.h"
#include "cyber/timer/timer.h"

#include "modules/chassis/proto/chassis_config.pb.h"
#include "modules/chassis/proto/frame_down_stream.pb.h"

#include "modules/chassis/parser/raw_manage.h"
#include "modules/chassis/parser/circular_buffer.h"
#include "modules/chassis/parser/pack_libs/servo_packer.h"
#include "modules/chassis/parser/pack_libs/llaser_packer.h"

namespace mstf {
namespace chss {
namespace parser {

    using namespace /*mstf::chss::*/proto;
    using namespace /*mstf::chss::*/parser;

    using FrameProcessor = std::function<int(const
            Message*, const std::string&)>;

    class ParserBaseItf {
        public:
            ParserBaseItf(const ChassisConfig* cc,
                    const SensorInfo* si) {
                AINFO << "ParserBaseItf construct for " <<
                    si->name();
                /*
                AINFO << "cc: " << cc;
                AINFO << "si: " << si;
                */
                chs_conf_ = cc;
                snsr_info_ = si;
            }

            virtual ~ParserBaseItf() {
                AINFO << "ParserBaseItf de-construction";
            }

            ParserBaseItf(const ParserBaseItf&) = delete;
            ParserBaseItf& operator=(const ParserBaseItf&) = delete;

            int Init(const EE_COMM_PORT_TYPE type,
                    const CircularBufSetting* cbs = nullptr) {
                AINFO << "Init ParserBaseItf, port: " << type <<
                    ", dev: " << snsr_info_->name() <<
                    ", cbuf:\n" <<
                    (cbs == nullptr ? "no cbuf" : cbs->DebugString());
                switch (type) {
                    case E_COMM_PORT_I2C:
                    case E_COMM_PORT_SERIAL: {
                        if (cbs != nullptr) {
                            cbuf_raw_ = std::make_unique<CircularBuffer<uint8_t>>
                                (cbs->circlular_size(), cbs->circle_name());
#if USE_PARSER_TIMER
                            tim_.reset(new cyber::Timer(cbs->parser_freq(),
                                        [this] ()->void {
                                        if (cbuf_raw_->CheckCbuf())
                                            cbuf_raw_->ReleaseCbuf();
                                        },
                                        false)
                                    );
#endif
                        } else {
                            AINFO << "no circular buf for uart port";
                        }
                        const SensorIndicator* it = nullptr;
                        if ((it = GetIndicator()) != nullptr) {
                            if (it->type() == E_DEVICE_LINELASER)
                                packer_ = std::make_unique<LlaserPacker>(snsr_info_->name());
                            //else if (it->type() == E_DEVICE_LINELASER())
                            else
                                AINFO << "packer TODO for: " << snsr_info_->name();
                        } else {
                            AERROR << "no indicator for the device!";
                            return -1;
                        }
                        RawManage::Instance()->RegisterRawListener
                            (std::bind(&ParserBaseItf::OnOriginalDataRaw,
                                       this, std::placeholders::_1,
                                       std::placeholders::_2), snsr_info_);
                    }
                        break;
                    case E_COMM_PORT_CAN: {
                        if (cbs != nullptr) {
                            cbuf_can_ = std::make_unique<CircularBuffer<uint8_t>>
                                (cbs->circlular_size(), cbs->circle_name());
#if USE_PARSER_TIMER
                            tim_.reset(new cyber::Timer(cbs->parser_freq(),
                                        [this] ()->void {
                                        if (cbuf_can_->CheckCbuf())
                                            cbuf_can_->ReleaseCbuf();
                                        },
                                        false)
                                    );
#endif
                            packer_ = std::make_unique<ServoPacker>(snsr_info_->name());
                        } else {
                            AINFO << "no circular buf for CAN port";
                        }
                        RawManage::Instance()->RegisterCanListener
                            (std::bind(&ParserBaseItf::OnOriginalDataCan,
                                       this, std::placeholders::_1,
                                       std::placeholders::_2), snsr_info_);
                    }
                        break;
                    case E_COMM_PORT_USB:
                    case E_COMM_PORT_GPIO:
                    case E_COMM_PORT_I2S:
                    case E_COMM_PORT_SDIO: {
                        RawManage::Instance()->RegisterSocListener
                            (std::bind(&ParserBaseItf::OnOriginalDataSoc,
                                       this, std::placeholders::_1), snsr_info_);
                    }
                        break;
                    default:
                        AERROR << "type error: " << type;
                        return -1;
                }

                return RawManage::Instance()->Init(chs_conf_, snsr_info_);
            }

            virtual int Init() {
                AINFO << "ParserBaseItf DFT Init ~ ";
                return 0;
            }

            virtual int Start() {
                AINFO << "ParserBaseItf DFT Start ~ " <<
                    snsr_info_->name();
                if (cbuf_raw_)
                    cbuf_raw_->Open();

                //data parser setting
#if USE_PARSER_TIMER
                if (tim_ != nullptr) {
                    AINFO << "Start ParserBaseItf timer for " <<
                        snsr_info_->name();
                    tim_->Start();
                }
#endif
                return RawManage::Instance()->Start(snsr_info_);
            }

            virtual int Stop() {
                AINFO << "ParserBaseItf DFT Stop ~ " <<
                    snsr_info_->name();
#if USE_PARSER_TIMER
                if (tim_ != nullptr) {
                    AINFO << "Stop ParserBaseItf timer" <<
                        snsr_info_->name();
                    tim_->Stop();
                }
#endif
                return RawManage::Instance()->Stop(snsr_info_);
            }

            virtual int Resume() {
                AINFO << "ParserBaseItf DFT Resume ~ " <<
                    snsr_info_->name();
                if (cbuf_raw_)
                    cbuf_raw_->Open();

#if USE_PARSER_TIMER
                if (tim_ != nullptr) {
                    AINFO << "Resume ParserBaseItf timer" <<
                        snsr_info_->name();
                    tim_->Start();
                }
#endif
                return RawManage::Instance()->Resume(snsr_info_);
            }

            virtual int Close() {
                AINFO << "ParserBaseItf DFT Close ~ " <<
                    snsr_info_->name();

                if (cbuf_raw_)
                    cbuf_raw_->Close();

#if USE_PARSER_TIMER
                if (tim_ != nullptr) {
                    AINFO << "Close ParserBaseItf timer" <<
                        snsr_info_->name();
                    tim_->Stop();
                }
#endif
                return RawManage::Instance()->Close(snsr_info_);
            }

            void SetProtobufHandle(const FrameProcessor& p) {
                frame_processor_ = p;
            }

            ////////// data upstream //////////
            virtual int ParseRawBuffer(const uint8_t*,
                    const size_t) {
                return 0;
            }

            virtual int ParseCanBuffer(const uint8_t*,
                    const size_t) {
                //ignore
                return 0;
            }

            virtual int ParseSocInfo(const Message&) {
                return 0;
            }

            virtual int ParseSigleFrame(const
                    std::vector<uint8_t>&,
                    const size_t) {
                AINFO << "ParserBaseItf DFT ParseSigleFrame";
                return 0;
            }
            ////////// data upstream //////////

            ////////// data downstream //////////
            virtual int WriteSocMessage(const DownToSocData& data) {
                return RawManage::Instance()->WriteSoc(data);
            }

            virtual int WriteMcuMessage(const DownToMcuData& data) {
                std::vector<uint8_t> packed_data =
                    packer_->PackMcuMessage(data);
                AWARN << "packer: " << packer_.get();
                return RawManage::Instance()->
                    WriteUart(snsr_info_, &packed_data[0], packed_data.size());
            }

            virtual int WriteServoMessage(const DownToServoData& data) {
                return 0;
            }

            virtual int WriteMiscMessage(const DownToMiscData& data) {
                return 0;
            }
            ////////// data downstream //////////

        private:
            void OnOriginalDataRaw(const uint8_t* buf,
                    const size_t size) {
                ACHECK(cbuf_raw_) <<
                    "no circularbuf for RAW!";
                if (!cbuf_raw_->Push(buf, size)) {
                    AWARN << "push circulor buf error!" <<
                        snsr_info_->name();
                }
                //notify the specific raw parser working
                if (!IsParsingRawBusy()) {
                    SetParsingRawBusy();
                    ParseRawBuffer(buf, size);
                    SetParsingRawFree();
                }
            }

            void OnOriginalDataCan(const uint8_t* buf,
                    const size_t len) {
                ACHECK(cbuf_can_) <<
                    "no circularbuf for CAN!";
                /*
                if (!cbuf_can_->Push(std::move(can))) {
                    AWARN << "push circulor buf error!" <<
                        snsr_info_->name();
                }
                */
                //notify the specific can parser work
                ParseCanBuffer(buf, len);
            }

            void OnOriginalDataSoc(const Message& msg) {
                //notify the specific soc parser work
                ParseSocInfo(msg);
            }

            void SetParsingRawFree() {
                is_parsing_busy_.store(false);
            }

            void SetParsingRawBusy() {
                is_parsing_busy_.store(true);
            }

            bool IsParsingRawBusy() {
                return is_parsing_busy_.load();
            }

        protected:
            //helpers
            const SensorIndicator* GetIndicator() {
                for (int i = 0; i < chs_conf_->gpio_dev().size(); i++)
                    if (&chs_conf_->gpio_dev(i).si() == snsr_info_)
                        return &chs_conf_->gpio_dev(i).sn_ind();

                for (int i = 0; i < chs_conf_->adc_dev().size(); i++)
                    if (&chs_conf_->adc_dev(i).si() == snsr_info_)
                        return &chs_conf_->adc_dev(i).sn_ind();

                for (int i = 0; i < chs_conf_->pwm_dev().size(); i++)
                    if (&chs_conf_->pwm_dev(i).si() == snsr_info_)
                        return &chs_conf_->pwm_dev(i).sn_ind();

                for (int i = 0; i < chs_conf_->lidar_dev().size(); i++)
                    if (&chs_conf_->lidar_dev(i).si() == snsr_info_)
                        return &chs_conf_->lidar_dev(i).sn_ind();

                for (int i = 0; i < chs_conf_->imu_dev().size(); i++)
                    if (&chs_conf_->imu_dev(i).si() == snsr_info_)
                        return &chs_conf_->imu_dev(i).sn_ind();

                for (int i = 0; i < chs_conf_->dtof_dev().size(); i++)
                    if (&chs_conf_->dtof_dev(i).si() == snsr_info_)
                        return &chs_conf_->dtof_dev(i).sn_ind();

                for (int i = 0; i < chs_conf_->camera_dev().size(); i++)
                    if (&chs_conf_->camera_dev(i).si() == snsr_info_)
                        return &chs_conf_->camera_dev(i).sn_ind();

                for (int i = 0; i < chs_conf_->mcu_dev().size(); i++)
                    if (&chs_conf_->mcu_dev(i).si() == snsr_info_)
                        return &chs_conf_->mcu_dev(i).sn_ind();

                for (int i = 0; i < chs_conf_->aud_dev().size(); i++)
                    if (&chs_conf_->aud_dev(i).si() == snsr_info_)
                        return &chs_conf_->aud_dev(i).sn_ind();

                for (int i = 0; i < chs_conf_->servo_dev().size(); i++)
                    if (&chs_conf_->servo_dev(i).si() == snsr_info_)
                        return &chs_conf_->servo_dev(i).sn_ind();

                for (int i = 0; i < chs_conf_->wireless_dev().size(); i++)
                    if (&chs_conf_->wireless_dev(i).si() == snsr_info_)
                        return &chs_conf_->wireless_dev(i).sn_ind();

                AERROR << "can't get sensor indicator for\n" <<
                    snsr_info_->DebugString();

                return nullptr;
            }

            const Message& GetDevConfig() {
                for (int i = 0; i < chs_conf_->gpio_dev().size(); i++)
                    if (&chs_conf_->gpio_dev(i).si() == snsr_info_)
                        return chs_conf_->gpio_dev(i);

                for (int i = 0; i < chs_conf_->adc_dev().size(); i++)
                    if (&chs_conf_->adc_dev(i).si() == snsr_info_)
                        return chs_conf_->adc_dev(i);

                for (int i = 0; i < chs_conf_->pwm_dev().size(); i++)
                    if (&chs_conf_->pwm_dev(i).si() == snsr_info_)
                        return chs_conf_->pwm_dev(i);

                for (int i = 0; i < chs_conf_->lidar_dev().size(); i++)
                    if (&chs_conf_->lidar_dev(i).si() == snsr_info_)
                        return chs_conf_->lidar_dev(i);

                for (int i = 0; i < chs_conf_->imu_dev().size(); i++)
                    if (&chs_conf_->imu_dev(i).si() == snsr_info_)
                        return chs_conf_->imu_dev(i);

                for (int i = 0; i < chs_conf_->dtof_dev().size(); i++)
                    if (&chs_conf_->dtof_dev(i).si() == snsr_info_)
                        return chs_conf_->dtof_dev(i);

                for (int i = 0; i < chs_conf_->camera_dev().size(); i++)
                    if (&chs_conf_->camera_dev(i).si() == snsr_info_)
                        return chs_conf_->camera_dev(i);

                for (int i = 0; i < chs_conf_->mcu_dev().size(); i++)
                    if (&chs_conf_->mcu_dev(i).si() == snsr_info_)
                        return chs_conf_->mcu_dev(i);

                for (int i = 0; i < chs_conf_->aud_dev().size(); i++)
                    if (&chs_conf_->aud_dev(i).si() == snsr_info_)
                        return chs_conf_->aud_dev(i);

                for (int i = 0; i < chs_conf_->servo_dev().size(); i++)
                    if (&chs_conf_->servo_dev(i).si() == snsr_info_)
                        return chs_conf_->servo_dev(i);

                for (int i = 0; i < chs_conf_->wireless_dev().size(); i++)
                    if (&chs_conf_->wireless_dev(i).si() == snsr_info_)
                        return chs_conf_->wireless_dev(i);

                for (int i = 0; i < chs_conf_->wireless_dev().size(); i++)
                    if (&chs_conf_->wireless_dev(i).si() == snsr_info_)
                        return chs_conf_->wireless_dev(i);

                AERROR << "can't get device itself for\n" <<
                    snsr_info_->DebugString();

                return *snsr_info_;
            }

            std::unique_ptr<CircularBuffer<uint8_t>>
                cbuf_raw_ = nullptr;

            std::unique_ptr<CircularBuffer<uint8_t>>
                cbuf_can_ = nullptr;

            //for parsing data periodically
#if USE_PARSER_TIMER
            std::unique_ptr<cyber::Timer> tim_ = nullptr;
#endif
            //for data packing and downstream
            std::unique_ptr<PackerBaseItf> packer_ = nullptr;

            const SensorInfo* snsr_info_ = nullptr;
            const ChassisConfig* chs_conf_ = nullptr;

            int frame_count_ = 0;
            FrameProcessor frame_processor_ = nullptr;

            std::atomic<bool> is_parsing_busy_ = {false};
    };

} //namespace parser
} //namespace chss
} //namespace mstf
