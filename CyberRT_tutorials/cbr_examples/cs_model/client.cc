#include "CyberRT_tutorials/examples/proto/examples.pb.h"

#include "cyber/cyber.h"
#include "cyber/time/rate.h"

using apollo::cyber::examples::proto::Driver;
using apollo::cyber::Rate;

int main(int argc, char* argv[])
{
    apollo::cyber::Init(argv[0]);
    std::shared_ptr<apollo::cyber::Node> node(
            apollo::cyber::CreateNode("client_node"));
    auto client = node->CreateClient<Driver, Driver>("test_server");
    auto driver_msg = std::make_shared<Driver>();
    driver_msg->set_msg_id(0);
    driver_msg->set_timestamp(0);

    Rate rate(0.4);

    while (apollo::cyber::OK()) {
        auto res = client->SendRequest(driver_msg);
        if (res != nullptr) {
            AINFO << "client: responese: " << res->ShortDebugString();
        } else {
            AINFO << "client: service may not ready.";
        }
        rate.Sleep();
    }

    apollo::cyber::WaitForShutdown();
    return 0;
}
