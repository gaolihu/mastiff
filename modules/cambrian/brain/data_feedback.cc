#include "modules/cambrian/brain/data_feedback.h"

namespace mstf {
namespace camb {
namespace brain {

    DataFeedback::DataFeedback() {
#ifdef CAMB_PKG_DBG
        AINFO << "DataFeedback construct";
#endif
    }

    DataFeedback::~DataFeedback() {
#ifdef CAMB_PKG_DBG
        AINFO << "DataFeedback de-construct";
#endif
    }

} //namespace brain
} //namespace camb
} //namespace mstf
