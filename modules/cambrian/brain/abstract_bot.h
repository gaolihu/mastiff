#pragma once

#include "modules/cambrian/brain/bot_itf_impl.h"

namespace mstf {
namespace camb {
namespace brain {

    class AbstractBot : public BotItfImpl {
        public:
            AbstractBot(const std::shared_ptr<CambrianConfig>&);

            virtual ~AbstractBot() final;

            virtual void Init() override;
            virtual void Start() override;
            virtual void Resume() override;
            virtual void Stop() override;
            virtual void Close() override;

            //message import
            template <typename MessageT>
            int PushMessage(const std::shared_ptr<MessageT>&);
    };

    template <typename MessageT>
    int AbstractBot::PushMessage(const
            std::shared_ptr<MessageT>& msg) {
//#ifdef CAMB_PKG_DBG
#if 0
        AINFO << "AbstractBot robot push msg: " <<
            cyber::message::GetMessageName<MessageT>();
#endif

        return BotItfImpl::MsgsInpouring<MessageT>(msg);
    }

} //namespace brain
} //namespace camb
} //namespace mstf
