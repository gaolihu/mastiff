#include "cyber/common/log.h"

#include "modules/cambrian/visualize/visual_client.h"

namespace mstf {
namespace camb {
namespace visual {

    VisualClient::VisualClient(const
            VisualizeOpts* vops) {
#ifdef CAMB_PKG_DBG
        AINFO << "VisualClient construct ~ 2\n" <<
            vops->DebugString();
#endif
        time_upload_last_ =
            std::chrono::steady_clock::now();

        web_server_ = std::make_unique<WebServer>
            (&vops->remote_ops());
        web_server_->RegisterRemoteHdl(std::bind(
                    &VisualClient::OnHandleRemote,
                    this, std::placeholders::_1));
    }

    VisualClient::~VisualClient() {
#ifdef CAMB_PKG_DBG
        AINFO << "VisualClient de-construct";
#endif
        if (web_server_.get() != nullptr)
            web_server_.reset();
    }

    void VisualClient::Init() {
#ifdef CAMB_PKG_DBG
        AINFO << "VisualClient init, sub: "
            "webserver / websocket";
#endif
        if (web_server_.get() != nullptr)
            web_server_->Init();
    }

    void VisualClient::Start() {
#ifdef CAMB_PKG_DBG
        AINFO << "VisualClient start";
#endif
        if (web_server_.get() != nullptr)
            web_server_->Start();
    }

    void VisualClient::Stop() {
#ifdef CAMB_PKG_DBG
        AINFO << "VisualClient stop";
#endif
        if (web_server_.get() != nullptr)
            web_server_->Stop();
    }

    void VisualClient::Close() {
#ifdef CAMB_PKG_DBG
        AINFO << "VisualClient close";
#endif
        if (web_server_.get() != nullptr) {
#if 0
            auto ngx = web_server_->FlushNginx();
            if (ngx == nullptr) {
                AWARN << "null msg!";
                return;
            }
            visual_rvs_(ngx);
            OnHandleRemote(ngx);
#endif
        }
    }

    ////////////////////////////////////////
    //handle message: REMOTE -> ROBOT
    void VisualClient::OnHandleRemote(const
            std::shared_ptr<Message>& msg) {
#ifdef CAMB_PKG_DBG
        AINFO << "Handle remote message: " <<
            msg->GetTypeName() <<
#if 0
            "\nmsg: {\n"
            << msg->DebugString() << "}";
#else
        "";
#endif
#endif

        if (msg.get() == nullptr)
            return;

#if 1
        //visual_rvs_(msg);
#else
#endif
    }

    int VisualClient::DoKeepRemoteAlive() {
        std::string str = "KeepRemoteAlivePost";

        KeepRemoteAlivePost msg;

        return web_server_->StuffNginx(dynamic_cast<
                Message*>(&msg), std::move(str));
    }

    ////////////////////////////////////////
    //handle message ---> upstream
    int VisualClient::SyncRemoteMap(const
            std::shared_ptr<OccupancyGrid>& msg) {
        AINFO << "update grid map";
        if (web_server_.get() != nullptr) {
            //web_server_->UpdateGridMap(msg);
        }

        return 0;
    }

    int VisualClient::SyncRemotePath(const
            std::shared_ptr<MarkerArray>& msg) {
        AINFO << "update path";
        if (web_server_.get() != nullptr) {
            //web_server_->UpdatePath(msg);
        }

        return 0;
    }

    /*
    int VisualClient::SyncRemoteRawImu(const
            std::shared_ptr<IMUdata>& msg) {
        AINFO << "update imu";
        if (web_server_.get() != nullptr) {
            //web_server_->SyncRemoteRawImu(msg);
        }

        return 0;
    }

    int VisualClient::SyncRemoteRawOdom(const
            std::shared_ptr<OdomData>& msg) {
        AINFO << "update odome";
        if (web_server_.get() != nullptr) {
            //web_server_->SyncRemoteRawOdom(msg);
        }

        return 0;
    }

    int VisualClient::SyncRemoteRawLaser(const
            std::shared_ptr<LaserData>& msg) {
        AINFO << "update laser";
        if (web_server_.get() != nullptr) {
            //web_server_->SyncRemoteRawLaser(msg);
        }

        return 0;
    }

    int VisualClient::SyncRemoteRawMisc(const
            std::shared_ptr<ChassisMixData>& msg) {
        AINFO << "update chassis mix";
        if (web_server_.get() != nullptr) {
            //web_server_->SyncRemoteRawMisc(msg);
        }

        return 0;
    }
    */

    /*
    void VisualClient::CacheFactory(const
            std::shared_ptr<ChassisFacotryData>& msg) {
        AINFO << "update factory";
        if (web_server_.get() != nullptr) {
            //web_server_->CacheFactory(msg);
        }
    }
    */

} //namespace Visual
} //namespace camb
} //namespace mstf
