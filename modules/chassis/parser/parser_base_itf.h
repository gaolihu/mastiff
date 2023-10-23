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
                AINFO << "Init ParserBaseItf, port type: " << type <<
                    ", sensor name: " << snsr_info_->name() <<
                    ", cbuf:\n" << (cbs == nullptr ? "no cbuf" : cbs->DebugString());
                switch (type) {
                    case E_COMM_PORT_I2C:
                    case E_COMM_PORT_SERIAL: {
                        if (cbs != nullptr) {
                            cbuf_raw_ = std::make_unique<CircularBuffer<uint8_t>>
                                (cbs->circlular_size(), cbs->circle_name());
                            tim_.reset(new cyber::Timer(cbs->parser_freq(),
                                        [this] ()->void {
                                        if (cbuf_raw_->CheckCbuf())
                                            cbuf_raw_->ReleaseCbuf();
                                        },
                                        false)
                                    );
                        } else {
                            AINFO << "no circular buf for uart port";
                        }
                        const SensorIndicator* it = nullptr;
                        if ((it = GetIndicator()) != nullptr) {
                            if (it->type() == E_DEVICE_LINELASER)
                                packer_ = std::make_unique<LlaserPacker>(snsr_info_->name());
                            //else if (it->type() == E_DEVICE_LINELASER())
                            else
                                AINFO << "TODO, packer!";
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
                            tim_.reset(new cyber::Timer(cbs->parser_freq(),
                                        [this] ()->void {
                                        if (cbuf_can_->CheckCbuf())
                                            cbuf_can_->ReleaseCbuf();
                                        },
                                        false)
                                    );
                            packer_ = std::make_unique<ServoPacker>(snsr_info_->name());
                        } else {
                            AINFO << "no circular buf for can port";
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
                AINFO << "ParserBaseItf DFT init";
                return 0;
            }
            virtual int Start() {
                AINFO << "ParserBaseItf DFT start " <<
                    snsr_info_->name();
                if (cbuf_raw_)
                    cbuf_raw_->Open();

                //data parser setting
                if (tim_ != nullptr) {
                    AINFO << "Start ParserBaseItf timer for " <<
                        snsr_info_->name();
                    tim_->Start();
                }
                return RawManage::Instance()->Start(snsr_info_);
            }
            virtual int Stop() {
                AINFO << "ParserBaseItf DFT Stop" <<
                    snsr_info_->name();
                if (cbuf_raw_)
                    cbuf_raw_->Close();

                if (tim_ != nullptr) {
                    AINFO << "Stop ParserBaseItf timer" <<
                        snsr_info_->name();
                    tim_->Stop();
                }
                return RawManage::Instance()->Stop(snsr_info_);
            }
            virtual int Close() {
                AINFO << "ParserBaseItf DFT Close" <<
                    snsr_info_->name();
                if (cbuf_raw_)
                    cbuf_raw_->Close();

                if (tim_ != nullptr) {
                    AINFO << "Close ParserBaseItf timer" <<
                        snsr_info_->name();
                    tim_->Stop();
                }
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
                //notify the specific raw parser work
                ParseRawBuffer(buf, size);
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

            //helper
            const SensorIndicator* GetIndicator() {
                if (chs_conf_->has_gpio_dev())
                    if (&chs_conf_->gpio_dev().si() == snsr_info_)
                        return &chs_conf_->gpio_dev().sn_ind();

                if (chs_conf_->has_adc_dev())
                    if (&chs_conf_->adc_dev().si() == snsr_info_)
                        return &chs_conf_->adc_dev().sn_ind();

                if (chs_conf_->has_pwm_dev())
                    if (&chs_conf_->pwm_dev().si() == snsr_info_)
                        return &chs_conf_->pwm_dev().sn_ind();

                if (chs_conf_->has_lidar_dev())
                    if (&chs_conf_->lidar_dev().si() == snsr_info_)
                        return &chs_conf_->lidar_dev().sn_ind();

                if (chs_conf_->has_imu_dev())
                    if (&chs_conf_->imu_dev().si() == snsr_info_)
                        return &chs_conf_->imu_dev().sn_ind();

                if (chs_conf_->has_dtof_dev())
                    if (&chs_conf_->dtof_dev().si() == snsr_info_)
                        return &chs_conf_->dtof_dev().sn_ind();

                if (chs_conf_->has_camera_dev())
                    if (&chs_conf_->camera_dev().si() == snsr_info_)
                        return &chs_conf_->camera_dev().sn_ind();

                if (chs_conf_->has_mcu_dev())
                    if (&chs_conf_->mcu_dev().si() == snsr_info_)
                        return &chs_conf_->mcu_dev().sn_ind();

                if (chs_conf_->has_aud_dev())
                    if (&chs_conf_->aud_dev().si() == snsr_info_)
                        return &chs_conf_->aud_dev().sn_ind();

                if (chs_conf_->has_servo_dev())
                    if (&chs_conf_->servo_dev().si() == snsr_info_)
                        return &chs_conf_->servo_dev().sn_ind();

                if (chs_conf_->has_wireless_dev())
                    if (&chs_conf_->wireless_dev().si() == snsr_info_)
                        return &chs_conf_->wireless_dev().sn_ind();

                if (chs_conf_->has_linelaser_dev())
                    if (&chs_conf_->linelaser_dev().si() == snsr_info_)
                        return &chs_conf_->linelaser_dev().sn_ind();

                AERROR << "can't get sensor indicator!";

                return nullptr;
            }

        protected:
            std::unique_ptr<CircularBuffer<uint8_t>>
                cbuf_raw_ = nullptr;

            std::unique_ptr<CircularBuffer<uint8_t>>
                cbuf_can_ = nullptr;

            //for parsing data periodically
            std::unique_ptr<cyber::Timer> tim_ = nullptr;

            //for data packing and downstream
            std::unique_ptr<PackerBaseItf> packer_ = nullptr;

            const SensorInfo* snsr_info_ = nullptr;
            const ChassisConfig* chs_conf_ = nullptr;

            FrameProcessor frame_processor_ = nullptr;
    };

} //namespace parser
} //namespace chss
} //namespace mstf
