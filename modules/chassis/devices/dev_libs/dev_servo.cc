#include "cyber/common/log.h"

#include "modules/chassis/parser/parse_libs/servo_parser.h"
#include "modules/chassis/devices/data_transact.h"
#include "modules/chassis/devices/dev_libs/dev_servo.h"

namespace mstf {
namespace chss {
namespace device {

    DevServo::DevServo(const ChassisConfig* cc) :
            DeviceBaseItf(cc->servo_dev().si(),
                    cc->servo_dev().sn_ind()) {
#ifdef CHSS_PKG_DBG
        AINFO << "DevServo construct" <<
#if 0
            ":\n{\n" <<
            cc->servo_dev().DebugString() << "}";
#else
            "";
#endif
#endif

        data_parser_ = std::make_unique
            <ServoParser>(cc, &cc->servo_dev().si());
        DataTransact::Instance()->RegisterDevice(
                cc->servo_dev().si().name(),
                cc->servo_dev().sn_ind(),
                dynamic_cast<DeviceBaseItf*>(this));
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
        DeviceBaseItf::Init();

        DownToServoData data;
        data.mutable_config()->set_opt(E_SUBDEV_OPTS_INIT);

        return data_parser_->WriteServoMessage(data);
    }

    int DevServo::Start(void) {
#ifdef CHSS_PKG_DBG
        AINFO << "DevServo start";
#endif
        DeviceBaseItf::Start();

        DownToServoData data;

        //load servo motor register values
        data.mutable_config()->set_opt(E_SUBDEV_OPTS_STORE);
        data_parser_->WriteServoMessage(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        //load servo motor registers
        data.mutable_config()->set_opt(E_SUBDEV_OPTS_START);
        data_parser_->WriteServoMessage(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        //enable axle
        data.mutable_config()->set_opt(E_SUBDEV_OPTS_ENABLE);
        return data_parser_->WriteServoMessage(data);
    }

    int DevServo::Stop(void) {
#ifdef CHSS_PKG_DBG
        AINFO << "DevServo stop";
#endif
        DeviceBaseItf::Stop();

        DownToServoData data;

        //load servo motor register values
        data.mutable_config()->set_opt(E_SUBDEV_OPTS_STOP);
        return data_parser_->WriteServoMessage(data);
    }

    void DevServo::Close(void) {
#ifdef CHSS_PKG_DBG
        AINFO << "DevServo close";
#endif
        Stop();
        DownToServoData data;

        data.mutable_config()->set_opt(E_SUBDEV_OPTS_DISABLE);
        data_parser_->WriteServoMessage(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        data.mutable_config()->set_opt(E_SUBDEV_OPTS_CLOSE);
        data_parser_->WriteServoMessage(data);

        DeviceBaseItf::Close();
    }

} //namespace device
} //namespace chss
} //namespace mstf
