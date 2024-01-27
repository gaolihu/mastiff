#pragma once

#include "cyber/cyber.h"
#include "cyber/common/log.h"
#include "cyber/common/util.h"

#include "modules/cambrian/proto/cambrian_data.pb.h"
#include "modules/cambrian/proto/cambrian_arch.pb.h"

#include "modules/cambrian/brain/implement_hub.h"
#include "modules/cambrian/visualize/visual_client.h"
#include "modules/cambrian/storage/dbase_client.h"

namespace mstf {
namespace camb {
namespace brain {

    using namespace apollo;
    using namespace google::protobuf;
    using namespace /*mstf::camb::*/dbase;
    using namespace /*mstf::camb::*/visual;
    using namespace /*mstf::camb::*/proto;

    class SysProcessor :
        public ImplementHub {

        public:
            SysProcessor(//DatabaseNotifyHook,
                    const std::shared_ptr<CambrianConfig>& cc);
            virtual ~SysProcessor() final;

            void Init();
            void Start();
            void Stop();
            void Resume();
            void Close();
            void Loop(const int);

            void PushRandomEvents(const
                    EE_RANDOM_EVENTS_TRIGER evt,
                    const int val);

            //import data from database / infermachine
            void SetSystemInfoPtr(const std::shared_ptr<Message>&);
            void SetModeStatusPtr(const std::shared_ptr<Message>&);
            void SetVolatileInfoPtr(const std::shared_ptr<Message>&);
            void SetMultiZonesPtr(const std::shared_ptr<Message>&);

            //expose data to director
            void ExposeRandomEventsPtr();

            //set mcu params
            void ConfigChsParam(const McuSensorParamCtrl&);

        private:
            void UpdateSystemLoad();

            virtual void SetToggleButton(const bool) override;

            //keys
            virtual void SetShortKeyPower(const bool) override;
            virtual void SetShortKeyHome(const bool) override;
            virtual void SetLongKeyPower(const bool) override;
            virtual void SetLongKeyHome(const bool) override;

            //water status
            virtual void SetNoWater(const bool) override;
            virtual void SetFullWater(const bool) override;

            //dust box
            virtual void SetDustBox(const bool) override;

            //water tank
            virtual void SetWaterTank(const bool) override;

            //pallet
            virtual void SetLeftPallet(const bool) override;
            virtual void SetRightPallet(const bool) override;

            //bump
            virtual void SetLeftBump(const bool) override;
            virtual void SetRightBump(const bool) override;
            virtual void SetLidarLeftBump(const bool) override;
            virtual void SetLidarRightBump(const bool) override;
            virtual void SetLidarFrontBump(const bool) override;
            virtual void SetLidarTopBump(const bool) override;

            //wheel lift off ground
            virtual void SetWheelLeftLift(const bool) override;
            virtual void SetWheelRightLift(const bool) override;

            //along wall signal
            virtual void SetAlongWallTrig(const bool) override;

            //wheel pulse
            virtual void SetWheelLeftPulse(const uint32_t) override;
            virtual void SetWheelRightPulse(const uint32_t) override;

            //speed
            virtual void SetWheelLeftSpeed(const float) override;
            virtual void SetWheelRightSpeed(const float) override;

            //drop
            virtual void SetDropLeft(const bool) override;
            virtual void SetDropMidLeft(const bool) override;
            virtual void SetDropMidRight(const bool) override;
            virtual void SetDropRight(const bool) override;
            virtual void SetDropLeftRear(const bool) override;
            virtual void SetDropRightRear(const bool) override;

            virtual void SetDropLeftAdc(const uint32_t) override;
            virtual void SetDropMidLeftAdc(const uint32_t) override;
            virtual void SetDropMidRightAdc(const uint32_t) override;
            virtual void SetDropRightAdc(const uint32_t) override;
            virtual void SetDropLeftRearAdc(const uint32_t) override;
            virtual void SetDropRightRearAdc(const uint32_t) override;

            //charge
            virtual void SetChargeTouchDock(const bool) override;
            virtual void SetChargeOnDock(const bool) override;
            virtual void SetChargeFinished(const bool) override;

            //mid-scan
            virtual void SetMidScanDetect(const bool) override;

            //low power & dock
            virtual void SetLowPowerDocking(const bool) override;
            virtual void SetLowPowerPoweroff(const bool) override;

            //wheel current
            virtual void SetWheelLeftCurrent(const float) override;
            virtual void SetWheelRightCurrent(const float) override;

            //tof distance in mm
            virtual void SetTofDistance(uint32_t) override;

            //dock signal
            virtual void SetDockLeftSignal(uint32_t) override;
            virtual void SetDockMidLeftSignal(uint32_t) override;
            virtual void SetDockMidRightSignal(uint32_t) override;
            virtual void SetDockRightFrontSignal(uint32_t) override;
            virtual void SetDockRightSignal(uint32_t) override;
            virtual void SetDockLeftFrontSignal(uint32_t) override;
            virtual void SetDockRearMidLeftSignal(uint32_t) override;
            virtual void SetDockRearMidRightSignal(uint32_t) override;

            //battery
            virtual void SetBatteryLevel(const uint32_t) override;
            virtual void SetBatteryTemp(const uint32_t) override;

            //mcu online
            virtual void SetOnlineResult(const uint32_t) override;
            virtual void SetOnlineVersion(const uint32_t) override;
            virtual void SetOnlineHeartBeat(const uint32_t) override;

            //mcu info
            virtual void SetMcuSn(const std::string) override;
            virtual void SetMcuVersion(const std::string) override;
            virtual void SetMcuCompileTime(const std::string) override;

            //sensor enable stat
            virtual void SetMcuSensorEnStat(const bool, const bool,
                    const bool, const bool, const bool, const bool,
                    const bool, const bool, const bool, const bool,
                    const bool, const bool) override;

            //ctrl result
            virtual void SetCtrlSlaveResult(const uint32_t, const uint32_t) override;

            //flip camera result
            virtual void SetFlipCameraStatus(const uint32_t) override;

            //error code
            virtual void SetSlaveErrorCode(const uint32_t) override;

            //slave time
            virtual void SetSlaveSysTime(const uint32_t) override;

            virtual void GlobleMapUpdated() override {
                ImplementHub::NotifyMapUpdate();
            }

            virtual void PublishRandomEvents() override {
                ImplementHub::PushRandomEvents();
            }

        private:
            std::chrono::steady_clock::time_point time_whl_pulse_last_;
            std::chrono::steady_clock::time_point time_whl_speed_last_;
            std::chrono::steady_clock::time_point time_drop_last_;
            std::chrono::steady_clock::time_point time_drop_adc_last_;
            std::chrono::steady_clock::time_point time_mid_scan_last_;
            std::chrono::steady_clock::time_point time_whl_current_last_;
            std::chrono::steady_clock::time_point time_tof_last_;
            std::chrono::steady_clock::time_point time_dock_sig_last_;
            std::chrono::steady_clock::time_point time_slave_time_ms_last_;
            uint32_t slave_sysms_last_ = 0;

            //mcu params

            bool chs_online_ = false;

            std::shared_ptr<McuSensorParamCtrl> mcu_param_ = nullptr;

            std::weak_ptr<CambrianConfig> cmb_conf_ {};

            //1, visualize; 2, database
            std::unique_ptr<VisualClient> visualize_;
            std::unique_ptr<DbaseClient> database_;
    };

} //namespace brain
} //namespace camb
} //namespace mstf
