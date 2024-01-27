#include "modules/cambrian/visualize/websocket/wbskt_server.h"

namespace mstf {
namespace camb {
namespace visual {

    WbsktServer::WbsktServer(const WbsktOptions* wopts) {
#ifdef CAMB_PKG_DBG
        AINFO << "WbsktServer construct";
#endif
        wbskt_ops_ = wopts;
    }

    WbsktServer::~WbsktServer() {
#ifdef CAMB_PKG_DBG
        //TODO
        AINFO << "WbsktServer de-construct!";
#endif
        if (running_)
            Stop();
    }

    int WbsktServer::Init() {
#ifdef CAMB_PKG_DBG
        AINFO << "websocket init ";
#endif
        if (access(wbskt_ops_->ngx_html_pc_path().data(),
                    F_OK) != 0) {
            AWARN << "no html files: " <<
                wbskt_ops_->ngx_html_pc_path();
            inited_.store(false);
            return -1;
        }

        //fix venus.html
        std::string khtml = "sed -i s#@IP@#";
        khtml += wbskt_ops_->local_ip();
        khtml += "#g `grep -rl @IP@ ";
        khtml += wbskt_ops_->ngx_html_pc_path();
        khtml += " ";
        khtml += wbskt_ops_->ngx_html_cp_path();
        khtml += "`";
#ifdef CAMB_PKG_DBG
        //AINFO << "substitude IP: " << khtml;
#endif
        if (system(khtml.c_str())) {
            AWARN << "fix ip error!";
        }

        khtml = "sed -i s#@PORT@#";
        khtml += std::to_string(wbskt_ops_->local_port());
        khtml += "#g `grep -rl @PORT@ ";
        khtml += wbskt_ops_->ngx_html_pc_path();
        khtml += " ";
        khtml += wbskt_ops_->ngx_html_cp_path();
        khtml += "`";
#ifdef CAMB_PKG_DBG
        //AINFO << "substitude PORT: " << khtml;
#endif
        if (system(khtml.c_str())) {
            AWARN << "fix port error!";
        }

        //stop nginx service
#ifdef CAMB_PKG_DBG
        AINFO << "stop remote NGINX service: " <<
            wbskt_ops_->ngx_name();
#endif
        std::string kngx = "killall -9 " +
            wbskt_ops_->ngx_name();
        if (system(kngx.c_str())) {
            AERROR << "killall NGINX error!'";
        } else {
            AWARN << "stop NGINX sevice OK!";
        }

        //start nginx service
        std::string sngx = wbskt_ops_->ngx_bin_path() +
            "/" + wbskt_ops_->ngx_name();
        if (system(sngx.c_str())) {
            AERROR << "start NGINX error!'";
        } else {
            AWARN << "start NGINX sevice OK!";
        }

        std::string rngx = wbskt_ops_->ngx_bin_path() +
            "/" + wbskt_ops_->ngx_name()
            + " -s reload";
        if (system(rngx.c_str())) {
            AERROR << "reload NGINX error!'";
        } else {
            AWARN << "reload NGINX sevice OK!";
        }

        server_.set_error_channels(websocketpp::log::elevel::all);
        //server_.clear_access_channels(websocketpp::log::alevel::frame_payload);
        server_.clear_access_channels(websocketpp::log::alevel::all);

        server_.init_asio();

        server_.set_open_handler(bind(&WbsktServer::on_open, this, _1));
        server_.set_close_handler(bind(&WbsktServer::on_close, this, _1));
        server_.set_message_handler(bind(&WbsktServer::on_message, this, _1, _2));

        inited_.store(true);

        return 0;
    }

    int WbsktServer::Start() {
#ifdef CAMB_PKG_DBG
        AINFO << "websocket start";
#endif
        if (!inited_) {
            AWARN << "websocket not initialized!";
            return -1;
        }

        if (running_) {
            AWARN << "websocket already started!";
            //already stoped
            return 0;
        }

        running_.store(true);

        try {
            proc_thread_ = thread(bind(&WbsktServer::process_messages, this));
        } catch(websocketpp::exception const & e) {
            AWARN << "start websocket process error: " << e.what();
        }
        AINFO << "websocket message processing thread start OK";

        try {
            wbsc_thread_ = thread(bind(&WbsktServer::run, this));
        } catch(websocketpp::exception const & e) {
            AWARN << "start websocket core error: " << e.what();
        }
        AINFO << "websocket message receiveing thread start OK";

        //wait process & cap thread
        std::this_thread::sleep_for(std::chrono::
                milliseconds(80));

        return 0;
    }

    int WbsktServer::Stop() {
#ifdef CAMB_PKG_DBG
        AINFO << "websocket stop";
#endif
        if (!inited_) {
            AWARN << "websocket not initialized!";
            return -1;
        }

        if (!running_) {
            AWARN << "websocket already stoped!";
            //already stoped
            return 0;
        }

#ifdef CAMB_PKG_DBG
        AINFO << "websocket stop \"core\"";
#endif
        //stop wesocket core thread
        server_.stop();
        wbsc_thread_.join();

        //wait for exit message handle
        std::this_thread::sleep_for(std::chrono::
                milliseconds(20));

        //stop wesocket process thread
#ifdef CAMB_PKG_DBG
        AINFO << "websocket stop \"process\"";
#endif
        running_.store(false);
        act_cond_.notify_one();
        proc_thread_.join();

        //restore venus.html
        std::string khtml = "sed -i s#";
        khtml += wbskt_ops_->local_ip();
        khtml += "#@IP@#g `grep -rl ";
        khtml += wbskt_ops_->local_ip();
        khtml += " ";
        khtml += wbskt_ops_->ngx_html_pc_path();
        khtml += " ";
        khtml += wbskt_ops_->ngx_html_cp_path();
        khtml += "`";
#ifdef CAMB_PKG_DBG
        //AINFO << "restore IP: " << khtml;
#endif
        if (system(khtml.c_str())) {
            AERROR << "restore ip error!";
        }

        khtml = "sed -i s#";
        khtml += std::to_string(wbskt_ops_->local_port());
        khtml += "#@PORT@#g `grep -rl ";
        khtml += std::to_string(wbskt_ops_->local_port());
        khtml += " ";
        khtml += wbskt_ops_->ngx_html_pc_path();
        khtml += " ";
        khtml += wbskt_ops_->ngx_html_cp_path();
        khtml += "`";
#ifdef CAMB_PKG_DBG
        //AINFO << "restore PORT: " << khtml;
#endif
        if (system(khtml.c_str())) {
            AERROR << "restore port error!";
        }

        //stop nginx service
#ifdef CAMB_PKG_DBG
        AINFO << "stop remote NGINX service: " <<
            wbskt_ops_->ngx_name();
#endif
        std::string kngx = "killall -9 " +
            wbskt_ops_->ngx_name();
        if (system(kngx.c_str())) {
            AERROR << "killall NGINX error!'";
        } else {
            AWARN << "stop NGINX sevice OK!";
        }

        return 0;
    }

    int WbsktServer::Close() {
        AINFO << "websocket close";
        inited_.store(false);
        return 0;
    }

    bool WbsktServer::Readable() const {
        //AINFO << "websocket readable";
        return actions_.empty() ? false : true;
    }

    bool WbsktServer::Writable() const {
        //AINFO << "websocket writable";
        return conns_.size() > 0 ? true : false;
    }

    std::string WbsktServer::Status() const {
        AINFO << "websocket status";
        return Status();
    }

    int WbsktServer::SendWebMsg(const std::string& s) {
        if (!Writable())
            return -1;

        con_list::iterator it;
        websocketpp::lib::error_code ec;

        if (!running_.load())
            return -1;

        for (it = conns_.begin(); it != conns_.end(); ++it) {
            server_.send(*it, s, websocketpp::frame::opcode::text, ec);
            if (ec) {
                AINFO << "sending message error: " << ec.message();
                return -1;
            }
        }

        return 0;
    }

    int WbsktServer::SendWebMsg(const void* p, const int len) {
        if (!Writable())
            return -1;

        con_list::iterator it;
        websocketpp::lib::error_code ec;

        if (!running_.load())
            return -1;

        for (it = conns_.begin(); it != conns_.end(); ++it) {
            server_.send(*it, p, len, websocketpp::frame::opcode::binary, ec);
            if (ec) {
                AINFO << "sending message error: " << ec.message();
                return -1;
            }
        }

        return 0;
    }

    int WbsktServer::RecvWebMsg(std::string&, int&) {
        //AINFO << "websocket recv";
        //TODO
        return 0;
    }

} //namespace visual
} //namespace camb
} //namespace mstf
