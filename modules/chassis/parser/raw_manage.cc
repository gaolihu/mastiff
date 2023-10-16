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
    }

    int RawManage::Init(const ChassisConfig* cc,
            const SensorInfo* si) {
#ifdef CHSS_PKG_DBG
        AINFO << "RawManage init, ccp: " << cc <<
            ", sensor info: " << si;
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

        //3-1, soc - audio
        if (chs_conf_->has_aud_dev() &&
                &chs_conf_->aud_dev().si() == si) {
            soc_data_->Init(cc);
        }

        //3-2, soc - audio
        // TODO         
        return 0;
    }

    int RawManage::Start(const SensorInfo* si) {
        if (chs_conf_->has_servo_dev() &&
                &chs_conf_->servo_dev().si() == si) {
            //1, CAN driver start
            can_data_->Start();
        } else if (chs_conf_->has_aud_dev() &&
                &chs_conf_->aud_dev().si() == si) {
            //2, SOC driver start
            soc_data_->Start();
        } else if (chs_conf_->has_servo_dev() &&
                &chs_conf_->servo_dev().si() == si) {
            //3, UART driver start
            // TODO
            //serial_data_->Start();
        }

        return 0;
    }

    int RawManage::Stop(const SensorInfo* si) {
        if (chs_conf_->has_servo_dev() &&
                &chs_conf_->servo_dev().si() == si) {
            can_data_->Stop();
        }

        return 0;
    }

    int RawManage::Close(const SensorInfo* si) {
        if (chs_conf_->has_servo_dev() &&
                &chs_conf_->servo_dev().si() == si) {
            can_data_->Close();
        }

        return 0;
    }

    //will not used
    size_t RawManage::WriteCan(const
            std::vector<uint8_t>& msg) {
        return can_data_->WritePort(msg);
    }

    size_t RawManage::WriteCan(const int id,
            const uint8_t* info,
            const size_t len) {
        if (info == nullptr || len == 0) {
            AWARN << "no contents to write for CAN!";
            return 0;
        }

        return can_data_->WritePort(id, info, len);
    }

    size_t RawManage::WriteUart(const uint8_t* info,
            const uint32_t len) {
        if (info == nullptr || len == 0) {
            AWARN << "no contents to write for UART!";
            return 0;
        }
        //return uart_data_->WritePort(info, len);
        return 0;
    }

    bool RawManage::WriteSoc(const Message& info) {
        return soc_data_->SocWrite(info);
    }

    void RawManage::OnRecvCan(const uint8_t* buf,
            const size_t len) {
        _CanMessageHandle(buf, len);
    }

    void RawManage::OnRecvUart(const uint8_t* buf,
            const size_t l) {
#ifdef CHSS_PKG_DBG
        AINFO << "receive uart data: " << buf <<
            ", len: " << l;
#endif
    }

    void RawManage::OnRecvSoc(const Message& msg) {
#ifdef CHSS_PKG_DBG
        AINFO << "receive soc data: " << msg.DebugString();
#endif
    }

} //namespace parser
} //namespace chss
} //namespace mstf
