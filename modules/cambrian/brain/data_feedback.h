#pragma once

#include "cyber/common/log.h"

#include "modules/cambrian/proto/cambrian_data.pb.h"
#include "modules/chassis/proto/input_output_chs.pb.h"

namespace mstf {
namespace camb {
namespace brain {

    class DataFeedback {
        public:
            DataFeedback();
            virtual ~DataFeedback();

        private:
            //bizlogic client
    };

} //namespace brain
} //namespace camb
} //namespace mstf
