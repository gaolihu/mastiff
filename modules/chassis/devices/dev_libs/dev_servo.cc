#include "modules/chassis/devices/dev_parse_link.h"
#include "modules/chassis/devices/dev_libs/dev_servo.h"
#include "modules/chassis/parser/pack_libs/servo_packer.h"
#include "modules/chassis/parser/parse_libs/servo_parser.h"

namespace mstf {
namespace chss {
namespace device {

    DevServo::DevServo(const
            SensorIndicator& idc) :
            DeviceBaseItf(idc) {
#ifdef CHSS_PKG_DBG
        AINFO << "DevServo construct";
#endif
        data_packer_ = std::make_unique
            <ServoPacker>(idc.ihi().name());
        data_parser_ = std::make_unique
            <ServoParser>(idc);

        DevParseLink::Instance()->RegisterDevice(
                idc, dynamic_cast<DeviceBaseItf*>(this));
    }

    DevServo::~DevServo() {
#ifdef CHSS_PKG_DBG
        AINFO << "DevServo de-construct";
#endif
    }

    int DevServo::Init(void) {
#ifdef CHSS_PKG_DBG
        AINFO << "DevServo init";
#endif
        if (DeviceBaseItf::Init() != 0) {
            AERROR << "init servo error!";
            return -1;
        }

        ServoSetting org;
        org.mutable_dev_manage()->set_sub_opts(E_SUBDEV_OPTS_INIT);
        auto data = data_packer_->PackServoMtrSetting(org);
        AINFO << "E_SUBDEV_OPTS_INIT";

        return data_parser_->WriteServoMessage(data);
    }

    int DevServo::Start(void) {
#ifdef CHSS_PKG_DBG
        AINFO << "DevServo start";
#endif
        if (DeviceBaseItf::Start() != 0) {
            AERROR << "start servo error!";
            return -1;
        }

        ServoSetting org;

        //load servo motor register values
        org.mutable_dev_manage()->set_sub_opts(E_SUBDEV_OPTS_STORE);
        auto data = data_packer_->PackServoMtrSetting(org);
        AINFO << "E_SUBDEV_OPTS_STORE";
        data_parser_->WriteServoMessage(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        //load servo motor registers
        org.mutable_dev_manage()->set_sub_opts(E_SUBDEV_OPTS_START);
        data = data_packer_->PackServoMtrSetting(org);
        AINFO << "E_SUBDEV_OPTS_START";
        data_parser_->WriteServoMessage(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        //enable axle
        org.mutable_dev_manage()->set_sub_opts(E_SUBDEV_OPTS_ENABLE);
        data = data_packer_->PackServoMtrSetting(org);
        AINFO << "E_SUBDEV_OPTS_ENABLE";
        return data_parser_->WriteServoMessage(data);
    }

    int DevServo::Stop(void) {
#ifdef CHSS_PKG_DBG
        AINFO << "DevServo stop";
#endif
        if (DeviceBaseItf::Stop()) {
            AERROR << "stop servo error!";
            return -1;
        }

        ServoSetting org;

        //load servo motor register values
        org.mutable_dev_manage()->set_sub_opts(E_SUBDEV_OPTS_STOP);
        auto data = data_packer_->PackServoMtrSetting(org);
        AINFO << "E_SUBDEV_OPTS_STOP";
        return data_parser_->WriteServoMessage(data);
    }

    int DevServo::Resume(void) {
#ifdef CHSS_PKG_DBG
        AINFO << "DevServo resume";
#endif
        if (DeviceBaseItf::Resume()) {
            AERROR << "resume servo error!";
            return -1;
        }

        return 0;
    }

    int DevServo::Close(void) {
#ifdef CHSS_PKG_DBG
        AINFO << "DevServo close";
#endif
        Stop();

        ServoSetting org;

        org.mutable_dev_manage()->set_sub_opts(E_SUBDEV_OPTS_DISABLE);
        auto data = data_packer_->PackServoMtrSetting(org);
        data_parser_->WriteServoMessage(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        org.mutable_dev_manage()->set_sub_opts(E_SUBDEV_OPTS_DISABLE);
        data = data_packer_->PackServoMtrSetting(org);
        data_parser_->WriteServoMessage(data);

        DeviceBaseItf::Close();

        return 0;
    }

    int DevServo::SetSpeed(const ChsMovementCtrl& cc) {
        auto data = data_packer_->PackServoMtrControl(cc);
        return data_parser_->WriteServoMessage(data);
    }

} //namespace device
} //namespace chss
} //namespace mstf
