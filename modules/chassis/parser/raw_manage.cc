#include "modules/chassis/parser/raw_manage.h"

namespace mstf {
namespace chss {
namespace parser {

    RawManage::RawManage() {
#ifdef CHSS_PKG_DBG
        AINFO << "RawManage sigleton construct";
#endif
        soc_data_ = std::make_unique<SocData>
            (std::bind(&RawManage::OnRecvSoc, this,
                       std::placeholders::_1));
    }

    RawManage::~RawManage() {
#ifdef CHSS_PKG_DBG
        AINFO << "RawManage sigleton de-construct";
#endif
        for (auto x : dev_serial_map_) {
            if (x.second)
                delete x.second;
        }
        for (auto x : dev_i2c_map_) {
            if (x.second)
                delete x.second;
        }
    }

    int RawManage::Init(const ChassisConfig* cc,
            const SensorInfo* si) {
#ifdef CHSS_PKG_DBG
        AINFO << "bind specific driver & data channel" <<
#if 0
            ", chs config: " << cc <<
            ", sensor info: " << si;
#else
            ": " << si->name();
#endif
#endif
        if (chs_conf_ == nullptr) {
            //import chassis config
            chs_conf_ = cc;
        } else if (chs_conf_ != cc) {
            AERROR << "use different config!";
        }

        //1, servo motor by CAN
        for (int i = 0; i < chs_conf_->servo_dev().size(); i++) {
            if(&chs_conf_->servo_dev(i).si() == si) {
                can_data_ = std::make_unique<CanData>
                    (std::bind(&RawManage::OnRecvCan, this,
                               std::placeholders::_1,
                               std::placeholders::_2),
                     &chs_conf_->servo_dev(i).can_conf());
                can_data_->Init();
                sis_.emplace_back(si);
            }
        }

        //2, serial devices line-laser
        for (int i = 0; i < chs_conf_->linelaser_dev().size(); i++) {
            if(&chs_conf_->linelaser_dev(i).si() == si) {
                auto s = new SerialData
                    (std::bind(&RawManage::OnRecvUart, this,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               std::placeholders::_3),
                     &chs_conf_->linelaser_dev(i).uart_conf(),
                     si);
                s->Init();
                sis_.emplace_back(si);
                dev_serial_map_[si] = s;
            }
        }

        //3, serial devices lidar
        for (int i = 0; i < chs_conf_->lidar_dev().size(); i++) {
            if(&chs_conf_->lidar_dev(i).si() == si) {
                auto s = new SerialData
                    (std::bind(&RawManage::OnRecvUart, this,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               std::placeholders::_3),
                     &chs_conf_->lidar_dev(i).uart_conf(),
                     si);
                s->Init();
                sis_.emplace_back(si);
                dev_serial_map_[si] = s;
            }
        }

        //3, serial/iic devices imu
        for (int i = 0; i < chs_conf_->imu_dev().size(); i++) {
            if(&chs_conf_->imu_dev(i).si() == si) {
                if (chs_conf_->imu_dev(i).has_uart_conf()) {
                    auto s = new SerialData(
                            std::bind(&RawManage::OnRecvUart, this,
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3),
                            &chs_conf_->imu_dev(i).uart_conf(),
                            si);
                    s->Init();
                    sis_.emplace_back(si);
                    dev_serial_map_[si] = s;
                } else if (chs_conf_->imu_dev(i).has_i2c_conf()) {
                    auto s = new IIcData(
                            std::bind(&RawManage::OnRecvIIc, this,
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3),
                            &chs_conf_->imu_dev(i).i2c_conf(),
                            si);
                    s->Init();
                    sis_.emplace_back(si);
                    dev_i2c_map_[si] = s;
                }
            }
        }

        //4-0, soc - set chs config
        soc_data_->Init(0, cc);

        //4-1, soc - audio
        for (int i = 0; i < chs_conf_->aud_dev().size(); i++) {
            if(&chs_conf_->aud_dev(i).si() == si)
                soc_data_->Init(&chs_conf_->aud_dev(i).si());
        }

        //4-2, soc - network
        for (int i = 0; i < chs_conf_->wireless_dev().size(); i++) {
            if(&chs_conf_->wireless_dev(i).si() == si)
                soc_data_->Init(&chs_conf_->wireless_dev(i).si());
        }

        //4-3, soc - camera
        for (int i = 0; i < chs_conf_->camera_dev().size(); i++) {
            if(&chs_conf_->camera_dev(i).si() == si)
                soc_data_->Init(&chs_conf_->camera_dev(i).si());
        }

        //4-4, soc - network
        for (int i = 0; i < chs_conf_->wireless_dev().size(); i++) {
            if(&chs_conf_->wireless_dev(i).si() == si)
                sis_.emplace_back(si);
        }

        //4-5, soc - camera
        for (int i = 0; i < chs_conf_->camera_dev().size(); i++) {
            if(&chs_conf_->camera_dev(i).si() == si)
                sis_.emplace_back(si);
        }

        return 0;
    }

    int RawManage::Start(const SensorInfo* si) {
        for (int i = 0; i < chs_conf_->servo_dev().size(); i++) {
            if(&chs_conf_->servo_dev(i).si() == si)
                //CAN driver start
                return can_data_->Start();
        }

        for (int i = 0; i < chs_conf_->linelaser_dev().size(); i++) {
            if(&chs_conf_->linelaser_dev(i).si() == si) {
                //UART linelaser driver start
                if (auto s = _FindSerialData(si)) {
                    return s->Start();
                }
                AERROR << "start UART linelaser error!";
            }
        }

        for (int i = 0; i < chs_conf_->lidar_dev().size(); i++) {
            if(&chs_conf_->lidar_dev(i).si() == si) {
                //UART lidar driver start
                if (auto s = _FindSerialData(si)) {
                    return s->Start();
                }
                AERROR << "start UART linelaser error!";
            }
        }

        for (int i = 0; i < chs_conf_->imu_dev().size(); i++) {
            if(&chs_conf_->imu_dev(i).si() == si) {
                if (chs_conf_->imu_dev(i).has_uart_conf()) {
                    //UART IMU driver start
                    if (auto s = _FindSerialData(si)) {
                        return s->Start();
                    }
                } else if (chs_conf_->imu_dev(i).has_i2c_conf()) {
                    //IIC IMU driver start
                    if (auto s = _FindI2cData(si)) {
                        return s->Start();
                    }
                }
                AERROR << "start UART IMU error!";
            }
        }

        for (int i = 0; i < chs_conf_->aud_dev().size(); i++) {
            if(&chs_conf_->aud_dev(i).si() == si) {
                //audio driver start
                if (auto s = _FindSerialData(si)) {
                    return s->Start();
                }
                AERROR << "start AUDIO error!";
            }
        }

        for (int i = 0; i < chs_conf_->camera_dev().size(); i++) {
            if(&chs_conf_->camera_dev(i).si() == si) {
                //camera driver start
                if (auto s = _FindSerialData(si)) {
                    return s->Start();
                }
                AERROR << "start camera error!";
            }
        }

        AWARN << "Start TODO for\n" << si->DebugString();

        return -1;
    }

    int RawManage::Stop(const SensorInfo* si) {
        for (int i = 0; i < chs_conf_->servo_dev().size(); i++) {
            if(&chs_conf_->servo_dev(i).si() == si) {
                AINFO << "stop servo motor CAN";
                return can_data_->Stop();
            }
        }

        for (int i = 0; i < chs_conf_->linelaser_dev().size(); i++) {
            if(&chs_conf_->linelaser_dev(i).si() == si) {
                //UART IMU driver start
                if (auto s = _FindSerialData(si)) {
                    return s->Stop();
                }
                AERROR << "find UART IMU stop error!";
            }
        }

        for (int i = 0; i < chs_conf_->lidar_dev().size(); i++) {
            if(&chs_conf_->lidar_dev(i).si() == si) {
                //UART IMU driver start
                if (auto s = _FindSerialData(si)) {
                    return s->Stop();
                }
                AERROR << "find UART lidar stop error!";
            }
        }

        for (int i = 0; i < chs_conf_->aud_dev().size(); i++) {
            if(&chs_conf_->aud_dev(i).si() == si) {
                //audio driver start
                //return s->Stop();
            }
        }
        AWARN << "Stop TODO for\n" << si->DebugString();

        return -1;
    }

    int RawManage::Resume(const SensorInfo* si) {
        for (int i = 0; i < chs_conf_->servo_dev().size(); i++) {
            if(&chs_conf_->servo_dev(i).si() == si) {
                //return s->Resume();
            }
        }

        for (int i = 0; i < chs_conf_->linelaser_dev().size(); i++) {
            if(&chs_conf_->linelaser_dev(i).si() == si) {
                //CAN driver resume
                if (auto s = _FindSerialData(si)) {
                    return s->Resume();
                }
                AERROR << "find UART linerlaser resume error!";
            }
        }

        for (int i = 0; i < chs_conf_->imu_dev().size(); i++) {
            if(&chs_conf_->imu_dev(i).si() == si) {
                //UART IMU driver resume
                if (auto s = _FindSerialData(si)) {
                    return s->Resume();
                }
                AERROR << "find UART linerlaser resume error!";
            }
        }

        for (int i = 0; i < chs_conf_->lidar_dev().size(); i++) {
            if(&chs_conf_->lidar_dev(i).si() == si) {
                //UART lidar driver resume
                if (auto s = _FindSerialData(si)) {
                    return s->Resume();
                }
                AERROR << "find UART lidar resume error!";
            }
        }

        AWARN << "Resume TODO for\n" << si->DebugString();

        return -1;
    }

    int RawManage::Close(const SensorInfo* si) {
        for (int i = 0; i < chs_conf_->servo_dev().size(); i++) {
            if(&chs_conf_->servo_dev(i).si() == si) {
                AINFO << "close servo motor CAN";
                can_data_->Close();
                return 0;
            }
        }

        for (int i = 0; i < chs_conf_->linelaser_dev().size(); i++) {
            if(&chs_conf_->linelaser_dev(i).si() == si) {
                if (auto s = _FindSerialData(si)) {
                    s->Close();
                    return 0;
                }
                AERROR << "find UART linelaser close error!";
            }
        }

        for (int i = 0; i < chs_conf_->lidar_dev().size(); i++) {
            if(&chs_conf_->lidar_dev(i).si() == si) {
                if (auto s = _FindSerialData(si)) {
                    s->Close();
                    return 0;
                }
                AERROR << "find UART lidar close error!";
            }
        }

        for (int i = 0; i < chs_conf_->imu_dev().size(); i++) {
            if(&chs_conf_->imu_dev(i).si() == si) {
                if (auto s = _FindSerialData(si)) {
                    s->Close();
                    return 0;
                }
                AERROR << "find UART IMU close error!";
            }
        }

        for (int i = 0; i < chs_conf_->aud_dev().size(); i++) {
            if(&chs_conf_->aud_dev(i).si() == si) {
                return 0;
            }
        }

        AWARN << "Close TODO for\n" << si->DebugString();

        return -1;
    }

    size_t RawManage::WriteUart(const SensorInfo* si,
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
            AWARN << "no contents to write for UART!";
            return 0;
        }
        if (auto s = _FindSerialData(si)) {
            return s->Push(info, len);
        }

        AERROR << "write UART error!";

        return 0;
    }

    size_t RawManage::WriteCan(const int id,
            const uint8_t* info,
            const size_t len) {
#if 0
        if (info == nullptr || len == 0) {
            AWARN << "no contents to write for CAN!";
            return 0;
        }
#endif
        return can_data_->WritePort(id, info, len);
    }

    bool RawManage::WriteSoc(const Message& info) {
        return soc_data_->SocWrite(info);
    }

    void RawManage::OnRecvCan(const uint8_t* buf,
            const size_t len) {
        _CanMessageHandle(buf, len);
    }

    void RawManage::OnRecvUart(const void* conf,
            const uint8_t* buf,
            const size_t len) {
        _UartMessageHandle(conf, buf, len);
    }

    void RawManage::OnRecvIIc(const void* conf,
            const uint8_t* buf,
            const size_t len) {
        _IIcMessageHandle(conf, buf, len);
    }

    void RawManage::OnRecvSoc(const Message& msg) {
#ifdef CHSS_PKG_DBG
        AINFO << "receive soc data: " << msg.DebugString();
#endif
        //TODO
        _SocMessageHandle(msg);
    }

} //namespace parser
} //namespace chss
} //namespace mstf
