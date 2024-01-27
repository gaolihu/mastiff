#pragma once

#include <mutex>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "cyber/common/log.h"

#include "fringe/visualization/nginx/webase_itf.h"

namespace venus {
namespace fringe {
namespace visualization {

    class SockectIo : public WebaseItf {
        public:
            SockectIo(const std::string, const int, const int);
            ~SockectIo();

            virtual int Init() override;
            virtual int Start() override;
            virtual int Stop() override;

            virtual int Fresh() const override;
            virtual bool Readable() const override;
            virtual bool Writable() const override;
            virtual std::string Status()  const override;

            virtual int SendWebMsg(const std::string&) override;
            virtual int RecvWebMsg(std::string&, int&) override;

        private:
            bool initialzed_ = false;
            //tcp io related
            std::string native_ip_ = "";
            int native_port_ = 0;
            int protocol_ = 0;

            struct sockaddr_in server_addr_ = {};
            struct sockaddr_in client_addr_ = {};

            int native_server_sock_ = -1;
            int remote_nginx_sock_ = -1;

            std::string web_raw_data_ = "";
            int web_raw_len_ = 0;

            std::mutex rd_mutex_;
            std::mutex wr_mutex_;
    };

} //namespace visualization
} //namespace fringe
} //namespace venus
