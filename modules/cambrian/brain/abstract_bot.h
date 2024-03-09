#pragma once

#include "modules/cambrian/brain/bot_itf_impl.h"

namespace mstf {
namespace camb {
namespace brain {

    class AbstractBot : public BotItfImpl {
        public:
            AbstractBot(const std::shared_ptr<CambrianConfig>&);

            virtual ~AbstractBot() final;

            virtual void Init(const
                    BaseMsgCaller&) override;
            virtual void Start() override;
            virtual void Resume() override;
            virtual void Stop() override;
            virtual void Close() override;
    };

} //namespace brain
} //namespace camb
} //namespace mstf
