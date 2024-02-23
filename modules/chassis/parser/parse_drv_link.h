#pragma once

#include "cyber/common/macros.h"

#include "modules/chassis/proto/chassis_config.pb.h"

#include "modules/chassis/parser/common_inf.h"
#include "modules/chassis/drivers/can/can_data.h"
#include "modules/chassis/drivers/uart/serial_data.h"
#include "modules/chassis/drivers/i2c/i2c_data.h"
#include "modules/chassis/drivers/soc/soc_data.h"

namespace mstf {
namespace chss {
namespace parser {

    using namespace apollo;
    using namespace /*mstf::chss::*/driver;
    using namespace /*mstf::chss::*/proto;

    class ParseDrvLink {
        public:
            DECLARE_SINGLETON(ParseDrvLink)
            virtual ~ParseDrvLink();

        public:
            int Init(const SensorIndicator*);
            int Start(const SensorIndicator*);
            int Stop(const SensorIndicator*);
            int Resume(const SensorIndicator*);
            int Close(const SensorIndicator*);

            inline void Finish() {
                delete this;
            }

            //register low layer data handler
            inline void RegisterCanListener(const
                    CanDataListener& l, const SensorIndicator* si) {
                dev_can_hdls_[const_cast<SensorIndicator*>(si)] = l;
            }

            inline void RegisterI2cListener(const
                    IIcDataListener& l, const SensorIndicator* si) {
                dev_i2c_hdls_[const_cast<SensorIndicator*>(si)] = l;
            }

            inline void RegisterUartListener(const
                    UartDataListener& l, const SensorIndicator* si) {
                dev_serial_hdls_[const_cast<SensorIndicator*>(si)] = l;
            }

            inline void RegisterSocListener(const
                    SocDataListener& l, const SensorIndicator* si) {
                soc_hdls_[const_cast<SensorIndicator*>(si)] = l;
            }

            // --------------------- data downstream ---------------------
            // write SOC
            bool WriteSoc(const SensorIndicator*, const Message&);

            // write CAN
            size_t WriteCan(const SensorIndicator*,
                    const int, const uint8_t*, const size_t);
            inline size_t WriteCan(const SensorIndicator* si,
                    const int id,
                    const std::vector<uint8_t>& data) {
                if (data.empty())
                    return 0;
                return WriteCan(si, id, &data[0], data.size());
            }

            // write IIC
            size_t WriteI2c(const SensorIndicator*,
                    const uint8_t*, const size_t);
            inline size_t WriteI2c(const SensorIndicator* si,
                    const std::vector<uint8_t>& data) {
                if (data.empty())
                    return 0;
                return WriteI2c(si, &data[0], data.size());
            }

            // write UART
            size_t WriteUart(const SensorIndicator*,
                    const uint8_t*, const size_t);
            inline size_t WriteUart(const SensorIndicator* si,
                    const std::vector<uint8_t>& data) {
                if (data.empty())
                    return 0;
                return WriteUart(si, &data[0], data.size());
            }

            // --------------------- data upstream ---------------------
            void OnRecvCan(const SensorIndicator*, const uint8_t*, const size_t);
            void OnRecvIIc(const SensorIndicator*, const uint8_t*, const size_t);
            void OnRecvUart(const SensorIndicator*, const uint8_t*, const size_t);
            void OnRecvSoc(const SensorIndicator*, const Message&,
                    const uint8_t* = nullptr, const size_t = 0);

        private:
            //helpers
            //1, message listeners & callback
            inline void _CanMessageHandle(const SensorIndicator* si,
                    const uint8_t* buf,
                    const size_t len) {
                for (auto x : dev_can_hdls_) {
                    if (x.first == si) {
                        x.second(si, buf, len);
                        return;
                    }
                }
                AERROR << "can't get can data handle\n" <<
                    si->DebugString();
            }

            inline void _IIcMessageHandle(const SensorIndicator* si,
                    const uint8_t* buf,
                    const size_t len) {
                for (auto x : dev_i2c_hdls_) {
                    if (x.first == si) {
                        x.second(si, buf, len);
                        return;
                    }
                }
                AERROR << "can't get i2c data handle\n" <<
                    si->DebugString();
            }

            inline void _UartMessageHandle(const SensorIndicator* si,
                    const uint8_t* buf,
                    const size_t len) {
                for (auto x : dev_serial_hdls_) {
                    if (x.first == si) {
                        x.second(si, buf, len);
                        return;
                    }
                }
                AERROR << "can't get uart data handle\n" <<
                    si->DebugString();
            }

            inline void _SocMessageHandle(const SensorIndicator* si,
                    const Message& msg,
                    const uint8_t* buf,
                    const size_t len) {
                for (auto x : soc_hdls_) {
                    if (x.first == si) {
                        x.second(si, msg, buf, len);
                        return;
                    }
                }
                AERROR << "can't get soc data handle\n" <<
                    si->DebugString();
            }

            //2, find data drivers
            inline std::shared_ptr<CanData>
                _FindCanData(const SensorIndicator* si) {
                    std::map<const SensorIndicator*,
                    std::shared_ptr<CanData>>::iterator i;
                    for (i = dev_can_data_.begin();
                            i != dev_can_data_.end(); i++) {
                        if (i->first == si) {
                            return i->second;
                        }
                    }
                    AERROR << "can't get can data for\n" <<
                        si->DebugString();

                    return {};
                }

            inline std::shared_ptr<SerialData>
                _FindSerialData(const SensorIndicator* si) {
                    for (auto x : dev_serial_data_) {
                        if (x.first == si) {
                            return x.second;
                        }
                    }
                    AERROR << "can't get serial data for\n" <<
                        si->DebugString();

                    return {};
                }

            inline std::shared_ptr<IIcData>
                _FindI2cData(const SensorIndicator* si) {
                    for (auto x : dev_iic_data_) {
                        if (x.first == si) {
                            return x.second;
                        }
                    }
                    AERROR << "can't get i2c data for\n" <<
                        si->DebugString();

                    return {};
                }

            inline std::shared_ptr<DriveDataItf>
                _FindDriverBase(const SensorIndicator* si) {
                    for (auto x : dev_drivers_) {
                        if (x.first == si) {
                            return x.second;
                        }
                    }
                    AERROR << "can't get driver base for\n" <<
                        si->DebugString();

                    return {};
                }

        private:
            //CAN port
            std::map<const SensorIndicator*,
                std::shared_ptr<CanData>> dev_can_data_ {};
            // =&ParserBaseItf::OnOriginalDataCan()
            std::map<const SensorIndicator*,
                CanDataListener> dev_can_hdls_ {};

            //UART port
            std::map<const SensorIndicator*,
                std::shared_ptr<SerialData>> dev_serial_data_ {};
            // =&ParserBaseItf::OnOriginalDataUart()
            std::map<const SensorIndicator*,
                UartDataListener> dev_serial_hdls_ {};

            //I2C port
            std::map<const SensorIndicator*,
                std::shared_ptr<IIcData>> dev_iic_data_ {};
            // =&ParserBaseItf::OnOriginalDataI2c()
            std::map<const SensorIndicator*,
                IIcDataListener> dev_i2c_hdls_ {};

            //SPI port TODO
            //

            //soc data is unique
            std::shared_ptr<SocData> soc_data_ {};
            std::map<const SensorIndicator*,
                SocDataListener> soc_hdls_ {};

            //indicator & driver base pair
            std::map<const SensorIndicator*,
                std::shared_ptr<DriveDataItf>> dev_drivers_ {};
    };

} //namespace parser
} //namespace chss
} //namespace mstf
