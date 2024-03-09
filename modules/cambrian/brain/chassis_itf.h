#pragma once

namespace mstf {
namespace camb {
namespace brain {

    class ChassisItf {
        ///1, Liquid info
        virtual void CleanTankName(const std::string&) {}
        virtual void CleanTankStat(const bool) {}
        virtual void CleanTankLevel(const uint32_t) {}
        virtual void CleanTankEmpty(const uint32_t) {}
        virtual void CleanTankLow(const uint32_t) {}
        virtual void CleanTankFull(const uint32_t) {}
        virtual void CleanTankUpdataFreqms(const uint32_t) {}

        virtual void DirtyTankName(const std::string&) {}
        virtual void DirtyTankStat(const bool) {}
        virtual void DirtyTankLevel(const uint32_t) {}
        virtual void DirtyTankEmpty(const uint32_t) {}
        virtual void DirtyTankLow(const uint32_t) {}
        virtual void DirtyTankFull(const uint32_t) {}
        virtual void DirtyTankUpdateFreqms(const uint32_t) {}

        virtual void DustBoxName(const std::string&) {}
        virtual void DustBoxStat(const bool) {}
        virtual void DustBoxLevel(const uint32_t) {}
        virtual void DustBoxEmpty(const uint32_t) {}
        virtual void DustBoxLow(const uint32_t) {}
        virtual void DustBoxFull(const uint32_t) {}
        virtual void DustBoxUpdataFreqms(const uint32_t) {}

        virtual void DetergentTankName(const std::string&) {}
        virtual void DetergentTankStat(const bool) {}
        virtual void DetergentTankLevel(const uint32_t) {}
        virtual void DetergentTankEmpty(const uint32_t) {}
        virtual void DetergentTankLow(const uint32_t) {}
        virtual void DetergentTankFull(const uint32_t) {}
        virtual void DetergentTankUpdataFreqms(const uint32_t) {}

        ///2, ADC info
        virtual void DiscBrushLeftName(const std::string&) {}
        virtual void DiscBrushLeftStat(const bool) {}
        virtual void DiscBrushLeftVoltmv(const uint32_t) {}
        virtual void DiscBrushLeftCurrentma(const uint32_t) {}
        virtual void DiscBrushLeftTemperature(const uint32_t) {}
        virtual void DiscBrushLeftUpdateFreqms(const uint32_t) {}

        virtual void DiscBrushRightName(const std::string&) {}
        virtual void DiscBrushRightStat(const bool) {}
        virtual void DiscBrushRightVoltmv(const uint32_t) {}
        virtual void DiscBrushRightCurrentma(const uint32_t) {}
        virtual void DiscBrushRightTemperature(const uint32_t) {}
        virtual void DiscBrushRightUpdateFreqms(const uint32_t) {}

        virtual void RollBrushFrontName(const std::string&) {}
        virtual void RollBrushFrontStat(const bool) {}
        virtual void RollBrushFrontVoltmv(const uint32_t) {}
        virtual void RollBrushFrontCurrentma(const uint32_t) {}
        virtual void RollBrushFrontTemperature(const uint32_t) {}
        virtual void RollBrushFrontUpdateFreqms(const uint32_t) {}

        virtual void RollBrushRearName(const std::string&) {}
        virtual void RollBrushRearStat(const bool) {}
        virtual void RollBrushRearVoltmv(const uint32_t) {}
        virtual void RollBrushRearCurrentma(const uint32_t) {}
        virtual void RollBrushRearTemperature(const uint32_t) {}
        virtual void RollBrushRearUpdateFreqms(const uint32_t) {}

        virtual void SuckFanName(const std::string&) {}
        virtual void SuckFanStat(const bool) {}
        virtual void SuckFanVoltmv(const uint32_t) {}
        virtual void SuckFanCurrentma(const uint32_t) {}
        virtual void SuckFanTemperature(const uint32_t) {}
        virtual void SuckFanUpdateFreqms(const uint32_t) {}

        virtual void WheelLeftName(const std::string&) {}
        virtual void WheelLeftStat(const bool) {}
        virtual void WheelLeftVoltmv(const uint32_t) {}
        virtual void WheelLeftCurrentma(const uint32_t) {}
        virtual void WheelLeftTemperature(const uint32_t) {}
        virtual void WheelLeftUpdateFreqms(const uint32_t) {}

        virtual void WheelRightName(const std::string&) {}
        virtual void WheelRightStat(const bool) {}
        virtual void WheelRightVoltmv(const uint32_t) {}
        virtual void WheelRightCurrentma(const uint32_t) {}
        virtual void WheelRightTemperature(const uint32_t) {}
        virtual void WheelRightUpdateFreqms(const uint32_t) {}

        virtual void BatteryName(const std::string&) {}
        virtual void BatteryStat(const bool) {}
        virtual void BatteryVoltmv(const uint32_t) {}
        virtual void BatteryCurrentma(const uint32_t) {}
        virtual void BatteryTemperature(const uint32_t) {}
        virtual void BatteryUpdateFreqms(const uint32_t) {}

        ///3, PWM info
        virtual void PwmDiscBrushLeftName(const std::string&) {}
        virtual void PwmDiscBrushLeftStat(const bool) {}
        virtual void PwmDiscBrushLeftDuty(const uint32_t) {}
        virtual void PwmDiscBrushLeftCyclens(const uint32_t) {}
        virtual void PwmDiscBrushLeftPositivens(const uint32_t) {}
        virtual void PwmDiscBrushLeftUpdateFreqms(const uint32_t) {}

        virtual void PwmDiscBrushRightName(const std::string&) {}
        virtual void PwmDiscBrushRightStat(const bool) {}
        virtual void PwmDiscBrushRightDuty(const uint32_t) {}
        virtual void PwmDiscBrushRightCyclens(const uint32_t) {}
        virtual void PwmDiscBrushRightPositivens(const uint32_t) {}
        virtual void PwmDiscBrushRightUpdateFreqms(const uint32_t) {}

        virtual void PwmRollBrushFrontName(const std::string&) {}
        virtual void PwmRollBrushFrontStat(const bool) {}
        virtual void PwmRollBrushFrontDuty(const uint32_t) {}
        virtual void PwmRollBrushFrontCyclens(const uint32_t) {}
        virtual void PwmRollBrushFrontPositivens(const uint32_t) {}
        virtual void PwmRollBrushFrontUpdateFreqms(const uint32_t) {}

        virtual void PwmRollBrushRearName(const std::string&) {}
        virtual void PwmRollBrushRearStat(const bool) {}
        virtual void PwmRollBrushRearDuty(const uint32_t) {}
        virtual void PwmRollBrushRearCyclens(const uint32_t) {}
        virtual void PwmRollBrushRearPositivens(const uint32_t) {}
        virtual void PwmRollBrushRearUpdateFreqms(const uint32_t) {}

        virtual void PwmSuckFanName(const std::string&) {}
        virtual void PwmSuckFanStat(const bool) {}
        virtual void PwmSuckFanDuty(const uint32_t) {}
        virtual void PwmSuckFanCyclens(const uint32_t) {}
        virtual void PwmSuckFanPositivens(const uint32_t) {}
        virtual void PwmSuckFanUpdateFreqms(const uint32_t) {}

        virtual void PwmBatteryName(const std::string&) {}
        virtual void PwmBatteryStat(const bool) {}
        virtual void PwmBatteryDuty(const uint32_t) {}
        virtual void PwmBatteryCyclens(const uint32_t) {}
        virtual void PwmBatteryPositivens(const uint32_t) {}
        virtual void PwmBatteryUpdateFreqms(const uint32_t) {}

        ///4, GPIO info
        virtual void GpioEmergencyStopName(const std::string&) {}
        virtual void GpioEmergencyStopStat(const bool) {}
        virtual void GpioEmergencyStopGear(const uint32_t) {}
        virtual void GpioEmergencyStopOnms(const uint32_t) {}
        virtual void GpioEmergencyStopOffms(const uint32_t) {}
        virtual void GpioEmergencyStopUpdateFreqms(const uint32_t) {}

        virtual void GpioEmergencyBreakName(const std::string&) {}
        virtual void GpioEmergencyBreakStat(const bool) {}
        virtual void GpioEmergencyBreakGear(const uint32_t) {}
        virtual void GpioEmergencyBreakOnms(const uint32_t) {}
        virtual void GpioEmergencyBreakOffms(const uint32_t) {}
        virtual void GpioEmergencyBreakUpdateFreqms(const uint32_t) {}

        virtual void GpioKeyName(const std::string&) {}
        virtual void GpioKeyStat(const bool) {}
        virtual void GpioKeyGear(const uint32_t) {}
        virtual void GpioKeyOnms(const uint32_t) {}
        virtual void GpioKeyOffms(const uint32_t) {}
        virtual void GpioKeyUpdateFreqms(const uint32_t) {}

        virtual void GpioGearName(const std::string&) {}
        virtual void GpioGearStat(const bool) {}
        virtual void GpioGearGear(const uint32_t) {}
        virtual void GpioGearOnms(const uint32_t) {}
        virtual void GpioGearOffms(const uint32_t) {}
        virtual void GpioGearUpdateFreqms(const uint32_t) {}

        virtual void GpioLogoLedName(const std::string&) {}
        virtual void GpioLogoLedStat(const bool) {}
        virtual void GpioLogoLedGear(const uint32_t) {}
        virtual void GpioLogoLedOnms(const uint32_t) {}
        virtual void GpioLogoLedOffms(const uint32_t) {}
        virtual void GpioLogoLedUpdateFreqms(const uint32_t) {}

        virtual void GpioTurnLeftLedName(const std::string&) {}
        virtual void GpioTurnLeftLedStat(const bool) {}
        virtual void GpioTurnLeftLedGear(const uint32_t) {}
        virtual void GpioTurnLeftLedOnms(const uint32_t) {}
        virtual void GpioTurnLeftLedOffms(const uint32_t) {}
        virtual void GpioTurnLeftLedUpdateFreqms(const uint32_t) {}

        virtual void GpioTurnRightLedName(const std::string&) {}
        virtual void GpioTurnRightLedStat(const bool) {}
        virtual void GpioTurnRightLedGear(const uint32_t) {}
        virtual void GpioTurnRightLedOnms(const uint32_t) {}
        virtual void GpioTurnRightLedOffms(const uint32_t) {}
        virtual void GpioTurnRightLedUpdateFreqms(const uint32_t) {}

        virtual void GpioHeadLightLedName(const std::string&) {}
        virtual void GpioHeadLightLedStat(const bool) {}
        virtual void GpioHeadLightLedGear(const uint32_t) {}
        virtual void GpioHeadLightLedOnms(const uint32_t) {}
        virtual void GpioHeadLightLedOffms(const uint32_t) {}
        virtual void GpioHeadLightLedUpdateFreqms(const uint32_t) {}

        virtual void GpioDrainValveName(const std::string&) {}
        virtual void GpioDrainValveStat(const bool) {}
        virtual void GpioDrainValveGear(const uint32_t) {}
        virtual void GpioDrainValveOnms(const uint32_t) {}
        virtual void GpioDrainValveOffms(const uint32_t) {}
        virtual void GpioDrainValveUpdateFreqms(const uint32_t) {}

        virtual void GpioFillValveName(const std::string&) {}
        virtual void GpioFillValveStat(const bool) {}
        virtual void GpioFillValveGear(const uint32_t) {}
        virtual void GpioFillValveOnms(const uint32_t) {}
        virtual void GpioFillValveOffms(const uint32_t) {}
        virtual void GpioFillValveUpdateFreqms(const uint32_t) {}

        virtual void GpioSprayValveName(const std::string&) {}
        virtual void GpioSprayValveStat(const bool) {}
        virtual void GpioSprayValveGear(const uint32_t) {}
        virtual void GpioSprayValveOnms(const uint32_t) {}
        virtual void GpioSprayValveOffms(const uint32_t) {}
        virtual void GpioSprayValveUpdateFreqms(const uint32_t) {}

        virtual void GpioDetergentValveName(const std::string&) {}
        virtual void GpioDetergentValveStat(const bool) {}
        virtual void GpioDetergentValveGear(const uint32_t) {}
        virtual void GpioDetergentValveOnms(const uint32_t) {}
        virtual void GpioDetergentValveOffms(const uint32_t) {}
        virtual void GpioDetergentValveUpdateFreqms(const uint32_t) {}

        virtual void GpioWaterScrapeName(const std::string&) {}
        virtual void GpioWaterScrapeStat(const bool) {}
        virtual void GpioWaterScrapeGear(const uint32_t) {}
        virtual void GpioWaterScrapeOnms(const uint32_t) {}
        virtual void GpioWaterScrapeOffms(const uint32_t) {}
        virtual void GpioWaterScrapeUpdateFreqms(const uint32_t) {}

        virtual void GpioBumpName(const std::string&) {}
        virtual void GpioBumpStat(const bool) {}
        virtual void GpioBumpGear(const uint32_t) {}
        virtual void GpioBumpOnms(const uint32_t) {}
        virtual void GpioBumpOffms(const uint32_t) {}
        virtual void GpioBumpUpdateFreqms(const uint32_t) {}

        ///5, INFRA info
        virtual void InfraDropName(const std::string&) {}
        virtual void InfraDropStat(const bool) {}
        virtual void InfraDropValue(const uint32_t) {}
        virtual void InfraDropUpdateFreqms(const uint32_t) {}

        ///6, SOC info
        virtual void SocCameraDev(const uint32_t, const std::string&) {}
        virtual void SocCameraStat(const uint32_t, const bool) {}

        virtual void SocWifiMode(const std::string&) {}
        virtual void SocWifiIp(const std::string&) {}
        virtual void SocWifiMac(const std::string&) {}
        virtual void SocWifiDns(const std::string&) {}
        virtual void SocWifiInfo(const std::string&) {}
        virtual void SocWifiName(const std::string&) {}
        virtual void SocWifiBssid(const std::string&) {}
        virtual void SocWifiStat(const std::string&) {}
        virtual void SocWifiScanList(const std::string&) {}
        virtual void SocWifiIsOnline(const std::string&) {}
        virtual void SocWifiPingms(const uint32_t) {}
        virtual void SocWifiResult(const std::string&) {}

        virtual void Soc4GName(const std::string&) {}
        virtual void Soc4GAccount(const uint32_t) {}

        virtual void SocAudioName(const std::string&) {}
        virtual void SocAudioPack(const std::string&) {}
        virtual void SocAudioVolume(const uint32_t) {}
        virtual void SocAudioMuteSwitch(const bool) {}

        virtual void SocLidarName(const std::string&) {}
        virtual void SocLidarStat(const bool) {}
        virtual void SocLidarFreq(const uint32_t) {}

        virtual void SocImuName(const std::string&) {}
        virtual void SocImuStat(const bool) {}
        virtual void SocImuFreq(const uint32_t) {}

        virtual void SocLlaserName(const std::string&) {}
        virtual void SocLlaserStat(const bool) {}
        virtual void SocLlaserFreq(const uint32_t) {}

        virtual void SocSsonicName(const std::string&) {}
        virtual void SocSsonicStat(const bool) {}
        virtual void SocSsonicFreq(const uint32_t) {}

        virtual void SocTofName(const std::string&) {}
        virtual void SocTofStat(const bool) {}
        virtual void SocTofFreq(const uint32_t) {}

        virtual void SocJsName(const std::string&) {}
        virtual void SocJsStat(const bool) {}
        virtual void SocJsFreq(const uint32_t) {}
    };

} //namespace brain
} //namespace camb
} //namespace mstf
