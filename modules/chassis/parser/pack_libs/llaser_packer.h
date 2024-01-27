#pragma once

#include "cyber/common/log.h"

#include "modules/chassis/parser/packer_base_itf.h"

namespace mstf {
namespace chss {
namespace parser {

    class LlaserPacker : public PackerBaseItf{
        public:
            LlaserPacker(const std::string&);
            virtual ~LlaserPacker();

            const std::vector<uint8_t>
            PackLlaserRawSetting(const
                    LineLaserSetting&) override;

        private:
    };

} //namespace parser
} //namespace chss
} //namespace mstf
