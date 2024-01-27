#pragma once

#include "cyber/common/log.h"

#include "modules/chassis/proto/chassis_config.pb.h"

#include "modules/chassis/parser/parser_base_itf.h"
#include "modules/chassis/parser/packer_base_itf.h"

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
            DeviceBaseItf(const SensorIndicator& ind) {
                sensor_ind_ = const_cast<SensorIndicator*>(&ind);

                sensor_ind_->set_status(EE_DEVICE_STATUS::
                        E_STATUS_UNINITIALIZED);
#ifdef CHSS_PKG_DBG
                AINFO << "DeviceBaseItf for <" <<
                    sensor_ind_->ihi().name() << ">";
#endif
            }

            virtual ~DeviceBaseItf() {
#ifdef CHSS_PKG_DBG
                AINFO << "DeviceBaseItf de-construct <" <<
                    sensor_ind_->ihi().name() << ">";
#endif
            }

            // control APIs
            // for private use
            virtual int Init() {
                if (data_parser_)
                    return data_parser_->Init();
                AWARN << "Init: no parser for " <<
                    sensor_ind_->ihi().name();
                return -1;
            }

            virtual int Start(void) {
                if (data_parser_)
                    return data_parser_->Start();
                AWARN << "Start: no parser for " <<
                    sensor_ind_->ihi().name();
                return -1;
            }

            virtual int Stop(void) {
                if (data_parser_)
                    return data_parser_->Stop();
                AWARN << "Stop: no parser for " <<
                    sensor_ind_->ihi().name();
                return -1;
            }

            virtual int Resume(void) {
                if (data_parser_)
                    return data_parser_->Resume();
                AWARN << "Resume: no parser for " <<
                    sensor_ind_->ihi().name();
                return -1;
            }

            virtual int Close(void) {
                if (data_parser_)
                    return data_parser_->Close();
                AWARN << "Close: no parser for " <<
                    sensor_ind_->ihi().name();
                return -1;
            }

            //helpers
            std::shared_ptr<PackerBaseItf>
                GetDevicePacker(void) const {
                return data_packer_;
            }

            std::shared_ptr<ParserBaseItf>
                GetDeviceParser(void) const {
                return data_parser_;
            }

            int InitSensor(const SensorIndicator* si) {
                if (si != sensor_ind_) {
                    AWARN << "DeviceBase indicator err\n" <<
                        si->DebugString() <<
                        " :\n" << sensor_ind_->DebugString();
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
                } else
                    AWARN << "DeviceBase start error!";
                return ret;
            }

            int StopSensor() {
                int ret = -1;
                if (GetDeviceStatus() == EE_DEVICE_STATUS::
                        E_STATUS_UNINITIALIZED) {
                    AWARN << GetSensorName() <<
                        " uninitialized, no stop!";
                    return ret;
                } else if (GetDeviceStatus() == EE_DEVICE_STATUS::
                        E_STATUS_IDLE) {
                    AWARN << GetSensorName() <<
                        " already stoped, no stop!";
                    return 0;
                }
                ret = Stop();
                if (ret == 0) {
                    sensor_working_ = false;
                    sensor_stoped_ = true;
                    SetDeviceStatus(EE_DEVICE_STATUS::
                            E_STATUS_IDLE);
                } else
                    AWARN << "DeviceBase stop error!";
                CtrlSwitch(false);
                return ret;
            }

            int ResumeSensor() {
                int ret = -1;
                if (GetDeviceStatus() == EE_DEVICE_STATUS::
                        E_STATUS_UNINITIALIZED) {
                    AWARN << GetSensorName() <<
                        " uninitialized, no resume!";
                    return ret;
                } else if (GetDeviceStatus() == EE_DEVICE_STATUS::
                        E_STATUS_WORKING) {
                    AWARN << GetSensorName() <<
                        " already working, no resume!";
                    return 0;
                }
                ret = Resume();
                if (ret == 0) {
                    sensor_working_ = true;
                    sensor_stoped_ = false;
                    SetDeviceStatus(EE_DEVICE_STATUS::
                            E_STATUS_WORKING);
                } else
                    AWARN << "DeviceBase resume error!";
                CtrlSwitch(true);
                return ret;
            }

            int CloseSensor() {
                int ret = -1;
                if (GetDeviceStatus() == EE_DEVICE_STATUS::
                        E_STATUS_UNINITIALIZED) {
                    AWARN << GetSensorName() <<
                        " uninitialized, no close!";
                    return ret;
                } else if (GetDeviceStatus() == EE_DEVICE_STATUS::
                        E_STATUS_SHUTDOWN) {
                    AWARN << GetSensorName() <<
                        " already closed, no close!";
                    return ret;
                }
                ret = Close();
                if (ret == 0) {
                    sensor_working_ = false;
                    sensor_stoped_ = true;
                    SetDeviceStatus(EE_DEVICE_STATUS::
                            E_STATUS_SHUTDOWN);
                } else
                    AWARN << "DeviceBase close error!";
                CtrlSwitch(false);
                return ret;
            }

            // status APIs
            const std::string CheckDeviceInfo() const {
                return std::string("-> DeviceBase info:\n" +
                        sensor_ind_->ihi().DebugString() +
                        "-> DeviceBase indicator:\n" +
                        sensor_ind_->DebugString());
            }

            const std::string CheckDeviceStatus() const {
                //TODO
                return std::string("No Error");
            }

            EE_DEVICE_STATUS GetDeviceStatus() const {
                return sensor_ind_->status();
            };

            bool IsDeviceInited(const SensorIndicator*) const {
                return sensor_inited_;
            }

            bool IsDeviceWorking(const SensorIndicator*) const {
                return sensor_working_;
            }

            bool IsDeviceStoped(const SensorIndicator*) const {
                return sensor_stoped_;
            }

            SensorIndicator* GetSensorIndicator() const {
                return sensor_ind_;
            }

            const std::string& GetSensorName() const {
                return sensor_ind_->ihi().name();
            }

            const std::string& GetSensorManufacture() const {
                return sensor_ind_->ihi().manufacture_by();
            }

            const std::string& GetSensorType() const {
                return sensor_ind_->ihi().type();
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

            //data channel
            virtual int SetSpeed(const ChsMovementCtrl&) {
                AERROR << "shall override speed in derives!!!";
                return -1;
            }

            virtual int SetAdc(const ChsPeriphAdcCtrl&) {
                AERROR << "shall override adc in derives!!!";
                return -1;
            }

            virtual int SetPwm(const ChsPeriphPwmCtrl&) {
                AERROR << "shall override pwm in derives!!!";
                return -1;
            }

            virtual int SetGpio(const ChsPeriphGpioCtrl&) {
                AERROR << "shall override gpio in derives!!!";
                return -1;
            }

            virtual int SetInfra(const ChsPeriphInfraCtrl&) {
                AERROR << "shall override infra in derives!!!";
                return -1;
            }

            virtual int SetMisc(const ChsSocMiscCtrl&) {
                AERROR << "shall override misc in derives!!!";
                return -1;
            }

            virtual int SetUpdate(const ChsFirmWareUpdate&) {
                AERROR << "shall override update in derives!!!";
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
            std::shared_ptr<PackerBaseItf> data_packer_ {};
            std::shared_ptr<ParserBaseItf> data_parser_ {};

        private:
            DeviceBaseItf(const DeviceBaseItf&) = delete;

            bool sensor_inited_ = false;
            bool sensor_working_ = false;
            bool sensor_stoped_ = true;
            bool sensor_fault_ = false;

            SensorIndicator* sensor_ind_;

            SensorGpioSwitch sensor_sw_ = nullptr;
            SensorPwmRegulator pwm_regulator_ = nullptr;
    };

} //namespace device
} //namespace chss
} //namespace mstf
