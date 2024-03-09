#pragma once

#include "cyber/cyber.h"
#include "cyber/common/log.h"
#include "cyber/common/util.h"

#include "modules/cambrian/brain/implement_hub.h"
#include "modules/cambrian/visualize/visual_client.h"
#include "modules/cambrian/storage/dbase_client.h"

namespace mstf {
namespace camb {
namespace brain {

    using namespace apollo;
    using namespace google::protobuf;
    using namespace /*mstf::camb::*/proto;
    using namespace /*mstf::camb::*/dbase;
    using namespace /*mstf::camb::*/visual;

    class SysProcessor :
        public ImplementHub {

        public:
            SysProcessor(const
                    std::shared_ptr<CambrianConfig>&);
            virtual ~SysProcessor() final;

            void Init();
            void Start();
            void Stop();
            void Resume();
            void Close();

            template <typename MessageT>
            inline int PushInMessage(const
                    std::shared_ptr<MessageT>& msg) {
                return GetMessagePool()->
                    EnqueueMessage<MessageT>(msg);
            }

        private:
            void ParseChssMisc();
            void UpdateSystemLoad();

            //////////////////From Chassis///////////////////
            ///1, Liquid info
            virtual void CleanTankName(const std::string&) override;
            virtual void CleanTankStat(const bool) override;
            virtual void CleanTankLevel(const uint32_t) override;
            virtual void CleanTankEmpty(const uint32_t) override;
            virtual void CleanTankLow(const uint32_t) override;
            virtual void CleanTankFull(const uint32_t) override;
            virtual void CleanTankUpdataFreqms(const uint32_t) override;

            virtual void DirtyTankName(const std::string&) override;
            virtual void DirtyTankStat(const bool) override;
            virtual void DirtyTankLevel(const uint32_t) override;
            virtual void DirtyTankEmpty(const uint32_t) override;
            virtual void DirtyTankLow(const uint32_t) override;
            virtual void DirtyTankFull(const uint32_t) override;
            virtual void DirtyTankUpdateFreqms(const uint32_t) override;

            virtual void DustBoxName(const std::string&) override;
            virtual void DustBoxStat(const bool) override;
            virtual void DustBoxLevel(const uint32_t) override;
            virtual void DustBoxEmpty(const uint32_t) override;
            virtual void DustBoxLow(const uint32_t) override;
            virtual void DustBoxFull(const uint32_t) override;
            virtual void DustBoxUpdataFreqms(const uint32_t) override;

            virtual void DetergentTankName(const std::string&) override;
            virtual void DetergentTankStat(const bool) override;
            virtual void DetergentTankLevel(const uint32_t) override;
            virtual void DetergentTankEmpty(const uint32_t) override;
            virtual void DetergentTankLow(const uint32_t) override;
            virtual void DetergentTankFull(const uint32_t) override;
            virtual void DetergentTankUpdataFreqms(const uint32_t) override;

            ///2, ADC info
            virtual void DiscBrushLeftName(const std::string&) override;
            virtual void DiscBrushLeftStat(const bool) override;
            virtual void DiscBrushLeftVoltmv(const uint32_t) override;
            virtual void DiscBrushLeftCurrentma(const uint32_t) override;
            virtual void DiscBrushLeftTemperature(const uint32_t) override;
            virtual void DiscBrushLeftUpdateFreqms(const uint32_t) override;

            virtual void DiscBrushRightName(const std::string&) override;
            virtual void DiscBrushRightStat(const bool) override;
            virtual void DiscBrushRightVoltmv(const uint32_t) override;
            virtual void DiscBrushRightCurrentma(const uint32_t) override;
            virtual void DiscBrushRightTemperature(const uint32_t) override;
            virtual void DiscBrushRightUpdateFreqms(const uint32_t) override;

            virtual void RollBrushFrontName(const std::string&) override;
            virtual void RollBrushFrontStat(const bool) override;
            virtual void RollBrushFrontVoltmv(const uint32_t) override;
            virtual void RollBrushFrontCurrentma(const uint32_t) override;
            virtual void RollBrushFrontTemperature(const uint32_t) override;
            virtual void RollBrushFrontUpdateFreqms(const uint32_t) override;

            virtual void RollBrushRearName(const std::string&) override;
            virtual void RollBrushRearStat(const bool) override;
            virtual void RollBrushRearVoltmv(const uint32_t) override;
            virtual void RollBrushRearCurrentma(const uint32_t) override;
            virtual void RollBrushRearTemperature(const uint32_t) override;
            virtual void RollBrushRearUpdateFreqms(const uint32_t) override;

            virtual void SuckFanName(const std::string&) override;
            virtual void SuckFanStat(const bool) override;
            virtual void SuckFanVoltmv(const uint32_t) override;
            virtual void SuckFanCurrentma(const uint32_t) override;
            virtual void SuckFanTemperature(const uint32_t) override;
            virtual void SuckFanUpdateFreqms(const uint32_t) override;

            virtual void WheelLeftName(const std::string&) override;
            virtual void WheelLeftStat(const bool) override;
            virtual void WheelLeftVoltmv(const uint32_t) override;
            virtual void WheelLeftCurrentma(const uint32_t) override;
            virtual void WheelLeftTemperature(const uint32_t) override;
            virtual void WheelLeftUpdateFreqms(const uint32_t) override;

            virtual void WheelRightName(const std::string&) override;
            virtual void WheelRightStat(const bool) override;
            virtual void WheelRightVoltmv(const uint32_t) override;
            virtual void WheelRightCurrentma(const uint32_t) override;
            virtual void WheelRightTemperature(const uint32_t) override;
            virtual void WheelRightUpdateFreqms(const uint32_t) override;

            virtual void BatteryName(const std::string&) override;
            virtual void BatteryStat(const bool) override;
            virtual void BatteryVoltmv(const uint32_t) override;
            virtual void BatteryCurrentma(const uint32_t) override;
            virtual void BatteryTemperature(const uint32_t) override;
            virtual void BatteryUpdateFreqms(const uint32_t) override;

            ///3, PWM info
            virtual void PwmDiscBrushLeftName(const std::string&) override;
            virtual void PwmDiscBrushLeftStat(const bool) override;
            virtual void PwmDiscBrushLeftDuty(const uint32_t) override;
            virtual void PwmDiscBrushLeftCyclens(const uint32_t) override;
            virtual void PwmDiscBrushLeftPositivens(const uint32_t) override;
            virtual void PwmDiscBrushLeftUpdateFreqms(const uint32_t) override;

            virtual void PwmDiscBrushRightName(const std::string&) override;
            virtual void PwmDiscBrushRightStat(const bool) override;
            virtual void PwmDiscBrushRightDuty(const uint32_t) override;
            virtual void PwmDiscBrushRightCyclens(const uint32_t) override;
            virtual void PwmDiscBrushRightPositivens(const uint32_t) override;
            virtual void PwmDiscBrushRightUpdateFreqms(const uint32_t) override;

            virtual void PwmRollBrushFrontName(const std::string&) override;
            virtual void PwmRollBrushFrontStat(const bool) override;
            virtual void PwmRollBrushFrontDuty(const uint32_t) override;
            virtual void PwmRollBrushFrontCyclens(const uint32_t) override;
            virtual void PwmRollBrushFrontPositivens(const uint32_t) override;
            virtual void PwmRollBrushFrontUpdateFreqms(const uint32_t) override;

            virtual void PwmRollBrushRearName(const std::string&) override;
            virtual void PwmRollBrushRearStat(const bool) override;
            virtual void PwmRollBrushRearDuty(const uint32_t) override;
            virtual void PwmRollBrushRearCyclens(const uint32_t) override;
            virtual void PwmRollBrushRearPositivens(const uint32_t) override;
            virtual void PwmRollBrushRearUpdateFreqms(const uint32_t) override;

            virtual void PwmSuckFanName(const std::string&) override;
            virtual void PwmSuckFanStat(const bool) override;
            virtual void PwmSuckFanDuty(const uint32_t) override;
            virtual void PwmSuckFanCyclens(const uint32_t) override;
            virtual void PwmSuckFanPositivens(const uint32_t) override;
            virtual void PwmSuckFanUpdateFreqms(const uint32_t) override;

            virtual void PwmBatteryName(const std::string&) override;
            virtual void PwmBatteryStat(const bool) override;
            virtual void PwmBatteryDuty(const uint32_t) override;
            virtual void PwmBatteryCyclens(const uint32_t) override;
            virtual void PwmBatteryPositivens(const uint32_t) override;
            virtual void PwmBatteryUpdateFreqms(const uint32_t) override;

            ///4, GPIO info
            virtual void GpioEmergencyStopName(const std::string&) override;
            virtual void GpioEmergencyStopStat(const bool) override;
            virtual void GpioEmergencyStopGear(const uint32_t) override;
            virtual void GpioEmergencyStopOnms(const uint32_t) override;
            virtual void GpioEmergencyStopOffms(const uint32_t) override;
            virtual void GpioEmergencyStopUpdateFreqms(const uint32_t) override;

            virtual void GpioEmergencyBreakName(const std::string&) override;
            virtual void GpioEmergencyBreakStat(const bool) override;
            virtual void GpioEmergencyBreakGear(const uint32_t) override;
            virtual void GpioEmergencyBreakOnms(const uint32_t) override;
            virtual void GpioEmergencyBreakOffms(const uint32_t) override;
            virtual void GpioEmergencyBreakUpdateFreqms(const uint32_t) override;

            virtual void GpioKeyName(const std::string&) override;
            virtual void GpioKeyStat(const bool) override;
            virtual void GpioKeyGear(const uint32_t) override;
            virtual void GpioKeyOnms(const uint32_t) override;
            virtual void GpioKeyOffms(const uint32_t) override;
            virtual void GpioKeyUpdateFreqms(const uint32_t) override;

            virtual void GpioGearName(const std::string&) override;
            virtual void GpioGearStat(const bool) override;
            virtual void GpioGearGear(const uint32_t) override;
            virtual void GpioGearOnms(const uint32_t) override;
            virtual void GpioGearOffms(const uint32_t) override;
            virtual void GpioGearUpdateFreqms(const uint32_t) override;

            virtual void GpioLogoLedName(const std::string&) override;
            virtual void GpioLogoLedStat(const bool) override;
            virtual void GpioLogoLedGear(const uint32_t) override;
            virtual void GpioLogoLedOnms(const uint32_t) override;
            virtual void GpioLogoLedOffms(const uint32_t) override;
            virtual void GpioLogoLedUpdateFreqms(const uint32_t) override;

            virtual void GpioTurnLeftLedName(const std::string&) override;
            virtual void GpioTurnLeftLedStat(const bool) override;
            virtual void GpioTurnLeftLedGear(const uint32_t) override;
            virtual void GpioTurnLeftLedOnms(const uint32_t) override;
            virtual void GpioTurnLeftLedOffms(const uint32_t) override;
            virtual void GpioTurnLeftLedUpdateFreqms(const uint32_t) override;

            virtual void GpioTurnRightLedName(const std::string&) override;
            virtual void GpioTurnRightLedStat(const bool) override;
            virtual void GpioTurnRightLedGear(const uint32_t) override;
            virtual void GpioTurnRightLedOnms(const uint32_t) override;
            virtual void GpioTurnRightLedOffms(const uint32_t) override;
            virtual void GpioTurnRightLedUpdateFreqms(const uint32_t) override;

            virtual void GpioHeadLightLedName(const std::string&) override;
            virtual void GpioHeadLightLedStat(const bool) override;
            virtual void GpioHeadLightLedGear(const uint32_t) override;
            virtual void GpioHeadLightLedOnms(const uint32_t) override;
            virtual void GpioHeadLightLedOffms(const uint32_t) override;
            virtual void GpioHeadLightLedUpdateFreqms(const uint32_t) override;

            virtual void GpioDrainValveName(const std::string&) override;
            virtual void GpioDrainValveStat(const bool) override;
            virtual void GpioDrainValveGear(const uint32_t) override;
            virtual void GpioDrainValveOnms(const uint32_t) override;
            virtual void GpioDrainValveOffms(const uint32_t) override;
            virtual void GpioDrainValveUpdateFreqms(const uint32_t) override;

            virtual void GpioFillValveName(const std::string&) override;
            virtual void GpioFillValveStat(const bool) override;
            virtual void GpioFillValveGear(const uint32_t) override;
            virtual void GpioFillValveOnms(const uint32_t) override;
            virtual void GpioFillValveOffms(const uint32_t) override;
            virtual void GpioFillValveUpdateFreqms(const uint32_t) override;

            virtual void GpioSprayValveName(const std::string&) override;
            virtual void GpioSprayValveStat(const bool) override;
            virtual void GpioSprayValveGear(const uint32_t) override;
            virtual void GpioSprayValveOnms(const uint32_t) override;
            virtual void GpioSprayValveOffms(const uint32_t) override;
            virtual void GpioSprayValveUpdateFreqms(const uint32_t) override;

            virtual void GpioDetergentValveName(const std::string&) override;
            virtual void GpioDetergentValveStat(const bool) override;
            virtual void GpioDetergentValveGear(const uint32_t) override;
            virtual void GpioDetergentValveOnms(const uint32_t) override;
            virtual void GpioDetergentValveOffms(const uint32_t) override;
            virtual void GpioDetergentValveUpdateFreqms(const uint32_t) override;

            virtual void GpioWaterScrapeName(const std::string&) override;
            virtual void GpioWaterScrapeStat(const bool) override;
            virtual void GpioWaterScrapeGear(const uint32_t) override;
            virtual void GpioWaterScrapeOnms(const uint32_t) override;
            virtual void GpioWaterScrapeOffms(const uint32_t) override;
            virtual void GpioWaterScrapeUpdateFreqms(const uint32_t) override;

            virtual void GpioBumpName(const std::string&) override;
            virtual void GpioBumpStat(const bool) override;
            virtual void GpioBumpGear(const uint32_t) override;
            virtual void GpioBumpOnms(const uint32_t) override;
            virtual void GpioBumpOffms(const uint32_t) override;
            virtual void GpioBumpUpdateFreqms(const uint32_t) override;

            ///5, INFRA info
            virtual void InfraDropName(const std::string&) override;
            virtual void InfraDropStat(const bool) override;
            virtual void InfraDropValue(const uint32_t) override;
            virtual void InfraDropUpdateFreqms(const uint32_t) override;

            ///6, SOC info
            virtual void SocCameraDev(const uint32_t, const std::string&) override;
            virtual void SocCameraStat(const uint32_t, const bool) override;

            virtual void SocWifiMode(const std::string&) override;
            virtual void SocWifiIp(const std::string&) override;
            virtual void SocWifiMac(const std::string&) override;
            virtual void SocWifiDns(const std::string&) override;
            virtual void SocWifiInfo(const std::string&) override;
            virtual void SocWifiName(const std::string&) override;
            virtual void SocWifiBssid(const std::string&) override;
            virtual void SocWifiStat(const std::string&) override;
            virtual void SocWifiScanList(const std::string&) override;
            virtual void SocWifiIsOnline(const std::string&) override;
            virtual void SocWifiPingms(const uint32_t) override;
            virtual void SocWifiResult(const std::string&) override;

            virtual void Soc4GName(const std::string&) override;
            virtual void Soc4GAccount(const uint32_t) override;

            virtual void SocAudioName(const std::string&) override;
            virtual void SocAudioPack(const std::string&) override;
            virtual void SocAudioVolume(const uint32_t) override;
            virtual void SocAudioMuteSwitch(const bool) override;

            virtual void SocLidarName(const std::string&) override;
            virtual void SocLidarStat(const bool) override;
            virtual void SocLidarFreq(const uint32_t) override;

            virtual void SocImuName(const std::string&) override;
            virtual void SocImuStat(const bool) override;
            virtual void SocImuFreq(const uint32_t) override;

            virtual void SocLlaserName(const std::string&) override;
            virtual void SocLlaserStat(const bool) override;
            virtual void SocLlaserFreq(const uint32_t) override;

            virtual void SocSsonicName(const std::string&) override;
            virtual void SocSsonicStat(const bool) override;
            virtual void SocSsonicFreq(const uint32_t) override;

            virtual void SocTofName(const std::string&) override;
            virtual void SocTofStat(const bool) override;
            virtual void SocTofFreq(const uint32_t) override;

            virtual void SocJsName(const std::string&) override;
            virtual void SocJsStat(const bool) override;
            virtual void SocJsFreq(const uint32_t) override;

        private:
            std::chrono::steady_clock::time_point time_whl_pulse_last_;
            uint32_t slave_sysms_last_ = 0;

            std::weak_ptr<CambrianConfig> cmb_conf_ { };

            //1, visualize; 2, database
            std::unique_ptr<VisualClient> visualize_;
            std::unique_ptr<DbaseClient> database_;
        };

} //namespace brain
} //namespace camb
} //namespace mstf
