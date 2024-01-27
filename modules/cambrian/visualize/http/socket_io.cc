#include "fringe/visualization/nginx/socket_io.h"

namespace venus {
namespace fringe {
namespace visualization {

    SockectIo::SockectIo(const std::string ip, const int port, const int protocol) {
        AINFO << "SockectIo construct, IP: " << ip <<
            ", port: " << port;

        native_ip_ = ip;
        native_port_ = port;
        protocol_ = protocol;
    }

    SockectIo::~SockectIo() {
        AINFO << "SockectIo de-construct";
        Stop();
    }

    int SockectIo::Init() {
        memset(&server_addr_, 0, sizeof(server_addr_));
        server_addr_.sin_family = (protocol_ == 0 ? AF_INET : AF_UNIX);
        server_addr_.sin_addr.s_addr = inet_addr(native_ip_.c_str());
        server_addr_.sin_port = htons(native_port_);

        if (native_server_sock_ == -1) {
            if ((native_server_sock_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                AWARN << "server sock create error!";
                native_server_sock_ = -1;
                return -1;
            }
            AINFO << "server sock create OK!";
        }

        initialzed_ = true;

        return 0;
    }

    int SockectIo::Start() {
        AINFO << "nginx server start!";

        if (!initialzed_) {
            AWARN << "please init first of all!";
            return -1;
        }

        if (bind(native_server_sock_, (struct sockaddr *)&server_addr_,
                    sizeof(struct sockaddr)) < 0) {
            AWARN << "server sock bind error!";
            native_server_sock_ = -1;
            return -1;
        }

        AINFO << "ngx sock bind OK!";

        if ((listen(native_server_sock_, 10)) < 0) {
            AWARN << "server sock bind error!";
            native_server_sock_ = -1;
            return -1;
        }

        AINFO << "ngx sock listen OK!";

        return 0;
    }

    int SockectIo::Stop() {
        AINFO << "SockectIo stop!";

        if (native_server_sock_ != -1  ||
                native_server_sock_ == 0) {
            close(native_server_sock_);
            native_server_sock_ = -1;
        }

        initialzed_ = false;

        return 0;
    }

    int SockectIo::Fresh() const {
        return 0;
    }

    bool SockectIo::Readable() const {
        struct timeval tv = {.tv_sec = 1, .tv_usec = 0};

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(native_server_sock_, &fds);

        int ret = select(native_server_sock_ + 1,
                &fds, nullptr, nullptr, &tv);

        if (ret < 0) {
            AINFO << "nginx remote select error!";
            return false;
        } else if (ret == 0) {
            //AINFO << "nginx remote select timeout!";
            return false;
        } else if (FD_ISSET(native_server_sock_, &fds)) {
            //AINFO << "nginx remote select OK!";
            return true;
        }

        return false;
    }

    bool SockectIo::Writable() const {
        return true;
    }

    std::string SockectIo::Status()  const {
        return WebaseItf::Status();
    }

    int SockectIo::SendWebMsg(const std::string& str) {
        std::lock_guard<std::mutex> lg(wr_mutex_);

        if (native_server_sock_ > 0 && remote_nginx_sock_ > 0) {
            if ((write(remote_nginx_sock_, str.data(), str.size())) > 0) {
#ifdef CAMB_PKG_DBG
                AINFO << "send message success!, len: " <<
                    str.size() << ":\n" << str;
#endif
            } else {
                AWARN << "send message failed:\n" << str;
                return -1;
            }
        }
        return 0;
    }

    int SockectIo::RecvWebMsg(std::string& pot, int& len) {
        std::lock_guard<std::mutex> lg(rd_mutex_);

        pot = "";
        len = -1;

        char buffer[BUFSIZ] = {0};

        if (remote_nginx_sock_ == -1) {
#ifdef CAMB_PKG_DBG
            AINFO << "before accept, sock: " << remote_nginx_sock_;
#endif
            socklen_t sl = sizeof(client_addr_);
            remote_nginx_sock_ = accept(native_server_sock_,
                    (struct sockaddr *)&client_addr_, &sl);
#ifdef CAMB_PKG_DBG
            AINFO << "after accept, sock: " << remote_nginx_sock_;
#endif
        }

        if ((len = read(remote_nginx_sock_, buffer, BUFSIZ)) < 0) {
            AWARN << "recv data failed\n";
        } else if (len == 0) {
            close(remote_nginx_sock_);
            remote_nginx_sock_ = -1;
#ifdef CAMB_PKG_DBG
            AWARN << "recv remote disconncet, reset nginx sock to -1";
#endif
        } else {
            if (len < 30)
                return -1;

            web_raw_len_ = len;
            pot.assign(const_cast<char*>(buffer), len);
        }

        return 0;
    }

} //namespace visualization
} //namespace fringe
} //namespace venus
