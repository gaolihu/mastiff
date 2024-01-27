#pragma once

#include "modules/chassis/parser/parser_base_itf.h"

namespace mstf {
namespace chss {
namespace parser {

    class WirelessParser : public ParserBaseItf {
        public:
            WirelessParser(const SensorIndicator&);
            virtual  ~WirelessParser() final;

            virtual int Init() override;
            virtual int Start() override;
            virtual int Stop() override;
            virtual int Resume() override;
            virtual int Close() override;


        private:
            virtual int ParseSocMsg(const
                    SensorIndicator*, const Message&) override;
    };

} //namespace parser
} //namespace chss
} //namespace mstf
