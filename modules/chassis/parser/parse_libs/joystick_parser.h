#pragma once

#include <linux/joystick.h>
#include "modules/chassis/parser/parser_base_itf.h"

namespace mstf {
namespace chss {
namespace parser {

class JoyStickParser : public ParserBaseItf {
public:
    JoyStickParser(const SensorIndicator&);
    virtual ~JoyStickParser() final;

    virtual int Init() override;
    virtual int Start() override;
    virtual int Stop() override;
    virtual int Resume() override;
    virtual int Close() override;

private:
    virtual int ParseSocRaw(const SensorIndicator*,
            const uint8_t* buf, const size_t len) override;

    int ParseSigleFrame(const js_event& event);

    void ChangeLineSpeed(int flag);
    void ChangeAngularSpeed(int flag);

    float autorepeat_rate_   = 0.0;
    float deadzone_          = 0.05;
    float coalesce_interval_ = 0.001;

    const float line_speed_inc_        = 0.1;   // m/s
    const float angular_spped_inc_     = 0.25;  // rad/s
    const float max_line_speed_        = 1.0;
    const float min_line_speed_        = 0.02;
    const float max_angular_speed_     = 1.5;
    const float min_angular_speed_     = 0.125;
    const float default_line_speed_    = 0.05;
    const float defalut_angluar_speed_ = 0.025;
    float line_speed_scale_ = 0.0;
    float angular_speed_scale_ = 0.0;

    bool default_trig_val_ = false;
    bool sticky_buttons_   = false;
};

}  //namespace parser
}  //namespace chss
}  //namespace mstf
