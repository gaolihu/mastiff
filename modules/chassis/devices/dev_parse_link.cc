#include "modules/chassis/devices/transactor.h"
#include "modules/chassis/devices/dev_parse_link.h"

namespace mstf {
namespace chss {
namespace device {

    DevParseLink::DevParseLink() {
#ifdef CHSS_PKG_DBG
        AINFO << "DevParseLink singleton construct";
#endif
        Transactor::Instance()->SetMoveExecutor(
                std::bind(&DevParseLink::HandleChsMovement,
                    this, std::placeholders::_1));
        Transactor::Instance()->SetAdcExecutor(
                std::bind(&DevParseLink::HandleChsPeriphAdc,
                    this, std::placeholders::_1));
        Transactor::Instance()->SetPwmExecutor(
                std::bind(&DevParseLink::HandleChsPeriphPwm,
                    this, std::placeholders::_1));
        Transactor::Instance()->SetGpioExecutor(
                std::bind(&DevParseLink::HandleChsPeriphGpio,
                    this, std::placeholders::_1));
        Transactor::Instance()->SetInfraExecutor(
                std::bind(&DevParseLink::HandleChsPeriphInfra,
                    this, std::placeholders::_1));
        Transactor::Instance()->SetSocExecutor(
                std::bind(&DevParseLink::HandleChsSocMisc,
                    this, std::placeholders::_1));
        Transactor::Instance()->SetUpdateExecutor(
                std::bind(&DevParseLink::HandleChsUpdate,
                    this, std::placeholders::_1));
    }

    DevParseLink::~DevParseLink() {
#ifdef CHSS_PKG_DBG
        AINFO << "DevParseLink singleton de-construct!";
#endif
        for(auto it = ind_dev_pair_.begin();
                it != ind_dev_pair_.end(); it++) {
            AINFO << "de-construct devbase: " <<
                it->second->CheckDeviceInfo();
        }
    }

    int DevParseLink::RegisterDevice(const
            SensorIndicator& si,
            DeviceBaseItf* dev_base) {
#ifdef CHSS_PKG_DBG
        AINFO << "link parser & device " <<
            si.ihi().name();
#endif
        FrameProcessorSp proto_proc_sp =
            [this](const std::shared_ptr<Message>& msg,
                    const std::string& tpk)->int {
                return Transactor::Instance()->
                    OnTransferMessageOut(msg, tpk);
            };
        ind_dev_pair_[&si] = dev_base;

        auto parser = dev_base->GetDeviceParser();
        parser->SetProtobufHandleSp(proto_proc_sp);

        AINFO << "register msg export " <<
            si.ihi().name() <<
            ", index: " << ind_dev_pair_.size();

        return 0;
    }

    int DevParseLink::DeviceInit() {
        int i = 1, ret = 0;
        auto start_time = std::chrono::steady_clock::now();
        AINFO << "===== init all of the " <<
            ind_dev_pair_.size() <<
            " managed devices =====";

        for (auto it = ind_dev_pair_.begin();
                it != ind_dev_pair_.end(); it++, ++i) {
            DeviceBaseItf* dev_itf = it->second;
            if ((ret = dev_itf->InitSensor(it->first)) != 0) {
                AWARN << "init device: " <<
                    dev_itf->GetSensorName() << " failed!";
            }
#ifdef CHSS_PKG_DBG
            AINFO << "==init device finished index: " << i <<
                " ~ " << dev_itf->GetSensorName() <<
                (ret == 0 ? ", OK!" : ", NG!") <<
#if 0
                "\n(\n" <<
                dev_itf->CheckDeviceInfo() << ")";;
#else
                "";
#endif
#endif
        }

        auto end_time = std::chrono::steady_clock::now();
        AINFO << "===== DevParseLink init all " <<
            ind_dev_pair_.size() << " devices, take " <<
            std::chrono::duration<double, std::milli>
            (end_time - start_time).count() << "ms =====";

        return ret;
    }

    int DevParseLink::DeviceStart() {
        int i = 1, ret = 0;
        auto start_time = std::chrono::steady_clock::now();
        AINFO << "##### start all of the " <<
            ind_dev_pair_.size() <<
            " managed devices #####";
        /*
        //make sure GPIO start first of all
        dev_gpio_->Start();
        */

        for (auto it = ind_dev_pair_.begin();
                it != ind_dev_pair_.end(); it++, ++i) {
            DeviceBaseItf* dev_itf = it->second;
            if ((ret = dev_itf->StartSensor()) != 0) {
                AERROR << "start device: " <<
                    dev_itf->GetSensorName() << " failed!";
                ret = -1;
            }
#ifdef CHSS_PKG_DBG
            AINFO << "##start device finished index: " << i <<
                " ~ " << dev_itf->GetSensorName() <<
                (ret == 0 ? ", OK!" : ", NG!") <<
#if 0
                "\n(\n" <<
                dev_itf->CheckDeviceInfo() << ")";;
#else
                "";
#endif
#endif
        }

        auto end_time = std::chrono::steady_clock::now();
        AINFO << "##### DevParseLink start all " <<
            ind_dev_pair_.size() << " devices, take " <<
            std::chrono::duration<double, std::milli>
            (end_time - start_time).count() << "ms #####";

        return ret;
    }

    int DevParseLink::DeviceStop() {
        int i = 1, ret = 0;
        auto start_time = std::chrono::steady_clock::now();
        AINFO << "@@@@@ stop all of the " <<
            ind_dev_pair_.size() <<
            " managed devices @@@@@";
        for (auto it = ind_dev_pair_.begin();
                it != ind_dev_pair_.end(); it++, ++i) {
            DeviceBaseItf* dev_itf = it->second;
            if ((ret = dev_itf->StopSensor()) != 0) {
                AWARN << "stop device: " <<
                    dev_itf->GetSensorName() << " failed!";
            }
#ifdef CHSS_PKG_DBG
            AINFO << "@@stop device finished index: " << i <<
                " ~ " << dev_itf->GetSensorName() <<
                (ret == 0 ? ", OK!" : ", NG!") <<
#if 0
                "\n(\n" <<
                dev_itf->CheckDeviceInfo() << ")";;
#else
                "";
#endif
#endif
        }

        auto end_time = std::chrono::steady_clock::now();
        AINFO << "@@@@@ DevParseLink stop all " <<
            ind_dev_pair_.size() << " devices, take " <<
            std::chrono::duration<double, std::milli>
            (end_time - start_time).count() << "ms @@@@@";

        return ret;
    }

    int DevParseLink::DeviceResume() {
        return 0;
    }

    int DevParseLink::DeviceClose() {
        int i = 1;
        auto start_time = std::chrono::steady_clock::now();
        AINFO << "----- close all of the " <<
            ind_dev_pair_.size() <<
            " managed devices -----";
        for (auto it = ind_dev_pair_.begin();
                it != ind_dev_pair_.end(); it++, ++i) {
            auto dev_itf = it->second;
            dev_itf->CloseSensor();
#ifdef CHSS_PKG_DBG
            AINFO << "--close device finished index: " << i <<
                " ~ " << dev_itf->GetSensorName() <<
#if 0
                "\n(\n" <<
                dev_itf->CheckDeviceInfo() << ")";;
#else
                "";
#endif
#endif
        std::this_thread::sleep_for(std::chrono::
                milliseconds(100));


        }

        auto end_time = std::chrono::steady_clock::now();
        AINFO << "----- DevParseLink close all " <<
            ind_dev_pair_.size() << " devices, take " <<
            std::chrono::duration<double, std::milli>
            (end_time - start_time).count() << "ms -----";

        return 0;
    }

    //operate conclusive device
    int DevParseLink::DeviceInit(const
            SensorIndicator* snr_ind) {
        for (auto it = ind_dev_pair_.begin();
                it != ind_dev_pair_.end(); it++) {
            auto snr_i = it->first;
            if (snr_i == snr_ind) {
                auto dev_itf = it->second;
                return dev_itf->InitSensor(snr_i);
            }
        }

        return -1;
    }

    int DevParseLink::DeviceStart(const
            SensorIndicator* snr_ind) {
        for (auto it = ind_dev_pair_.begin();
                it != ind_dev_pair_.end(); it++) {
            auto snr_i = it->first;
            if (snr_i == snr_ind) {
                auto dev_itf = it->second;
                return dev_itf->StartSensor();
            }
        }

        return -1;
    }

    int DevParseLink::DeviceStop(const
            SensorIndicator* snr_ind) {
        for (auto it = ind_dev_pair_.begin();
                it != ind_dev_pair_.end(); it++) {
            auto snr_i = it->first;
            if (snr_i == snr_ind) {
                auto dev_itf = it->second;
                return dev_itf->StopSensor();
            }
        }

        return -1;
    }

    int DevParseLink::DeviceResume(const
            SensorIndicator* snr_ind) {
        return -1;
    }

    int DevParseLink::DeviceClose(const
            SensorIndicator* snr_ind) {
        for (auto it = ind_dev_pair_.begin();
                it != ind_dev_pair_.end(); it++) {
            auto snr_i = it->first;
            if (snr_i == snr_ind) {
                auto dev_itf = it->second;
                dev_itf->CloseSensor();
                return 0;
            }
        }
        return 0;
    }

    int DevParseLink::HandleChsMovement(const
            ChsMovementCtrl& data) {
#ifdef CHSS_PKG_DBG
        //AINFO << "handle movement!";
#endif
        if (_GetDevBaseByType(E_DEVICE_SERVO))
            return _GetDevBaseByType(E_DEVICE_SERVO)->SetSpeed(data);

        AERROR << "control move NG!";

        return -1;
    }

    int DevParseLink::HandleChsPeriphAdc(const
            ChsPeriphAdcCtrl&) {
#ifdef CHSS_PKG_DBG
        AINFO << "handle adc!";
#endif
        return 0;
    }

    int DevParseLink::HandleChsPeriphPwm(const
            ChsPeriphPwmCtrl&) {
#ifdef CHSS_PKG_DBG
        AINFO << "handle pwm!";
#endif
        return 0;
    }

    int DevParseLink::HandleChsPeriphGpio(const
            ChsPeriphGpioCtrl&) {
#ifdef CHSS_PKG_DBG
        AINFO << "handle gpio!";
#endif
        return 0;
    }

    int DevParseLink::HandleChsPeriphInfra(const
            ChsPeriphInfraCtrl&) {
#ifdef CHSS_PKG_DBG
        AINFO << "handle infra!";
#endif
        return 0;
    }

    int DevParseLink::HandleChsSocMisc(const
            ChsSocMiscCtrl& data) {
#ifdef CHSS_PKG_DBG
        AINFO << "handle misc!";
#endif
        if (_GetDevBaseByType(E_DEVICE_AUDIO))
            return _GetDevBaseByType(E_DEVICE_AUDIO)->SetMisc(data);
        else if (_GetDevBaseByType(E_DEVICE_WIRELESS))
            return _GetDevBaseByType(E_DEVICE_WIRELESS)->SetMisc(data);

        AERROR << "no handler for misc message!";
        return -1;
    }

    int DevParseLink::HandleChsUpdate(const
            ChsFirmWareUpdate&) {
#ifdef CHSS_PKG_DBG
        AINFO << "handle update!";
#endif
        return 0;
    }

} //namespace device
} //namespace chss
} //namespace mstf
