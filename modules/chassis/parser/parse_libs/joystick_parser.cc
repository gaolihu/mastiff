#include "modules/chassis/parser/parse_drv_link.h"
#include "modules/chassis/parser/parse_libs/joystick_parser.h"

namespace mstf {
namespace chss {
namespace parser {

JoyStickParser::JoyStickParser(const
        SensorIndicator& si) : ParserBaseItf(si) {
    AINFO << "JoyStickParser construct";

    line_speed_scale_    = default_line_speed_;
    angular_speed_scale_ = default_line_speed_;

    ParseDrvLink::Instance()->RegisterSocListener
        (std::bind(&ParserBaseItf::OnOriginalDataSoc,
                   this, ::_1, ::_2, ::_3, ::_4), s_idc_);
}

JoyStickParser::~JoyStickParser() {
    AINFO << "JoyStickParser de-construct";
}

int JoyStickParser::Init() {
    ParseDrvLink::Instance()->Init(s_idc_);
    auto dev = dynamic_cast<const JoyStickDevConf*>(GetDevConfig());

    if (dev->has_joy_conf()) {
        return ParserBaseItf::Init();
    }
    else {
        AERROR << "Init Joy Stick error!";
        return -1;
    }
}

    int JoyStickParser::Start() {
        if (ParserBaseItf::Start() != 0) {
            AERROR << "JoyStickParser start error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Start(s_idc_);
    }

    int JoyStickParser::Stop() {
        if (ParserBaseItf::Stop() != 0) {
            AERROR << "JoyStickParser stop error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Stop(s_idc_);
    }

    int JoyStickParser::Resume() {
        if (ParserBaseItf::Resume() != 0) {
            AERROR << "JoyStickParser resume error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Resume(s_idc_);
    }

    int JoyStickParser::Close() {
        if (ParserBaseItf::Close() != 0) {
            AERROR << "JoyStickParser close error!";
            return -1;
        }

        return ParseDrvLink::Instance()->Close(s_idc_);
    }

int JoyStickParser::ParseSocRaw(const SensorIndicator*,
        const uint8_t* buf, const size_t len) {
    int cbuf_len = cbuf_raw_->CbufSize();
    if (cbuf_len < 8) {
        return 0;
    }

    std::vector<uint8_t> raw_data;
    raw_data.resize(cbuf_len);

    if (!cbuf_raw_->ReadSafely(raw_data)) {
        AFATAL << "read circular buffer for Joy failed";
        return -1;
    }

    cbuf_raw_->Restore(cbuf_len);

    if (cbuf_len != 8) {
        AERROR << "TODO...";
        return -1;
    }

    js_event event;
    memcpy(&event, buf, len);
    return ParseSigleFrame(event);
}

int JoyStickParser::ParseSigleFrame(const js_event& event) {
    // 参数转换
    // float autorepeat_interval = 1 / autorepeat_rate_;
    float scale             = -1. / (1. - deadzone_) / 32767.;
    float unscaled_deadzone = 32767. * deadzone_;

    switch (event.type) {
    case JS_EVENT_BUTTON:
    case JS_EVENT_BUTTON | JS_EVENT_INIT: {
        int     val = (int)event.value;
        uint8_t num = (uint8_t)event.number;

#ifdef CHSS_PKG_DBG
        AINFO << "button val: " << (int)event.value;
        AINFO << "button number: " << (int)event.number;
#endif

        if (num == 4 && val == 1) {  // Y
            ChangeLineSpeed(1);
            AINFO << "line speed up";
        }
        else if (num == 0 && val == 1) {  // A
            ChangeLineSpeed(-1);
            AINFO << "line speed down";
        }
        else if (num == 3 && val == 1) {  // X
            ChangeAngularSpeed(1);
            AINFO << "angular speed up";
        }
        else if (num == 1 && val == 1) {  // B
            ChangeAngularSpeed(-1);
            AINFO << "angular speed down";
        }
    } break;
    case JS_EVENT_AXIS | JS_EVENT_INIT:
    case JS_EVENT_AXIS: {
        float val = (float)event.value;
        if (val > unscaled_deadzone) {
            val -= unscaled_deadzone;
        }
        else if (val < -unscaled_deadzone) {
            val += unscaled_deadzone;
        }
        else {
            val = 0;
        }

        float final_val = val * scale;

        //auto chassis_ctrl = std::make_shared<ChassisCtrl>();

        float line_speed    = 0.0;
        float angular_speed = 0.0;

        if ((int)event.number == 0) {}
        else if ((int)event.number == 1) {
            line_speed = final_val * line_speed_scale_;
        }
        else if ((int)event.number == 2) {}
        else if ((int)event.number == 3) {
            angular_speed = final_val * angular_speed_scale_;
        }

#ifdef CHSS_PKG_DBG
        AINFO << "axes val: " << final_val << " line speed: " << line_speed << "angular speed: " << angular_speed;
        AINFO << "axes number: " << (int)event.number;
#endif
        /*
        chassis_ctrl->mutable_speed_ctrl()->set_linear(line_speed);
        chassis_ctrl->mutable_speed_ctrl()->set_angular(angular_speed);
        chassis_ctrl->mutable_speed_ctrl()->set_use_diff_speed(true);
        */
        //device::Transactor::Instance()->RecvChassCtrl(chassis_ctrl);
        //TODO, Glh, 2024/1/11
    } break;

    default:
        break;
    }

    return 0;
}

void JoyStickParser::ChangeLineSpeed(int flag) {
    if (flag == 1) {
        line_speed_scale_ += line_speed_inc_;
        line_speed_scale_ = line_speed_scale_ > max_line_speed_ ? max_line_speed_ : line_speed_scale_;
    }
    else if (flag == -1) {
        line_speed_scale_ -= line_speed_inc_;
        line_speed_scale_ = line_speed_scale_ < min_line_speed_ ? min_line_speed_ : line_speed_scale_;
    }
}
void JoyStickParser::ChangeAngularSpeed(int flag) {
    if (flag == 1) {
        angular_speed_scale_ += angular_spped_inc_;
        angular_speed_scale_ = angular_speed_scale_ > max_angular_speed_ ? max_angular_speed_ : angular_speed_scale_;
    }
    else if (flag == -1) {
        angular_speed_scale_ -= angular_spped_inc_;
        angular_speed_scale_ = angular_speed_scale_ < min_angular_speed_ ? min_angular_speed_ : angular_speed_scale_;
    }
}

}  //namespace parser
}  //namespace chss
}  //namespace mstf
