#pragma once

#include <set>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/common/thread.hpp>

#include "cyber/common/log.h"

#include "modules/cambrian/visualize/webase_itf.h"

namespace mstf {
namespace camb {
namespace visual {

    typedef websocketpp::server<websocketpp::config::asio> server;

    using websocketpp::connection_hdl;
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    using websocketpp::lib::bind;

    using websocketpp::lib::thread;
    using websocketpp::lib::mutex;
    using websocketpp::lib::lock_guard;
    using websocketpp::lib::unique_lock;
    using websocketpp::lib::condition_variable;

    /* on_open insert connection_hdl into channel
     * on_close remove connection_hdl from channel
     * on_message queue send to all channels
     */

    enum action_type {
        SUBSCRIBE,
        UNSUBSCRIBE,
        MESSAGE
    };

    struct action {
        action(action_type t, connection_hdl h) : type(t), hdl(h) {}
        action(action_type t, connection_hdl h, server::message_ptr m)
            : type(t), hdl(h), msg(m) {}

        action_type type;
        websocketpp::connection_hdl hdl;
        server::message_ptr msg;
    };

    class WbsktServer : public WebaseItf {
        public:
            WbsktServer(const WbsktOptions*);
            ~WbsktServer();

            int Init() override;;
            int Start() override;
            int Stop() override;
            int Close() override;

            bool Readable() const override;
            bool Writable() const override;
            std::string Status() const override;

            int SendWebMsg(const std::string&) override;
            int SendWebMsg(const void*, const int) override;
            int RecvWebMsg(std::string&, int&) override;

        private:
            void run() {
                // listen on specified port
                if (wbskt_ops_->local_port() == -1) {
                    AWARN << "port error!";
                    return;
                }

                //websocket reuse port
                server_.set_reuse_addr(true);

                AINFO << "websocket listen port: " <<
                    wbskt_ops_->local_port();
                server_.listen(websocketpp::lib::asio::ip::tcp::v4(),
                        wbskt_ops_->local_port());

                // Start the server accept loop
                AINFO << "websocket accept";
                server_.start_accept();

                // Start the ASIO io_service run loop
                AINFO << "websocket core thread running";
                try {
                    server_.run();
                } catch (const std::exception & e) {
                    AWARN << e.what();
                }

                AWARN << "websocket core thread exit";
            }

            // remote client open
            void on_open(connection_hdl hdl) {
                {
                    lock_guard<mutex> guard(act_lck_);
                    //AINFO << "on websock client remote opened";
                    actions_.push(action(SUBSCRIBE, hdl));
                }
                act_cond_.notify_one();
            }

            // remote client closed
            void on_close(connection_hdl hdl) {
                {
                    lock_guard<mutex> guard(act_lck_);
                    //AINFO << "on websock client remote closed";
                    actions_.push(action(UNSUBSCRIBE, hdl));
                }
                act_cond_.notify_one();
            }

            // remote client send message
            void on_message(connection_hdl hdl, server::message_ptr msg) {
                // queue message up for sending by processing thread
                {
                    lock_guard<mutex> guard(act_lck_);
                    //AINFO << "on recv client message";
                    actions_.push(action(MESSAGE, hdl, msg));
                }
                act_cond_.notify_one();
            }

            void process_messages() {
                AINFO << "websocket process messages start";

                while (running_.load()) {
                    unique_lock<mutex> lock(act_lck_);

                    if (actions_.empty()) {
#ifdef CAMB_PKG_DBG
                        AWARN << "websocket wait for processing";
#endif
                        act_cond_.wait(lock);
#ifdef CAMB_PKG_DBG
                        AWARN << "websocket start to process OK";
#endif
                        if (!running_.load()) {
                            lock.unlock();
#ifdef CAMB_PKG_DBG
                            AWARN << "websocket exit process, END";
#endif
                            break;
                        }
                    }

                    action a = actions_.front();
                    actions_.pop();

                    lock.unlock();

                    if (a.type == SUBSCRIBE) {
                        lock_guard<mutex> guard(conn_lck_);
                        conns_.insert(a.hdl);
                        AWARN << "websock client subscribe, conn size: " << conns_.size();
                    } else if (a.type == UNSUBSCRIBE) {
                        lock_guard<mutex> guard(conn_lck_);
                        conns_.erase(a.hdl);
                        AWARN << "websock client un-subscribe, conn size: " << conns_.size();
                    } else if (a.type == MESSAGE) {
                        lock_guard<mutex> guard(conn_lck_);

                        con_list::iterator it;
                        for (it = conns_.begin(); it != conns_.end(); ++it) {
#if 0
                            //just echo the origal message
                            //server_.send(*it, a.msg);
                            AINFO << "receive client message raw: "
                                << a.msg->get_payload();
#endif
                            if (a.msg->get_opcode() == websocketpp::frame::opcode::text) {
                                OnRecvTextWebMsg(a.msg->get_payload());
                            } else if (a.msg->get_opcode() == websocketpp::frame::opcode::binary) {
                                OnRecvBinaryWebMsg(a.msg->get_raw_payload());
                            }
                        }
                    } else {
                        // undefined.
                        AWARN << "undefined message type!";
                    }
                }

                std::queue<action>().swap(actions_);
                AWARN << "websocket message process thread exit";
            }

        private:
            typedef std::set<connection_hdl, std::owner_less<connection_hdl> > con_list;

            server server_;
            con_list conns_;
            std::queue<action> actions_;

            mutex act_lck_;
            mutex conn_lck_;
            condition_variable act_cond_;
            thread proc_thread_;
            thread wbsc_thread_;

            const WbsktOptions* wbskt_ops_ {};
            std::atomic<bool> inited_ { false };
            std::atomic<bool> running_ { false};
    };

} //namespace visual
} //namespace camb
} //namespace mstf
