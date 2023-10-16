#pragma once

#include "cyber/common/macros.h"

#include "modules/chassis/proto/chassis_config.pb.h"

#include "modules/chassis/drivers/can/can_data.h"
#include "modules/chassis/drivers/soc/soc_data.h"
//#include "modules/chassis/drivers/uart/serial_data.h"

namespace mstf {
namespace chss {
namespace parser {

    using namespace /*mstf::chss::*/driver;

    using RawDataListener = std::function<void
        (const uint8_t*, const size_t)>;

    class RawManage {
        public:
            DECLARE_SINGLETON(RawManage)
            virtual ~RawManage();

        public:
            int Init(const ChassisConfig*,
                    const SensorInfo*);
            int Start(const SensorInfo*);
            int Stop(const SensorInfo*);
            int Close(const SensorInfo*);

            inline void Finish() {
                delete this;
            }

            inline void RegisterRawListener(const
                    RawDataListener l, const SensorInfo* si) {
                uart_raw_handle_[const_cast<SensorInfo*>(si)] = l;
            }

            inline void RegisterCanListener(const
                    CanDataListener l, const SensorInfo* si) {
                can_raw_handle_[const_cast<SensorInfo*>(si)] = l;
            }

            inline void RegisterSocListener(const
                    SocDataListener l, const SensorInfo* si) {
                soc_raw_handle_[const_cast<SensorInfo*>(si)] = l;
            }

            // write serial & can
            size_t WriteCan(const std::vector<uint8_t>&);
            size_t WriteCan(const int, const uint8_t*, const size_t);
            size_t WriteUart(const uint8_t*, const uint32_t);
            bool WriteSoc(const Message&);

            // data come
            void OnRecvCan(const uint8_t*, const size_t);
            void OnRecvUart(const uint8_t*, const size_t);
            void OnRecvSoc(const Message&);

        private:
            inline void _CanMessageHandle(const uint8_t* buf,
                    const size_t len) {
                for (auto c : sis_) {
                    for (auto x : can_raw_handle_) {
                        if (x.first == c) {
                            x.second(buf, len);
                        }
                    }
                }
            }

            /*
            inline void _UartMessageHandle(const uint8_t* buf,
                    const size_t len) {
                for (auto c : sis_) {
                    for (auto x : uart_raw_handle_) {
                        if (x.first == c) {
                            x.second(buf, len);
                        }
                    }
                }
            }
            */

            inline void _SocMessageHandle(const Message& msg) {
                for (auto c : sis_) {
                    for (auto x : soc_raw_handle_) {
                        if (x.first == c) {
                            x.second(msg);
                        }
                    }
                }
            }

        private:
            //can
            std::unique_ptr<CanData> can_data_ = nullptr;
            // =&ParserBaseItf::OnOriginalDataCan()
            std::map<SensorInfo*, CanDataListener> can_raw_handle_;

            //serial TODO
            //std::unique_ptr<SerialData> serial_data_ = nullptr;
            std::map<SensorInfo*, RawDataListener> uart_raw_handle_;

            std::unique_ptr<SocData> soc_data_ = nullptr;
            std::map<SensorInfo*, SocDataListener> soc_raw_handle_;

            std::vector<const SensorInfo*> sis_;

            const ChassisConfig* chs_conf_ = nullptr;
    };

} //namespace parser
} //namespace chss
} //namespace mstf
