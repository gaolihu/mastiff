#include "modules/chassis/parser/common_inf.h"
#include "modules/chassis/parser/parse_drv_link.h"

using namespace std::placeholders;

namespace mstf {
namespace chss {
namespace parser {

    ParseDrvLink::ParseDrvLink() {
#ifdef CHSS_PKG_DBG
        AINFO << "ParseDrvLink sigleton construct";
#endif
        soc_data_ = std::make_shared<SocData>
            (std::bind(&ParseDrvLink::OnRecvSoc,
                       this, ::_1, ::_2, ::_3, ::_4),
             CommonItf::Instance()->GetChasisConfig());
    }

    ParseDrvLink::~ParseDrvLink() {
#ifdef CHSS_PKG_DBG
        AINFO << "ParseDrvLink sigleton de-construct";
#endif
    }

    int ParseDrvLink::Init(const SensorIndicator* si) {
#ifdef CHSS_PKG_DBG
        AINFO << "bind specific driver & channel: " <<
            si->ihi().name();
#endif
        //1, CAN dev: servo motor & MCU
        if (CommonItf::Instance()->GetDevCanConf(si)) {
            AINFO << "bind CAN driver with\n" <<
                si->DebugString();
            dev_can_data_[si] = std::make_shared<CanData>
                (std::bind(&ParseDrvLink::OnRecvCan,
                           this, ::_1, ::_2, ::_3),
                 CommonItf::Instance()->GetDevCanConf(si), si);
            dev_drivers_[si] = std::dynamic_pointer_cast<
                DriveDataItf>(dev_can_data_[si]);
            return dev_can_data_[si]->Init();
        }

        //2, UART dev: line laser & lidar & imu & mcu
        if (CommonItf::Instance()->GetDevUartConf(si)) {
            AINFO << "bind UART driver with\n" <<
                si->DebugString();
            dev_serial_data_[si] = std::make_shared<SerialData>
                (std::bind(&ParseDrvLink::OnRecvUart,
                           this, ::_1, ::_2, ::_3),
                 CommonItf::Instance()->GetDevUartConf(si), si);
            dev_drivers_[si] = std::dynamic_pointer_cast<
                DriveDataItf>(dev_serial_data_[si]);
            return dev_serial_data_[si]->Init();
        }

        //3, I2C dev: imu
        if (CommonItf::Instance()->GetDevI2cConf(si)) {
            AINFO << "bind IIC driver with\n" <<
                si->DebugString();
            dev_iic_data_[si] = std::make_shared<IIcData>
                (std::bind(&ParseDrvLink::OnRecvIIc,
                           this, ::_1, ::_2, ::_3),
                 CommonItf::Instance()->GetDevI2cConf(si), si);
            dev_drivers_[si] = std::dynamic_pointer_cast<
                DriveDataItf>(dev_iic_data_[si]);
            return dev_iic_data_[si]->Init();
        }

        //3, SPI dev: TODO
        //
        //4, SOC dev: joystick & audio & wifi & 4g
        if (CommonItf::Instance()->IsLacateSoc(si)) {
            AINFO << "bind SOC driver with " <<
                si->ihi().name();
            dev_drivers_[si] = std::dynamic_pointer_cast<
                DriveDataItf>(soc_data_);
            return soc_data_->Init(si);
        }

        AERROR << "bind driver errror\n" <<
            si->DebugString();

        return -1;
    }

    int ParseDrvLink::Start(const SensorIndicator* si) {
#ifdef CHSS_PKG_DBG
        AINFO << "ParseDrvLink start device: " <<
            si->ihi().name();
#endif
        auto drv_base_itf = _FindDriverBase(si);

        if (drv_base_itf != nullptr) {
            return drv_base_itf->Start(si);
        }

        //return soc_data_->Start(si);

        /*
        for (int i = 0; i < chs_conf_->aud_dev().size(); i++) {
            if(&chs_conf_->aud_dev(i).sn_ind() == si) {
                //audio driver start
                return soc_data_->Start(si);
            }
        }
        for (int i = 0; i < chs_conf_->wireless_dev().size(); i++) {
            if(&chs_conf_->wireless_dev(i).sn_ind() == si) {
                //audio driver start
                return soc_data_->Start(si);
            }
        }
        for (int i = 0; i < chs_conf_->camera_dev().size(); i++) {
            if(&chs_conf_->camera_dev(i).sn_ind() == si) {
                //camera driver start
                return soc_data_->Start(si);
            }
        }
        */

        AWARN << "Start TODO for\n" << si->DebugString();

        return -1;
    }

    int ParseDrvLink::Stop(const SensorIndicator* si) {
#ifdef CHSS_PKG_DBG
        AINFO << "ParseDrvLink stop device";
#endif
        auto drv_base_itf = _FindDriverBase(si);

        if (drv_base_itf != nullptr) {
            return drv_base_itf->Stop(si);
        }

        //return soc_data_->Stop(si);

#if 0
        for (int i = 0; i < chs_conf_->servo_dev().size(); i++) {
            if(&chs_conf_->servo_dev(i).sn_ind() == si) {
                AINFO << "stop servo motor CAN";
                return dev_can_data_->Stop();
            }
        }

        for (int i = 0; i < chs_conf_->linelaser_dev().size(); i++) {
            if(&chs_conf_->linelaser_dev(i).sn_ind() == si) {
                //UART IMU driver start
                if (auto s = _FindSerialData(si)) {
                    return s->Stop();
                }
                AERROR << "find UART IMU stop error!";
            }
        }

        for (int i = 0; i < chs_conf_->lidar_dev().size(); i++) {
            if(&chs_conf_->lidar_dev(i).sn_ind() == si) {
                //UART IMU driver start
                if (auto s = _FindSerialData(si)) {
                    return s->Stop();
                }
                AERROR << "find UART lidar stop error!";
            }
        }

        for (int i = 0; i < chs_conf_->imu_dev().size(); i++) {
            if(&chs_conf_->imu_dev(i).sn_ind() == si) {
                //UART IMU driver stop
                if (auto s = _FindSerialData(si)) {
                    return s->Stop();
                }
                AERROR << "find UART IMU stop error!";
            }
        }

        for (int i = 0; i < chs_conf_->joystick_dev().size(); i++) {
            if(&chs_conf_->joystick_dev(i).sn_ind() == si) {
                /*
                if (auto j = _FindJoyData(si)) {
                    return j->Stop();
                }
                */
                AERROR << "find JoyStick stop error!";
            }
        }

        for (int i = 0; i < chs_conf_->aud_dev().size(); i++) {
            if(&chs_conf_->aud_dev(i).sn_ind() == si) {
                // audio driver stop
                return soc_data_->Stop(si);
            }
            AERROR << "stop audio driver error!";
        }

        for (int i = 0; i < chs_conf_->wireless_dev().size(); i++) {
            if(&chs_conf_->wireless_dev(i).sn_ind() == si) {
                // wireless driver stop
                return soc_data_->Stop(si);
            }
            AERROR << "stop wireless driver error!";
        }

        for (int i = 0; i < chs_conf_->camera_dev().size(); i++) {
            if(&chs_conf_->camera_dev(i).sn_ind() == si) {
                // camera driver stop
                return soc_data_->Stop(si);
            }
            AERROR << "stop camera driver error!";
        }
#endif
        AWARN << "Stop TODO for\n" << si->DebugString();

        return -1;
    }

    int ParseDrvLink::Resume(const SensorIndicator* si) {
#ifdef CHSS_PKG_DBG
        AINFO << "ParseDrvLink resume device";
#endif
        auto drv_base_itf = _FindDriverBase(si);

        if (drv_base_itf != nullptr) {
            return drv_base_itf->Resume(si);
        }

        //return soc_data_->Resume(si);

#if 0
        for (int i = 0; i < chs_conf_->servo_dev().size(); i++) {
            if(&chs_conf_->servo_dev(i).sn_ind() == si) {
                //return s->Resume();
            }
        }

        for (int i = 0; i < chs_conf_->linelaser_dev().size(); i++) {
            if(&chs_conf_->linelaser_dev(i).sn_ind() == si) {
                //CAN driver resume
                if (auto s = _FindSerialData(si)) {
                    return s->Resume();
                }
                AERROR << "find UART linerlaser resume error!";
            }
        }

        for (int i = 0; i < chs_conf_->imu_dev().size(); i++) {
            if(&chs_conf_->imu_dev(i).sn_ind() == si) {
                //UART IMU driver resume
                if (auto s = _FindSerialData(si)) {
                    return s->Resume();
                }
                AERROR << "find UART linerlaser resume error!";
            }
        }

        for (int i = 0; i < chs_conf_->lidar_dev().size(); i++) {
            if(&chs_conf_->lidar_dev(i).sn_ind() == si) {
                //UART lidar driver resume
                if (auto s = _FindSerialData(si)) {
                    return s->Resume();
                }
                AERROR << "find UART lidar resume error!";
            }
        }

        for (int i = 0; i < chs_conf_->joystick_dev().size(); i++) {
            if(&chs_conf_->joystick_dev(i).sn_ind() == si) {
                /*
                if(auto j = _FindJoyData(si)) {
                    return j->Resume();
                }
                */
                AERROR << "find JoyStick resume error!";
            }
        }

        for (int i = 0; i < chs_conf_->aud_dev().size(); i++) {
            if(&chs_conf_->aud_dev(i).sn_ind() == si) {
                // audio driver Resume
                return soc_data_->Resume(si);
            }
            AERROR << "Resume audio driver error!";
        }

        for (int i = 0; i < chs_conf_->wireless_dev().size(); i++) {
            if(&chs_conf_->wireless_dev(i).sn_ind() == si) {
                // wireless driver Resume
                return soc_data_->Resume(si);
            }
            AERROR << "Resume wireless driver error!";
        }

        for (int i = 0; i < chs_conf_->camera_dev().size(); i++) {
            if(&chs_conf_->camera_dev(i).sn_ind() == si) {
                // camera driver Resume
                return soc_data_->Resume(si);
            }
            AERROR << "Resume camera driver error!";
        }

#endif
        AWARN << "Resume TODO for\n" << si->DebugString();

        return -1;
    }

    int ParseDrvLink::Close(const SensorIndicator* si) {
#ifdef CHSS_PKG_DBG
        AINFO << "ParseDrvLink close device";
#endif
        auto drv_base_itf = _FindDriverBase(si);

        if (drv_base_itf != nullptr) {
            return drv_base_itf->Close(si);
        }

        //return soc_data_->Close(si);
#if 0
        for (int i = 0; i < chs_conf_->servo_dev().size(); i++) {
            if(&chs_conf_->servo_dev(i).sn_ind() == si) {
                AINFO << "close servo motor CAN";
                dev_can_data_->Close();
                return 0;
            }
        }

        for (int i = 0; i < chs_conf_->linelaser_dev().size(); i++) {
            if(&chs_conf_->linelaser_dev(i).sn_ind() == si) {
                if (auto s = _FindSerialData(si)) {
                    s->Close();
                    return 0;
                }
                AERROR << "find UART linelaser close error!";
            }
        }

        for (int i = 0; i < chs_conf_->lidar_dev().size(); i++) {
            if(&chs_conf_->lidar_dev(i).sn_ind() == si) {
                if (auto s = _FindSerialData(si)) {
                    s->Close();
                    return 0;
                }
                AERROR << "find UART lidar close error!";
            }
        }

        for (int i = 0; i < chs_conf_->imu_dev().size(); i++) {
            if(&chs_conf_->imu_dev(i).sn_ind() == si) {
                if (auto s = _FindSerialData(si)) {
                    s->Close();
                    return 0;
                }
                AERROR << "find UART IMU close error!";
            }
        }

        for (int i = 0; i < chs_conf_->joystick_dev().size(); i++) {
            if(&chs_conf_->joystick_dev(i).sn_ind() == si) {
                /*
                if(auto j = _FindJoyData(si)) {
                    j->Close();
                    return 0;
                }
                */
                AERROR << "find Joy Stick close error!";
            }
        }

        for (int i = 0; i < chs_conf_->aud_dev().size(); i++) {
            if(&chs_conf_->aud_dev(i).sn_ind() == si) {
                // audio driver stop
                return soc_data_->Stop(si);
            }
            AERROR << "stop audio driver error!";
        }

        for (int i = 0; i < chs_conf_->wireless_dev().size(); i++) {
            if(&chs_conf_->wireless_dev(i).sn_ind() == si) {
                // wireless driver stop
                return soc_data_->Stop(si);
            }
            AERROR << "stop wireless driver error!";
        }

        for (int i = 0; i < chs_conf_->camera_dev().size(); i++) {
            if(&chs_conf_->camera_dev(i).sn_ind() == si) {
                // camera driver stop
                return soc_data_->Stop(si);
            }
            AERROR << "stop camera driver error!";
        }
#endif

        AWARN << "Close TODO for\n" << si->DebugString();

        return -1;
    }

    //write data downwards
    size_t ParseDrvLink::WriteUart(const SensorIndicator* si,
            const uint8_t* info,
            const size_t len) {
#if 1
        std::ostringstream oss;
        for (size_t i = 0; i < len; i++) {
            oss << std::hex << std::setw(2) << std::setfill('0') <<
                static_cast<int>(info[i]) << " ";
        }
        AINFO << "write UART: [ " << oss.str() << " ]";
#endif
        if (info == nullptr || len == 0) {
            AWARN << "no contents to write for UART: " << len;
            return 0;
        }
        if (auto s = _FindSerialData(si)) {
            return s->WritePort(info, len);
        }

        AERROR << "write UART error!";

        return 0;
    }

    size_t ParseDrvLink::WriteCan(const SensorIndicator* si,
            const int id,
            const uint8_t* info,
            const size_t len) {
#if 1
        if (info == nullptr || len == 0) {
            AWARN << "no contents to write for CAN: " << len;
            return 0;
        }
#endif
        if (auto s = _FindCanData(si)) {
            return s->WritePort(id, info, len);
        }

        return -1;
    }

    size_t ParseDrvLink::WriteI2c(const SensorIndicator* si,
            const uint8_t* info,
            const size_t len) {
#if 0
        if (info == nullptr || len == 0) {
            AWARN << "no contents to write for I2C: " << len;
            return 0;
        }
#endif
        if (auto s = _FindI2cData(si)) {
            return s->WritePort(info, len);
        }

        return -1;
    }

    //on receive data upwards
    bool ParseDrvLink::WriteSoc(const Message& info) {
        //return soc_data_->SocWrite(info);
        return 0;
    }

    void ParseDrvLink::OnRecvCan(const SensorIndicator* si,
            const uint8_t* buf, const size_t len) {
        _CanMessageHandle(si, buf, len);
    }

    void ParseDrvLink::OnRecvUart(const SensorIndicator* si,
            const uint8_t* buf, const size_t len) {
        _UartMessageHandle(si, buf, len);
    }

    void ParseDrvLink::OnRecvIIc(const SensorIndicator* si,
            const uint8_t* buf, const size_t len) {
        _IIcMessageHandle(si, buf, len);
    }

    void ParseDrvLink::OnRecvSoc(const SensorIndicator* si,
            const Message& msg,
            const uint8_t* buf,
            const size_t len) {
#ifdef CHSS_PKG_DBG
        AINFO << "receive soc data: " << msg.DebugString();
#endif
        _SocMessageHandle(si, msg, buf, len);
    }

} //namespace parser
} //namespace chss
} //namespace mstf
