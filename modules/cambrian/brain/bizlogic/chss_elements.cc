#include "modules/cambrian/brain/bizlogic/chss_elements.h"

namespace mstf {
namespace camb {
namespace brain {

    ChssElements::ChssElements() {
#ifdef CAMB_PKG_DBG
        AINFO << "ChssElements construct";
#endif
        elm_chs_ctrl_ = std::make_shared<MiscChassisCtrl>();
    }

    ChssElements::~ChssElements() {
#ifdef CAMB_PKG_DBG
        AINFO << "ChssElements de-construct";
#endif
    }

    void ChssElements::RegisterBtNodes
        (BehaviorTreeFactory* bf) {
#ifdef CAMB_PKG_DBG
        AINFO << "register the element nodes";
#endif
        bf->registerSimpleAction("ElmChsCheckOnline",
                std::bind(&ChssElements::ElmChsCheckOnline, this));
    }

    NodeStatus ChssElements::ElmChsCheckOnline() {
#ifdef CAMB_PKG_DBG
        AINFO << "chassis online check";
#endif
        return NodeStatus::SUCCESS;
    }

} //namespace brain
} //namespace camb
} //namespace mstf
