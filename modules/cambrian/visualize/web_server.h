#pragma once

#include "modules/cambrian/visualize/websocket/wbskt_server.h"

namespace mstf {
namespace camb {
namespace visual {

    class WebServer {
        public:
            WebServer(const RemoteOpts*);
            ~WebServer();

            void Init();
            void Start();
            void Stop();

            std::shared_ptr<Message> FlushNginx();
            int StuffNginx(Message*, const std::string&&);

            int SendMessage(const std::string);
            int RecvMessage(std::string& s , int& len);

            inline void RegisterRemoteHdl(const
                    RemoteMessageHdl& hdl) {
                message_hdl_ = hdl;
            }

        private:
#ifdef USE_SOCKETIO
            std::unique_ptr<SockectIo> sock_io_;
#else
            //websocket
            std::unique_ptr<WbsktServer> wsck_svr_;
#endif

            std::unique_ptr<RemoteParser> html_ps_;

            //handler for remote messages
            RemoteMessageHdl message_hdl_;

            std::shared_ptr<RemoteOpts> ropts_;
    };

} //namespace Visual
} //namespace camb
} //namespace mstf
