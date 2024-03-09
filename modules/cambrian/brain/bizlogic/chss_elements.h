#pragma once

#include "modules/cambrian/brain/bizlogic/element_itf.h"

namespace mstf {
namespace camb {
namespace brain {

    class ChssElements : public ElementItf {
        public:
            ChssElements();
            ChssElements& operator=(const ChssElements&) = delete;
            ChssElements(const ChssElements&) = delete;
            virtual ~ChssElements();

            virtual void RegisterBtNodes(BehaviorTreeFactory*) override;

            //override APIs
            //virtual NodeStatus ElmClearCtrl() override;
            //virtual NodeStatus ElmReleaseCtrl() override;

            //chassis ITFs
            virtual NodeStatus ElmChsCheckOnline() override;

        protected:
            std::shared_ptr<MiscChassisCtrl> elm_chs_ctrl_ = nullptr;
    };


} //namespace brain
} //namespace camb
} //namespace mstf
