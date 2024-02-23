#pragma once

#include "modules/chassis/parser/parser_base_itf.h"

namespace mstf {
namespace chss {
namespace parser {

    class AudioParser : public ParserBaseItf {

        public:
            AudioParser(const SensorIndicator&);
            virtual  ~AudioParser() final;

            virtual int Init() override;
            virtual int Start() override;
            virtual int Stop() override;
            virtual int Resume() override;
            virtual int Close() override;

            virtual int WriteSocMessage(const Message&) override;
    };

} //namespace parser
} //namespace chss
} //namespace mstf
