#include "cyber/cyber.h"
#include "cyber/time/clock.h"

#include "modules/chassis/proto/chassis_config.pb.h"

#include "modules/chassis/dispatch_chs.h"  //publish
#include "modules/chassis/receive_ctrl.h"  //subscribe

using namespace apollo;
using namespace mstf;
using namespace chss;

static ::cyber::Time chas_pkg_start_time_;

static std::shared_ptr<::proto::ChassisConfig> chs_conf_;

static std::unique_ptr<DispatchChs> chas_dispatcher_;
static std::unique_ptr<ReceiveCtrl> ctrl_receiver_;

int main(int argc, char **argv) {
    AINFO << "chassis start, name: " << PKG_NAME;

    ::cyber::Init(PKG_NAME);

    chas_pkg_start_time_ =
        ::cyber::Time(::cyber::Clock::NowInSeconds());
    AINFO << "Now Time: " << chas_pkg_start_time_.ToString();

    std::string chs_path = ::cyber::common::WorkRoot() +
        "../conf/ChassisConfig.conf";

    chs_conf_ = std::make_shared<::proto::ChassisConfig>();

    if (!::cyber::common::GetProtoFromFile(chs_path,
                chs_conf_.get())) {
        AFATAL << "get chassis config failed!";
        return -1;
    }

    AINFO << "chassis config path: " << chs_path <<
        ", root: " << ::cyber::common::WorkRoot() <<
#if 0
        ", config: " << chs_conf_->DebugString();
#else
        "";
#endif

    //1, dispatch chassis messages
    chas_dispatcher_ = std::make_unique<DispatchChs>
        (&chs_conf_->chs_topic_conf());
    AINFO << "UPSTREAM >>> publisher for releasing chassis OK";

    //2, chassis control reader downstream ReceiveCtrl
    ctrl_receiver_ = std::make_unique<ReceiveCtrl>(chs_conf_);
    AINFO << "<<< DOWNSTREAM subscriber for receiving control OK";

    //exit handler
    auto chassis_exit_hdl = [&](void) {
        AWARN << "get exit signal, shutdown!";

        ctrl_receiver_.reset();
        chas_dispatcher_.reset();
        chs_conf_.reset();

        AINFO << "chassis shutdown finish " <<
            (system("stty echo") ? "NG" : "OK");

        std::this_thread::sleep_for(std::chrono::
                milliseconds(100));
    };

    ::cyber::RegisterUserExitHandle(chassis_exit_hdl);

    AINFO << "Cyber Chassis Wait\n...";

    //main thread block...
    ::cyber::WaitForShutdown();

    AWARN << "Chassis Terminate\n...";

    return 0;
}
