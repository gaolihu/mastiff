#include "modules/cambrian/brain/signals_kick.h"

namespace mstf {
namespace camb {
namespace brain {

    SignalsKick::SignalsKick() {
#ifdef CAMB_PKG_DBG
        AINFO << "SignalsKick construct";
#endif
    }

    SignalsKick::~SignalsKick() {
#ifdef CAMB_PKG_DBG
        AINFO << "SignalsKick de-construct";
#endif
    }

} //namespace brain
} //namespace camb
} //namespace mstf
