#pragma once

#include "cyber/common/log.h"

#include "modules/cambrian/proto/cambrian_data.pb.h"
#include "modules/chassis/proto/chss_io.pb.h"

namespace mstf {
namespace camb {
namespace brain {

    using namespace ventura::common_msgs::nav_msgs;
    using namespace ventura::common_msgs::sensor_msgs;
    using namespace ventura::common_msgs::geometry_msgs;

    using namespace apollo;
    using namespace /*mstf::camb::*/proto;
    using namespace /*mstf::*/chss::proto;
    using namespace google::protobuf;

    using DatabaseNotifyHook = std::function<void()>;
    using ImplementorNotifyHook = std::function<void()>;

    class SignalsKick {
        public:
            SignalsKick();
            virtual ~SignalsKick();

            inline void HookImplementor(const
                    ImplementorNotifyHook& hk) {
                //AINFO << "register implementor kicker";
                im_notify_hook_ = hk;
            }
            inline int KickImplementorHook() {
                //AINFO << "implementor kick notify";
                im_notify_hook_();
                return 0;
            }

        private:
            //notify hook
            DatabaseNotifyHook db_notify_hook_ = nullptr;
            ImplementorNotifyHook im_notify_hook_ = nullptr;
    };

} //namespace brain
} //namespace camb
} //namespace mstf
