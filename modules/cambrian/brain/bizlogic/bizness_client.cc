#include "modules/cambrian/brain/bizlogic/bizness_client.h"

namespace mstf {
namespace camb {
namespace brain {

    BiznessClient::BiznessClient() {
#ifdef CAMB_PKG_DBG
        AINFO << "BiznessClient construct ~ 1";
#endif
    }

    BiznessClient::~BiznessClient() {
#ifdef CAMB_PKG_DBG
        AINFO << "BiznessClient de-construct";
#endif
    }

} //namespace brain
} //namespace camb
} //namespace mstf
