#pragma once

#include "cyber/common/log.h"

#include "modules/chassis/proto/chassis_config.pb.h"

#include "modules/chassis/parser/parser_base_itf.h"

namespace mstf {
namespace chss {
namespace device {

    using namespace /*mstf::chss::*/proto;
    using namespace /*mstf::chss::*/parser;

    using SensorGpioSwitch = std::function<int(const bool)>;
    using SensorPwmRegulator = std::function<int(const
            int freq_k, const int duty_percent)>;

    class DeviceBaseItf {
        public:
            DeviceBaseItf(const SensorInfo& si,
                    const SensorIndicator& ind) {
                sensor_info_ = (const_cast<SensorInfo*>(&si));
                sensor_ind_ = const_cast<SensorIndicator*>(&ind);

                sensor_ind_->set_status(EE_DEVICE_STATUS::
                        E_STATUS_UNINITIALIZED);
#ifdef CHSS_PKG_DBG
                AWARN << "DeviceBaseItf for <" <<
                    sensor_info_->name() <<
#if 0
                    " >, sensor_info_ p: " << sensor_info_ <<
                    ", sensor_ind_ p: " << sensor_ind_;
                AINFO << "sensor_info_:\n" << sensor_info_->DebugString();
                AINFO << "sensor_ind_:\n" << sensor_ind_->DebugString();
#else
                    ">";
#endif
#endif
            }

            virtual ~DeviceBaseItf() {
#ifdef CHSS_PKG_DBG
                AWARN << "DeviceBaseItf de-construct <" <<
                    sensor_info_->name() <<
#if 0
                    " >>, sensor_info_ p: " << sensor_info_ <<
                    ", sensor_ind_ p: " << sensor_ind_;
                AINFO << "sensor_info_:\n" << sensor_info_->DebugString();
                AINFO << "sensor_ind_:\n" << sensor_ind_->DebugString();
#else
                    ">";
#endif
#endif
                data_parser_.reset();
            }

            // control APIs
            // for private use
            virtual int Init() {
                return data_parser_->Init();
            }
            virtual int Start(void) {
                return data_parser_->Start();
            }
            virtual int Stop(void) {
                return data_parser_->Stop();
            }
            virtual void Close(void) {
                data_parser_->Close();
            }

            void* GetDeviceParser(void) {
                if (data_parser_)
                    return data_parser_.get();
                else
                    return nullptr;
            }

            int InitSensor(SensorIndicator* si) {
                if (si != sensor_ind_) {
                    AWARN << "DeviceBase indicator not matched!";
                    return -1;
                }
                int ret = Init();
                if (ret == 0) {
                    sensor_inited_ = true;
                    sensor_working_ = false;
                    sensor_stoped_ = true;
                    SetDeviceStatus(EE_DEVICE_STATUS::
                            E_STATUS_INITIALIZED);
                } else
                    AWARN << "DeviceBase init error!";
                return ret;
            }

            int StartSensor() {
                int ret = -1;
                CtrlSwitch(true);
                std::this_thread::sleep_for(std::chrono::
                        milliseconds(10));
                if (GetDeviceStatus() == EE_DEVICE_STATUS::
                        E_STATUS_UNINITIALIZED) {
                    AWARN << GetSensorName() <<
                        " uninitialized, no start!";
                    return ret;
                } else if (GetDeviceStatus() == EE_DEVICE_STATUS::
                        E_STATUS_WORKING) {
                    AWARN << GetSensorName() <<
                        " already started, no start!";
                    return 0;
                }
                ret = Start();
                if (ret == 0) {
                    sensor_working_ = true;
                    sensor_stoped_ = false;
                    SetDeviceStatus(EE_DEVICE_STATUS::
                            E_STATUS_WORKING);
                }
                return ret;
            }

            int StopSensor() {
                int ret = -1;
                if (GetDeviceStatus() == EE_DEVICE_STATUS::
                        E_STATUS_UNINITIALIZED) {
                    AWARN << GetSensorName() <<
                        " uninitialized, no stop!";
                    return ret;
                }
                ret = Stop();
                if (ret == 0) {
                    sensor_working_ = false;
                    sensor_stoped_ = true;
                    SetDeviceStatus(EE_DEVICE_STATUS::
                            E_STATUS_IDLE);
                }
                CtrlSwitch(false);
                return ret;
            }

            void CloseSensor() {
                if (GetDeviceStatus() == EE_DEVICE_STATUS::
                        E_STATUS_UNINITIALIZED) {
                    AWARN << GetSensorName() <<
                        " uninitialized, no close!";
                    return;
                }
                sensor_working_ = false;
                sensor_stoped_ = true;
                SetDeviceStatus(EE_DEVICE_STATUS::
                        E_STATUS_SHUTDOWN);
                CtrlSwitch(false);
                Close();
            }

            // status APIs
            std::string CheckDeviceInfo() {
                return std::string("-> DeviceBase info:\n" +
                        sensor_info_->DebugString() +
                        "-> DeviceBase indicator:\n" +
                        sensor_ind_->DebugString());
            }

            std::string CheckDeviceStatus() {
                //TODO
                return std::string("No Error");
            }

            EE_DEVICE_STATUS GetDeviceStatus() {
                return sensor_ind_->status();
            };

            bool IsDeviceInited(SensorIndicator*) {
                return sensor_inited_;
            }

            bool IsDeviceWorking(SensorIndicator*) {
                return sensor_working_;
            }

            bool IsDeviceStoped(SensorIndicator*) {
                return sensor_stoped_;
            }

            SensorInfo* GetSensorInfo() {
                return sensor_info_;
            }

            SensorIndicator* GetSensorIndicator() {
                return sensor_ind_;
            }

            std::string GetSensorName() {
                return sensor_info_->name();
            }

            std::string GetSensorManufacture() {
                return sensor_info_->manufacture_by();
            }

            std::string GetSensorType() {
                return sensor_info_->type();
            }

            void RegisterGpioSwitch(SensorGpioSwitch sw) {
                sensor_sw_ = sw;
            };

            void RegisterPwmRegulate(SensorPwmRegulator pr) {
                pwm_regulator_ = pr;
            };

            int RegulatePwm(const int freq_k = 0,
                    const int duty_percent = 0) {
                if (pwm_regulator_)
                    return pwm_regulator_(freq_k, duty_percent);
                else
                    AERROR << "do not support pwm regulation";

                return -1;
            }

        private:
            void SetDeviceStatus(EE_DEVICE_STATUS ds) {
                sensor_ind_->set_status(ds);
            };

            int CtrlSwitch(const bool sw) {
                if (sensor_sw_)
                    return sensor_sw_(sw);

                return -1;
            }

        protected:
            std::shared_ptr<ParserBaseItf> data_parser_ = nullptr;

        private:
            DeviceBaseItf(const DeviceBaseItf&) = delete;

            bool sensor_inited_ = false;
            bool sensor_working_ = false;
            bool sensor_stoped_ = true;
            bool sensor_fault_ = false;

            SensorInfo* sensor_info_;
            SensorIndicator* sensor_ind_;

            SensorGpioSwitch sensor_sw_ = nullptr;
            SensorPwmRegulator pwm_regulator_ = nullptr;
    };

} //namespace device
} //namespace chss
} //namespace mstf
