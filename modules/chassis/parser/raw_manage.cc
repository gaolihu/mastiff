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
        for (auto x : serial_data_) {
            if (x.second)
                delete x.second;
        }
    }

    int RawManage::Init(const ChassisConfig* cc,
            const SensorInfo* si) {
#ifdef CHSS_PKG_DBG
        AINFO << "RawManage init " <<
            "specific dev driver & data channel" <<
#if 0
            ", chs config: " << cc <<
            ", sensor info: " << si;
#else
            "";
#endif
#endif
        if (chs_conf_ == nullptr) {
            //import chassis config
            chs_conf_ = cc;
        } else if (chs_conf_ != cc) {
            AERROR << "use different config!";
        }

        //1, servo motor by CAN
        if (chs_conf_->has_servo_dev() &&
                &chs_conf_->servo_dev().si() == si) {
            //judge port type
            can_data_ = std::make_unique<CanData>
                (std::bind(&RawManage::OnRecvCan, this,
                           std::placeholders::_1,
                           std::placeholders::_2),
                 &chs_conf_->servo_dev().can_conf());
            can_data_->Init();
            sis_.emplace_back(si);
        }

        //2, serial devices line-laser
        if (chs_conf_->has_linelaser_dev() &&
                &chs_conf_->linelaser_dev().si() == si) {
            //judge port type
            auto s = new SerialData
                (std::bind(&RawManage::OnRecvUart, this,
                           std::placeholders::_1,
                           std::placeholders::_2),
                 &chs_conf_->linelaser_dev().uart_conf());
            s->Init();
            sis_.emplace_back(si);
            serial_data_[si] = s;
        }

        //3, serial devices lidar
        if (chs_conf_->has_lidar_dev() &&
                &chs_conf_->lidar_dev().si() == si) {
            //judge port type
            auto s = new SerialData
                (std::bind(&RawManage::OnRecvUart, this,
                           std::placeholders::_1,
                           std::placeholders::_2),
                 &chs_conf_->lidar_dev().uart_conf());
            s->Init();
            sis_.emplace_back(si);
            serial_data_[si] = s;
        }

        //3-1, soc - audio
        if (chs_conf_->has_aud_dev() &&
                &chs_conf_->aud_dev().si() == si) {
            soc_data_->Init(cc);
        }

        //3-2, soc - network
        // TODO         
        return 0;
    }

    int RawManage::Start(const SensorInfo* si) {
        if (chs_conf_->has_servo_dev() &&
                &chs_conf_->servo_dev().si() == si) {
            //1, CAN driver start
            return can_data_->Start();
        } else if (chs_conf_->has_aud_dev() &&
                &chs_conf_->aud_dev().si() == si) {
            //2, SOC driver start
            return soc_data_->Start();
        } else if (chs_conf_->has_linelaser_dev() &&
                &chs_conf_->linelaser_dev().si() == si) {
            //3, UART-linelaser start
            if (auto s = _FindSerialData(si)) {
                return s->Start();
            }
            AERROR << "start uart linelaser error!";
        } else if (chs_conf_->has_lidar_dev() &&
                &chs_conf_->lidar_dev().si() == si) {
            //3, UART-lidar start
            if (auto s = _FindSerialData(si)) {
                return s->Start();
            }
            AERROR << "find uart lidar start error!";
        }

        AWARN << "Start TODO for\n" << si->DebugString();

        return 0;
    }

    int RawManage::Stop(const SensorInfo* si) {
        if (chs_conf_->has_servo_dev() &&
                &chs_conf_->servo_dev().si() == si) {
            //1, CAN driver stop
            can_data_->Stop();
        } else if (chs_conf_->has_aud_dev() &&
                &chs_conf_->aud_dev().si() == si) {
            //2, SOC driver start
            //return soc_data_->Stop();
            //TODO
        } else if (chs_conf_->has_linelaser_dev() &&
                &chs_conf_->linelaser_dev().si() == si) {
            //3, UART-linelaser stop
            if (auto s = _FindSerialData(si)) {
                return s->Stop();
            }
            AERROR << "stop uart linelaser error!";
        } else if (chs_conf_->has_lidar_dev() &&
                &chs_conf_->lidar_dev().si() == si) {
            //3, UART-lidar stop
            if (auto s = _FindSerialData(si)) {
                return s->Start();
            }
            AERROR << "find uart lidar stop error!";
        }

        AWARN << "Stop TODO for\n" << si->DebugString();

        return 0;
    }

    int RawManage::Close(const SensorInfo* si) {
        if (chs_conf_->has_servo_dev() &&
                &chs_conf_->servo_dev().si() == si) {
            //1, CAN driver close
            can_data_->Close();
        } else if (chs_conf_->has_aud_dev() &&
                &chs_conf_->aud_dev().si() == si) {
            //2, SOC driver close
            //return soc_data_->Close();
        } else if (chs_conf_->has_linelaser_dev() &&
                &chs_conf_->linelaser_dev().si() == si) {
            //3, UART-linelaser close
            if (auto s = _FindSerialData(si)) {
                return s->Close();
            }
            AERROR << "close uart linelaser error!";
        } else if (chs_conf_->has_lidar_dev() &&
                &chs_conf_->lidar_dev().si() == si) {
            //3, UART-lidar close
            if (auto s = _FindSerialData(si)) {
                return s->Close();
            }
            AERROR << "find uart lidar close error!";
        }

        AWARN << "Close TODO for\n" << si->DebugString();

        return 0;
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
        AINFO << "write uart: [ " << oss.str() << " ]";
#endif
        if (info == nullptr || len == 0) {
            AWARN << "no contents to write for UART!";
            return 0;
        }
        if (auto s = _FindSerialData(si)) {
            return s->Push(info, len);
        }

        AERROR << "write uart error!";

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

    void RawManage::OnRecvUart(const uint8_t* buf,
            const size_t len) {
        _UartMessageHandle(buf, len);
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
