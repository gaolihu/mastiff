#include "cyber/cyber.h"
#include "cyber/time/clock.h"

#include "modules/cambrian/proto/cambrian_arch.pb.h"

#include "modules/cambrian/dispatcher.h"    //publish
#include "modules/cambrian/receiver.h"      //subscribe

using namespace apollo;
using namespace mstf;
using namespace camb;

static ::cyber::Time camb_pkg_start_time_;

static std::shared_ptr<::proto::CambrianConfig> cmb_conf_;

static std::unique_ptr<DispatchMsg> dispatcher_;
static std::unique_ptr<ReceiveMsg> receiver_;

static std::string logo_ = R"(
   _____                _               .
  / ____|              | |             (_)
 | |     __ _ _ __ ___ | |__  _ __ __ _ _ _ __
 | |    / _` | '_ ` _ \| '_ \| '__/ _` | | '_ \
 | |___| (_| | | | | | | |_) | | | (_| | | | | |
  \_____\__,_|_| |_| |_|_.__/|_|  \__,_|_|_| |_|

)";

int main(int argc, char **argv) {
    AINFO << logo_;
    AINFO << "cambrian start, name: " << PKG_NAME;

    ::cyber::Init(PKG_NAME);

    camb_pkg_start_time_ =
        ::cyber::Time(::cyber::Clock::NowInSeconds());
    AINFO << "Now Time: " << camb_pkg_start_time_.ToString();

    std::string cmb_path = ::cyber::common::WorkRoot() +
        "/../conf/CambrianConfig.conf";

    cmb_conf_ = std::make_shared<::proto::CambrianConfig>();

    if (!::cyber::common::GetProtoFromFile(cmb_path,
                cmb_conf_.get())) {
        AFATAL << "get cambrian config failed!";
        return -1;
    }

    AINFO << "cambrian config path: " << cmb_path <<
        ", root: " << ::cyber::common::WorkRoot() <<
#if 0
        ", config: " << cmb_conf_->DebugString();
#else
        "";
#endif

    //1, cambrian control reader downstream
    receiver_ = std::make_unique<ReceiveMsg>(cmb_conf_);
    AWARN << "<<< DOWNSTREAM subscriber for receiving msg OK";

    //2, dispatch cambrian messages
    dispatcher_ = std::make_unique<DispatchMsg>(cmb_conf_);
    AWARN << "UPSTREAM >>> publisher for releasing cambrian OK";

    dispatcher_->DispatchInit();

    //exit handler
    auto cambsis_exit_hdl = [&](void) {
        AWARN << "get exit signal, shutdown!";

        receiver_.reset();
        dispatcher_.reset();
        cmb_conf_.reset();

        AINFO << "cambrian shutdown finish " <<
            (system("stty echo") ? "NG" : "OK");

        std::this_thread::sleep_for(std::chrono::
                milliseconds(100));
    };

    ::cyber::RegisterUserExitHandle(cambsis_exit_hdl);

    AINFO << "Cyber cambrian Wait\n...";

    //main thread block...
    ::cyber::WaitForShutdown();

    AWARN << "cambrian Terminate\n...";

    return 0;
}
