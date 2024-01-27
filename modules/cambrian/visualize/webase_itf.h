#pragma once

#include <functional>

#include "modules/cambrian/visualize/remote_parse.h"

namespace mstf {
namespace camb {
namespace visual {

    class WebaseItf {
        public:
            WebaseItf() = default;
            virtual ~WebaseItf() = default;

            // common
            virtual int Init() { return 0; };
            virtual int Start() { return 0; };
            virtual int Stop() { return 0; };
            virtual int Close() { return 0; };

            // for socket IO
            virtual bool Fresh() const { return false; };
            virtual bool Readable() const { return false; };
            virtual bool Writable() const { return false; };
            virtual std::string Status() const { return web_status_; };
            virtual void SetStatus(const std::string& stat) { web_status_ = stat; };

            virtual int SendWebMsg(const std::string&) { return 0; };
            virtual int SendWebMsg(const void*, const int) { return 0; };
            virtual int RecvWebMsg(std::string&, int&) { return 0; };

            virtual int OnRecvTextWebMsg(const std::string& s) const {
                if (web_handle_)
                    return web_handle_(s);

                //WARN: no msg handle
                return -1;
            };

            virtual int OnRecvBinaryWebMsg(std::string& s) const {
                if (web_handle_)
                    return web_handle_(s);

                //WARN: no msg handle
                return -1;
            };

            virtual void SetWebStrHandle(const
                    RemoteStringHdl& hd) {
                web_handle_ = hd;
            };

            // for html parser
            // TODO

        private:
            std::string web_status_ = "";
            std::function<int(const std::string&)> web_handle_ = nullptr;
    };

} //namespace Visual
} //namespace camb
} //namespace mstf
