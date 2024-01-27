#include "cyber/cyber.h"
#include "cyber/time/clock.h"

#include "modules/chassis/dispatcher.h"     //publish
#include "modules/chassis/receiver.h"       //subscribe
#include "modules/chassis/devices/device_manager.h"

using namespace apollo;
using namespace mstf;
using namespace chss;

static ::cyber::Time chas_pkg_start_time_;

static std::shared_ptr<::proto::ChassisConfig> chs_conf_;

static std::unique_ptr<DispatchMsg> chas_dispatcher_;
static std::unique_ptr<ReceiveMsg> ctrl_receiver_;
static std::unique_ptr<DeviceManager> devs_mgr_;

static std::string logo_ = R"(
   ________                    _
  / ____/ /_  ____ ___________(_)____
 / /   / __ \/ __ `/ ___/ ___/ / ___/
/ /___/ / / / /_/ (__  |__  ) (__  )
\____/_/ /_/\__,_/____/____/_/____/
)";


int main(int argc, char **argv) {
    AINFO << logo_;
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

    //1, chassis control reader downstream
    ctrl_receiver_ = std::make_unique<ReceiveMsg>(chs_conf_);
    AWARN << "<<< DOWNSTREAM subscriber for receiving control OK";

    //2, dispatch chassis messages
    chas_dispatcher_ = std::make_unique<DispatchMsg>(chs_conf_);
    AWARN << "UPSTREAM >>> publisher for releasing chassis OK";

#if 1
    //3, device manager
    devs_mgr_ = std::make_unique<DeviceManager>(chs_conf_);
    devs_mgr_->DeviceInit();
#if 1   //for testing
    AINFO << "start to powerup peripheral devices, for testing";
    devs_mgr_->DeviceStart();
#endif
#endif

    //exit handler
    auto chassis_exit_hdl = [&](void) {
        AWARN << "get exit signal, shutdown!";

        devs_mgr_->DeviceClose();

        devs_mgr_.reset();
        chas_dispatcher_.reset();
        ctrl_receiver_.reset();

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
