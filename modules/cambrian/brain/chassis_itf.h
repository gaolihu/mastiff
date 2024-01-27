#pragma once

namespace mstf {
namespace camb {
namespace brain {

    class ChassisItf {
        //toggle button
        virtual void SetToggleButton(const bool) = 0;

        //keys
        virtual void SetShortKeyPower(const bool) = 0;
        virtual void SetShortKeyHome(const bool) = 0;
        virtual void SetLongKeyPower(const bool) = 0;
        virtual void SetLongKeyHome(const bool) = 0;

        //water status
        virtual void SetNoWater(const bool) = 0;
        virtual void SetFullWater(const bool) = 0;

        //dust box
        virtual void SetDustBox(const bool) = 0;

        //water tank
        virtual void SetWaterTank(const bool) = 0;

        //pallet
        virtual void SetLeftPallet(const bool) = 0;
        virtual void SetRightPallet(const bool) = 0;

        //bump
        virtual void SetLeftBump(const bool) = 0;
        virtual void SetRightBump(const bool) = 0;
        virtual void SetLidarLeftBump(const bool) = 0;
        virtual void SetLidarRightBump(const bool) = 0;
        virtual void SetLidarFrontBump(const bool) = 0;
        virtual void SetLidarTopBump(const bool) = 0;

        //wheel lift off ground
        virtual void SetWheelLeftLift(const bool) = 0;
        virtual void SetWheelRightLift(const bool) = 0;

        //along wall signal
        virtual void SetAlongWallTrig(const bool) = 0;

        //wheel pulse
        virtual void SetWheelLeftPulse(const uint32_t) = 0;
        virtual void SetWheelRightPulse(const uint32_t) = 0;

        //speed
        virtual void SetWheelLeftSpeed(const float) = 0;
        virtual void SetWheelRightSpeed(const float) = 0;

        //drop
        virtual void SetDropLeft(const bool) = 0;
        virtual void SetDropMidLeft(const bool) = 0;
        virtual void SetDropMidRight(const bool) = 0;
        virtual void SetDropRight(const bool) = 0;
        virtual void SetDropLeftRear(const bool) = 0;
        virtual void SetDropRightRear(const bool) = 0;

        virtual void SetDropLeftAdc(const uint32_t) = 0;
        virtual void SetDropMidLeftAdc(const uint32_t) = 0;
        virtual void SetDropMidRightAdc(const uint32_t) = 0;
        virtual void SetDropRightAdc(const uint32_t) = 0;
        virtual void SetDropLeftRearAdc(const uint32_t) = 0;
        virtual void SetDropRightRearAdc(const uint32_t) = 0;

        //charge
        virtual void SetChargeTouchDock(const bool) = 0;
        virtual void SetChargeOnDock(const bool) = 0;
        virtual void SetChargeFinished(const bool) = 0;

        //mid-scan
        virtual void SetMidScanDetect(const bool) = 0;

        //low power & dock
        virtual void SetLowPowerDocking(const bool) = 0;
        virtual void SetLowPowerPoweroff(const bool) = 0;

        //wheel current
        virtual void SetWheelLeftCurrent(const float) = 0;
        virtual void SetWheelRightCurrent(const float) = 0;

        //tof distance in mm
        virtual void SetTofDistance(uint32_t) = 0;

        //dock signal
        virtual void SetDockLeftSignal(uint32_t) = 0;
        virtual void SetDockMidLeftSignal(uint32_t) = 0;
        virtual void SetDockMidRightSignal(uint32_t) = 0;
        virtual void SetDockRightFrontSignal(uint32_t) = 0;
        virtual void SetDockRightSignal(uint32_t) = 0;
        virtual void SetDockLeftFrontSignal(uint32_t) = 0;
        virtual void SetDockRearMidLeftSignal(uint32_t) = 0;
        virtual void SetDockRearMidRightSignal(uint32_t) = 0;

        //battery
        virtual void SetBatteryLevel(const uint32_t) = 0;
        virtual void SetBatteryTemp(const uint32_t) = 0;

        //mcu online
        virtual void SetOnlineResult(const uint32_t) = 0;
        virtual void SetOnlineVersion(const uint32_t) = 0;
        virtual void SetOnlineHeartBeat(const uint32_t) = 0;

        //mcu info
        virtual void SetMcuSn(const std::string) = 0;
        virtual void SetMcuVersion(const std::string) = 0;
        virtual void SetMcuCompileTime(const std::string) = 0;

        virtual void SetMcuSensorEnStat(const bool, const bool,
                const bool, const bool, const bool, const bool,
                const bool, const bool, const bool, const bool,
                const bool, const bool) = 0;

        //ctrl result
        virtual void SetCtrlSlaveResult(const uint32_t, const uint32_t) = 0;

        //flip camera result
        virtual void SetFlipCameraStatus(const uint32_t) = 0;

        //error code
        virtual void SetSlaveErrorCode(const uint32_t) = 0;

        //slave time
        virtual void SetSlaveSysTime(const uint32_t) = 0;
    };

} //namespace brain
} //namespace camb
} //namespace mstf
