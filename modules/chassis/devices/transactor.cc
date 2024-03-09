#include "modules/chassis/devices/transactor.h"

namespace mstf {
namespace chss {
namespace device {

    Transactor::Transactor() {
#ifdef CHSS_PKG_DBG
        AINFO << "Transactor singleton construct";
#endif
#ifdef KEY_SIMULATE
        key_sim_ = std::make_shared<KeySimulate>();
        key_sim_->RegisterHandle([&] (const
                    std::shared_ptr<Message>& m)->int {
                return RecvChassCtrl(
                        std::dynamic_pointer_cast
                        <MiscChassisCtrl>(m));
                });
        key_sim_->StartSimulate();
#endif
    }

    Transactor::~Transactor() {
#ifdef CHSS_PKG_DBG
        AINFO << "Transactor singleton de-construct!";
#endif
        msg_publisher_pair_.clear();
        msgs_receiver_pair_.clear();
    }

    //downstream
    int Transactor::RecvChassCtrl(const
            std::shared_ptr<MiscChassisCtrl>& ctrl) {
//#ifdef CHSS_PKG_DBG
#if 0
        //too much noise
        AINFO << "receive chassis control:\n" <<
            ctrl->DebugString();
#endif
        int ret = -1;

        //1, ctrl wheel
        if (ctrl->has_move_ctrl()) {
            if ((ret = _CtrlChsMovement(ctrl->move_ctrl())) != 0) {
                AWARN << "speed control problem!";
            }
            return ret;
        }

        //2, ctrl adc
        if (ctrl->has_adc_ctrl())
            if ((ret = _CtrlChsPeriphAdc(ctrl->adc_ctrl())) != 0)
                AWARN << "adc control problem!";

        //3, ctrl pwm
        if (ctrl->has_pwm_ctrl())
            if ((ret = _CtrlChsPeriphPwm(ctrl->pwm_ctrl())) != 0)
                AWARN << "pwm control problem!";

        //4, ctrl gpio
        if (ctrl->has_gpio_ctrl())
            if ((ret = _CtrlChsPeriphGpio(ctrl->gpio_ctrl())) != 0)
                AWARN << "gpio control problem!";

        //5, ctrl infra
        if (ctrl->has_infra_ctrl())
            if ((ret = _CtrlChsPeriphInfra(ctrl->infra_ctrl())) != 0)
                AWARN << "infra control problem!";

        //6, ctrl misc
        if (ctrl->has_misc_ctrl())
            if ((ret = _CtrlChsSocMisc(ctrl->misc_ctrl())) != 0)
                AWARN << "misc control problem!";

        //7, ctrl update
        if (ctrl->has_firmware_update())
            if ((ret = _CtrlChsUpdate(ctrl->firmware_update())) != 0)
                AWARN << "soc control problem!";

        return ret;
    }

    int Transactor::RecvChassMove(const std::shared_ptr
            <ventura::common_msgs::geometry_msgs::Twist>& move) {
        int ret = -1;
        ChsMovementCtrl chs_move;

        chs_move.mutable_diff_spd()->set_linear(move->linear().x());
        chs_move.mutable_diff_spd()->set_angular(move->angular().z());

        if ((ret = _CtrlChsMovement(chs_move) != 0))
            AWARN << "wheel control problem twist!";

        return ret;
    }

} //namespace device
} //namespace chss
} //namespace mstf
