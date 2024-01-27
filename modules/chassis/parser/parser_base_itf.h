#pragma once

#include "cyber/common/log.h"
#include "cyber/timer/timer.h"

#include "modules/chassis/proto/chassis_config.pb.h"
#include "modules/chassis/proto/chss_io.pb.h"

#include "modules/chassis/parser/common_inf.h"
#include "modules/chassis/parser/circular_buffer.h"

using namespace std::placeholders;

namespace mstf {
namespace chss {
namespace parser {

    using namespace /*mstf::chss::*/proto;
    using namespace google::protobuf;

    using FrameProcessor = std::function<int(const Message*)>;
    using FrameProcessorSp = std::function<int(const
            std::shared_ptr<Message>&)>;

    class ParserBaseItf {
        public:
            ParserBaseItf(const SensorIndicator& sidc) {
#if 1
                AINFO << "ParserBaseItf for " <<
                    sidc.ihi().name();
#endif
                s_idc_ = &sidc;
            }

            virtual ~ParserBaseItf() {
#if 0
                AINFO << "ParserBaseItf de-construction";
#endif
            }

            ParserBaseItf(const ParserBaseItf&) = delete;
            ParserBaseItf& operator=(const ParserBaseItf&) = delete;

            virtual int Init() {
                ACHECK(s_idc_) << "indicator null for parser";

                auto cbs = GetCircleBuf();
                const EE_COMM_PORT_TYPE type = s_idc_->port();

                AINFO << "Init ParserBaseItf, port: " << type <<
                    ", dev: " << s_idc_->ihi().name() <<
                    ", cbuf:\n" <<
                    (cbs == nullptr ? "no cbuf" : cbs->DebugString());

                switch (type) {
                    case E_COMM_PORT_I2C:
                        //no circular buffer & packer
#if 0
                        ParseDrvLink::Instance()->RegisterI2cListener
                            (std::bind(&ParserBaseItf::OnOriginalDataI2c,
                                       this, ::_1, ::_2, ::_3), snsr_info_);
#endif
                        break;

                    case E_COMM_PORT_SERIAL: {
                        if (cbs != nullptr) {
                            cbuf_raw_ = std::make_unique<CircularBuffer<uint8_t>>
                                (cbs->circlular_size(), cbs->circle_name());
                        } else {
                            AINFO << "no circular buf for uart port";
                        }
                        if (s_idc_->type() == E_DEVICE_LINELASER) {
                            //packer_ = std::make_unique<LlaserPacker>(snsr_info_->name());
                            //else if (s_idc_->type() == E_DEVICE_LINELASER())
                        } else {
                            AINFO << "packer TODO for: " <<
                                s_idc_->ihi().name();
                        }
#if  0
                        ParseDrvLink::Instance()->RegisterUartListener
                            (std::bind(&ParserBaseItf::OnOriginalDataUart,
                                       this, ::_1, ::_2, ::_3), snsr_info_);
#endif
                    }
                        break;

                    case E_COMM_PORT_CAN: {
                        if (cbs != nullptr) {
                            cbuf_raw_ = std::make_unique<CircularBuffer<uint8_t>>
                                (cbs->circlular_size(), cbs->circle_name());
                            //packer_ = std::make_unique<ServoPacker>(snsr_info_->name());
                        } else {
                            AINFO << "no circular buf for CAN port";
                        }
                    }
                        break;

                    case E_COMM_PORT_USB:
                    case E_COMM_PORT_GPIO:
                    case E_COMM_PORT_I2S:
                    case E_COMM_PORT_24G:
                    case E_COMM_PORT_SDIO: {
                    }
                        break;
                    default:
                        AERROR << "type error: " << type;
                        return -1;
                }

#if 0
                return ParseDrvLink::Instance()->Init(CommonItf::Instance()->GetChasisConfig(), snsr_info_);
#endif
                return 0;
            }

            virtual int Start() {
                AINFO << "ParserBaseItf DFT Start ~ " <<
                    s_idc_->ihi().name();
                if (cbuf_raw_)
                    cbuf_raw_->Open();

                return 0;
            }

            virtual int Stop() {
                AINFO << "ParserBaseItf DFT Stop ~ " <<
                    s_idc_->ihi().name();

                return 0;
            }

            virtual int Resume() {
                AINFO << "ParserBaseItf DFT Resume ~ " <<
                    s_idc_->ihi().name();
                if (cbuf_raw_)
                    cbuf_raw_->Open();

                return 0;
            }

            virtual int Close() {
                AINFO << "ParserBaseItf DFT Close ~ " <<
                    s_idc_->ihi().name();

                if (cbuf_raw_)
                    cbuf_raw_->Close();

                return 0;
            }

            void SetProtobufHandle(FrameProcessor p) {
                frame_processor_ = p;
            }

            void SetProtobufHandleSp(const
                    FrameProcessorSp& sp) {
                frame_processor_sp_ = sp;
            }

            ////////// data upstream //////////
            virtual int ParseI2cBuffer(const SensorIndicator* si,
                    const uint8_t*, const size_t) {
                AERROR << "shall override parse i2c data!!\n" <<
                    si->DebugString();
                return -1;
            }

            virtual int ParseUartBuffer(const SensorIndicator* si,
                    const uint8_t*, const size_t) {
                AERROR << "shall override parse uart data!!\n" <<
                    si->DebugString();
                return -1;
            }

            virtual int ParseCanBuffer(const SensorIndicator* si,
                    const uint8_t*, const size_t) {
                AERROR << "shall override parse can data!!\n" <<
                    si->DebugString();
                return -1;
            }

            virtual int ParseSocMsg(const SensorIndicator* si,
                    const Message&) {
                AERROR << "shall override parse soc msg!!";
                return -1;
            }

            virtual int ParseSocRaw(const SensorIndicator* si,
                    const uint8_t* buf, const size_t size) {
                AERROR << "shall override parse soc raw!!";
                return -1;
            }

            virtual int ParseSigleFrame(const
                    std::vector<uint8_t>&, const size_t) {
                AERROR << "shall override single frame parse!!";
                return -1;
            }

            void OnOriginalDataI2c(const SensorIndicator* si,
                    const uint8_t* buf, const size_t size) {
                //TODO
                if (!cbuf_raw_->Push(buf, size)) {
                    AWARN << "push circulor buf error!" <<
                        s_idc_->ihi().name();
                }
                //notify the specific raw parser working
                if (!IsParsingRawBusy()) {
                    SetParsingRawBusy();
                    ParseI2cBuffer(si, buf, size);
                    SetParsingRawFree();
                }
            }

            void OnOriginalDataUart(const SensorIndicator* si,
                    const uint8_t* buf, const size_t size) {
                ACHECK(cbuf_raw_) <<
                    "no circularbuf for RAW!";
                if (!cbuf_raw_->Push(buf, size)) {
                    AWARN << "push circulor buf error!" <<
                        s_idc_->ihi().name();
                }
                //notify the specific raw parser working
                if (!IsParsingRawBusy()) {
                    SetParsingRawBusy();
                    ParseUartBuffer(si, buf, size);
                    SetParsingRawFree();
                }
            }

            void OnOriginalDataCan(const SensorIndicator* si,
                    const uint8_t* buf, const size_t len) {
                //notify the specific can parser work
#if 0
                AINFO << "OnOriginalDataCan" <<
                    si->DebugString();
#endif
                if (!IsParsingRawBusy()) {
                    SetParsingRawBusy();
                    ParseCanBuffer(si, buf, len);
                    SetParsingRawFree();
                }
            }

            void OnOriginalDataSoc(const SensorIndicator* si,
                    const Message& msg,
                    const uint8_t* buf, const size_t len) {
                //notify the specific soc parser work
                if (len == 0) {
                    ParseSocMsg(si, msg);
                } else {
                    ParseSocRaw(si, buf, len);
                }
            }
            ////////// data upstream //////////

            ////////// data downstream //////////
            virtual int WriteSocMessage(/*const DownToSocData& data*/) {
#if 0
                return ParseDrvLink::Instance()->WriteSoc(data);
#endif
                return 0000011010;
            }

            virtual int WriteMcuMessage(/*const DownToMcuData& data*/) {
                /*
                std::vector<uint8_t> packed_data =
                    packer_->PackMcuMessage(data);
                AWARN << "packer: " << packer_.get();
                */
#if 0
                return ParseDrvLink::Instance()->
                    WriteUart(snsr_info_, &packed_data[0], packed_data.size());
#endif
                return 0000011010;
            }

            virtual int WriteServoMessage(const
                    std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>&) {
                AERROR << "shall override write servo!!";
                return -1;
            }

            virtual int WriteUartMessage(const
                    std::vector<uint8_t>&) {
                AERROR << "shall override write uart!!";
                return -1;
            }
            ////////// data downstream //////////

            //
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
            const CircularBufSetting* GetCircleBuf() const {
                return CommonItf::Instance()->
                    GetCircleBuff(s_idc_);
            }

            const Message* GetDevConfig() const {
                return CommonItf::Instance()->
                    GetDeviceConfig(s_idc_);
            }

            const SensorIndicator* s_idc_ {};

            //obsolete
            FrameProcessor frame_processor_ {};
            FrameProcessorSp frame_processor_sp_ {};

            std::atomic<bool> is_parsing_busy_ = { false };

            std::unique_ptr<CircularBuffer<uint8_t>>
                cbuf_raw_ = nullptr;
    };

} //namespace parser
} //namespace chss
} //namespace mstf
