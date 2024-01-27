#pragma once

#include "modules/chassis/devices/transactor.h"
#include "modules/chassis/devices/msgs_impl_itf.h"

namespace mstf {
namespace chss {
namespace device {

    using namespace apollo;
    using namespace /*mstf::chss::*/proto;

    template <typename MessageT>
    class MsgsSendTmpl : public MsgsImplItf {
        public:
            MsgsSendTmpl(MsgsSendTmpl&) = delete;
            MsgsSendTmpl(const MsgsSendTmpl&) = delete;
            MsgsSendTmpl& operator=(const MsgsSendTmpl&) = delete;

            explicit MsgsSendTmpl<MessageT>(
                    std::function<
                    std::shared_ptr<cyber::Writer<MessageT>>()
                    > w) {
#ifdef CHSS_PKG_DBG
                AINFO << "tmpl send " <<
                    cyber::message::GetMessageName<MessageT>();
#endif
                writer_gen_ = w;

                Transactor::Instance()->
                    RegisterPublishers<MessageT>(std::bind(
                                &MsgsSendTmpl::MessagePublish, this,
                                std::placeholders::_1));
            }

            virtual ~MsgsSendTmpl() final {
#ifdef CHSS_PKG_DBG
                AINFO << "MsgsSendTmpl de-construct \"" <<
                    cyber::message::GetMessageName<MessageT>() << "\"";
#endif
            }

            virtual void MsgsFlowoutControl(bool sw) {
                if (sw)
                    GenerateMsgWriter();
                else
                    DestroyMsgWriter();
            }

            inline int MessagePublish(const
                    std::shared_ptr<Message>& msg) override {
#ifdef CHSS_PKG_DBG
                AINFO << "[" << this <<
                    "] publishing msg: \"" <<
                    cyber::message::GetMessageName<MessageT>() << "\"";
#endif
                if (writable_)
                    return writer_->Write(std::
                            dynamic_pointer_cast<MessageT>(msg));

                AWARN << "not writable!";

                return -1;
            }

        private:
            inline void GenerateMsgWriter() {
#ifdef CHSS_PKG_DBG
                AINFO << "generate writer \"" <<
                    cyber::message::GetMessageName<MessageT>() << "\"";
#endif
                writer_ = writer_gen_();
                writable_ = true;
            }

            inline void DestroyMsgWriter() {
#ifdef CHSS_PKG_DBG
                AINFO << "destroy writer \"" <<
                    cyber::message::GetMessageName<MessageT>() << "\"";
#endif
                writer_.reset();
                writable_ = false;
            }

        private:
            std::function<std::shared_ptr<cyber::Writer<MessageT>>()> writer_gen_ {};
            std::shared_ptr<cyber::Writer<MessageT>> writer_ {};

            bool writable_ { false };
    };

} //namespace device
} //namespace chss
} //namespace mstf
