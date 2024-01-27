#include "cyber/cyber.h"

#include "modules/cambrian/visualize/web_server.h"

namespace mstf {
namespace camb {
namespace visual {

    WebServer::WebServer(const RemoteOpts* rops) {
#ifdef CAMB_PKG_DBG
        AINFO << "WebServer construct";
#endif
        ropts_ = std::make_shared<RemoteOpts>();
        ropts_->CopyFrom(*rops);
#ifdef USE_SOCKETIO
        sock_io_.reset(new SockectIo(ropts_->wbskt_conf().local_ip(),
                ropts_->wbskt_conf().local_port(), 0));
#else
        wsck_svr_.reset(new WbsktServer(&ropts_->wbskt_conf()));
#endif
        html_ps_.reset(new RemoteParser());
#ifdef USE_SOCKETIO
        //TODO
#else
        //handle websocket messages
        RemoteStringHdl f = [&](const std::string& s)->int {
            auto ngx = html_ps_->DeformatNgxMsg(s);
            if (ngx == nullptr)
                return -1;
#ifdef CAMB_PKG_DBG
            /*
            AINFO << "remote msg type: " <<
                ngx->GetTypeName();
                */
#endif
            if (message_hdl_)
                message_hdl_(ngx);
            return 0;
        };

        wsck_svr_->SetWebStrHandle(f);
#endif
    }

    WebServer::~WebServer() {
#ifdef CAMB_PKG_DBG
        AINFO << "WebServer de-construct";
#endif
        Stop();
    }

    void WebServer::Init() {
#ifdef USE_SOCKETIO
        sock_io_->Init();
#else
        wsck_svr_->Init();
#endif
    }

    void WebServer::Start() {
#ifdef USE_SOCKETIO
        sock_io_->Start();
#else
        wsck_svr_->Start();
#endif
    }

    void WebServer::Stop() {
#ifdef USE_SOCKETIO
        sock_io_->Stop();
#else
        wsck_svr_->Stop();
#endif
    }

    std::shared_ptr<google::protobuf::Message>
        WebServer::FlushNginx() {
        //AINFO << "FlushNgx";

        int len = -1;
        std::string str;

#ifdef USE_SOCKETIO
        if (sock_io_->Readable()) {
            if (RecvMessage(str, len) == 0) {
                if (len == 0) {
                    //rops data, ignore
                    //AWARN << "read 0 from web!";
                } else if (len > 0) {
                    return html_ps_->DeformatNgxMsg(str);
                } else {
                    AWARN << "err, no data from web!";
                }
            } else {
                AWARN << "read ngx wrong!";
                //error
            }
        } else {
            AWARN << "none-readable!\n";
        }
#else
        if (wsck_svr_->Readable()) {
            if (RecvMessage(str, len) == 0) {
                if (len == 0) {
                    //no data, ignore
                    //AWARN << "read 0 from web!";
                } else if (len > 0) {
                    return html_ps_->DeformatNgxMsg(str);
                } else {
                    AWARN << "err, no data from web!";
                }
            } else {
                AWARN << "read ngx wrong!";
                //error
            }
        } else {
            AWARN << "none-readable!\n";
        }
#endif

        return nullptr;
    }

    int WebServer::StuffNginx(Message* msg,
            const std::string&& str) {
#ifdef USE_SOCKETIO
        return sock_io_->SendWebMsg(str);
#else
        if (!wsck_svr_->Writable()) {
            //ignore the message when remote un-connected
#ifdef CAMB_PKG_DBG
            AERROR_EVERY(200) << "unconnected!!! reject msg: " <<
                msg->GetTypeName();
#endif
            return -1;
        }

        auto des = html_ps_->FormatNgxMsg(msg, str);

        if (des == nullptr)
            return -1;

        int len = des->ByteSizeLong();
        std::vector<uint8_t> msg_t;

        msg_t.resize(len);
        if (!des->SerializeToArray(&msg_t[0], len)) {
        //if (!des->SerializePartialToArray(&msg_t[0], len)) {
#if 1
            AWARN << "stuff NGINX error, len: " << len
#if 1
                << ", msg: \n{\n" << des->DebugString() << "}";
#else
            ;
#endif
#endif
        }

        return wsck_svr_->SendWebMsg((void*)&msg_t[0], len);
#endif
    }

    //upstream to web page
    int WebServer::SendMessage(const std::string str) {
#ifdef USE_SOCKETIO
        return sock_io_->SendWebMsg(str);
#else
        return wsck_svr_->SendWebMsg(str);
#endif
    }

    //downstream to robot
    int WebServer::RecvMessage(std::string& s , int& len) {
#ifdef USE_SOCKETIO
        return sock_io_->RecvWebMsg(s, len);
#else
        return wsck_svr_->RecvWebMsg(s, len);
#endif
    }

} //namespace Visual
} //namespace camb
} //namespace mstf
