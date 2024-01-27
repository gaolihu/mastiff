#pragma once

#include "cyber/cyber.h"
#include "cyber/common/log.h"
#include "cyber/common/macros.h"

#include "modules/cambrian/brain/transactor.h"
#include "modules/cambrian/brain/msgs_impl_itf.h"

namespace mstf {
namespace camb {
namespace brain {

    using namespace apollo;
    using namespace /*mstf::camb::*/proto;

    template <typename MessageT>
    class MsgsRcvTmpl : public MsgsImplItf {
        public:
            MsgsRcvTmpl(MsgsRcvTmpl&) = delete;
            MsgsRcvTmpl(const MsgsRcvTmpl&) = delete;
            MsgsRcvTmpl& operator=(const MsgsRcvTmpl&) = delete;

            explicit MsgsRcvTmpl<MessageT>(
                    std::function<
                    std::shared_ptr<cyber::Reader<MessageT>>()
                    > r) {
#ifdef CAMB_PKG_DBG
                AINFO << "tmpl receive \"" <<
                    cyber::message::GetMessageName<MessageT>() << "\"";
#endif
                reader_gen_ = r;

                Transactor::Instance()->RegisterReceiver<MessageT>();
            }

            virtual ~MsgsRcvTmpl() final {
#ifdef CAMB_PKG_DBG
                AINFO << "MsgsRcvTmpl de-construct \"" <<
                    cyber::message::GetMessageName<MessageT>() << "\"";
#endif
            }

            virtual void MsgsFlowoutControl(bool sw) {
                if (sw)
                    GenerateMsgReader();
                else
                    DestroyMsgReader();
            }

            inline int MessageReceive(const
                    std::shared_ptr<Message>& msg) override {
#ifdef CAMB_PKG_DBG
                AINFO << "receiving msg: \"" <<
                    cyber::message::GetMessageName<MessageT>() << "\"";
#endif
                if (readable_) {
                    //foward messages
                    return Transactor::Instance()->
                        OnTransferMessageIn(msg);
                }

                AWARN << "not readable!";

                return -1;
            }

        private:
            inline void GenerateMsgReader() {
#ifdef CAMB_PKG_DBG
                AINFO << "generate reader \"" <<
                    cyber::message::GetMessageName<MessageT>() << "\"";
#endif
                reader_ = reader_gen_();
                readable_ = true;
            }

            inline void DestroyMsgReader() {
#ifdef CAMB_PKG_DBG
                AINFO << "destroy reader \"" <<
                    cyber::message::GetMessageName<MessageT>() << "\"";
#endif
                reader_.reset();
                readable_ = false;
            }

        private:
            std::function<std::shared_ptr<cyber::Reader<MessageT>>()> reader_gen_ {};
            std::shared_ptr<cyber::Reader<MessageT>> reader_ {};

            bool readable_ { false };
    };

} //namespace brain
} //namespace camb
} //namespace mstf
