#pragma once

#include "cyber/common/macros.h"

#include "modules/chassis/proto/chassis_config.pb.h"

#include "modules/chassis/drivers/can/can_data.h"
#include "modules/chassis/drivers/soc/soc_data.h"
#include "modules/chassis/drivers/uart/serial_data.h"
#include "modules/chassis/drivers/i2c/i2c_data.h"

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
            int Resume(const SensorInfo*);
            int Close(const SensorInfo*);

            inline void Finish() {
                delete this;
            }

            inline void RegisterRawListener(const
                    RawDataListener l, const SensorInfo* si) {
                dev_handle_map_[const_cast<SensorInfo*>(si)] = l;
            }

            inline void RegisterCanListener(const
                    CanDataListener l, const SensorInfo* si) {
                can_raw_handle_[const_cast<SensorInfo*>(si)] = l;
            }

            inline void RegisterSocListener(const
                    SocDataListener l, const SensorInfo* si) {
                soc_raw_handle_[const_cast<SensorInfo*>(si)] = l;
            }

            // write UART & CAN & SOC
            size_t WriteUart(const SensorInfo* si,
                    const uint8_t*, const size_t);
            inline size_t WriteUart(const SensorInfo* si,
                    const std::vector<uint8_t>& data) {
                if (data.empty())
                    return 0;
                return WriteUart(si, &data[0], data.size());
            }
            size_t WriteCan(const int, const uint8_t*, const size_t);
            inline size_t WriteCan(const int id, const
                    std::vector<uint8_t>& data) {
                if (data.empty())
                    return 0;
                return WriteCan(id, &data[0], data.size());
            }
            bool WriteSoc(const Message&);

            // data come
            void OnRecvCan(const uint8_t*, const size_t);
            void OnRecvIIc(const void*, const uint8_t*, const size_t);
            void OnRecvUart(const void*, const uint8_t*, const size_t);
            void OnRecvSoc(const Message&);

        private:
            //helpers
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

            inline void _UartMessageHandle(const void* conf,
                    const uint8_t* buf,
                    const size_t len) {
                for (auto x : dev_handle_map_) {
                    if (x.first == conf) {
                        x.second(buf, len);
                    }
                }
            }

            inline void _IIcMessageHandle(const void* conf,
                    const uint8_t* buf,
                    const size_t len) {
                for (auto x : dev_handle_map_) {
                    if (x.first == conf) {
                        x.second(buf, len);
                    }
                }
            }

            inline void _SocMessageHandle(const Message& msg) {
                for (auto c : sis_) {
                    for (auto x : soc_raw_handle_) {
                        if (x.first == c) {
                            x.second(msg);
                        }
                    }
                }
            }

            inline SerialData* _FindSerialData(const SensorInfo* si) {
                for (auto s : dev_serial_map_) {
                    if (s.first == si) {
                        return s.second;
                    }
                }
                AERROR << "can't get serial data for\n" <<
                    si->DebugString();

                return nullptr;
            }

            inline IIcData* _FindI2cData(const SensorInfo* si) {
                for (auto s : dev_i2c_map_) {
                    if (s.first == si) {
                        return s.second;
                    }
                }
                AERROR << "can't get i2c data for\n" <<
                    si->DebugString();

                return nullptr;
            }

        private:
            //can
            std::unique_ptr<CanData> can_data_ = nullptr;
            // =&ParserBaseItf::OnOriginalDataCan()
            std::map<SensorInfo*, CanDataListener> can_raw_handle_ {};

            //serial
            std::map<const SensorInfo*, SerialData*> dev_serial_map_ {};
            std::map<const SensorInfo*, IIcData*> dev_i2c_map_ {};
            std::map<const SensorInfo*, RawDataListener> dev_handle_map_ {};
            // =&ParserBaseItf::OnOriginalDataRaw()

            std::unique_ptr<SocData> soc_data_ = nullptr;
            std::map<SensorInfo*, SocDataListener> soc_raw_handle_ {};

            std::vector<const SensorInfo*> sis_ {};

            const ChassisConfig* chs_conf_ = nullptr;
    };

} //namespace parser
} //namespace chss
} //namespace mstf
