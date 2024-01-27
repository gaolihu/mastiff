#pragma once

#include <iomanip>

#include "modules/chassis/parser/packer_base_itf.h"
#include "modules/chassis/parser/parse_libs/ds20270da_driver.h"

namespace mstf {
namespace chss {
namespace parser {

    class ServoPacker : public PackerBaseItf{
        public:
            ServoPacker(const std::string&);
            virtual ~ServoPacker();

            virtual std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>
                        PackServoMtrControl(const
                                ChsMovementCtrl&) override;

            virtual std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>
                        PackServoMtrSetting(const
                                ServoSetting&) override;

        private:
#if 0
            std::vector<std::tuple<const int,
                const std::vector<uint8_t>>>
                    PackMotorDiffSpeedDouble(const
                            ChsMovementCtrl&);
#endif

            std::tuple<const int,
                const std::vector<uint8_t>>
                    PackMotorDiffSpeed(const
                            WheelDiffSpd&);

            std::tuple<const int,
                const std::vector<uint8_t>>
                    PackMotorWheelReverse(const bool);

            std::tuple<const int,
                const std::vector<uint8_t>>
                    PackMotorWheelSpeed(const
                            WheelDiffSpd&);

            const std::vector<uint8_t>
                PackServoMtrRaw(const
                        WheelDiffSpd&);

            void ConvertDiffSpeed2Wheel(const
                    WheelDiffSpd&, WheelSpeed&);
            void ConvertDiffSpeed2Rpm(const
                    WheelDiffSpd&, WheelRpm&);
            void ConvertWheelSpeed2Rpm(const
                    WheelSpeed&, WheelRpm&);

            /////////////////////////////////////////////
            //double reading registers
            bool FillDataContainerFreeRead(
                    std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td,
                    const std::string& des,
                    const int16_t* addr,
                    const int* value,
                    const int size);
            //double write registers
            bool FillDataContainerFreeWrite(
                    std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td,
                    const std::string& des,
                    const int16_t* addr,
                    const int* value,
                    const int size);
            bool FillDataContainerCanOpenConfig(
                    std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td,
                    const uint8_t* cmd,
                    const int16_t* addr,
                    const uint8_t* sub_id,
                    const int* value,
                    const int size);
            void FillDataContainerCanOpenActivate(
                    std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td);
            void FillDataContainerCanOpenStartReporting(
                    std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td);
            void FillDataContainerCanOpenStopReporting(
                    std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td);
            void FillDataContainerCanOpenSync(
                    std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td);
            void FillDataContainerCanOpenResetStatus(
                    std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td);
            void FillDataContainerCanOpenAxleSpeed(
                    std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td,
                    const int32_t left,
                    const int32_t right);
            void FillDataContainerCanOpenAxleEnable(
                    std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td);
            void FillDataContainerCanOpenAxleDisable(
                    std::vector<std::tuple<const int,
                    const std::vector<uint8_t>>>& td);
    };

} //namespace parser
} //namespace chss
} //namespace mstf
