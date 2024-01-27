#pragma once

#include "modules/cambrian/brain/bizlogic/bizness_client.h"

namespace mstf {
namespace camb {
namespace brain {

    class BizlogicItf {
        public:
            BizlogicItf() {
#ifdef CAMB_PKG_DBG
                AINFO << "BizlogicItf construct";
#endif
                bz_client_ = std::make_unique<BiznessClient>();
            }
            virtual ~BizlogicItf() {
#ifdef CAMB_PKG_DBG
                AINFO << "BizlogicItf de-construct";
#endif
            }

        private:
            //bizlogic client
            std::unique_ptr<BiznessClient> bz_client_;
    };

} //namespace brain
} //namespace camb
} //namespace mstf
