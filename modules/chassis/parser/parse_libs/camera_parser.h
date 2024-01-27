#pragma once

#include "modules/chassis/parser/parser_base_itf.h"

namespace mstf {
namespace chss {
namespace parser {

    class CameraParser : public ParserBaseItf {
        public:
            CameraParser(const SensorIndicator&);
            virtual  ~CameraParser() final;

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
