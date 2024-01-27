#include "modules/cambrian/brain/sys_processor.h"

namespace mstf {
namespace camb {
namespace brain {

#ifndef MCU_SEND_CYCLE_MS
#define MCU_SEND_CYCLE_MS 10.f
#endif
#ifndef MCU_SEND_TOLERANCE_MS
#define MCU_SEND_TOLERANCE_MS 1000.f
#endif

    SysProcessor::SysProcessor(//DatabaseNotifyHook db_hook,
            const std::shared_ptr<CambrianConfig>& cc)
        : ImplementHub(&cc->cmb_module_conf().impl_ops()) {
#ifdef CAMB_PKG_DBG
        AINFO << "SysProcessor construct";
#endif
        cmb_conf_ = cc;
        /*
        GetMessagePool()->db_notify_hook_ = db_hook;

        mcu_param_ = std::make_shared<McuSensorParamCtrl>();
        */
        visualize_ = std::make_unique<VisualClient>(
                &cc->cmb_module_conf().visual_ops());
        database_ = std::make_unique<DbaseClient>(
                &cc->cmb_module_conf().db_ops());
    }

    SysProcessor::~SysProcessor() {
        /*
#ifdef CAMB_PKG_DBG
        AINFO << "SysProcessor de-construct";
        AERROR << "volatile" <<
            //"\nsp:" << GetMessagePool()->vola_info_->DebugString() <<
            ", local sp: " << GetMessagePool()->vola_info_ <<
            ", use_count: " << GetMessagePool()->vola_info_.use_count();
#endif
*/
    }

    void SysProcessor::Init() {
#ifdef CAMB_PKG_DBG
        AINFO << "SysProcessor init";
#endif
        //firstly update system load
        //UpdateSystemLoad();

        /*
        ExposeRandomEventsPtr();
        time_whl_pulse_last_ = std::chrono::steady_clock::now();
        time_whl_speed_last_ = std::chrono::steady_clock::now();
        time_drop_last_ = std::chrono::steady_clock::now();
        time_drop_adc_last_ = std::chrono::steady_clock::now();
        time_mid_scan_last_ = std::chrono::steady_clock::now();
        time_whl_current_last_ = std::chrono::steady_clock::now();
        time_tof_last_ = std::chrono::steady_clock::now();
        time_dock_sig_last_ = std::chrono::steady_clock::now();
        time_slave_time_ms_last_ = std::chrono::steady_clock::now();
        */

        ImplementHub::Init();
        visualize_->Init();
        database_->DatabaseInit();
    }

    void SysProcessor::Start() {
#ifdef CAMB_PKG_DBG
        AINFO << "SysProcessor start";
#endif
        ImplementHub::Start();
        visualize_->Start();
    }

    void SysProcessor::Stop() {
#ifdef CAMB_PKG_DBG
        AINFO << "SysProcessor stop";
#endif
        ImplementHub::Stop();
        visualize_->Start();
    }

    void SysProcessor::Resume() {
#ifdef CAMB_PKG_DBG
        AINFO << "SysProcessor resume";
#endif
        ImplementHub::Start();
        visualize_->Start();
        database_->DatabaseInit();
    }

    void SysProcessor::Close() {
#ifdef CAMB_PKG_DBG
        AINFO << "SysProcessor close";
#endif
        ImplementHub::Close();
        visualize_->Close();
        database_->DataBaseExit();
    }

    void SysProcessor::Loop(const int loop_cnt) {
#ifdef CAMB_PKG_DBG
        AINFO << "SysProcessor Loop";
#endif
        LoopIncrease();
        ImplementHub::Loop(loop_cnt);

        if ((LoopCount() % (loop_cnt * 2)) == 0)
            UpdateSystemLoad();
    }

    void SysProcessor::PushRandomEvents(const
            EE_RANDOM_EVENTS_TRIGER evt, const int val) {
#ifdef CAMB_PKG_DBG
        //AINFO << "import random event: " << evt <<
            //", value: " << val;
#endif
        switch (evt) {
        case E_EVT_TOGGLE_BUTTON:
            GetMessagePool()->rnd_evts_->mutable_tog()->set_value(val);
            break;
        case E_EVT_SHORT_PWR:
            GetMessagePool()->rnd_evts_->mutable_spwr()->set_value(val);
            break;
        case E_EVT_LONG_PWR:
            GetMessagePool()->rnd_evts_->mutable_lpwr()->set_value(val);
            break;
        case E_EVT_SHORT_HOME:
            GetMessagePool()->rnd_evts_->mutable_shome()->set_value(val);
            break;
        case E_EVT_LONG_HOME:
            GetMessagePool()->rnd_evts_->mutable_lhome()->set_value(val);
            break;
        case E_EVT_NO_WATER:
            GetMessagePool()->rnd_evts_->mutable_no_wt()->set_value(val);
            break;
        case E_EVT_FULL_WATER:
            GetMessagePool()->rnd_evts_->mutable_fl_wt()->set_value(val);
            break;
        case E_EVT_DUST_BOX:
            GetMessagePool()->rnd_evts_->mutable_dbox()->set_value(val);
            break;
        case E_EVT_WATER_TANK:
            GetMessagePool()->rnd_evts_->mutable_tank()->set_value(val);
            break;
        case E_EVT_LEFT_PALLETE:
            GetMessagePool()->rnd_evts_->mutable_lplt()->set_value(val);
            break;
        case E_EVT_RIGHT_PALLETE:
            GetMessagePool()->rnd_evts_->mutable_rplt()->set_value(val);
            break;
        case E_EVT_LEFT_BUMP:
            GetMessagePool()->rnd_evts_->mutable_lbump()->set_value(val);
            break;
        case E_EVT_RIGHT_BUMP:
            GetMessagePool()->rnd_evts_->mutable_rbump()->set_value(val);
            break;
        case E_EVT_LLEFT_BUMP:
            GetMessagePool()->rnd_evts_->mutable_ld_lb()->set_value(val);
            break;
        case E_EVT_LRIGHT_BUMP:
            GetMessagePool()->rnd_evts_->mutable_ld_rb()->set_value(val);
            break;
        case E_EVT_LFRONT_BUMP:
            GetMessagePool()->rnd_evts_->mutable_ld_fb()->set_value(val);
            break;
        case E_EVT_LTOP_BUMP:
            GetMessagePool()->rnd_evts_->mutable_ld_tb()->set_value(val);
            break;
        case E_EVT_LWHL_LIFT:
            GetMessagePool()->rnd_evts_->mutable_llift()->set_value(val);
            break;
        case E_EVT_RWHL_LIFT:
            GetMessagePool()->rnd_evts_->mutable_rlift()->set_value(val);
            break;
        case E_EVT_ALONG_WALL:
            GetMessagePool()->rnd_evts_->mutable_awall()->set_value(val);
            break;
        case E_EVT_LEFT_DROP:
            GetMessagePool()->rnd_evts_->mutable_ldrop()->set_value(val);
            break;
        case E_EVT_MLEFT_DROP:
            GetMessagePool()->rnd_evts_->mutable_mldrop()->set_value(val);
            break;
        case E_EVT_MRIGHT_DROP:
            GetMessagePool()->rnd_evts_->mutable_mrdrop()->set_value(val);
            break;
        case E_EVT_RIGHT_DROP:
            GetMessagePool()->rnd_evts_->mutable_rdrop()->set_value(val);
            break;
        case E_EVT_LEFT_REAR_DROP:
            GetMessagePool()->rnd_evts_->mutable_lrdrop()->set_value(val);
            break;
        case E_EVT_RIGHT_REAR_DROP:
            GetMessagePool()->rnd_evts_->mutable_rrdrop()->set_value(val);
            break;
        case E_EVT_TOUCH_DOCK:
            GetMessagePool()->rnd_evts_->mutable_tch_dk()->set_value(val);
            break;
        case E_EVT_CHARGE_ON_DOCK:
            GetMessagePool()->rnd_evts_->mutable_charge()->set_value(val);
            break;
        case E_EVT_FINISH_CHARGE:
            GetMessagePool()->rnd_evts_->mutable_chrg_ok()->set_value(val);
            break;
        default:
            AERROR << "random event error: " << evt <<
                ", value: " << val;
            break;
        }

        GetMessagePool()->SetRandomTriger();
    }

    void SysProcessor::SetSystemInfoPtr(const
            std::shared_ptr<Message>& sys) {
        GetMessagePool()->sys_info_.reset(dynamic_cast<SystemInformation*>(sys.get()),
                [&](SystemInformation*){});
#ifdef CAMB_PKG_DBG
        AINFO << "import system info: " << sys <<
            ", use_count: " << sys.use_count() <<
            ", local sp: " << GetMessagePool()->sys_info_ <<
            ", use_count: " << GetMessagePool()->sys_info_.use_count() <<
            ", content p: " << GetMessagePool()->sys_info_.get();
#endif
    }

    void SysProcessor::SetModeStatusPtr(const
            std::shared_ptr<Message>& ms) {
        GetMessagePool()->mode_stat_.reset(dynamic_cast<RobotModeStatus*>(ms.get()),
                [&](RobotModeStatus*){});
#ifdef CAMB_PKG_DBG
        AINFO << "import mode & status: " << ms <<
            ", use_count: " << ms.use_count() <<
            ", local sp: " << GetMessagePool()->mode_stat_ <<
            ", use_count: " << GetMessagePool()->mode_stat_.use_count() <<
            ", content p: " << GetMessagePool()->mode_stat_.get();
#endif
    }

    void SysProcessor::SetVolatileInfoPtr(const
            std::shared_ptr<Message>& vola) {
        GetMessagePool()->vola_info_ = std::shared_ptr<VolatileInformation>(
                dynamic_cast<VolatileInformation*>(vola.get()),
                [&](VolatileInformation* /*p*/){
                /*AINFO << "ignore deleter: " << p;*/
                });
#ifdef CAMB_PKG_DBG
        AINFO << "import vola sp: " << vola <<
            ", use_count: " << vola.use_count() <<
            ", local sp: " << GetMessagePool()->vola_info_ <<
            ", use_count: " << GetMessagePool()->vola_info_.use_count() <<
            ", content p: " << GetMessagePool()->vola_info_.get();
#endif
    }

    void SysProcessor::SetMultiZonesPtr(const
            std::shared_ptr<Message>& mzp) {
        GetMessagePool()->mzps_.reset(dynamic_cast<MultiZonesParam*>(mzp.get()),
                [&](MultiZonesParam*){});
#ifdef CAMB_PKG_DBG
        AINFO << "import mzp sp: " << mzp <<
            ", use_count: " << mzp.use_count() <<
            ", local sp: " << GetMessagePool()->mzps_ <<
            ", use_count: " << GetMessagePool()->mzps_.use_count() <<
            ", content p: " << GetMessagePool()->mzps_.get();
#endif
    }

    void SysProcessor::ExposeRandomEventsPtr() {
        ImplementHub::SetRandomEvents(GetMessagePool()->rnd_evts_);
#ifdef CAMB_PKG_DBG
        AINFO << "expose random events" <<
            ", local sp: " << GetMessagePool()->rnd_evts_ <<
            ", use_count: " << GetMessagePool()->rnd_evts_.use_count() <<
            ", content p: " << GetMessagePool()->rnd_evts_.get();
#endif
    }

    void SysProcessor::ConfigChsParam(const
            McuSensorParamCtrl& mcu_param) {
        mcu_param_->CopyFrom(mcu_param);
//#ifdef CAMB_PKG_DBG
#if 0
        AINFO << "mcu param" <<
            ", tof: " << mcu_param_->tof_param().freq().value() <<
            ", wc: " << mcu_param_->whl_current().freq().value() <<
            ", dk: " << mcu_param_->dock_signal().freq().value() <<
            ", im: " << mcu_param_->imu_param().freq().value() <<
            ", el: " << mcu_param_->euler_param().freq().value() <<
            ", wp: " << mcu_param_->whl_pulse().freq().value() <<
            ", pd: " << mcu_param_->pose_param().freq().value() <<
            ", st: " << mcu_param_->slave_sys().freq().value() <<
            ", aw: " << mcu_param_->wdrop_adc().freq().value() <<
            ", ws: " << mcu_param_->speed_param().freq().value();
#endif
    }

    void SysProcessor::UpdateSystemLoad() {
        auto load = cyber::common::GetSysLoad();
        GetMessagePool()->vola_info_->mutable_brief_stat()->
            set_sys_load(std::get<0>(load));
//#ifdef DB_DATA_DBG
#if 1
        AINFO << "system load in last"
            " 2 mins: " << (100 * std::get<0>(load)) << "%"
            ", 5 mins: " << (100 * std::get<1>(load)) << "%"
            ", 10 mins: " << (100 * std::get<2>(load)) << "%";
#endif
    }

    //toggle button
    void SysProcessor::SetToggleButton(const bool button) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_gpio().has_toggle_button() ||
                button != GetMessagePool()->pph_info_->mutable_pp_gpio()->
                toggle_button().value())
            AINFO << "toggle button: " <<
                (button ? "ON" : "OFF");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_toggle_button()->set_value(button);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_TOGGLE_BUTTON, button);
    }

    //keys
    void SysProcessor::SetShortKeyPower(const bool key) {
#ifdef CAMB_PKG_DBG
        //AWARN << "short power key: " << key;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_key_power()->set_value(key);
        } else {
            AWARN << "empty ppi info!";
        }

        ImplementHub::InferMachine::PushPowerKey(E_CMD_FROM_KEY);

        //notifiy business
        PushRandomEvents(E_EVT_SHORT_PWR, key);
    }

    void SysProcessor::SetShortKeyHome(const bool key) {
#ifdef CAMB_PKG_DBG
        //AWARN << "short home key: " << key;
#endif
        ImplementHub::InferMachine::PushHomeKey(E_CMD_FROM_KEY);

        PushRandomEvents(E_EVT_SHORT_HOME, key);
    }

    void SysProcessor::SetLongKeyPower(const bool key) {
#ifdef CAMB_PKG_DBG
        //AWARN << "long power key: " << key;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_key_power_l()->set_value(key);
        } else {
            AWARN << "empty ppi info!";
        }

        ImplementHub::InferMachine::PushPowerKeyLong(E_CMD_FROM_KEY);

        PushRandomEvents(E_EVT_LONG_PWR, key);
    }

    void SysProcessor::SetLongKeyHome(const bool key) {
#ifdef CAMB_PKG_DBG
        //AWARN << "long home key: " << key;
#endif

        ImplementHub::InferMachine::PushHomeKeyLong(E_CMD_FROM_KEY);

        PushRandomEvents(E_EVT_LONG_HOME, key);
    }

    //water status
    void SysProcessor::SetNoWater(const bool nw) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_gpio().has_no_water() ||
                nw != GetMessagePool()->pph_info_->mutable_pp_gpio()->
                no_water().value())
            AINFO << "no-water status: " <<
                (nw == 0 ? "NO WATER" : "HAVE WATER");
#endif
        //1, set ppi
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_no_water()->set_value(nw);
        } else {
            AWARN << "empty ppi info!";
        }

        //2, set vola-brief
        if (GetMessagePool()->vola_info_) {
            GetMessagePool()->vola_info_->mutable_brief_stat()->
                set_water_level(nw);
        } else {
            AWARN << "empty vola info!";
        }

        PushRandomEvents(E_EVT_NO_WATER, nw);
    }

    void SysProcessor::SetFullWater(const bool fw) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_gpio().has_water_full() ||
                fw != GetMessagePool()->pph_info_->mutable_pp_gpio()->
                water_full().value())
            AINFO << "water full status: " <<
                (fw == 0 ? "NOT FULL" : "FULL");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_water_full()->set_value(fw);
        } else {
            AWARN << "empty ppi info!";
        }

        //water level
        if (GetMessagePool()->vola_info_) {
            GetMessagePool()->vola_info_->mutable_brief_stat()->
                set_water_level(fw);
        } else {
            AWARN << "empty vola info!";
        }

        PushRandomEvents(E_EVT_FULL_WATER, fw);
    }

    //dust box
    void SysProcessor::SetDustBox(const bool box) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_gpio().has_dust_box() ||
                box != GetMessagePool()->pph_info_->mutable_pp_gpio()->
                dust_box().value())
            AINFO << "dust box status: " <<
                ((box & 0x1) ? "MISS" : "OK");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_dust_box()->set_value(box);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_DUST_BOX, box);
    }

    //water tank
    void SysProcessor::SetWaterTank(const bool tank) {
#ifdef CAMB_PKG_DBG
        AWARN << "water tank status: " << std::hex << tank <<
            ", " << ((tank & 0x1) ? "MISS" : "OK");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_water_tank()->set_value(tank);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_WATER_TANK, tank);
    }

    //pallet
    void SysProcessor::SetLeftPallet(const bool pa) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_gpio().has_left_pallet() ||
                pa != GetMessagePool()->pph_info_->mutable_pp_gpio()->
                left_pallet().value())
            AINFO << "left pallet status: " <<
                (pa == 0 ? "OK" : "MISS");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_left_pallet()->set_value(pa);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_LEFT_PALLETE, pa);
    }

    void SysProcessor::SetRightPallet(const bool pa) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_gpio().has_right_pallet() ||
                pa != GetMessagePool()->pph_info_->mutable_pp_gpio()->
                right_pallet().value())
        AINFO << "right pallet status: " <<
            (pa == 0 ? "OK" : "MISS");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_right_pallet()->set_value(pa);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_RIGHT_PALLETE, pa);
    }

    //bump
    void SysProcessor::SetLeftBump(const bool bump) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_gpio().has_bump_left() ||
                bump != GetMessagePool()->pph_info_->mutable_pp_gpio()->
                bump_left().value())
            AINFO << "left bump status: " <<
                (bump ? "TRIG" : "OK");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_bump_left()->set_value(bump);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_LEFT_BUMP, bump);
    }

    void SysProcessor::SetRightBump(const bool bump) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_gpio().has_bump_right() ||
                bump != GetMessagePool()->pph_info_->mutable_pp_gpio()->
                bump_right().value())
            AINFO << "right bump status: " <<
                (bump ? "TRIG" : "OK");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_bump_right()->set_value(bump);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_RIGHT_BUMP, bump);
    }

    void SysProcessor::SetLidarLeftBump(const bool bump) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_gpio().has_lbump_left() ||
                bump != GetMessagePool()->pph_info_->mutable_pp_gpio()->
                lbump_left().value())
            AINFO << "left lidar bump status: " <<
                (bump ? "TRIG" : "OK");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_lbump_left()->set_value(bump);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_LLEFT_BUMP, bump);
    }

    void SysProcessor::SetLidarRightBump(const bool bump) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_gpio().has_lbump_right() ||
                bump != GetMessagePool()->pph_info_->mutable_pp_gpio()->
                lbump_right().value())
            AINFO << "right lidar bump status: " <<
                (bump ? "TRIG" : "OK");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_lbump_right()->set_value(bump);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_LRIGHT_BUMP, bump);
    }

    void SysProcessor::SetLidarFrontBump(const bool bump) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_gpio().has_lbump_front() ||
                bump != GetMessagePool()->pph_info_->mutable_pp_gpio()->
                lbump_front().value())
            AINFO << "front lidar bump status: " <<
                (bump ? "TRIG" : "OK");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_lbump_front()->set_value(bump);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_LFRONT_BUMP, bump);
    }

    void SysProcessor::SetLidarTopBump(const bool bump) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_gpio().has_lbump_top() ||
                bump != GetMessagePool()->pph_info_->mutable_pp_gpio()->
                lbump_top().value())
            AINFO << "top lidar bump status: " <<
                (bump ? "TRIG" : "OK");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_lbump_top()->set_value(bump);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_LTOP_BUMP, bump);
    }

    //wheel lift off ground
    void SysProcessor::SetWheelLeftLift(const bool lift) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_gpio().has_wheel_lift_left() ||
                lift != GetMessagePool()->pph_info_->mutable_pp_gpio()->
                wheel_lift_left().value())
            AINFO << "left wheel lift: " <<
                (lift ? "LIFT" : "OK");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_wheel_lift_left()->set_value(lift);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_LWHL_LIFT, lift);
    }

    void SysProcessor::SetWheelRightLift(const bool lift) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_gpio().has_wheel_lift_right() ||
                lift != GetMessagePool()->pph_info_->mutable_pp_gpio()->
                wheel_lift_right().value())
            AINFO << "right wheel lift: " <<
                (lift ? "LIFT" : "OK");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_wheel_lift_right()->set_value(lift);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_RWHL_LIFT, lift);
    }

    //along wall signal
    void SysProcessor::SetAlongWallTrig(const bool aw) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_gpio().has_along_wall_trg() ||
                aw != GetMessagePool()->pph_info_->mutable_pp_gpio()->
                along_wall_trg().value())
            AINFO << "along wall triger: " << aw;
#endif

#ifdef CAMB_PKG_DBG
        AINFO << "wall status: " << std::hex << aw <<
            ", signal come: " <<
            ((aw & 0x1) ? "YES" : "NO") <<
            ", signal gone: " <<
            ((aw & 0x2) ? "YES" : "NO");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_along_wall_trg()->set_value(aw);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_ALONG_WALL, aw);
    }

    //imu, periodically 4
    //eluer, periodically 5
    //odom, periodically 7
    //
    //wheel pulse, periodically 6
    void SysProcessor::SetWheelLeftPulse(const uint32_t wp) {
#ifdef CAMB_PKG_DBG
        auto time_now = std::chrono::steady_clock::now();
        auto pass = std::chrono::duration<double, std::milli>(
                        time_now - time_whl_pulse_last_).count();
        time_whl_pulse_last_ = time_now;
        auto off = pass - (float)(mcu_param_->
            whl_pulse().freq().value()
            * MCU_SEND_CYCLE_MS); //ms

        if (std::abs(off) >= MCU_SEND_TOLERANCE_MS) {
            AINFO << "[VI] left wheel pulse: " << wp <<
                ", host elapse: " << pass << "ms" <<
                ", dest: " << (mcu_param_->
                whl_pulse().freq().value()
                * MCU_SEND_CYCLE_MS) << "ms" <<
                ", Δ: " << off << "ms";
        }
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_lwhl_pulse()->set_value(wp);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    void SysProcessor::SetWheelRightPulse(const uint32_t wp) {
#ifdef CAMB_PKG_DBG
        //AINFO << "right wheel pulse: " << wp;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_rwhl_pulse()->set_value(wp);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    //speed, periodically 10
    void SysProcessor::SetWheelLeftSpeed(const float sp) {
#ifdef CAMB_PKG_DBG
        auto time_now = std::chrono::steady_clock::now();
        auto pass = std::chrono::duration<double, std::milli>(
                        time_now - time_whl_speed_last_).count();
        time_whl_speed_last_ = time_now;
        auto off = pass - (float)(mcu_param_->
            speed_param().freq().value()
            * MCU_SEND_CYCLE_MS); //ms

        if (std::abs(off) >= MCU_SEND_TOLERANCE_MS) {
            AINFO << "[X] left wheel speed: " << sp << "m/s" <<
                ", host elapse: " << pass << "ms" <<
                ", dest: " << (mcu_param_->
                speed_param().freq().value()
                * MCU_SEND_CYCLE_MS) << "ms" <<
                ", Δ: " << off << "ms";
        }
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_lwhl_speed()->set_value(sp);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    void SysProcessor::SetWheelRightSpeed(const float sp) {
#ifdef CAMB_PKG_DBG
        //AINFO << "right wheel speed: " << sp;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_gpio()->
                mutable_rwhl_speed()->set_value(sp);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    //if drop
    void SysProcessor::SetDropLeft(const bool drop) {
#ifdef CAMB_PKG_DBG
        auto time_now = std::chrono::steady_clock::now();
        auto pass = std::chrono::duration<double, std::milli>(
                        time_now - time_drop_last_).count();
        time_drop_last_ = time_now;

        if (!GetMessagePool()->pph_info_->pp_adc().has_ground_signal_left() ||
                drop != GetMessagePool()->pph_info_->mutable_pp_adc()->
                ground_signal_left().value())
            AINFO << "left drop, " <<
                "pass: " << pass << "ms, " <<
                (drop ? "YES DROP" : "OK");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_adc()->
                mutable_ground_signal_left()->
                set_value(drop);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_LEFT_DROP, drop);
    }

    void SysProcessor::SetDropMidLeft(const bool drop) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_adc().has_ground_signal_mleft() ||
                drop != GetMessagePool()->pph_info_->mutable_pp_adc()->
                ground_signal_mleft().value())
            AINFO << "middle left drop: " <<
                (drop ? "YES DROP" : "OK");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_adc()->
                mutable_ground_signal_mleft()->
                set_value(drop);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_MLEFT_DROP, drop);
    }

    void SysProcessor::SetDropMidRight(const bool drop) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_adc().has_ground_signal_mright() ||
                drop != GetMessagePool()->pph_info_->mutable_pp_adc()->
                ground_signal_mright().value())
            AINFO << "middle right drop: " <<
                (drop ? "YES DROP" : "OK");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_adc()->
                mutable_ground_signal_mright()->
                set_value(drop);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_MRIGHT_DROP, drop);
    }

    void SysProcessor::SetDropRight(const bool drop) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_adc().has_ground_signal_right() ||
                drop != GetMessagePool()->pph_info_->mutable_pp_adc()->
                ground_signal_right().value())
            AINFO << "right drop: " <<
                (drop ? "YES DROP" : "OK");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_adc()->
                mutable_ground_signal_right()->
                set_value(drop);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_RIGHT_DROP, drop);
    }

    void SysProcessor::SetDropLeftRear(const bool drop) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_adc().has_ground_signal_left_rear() ||
                drop != GetMessagePool()->pph_info_->mutable_pp_adc()->
                ground_signal_left_rear().value())
            AINFO << "left drop rear: " <<
                (drop ? "YES DROP" : "OK");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_adc()->
                mutable_ground_signal_left_rear()->
                set_value(drop);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_LEFT_REAR_DROP, drop);
    }

    void SysProcessor::SetDropRightRear(const bool drop) {
#ifdef CAMB_PKG_DBG
        if (!GetMessagePool()->pph_info_->pp_adc().has_ground_signal_right_rear() ||
                drop != GetMessagePool()->pph_info_->mutable_pp_adc()->
                ground_signal_right_rear().value())
        AINFO << "right drop rear: " <<
            (drop ? "YES DROP" : "OK");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_adc()->
                mutable_ground_signal_right_rear()->
                set_value(drop);
        } else {
            AWARN << "empty ppi info!";
        }

        PushRandomEvents(E_EVT_RIGHT_REAR_DROP, drop);
    }


    //drop ADC, periodically 9
    void SysProcessor::SetDropLeftAdc(const uint32_t adc) {
#ifdef CAMB_PKG_DBG
        auto time_now = std::chrono::steady_clock::now();
        auto pass = std::chrono::duration<double, std::milli>(
                        time_now - time_drop_adc_last_).count();
        time_drop_adc_last_ = time_now;
        auto off = pass - (float)(mcu_param_->
            wdrop_adc().freq().value()
            * MCU_SEND_CYCLE_MS); //ms

        if (std::abs(off) >= MCU_SEND_TOLERANCE_MS) {
            AINFO << "[IX] left drop adc: " << adc <<
                ", host elapse: " << pass << "ms" <<
                ", dest: " << (mcu_param_->
                wdrop_adc().freq().value()
                * MCU_SEND_CYCLE_MS) << "ms" <<
                ", Δ: " << off << "ms";
        }
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_infra()->
                mutable_ground_l()->
                set_value(adc);
        } else {
            AWARN << "empty ppi info!";
        }

        //TODO
    }

    void SysProcessor::SetDropMidLeftAdc(const uint32_t adc) {
#ifdef CAMB_PKG_DBG
        //AINFO << "middle left drop adc: " << adc;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_infra()->
                mutable_ground_l_front()->
                set_value(adc);
        } else {
            AWARN << "empty ppi info!";
        }

        //TODO
    }

    void SysProcessor::SetDropMidRightAdc(const uint32_t adc) {
#ifdef CAMB_PKG_DBG
        //AINFO << "middle right drop adc: " << adc;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_infra()->
                mutable_ground_r_front()->
                set_value(adc);
        } else {
            AWARN << "empty ppi info!";
        }

        //TODO
    }

    void SysProcessor::SetDropRightAdc(const uint32_t adc) {
#ifdef CAMB_PKG_DBG
        //AINFO << "right drop adc: " << adc;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_infra()->
                mutable_ground_r()->
                set_value(adc);
        } else {
            AWARN << "empty ppi info!";
        }

        //TODO
    }

    void SysProcessor::SetDropLeftRearAdc(const uint32_t adc) {
#ifdef CAMB_PKG_DBG
        //AINFO << "left rear drop adc: " << adc;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_infra()->
                mutable_ground_l_rear()->
                set_value(adc);
        } else {
            AWARN << "empty ppi info!";
        }

        //TODO
    }

    void SysProcessor::SetDropRightRearAdc(const uint32_t adc) {
#ifdef CAMB_PKG_DBG
        //AINFO << "right rear drop adc: " << adc;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_infra()->
                mutable_ground_r_rear()->
                set_value(adc);
        } else {
            AWARN << "empty ppi info!";
        }

        //TODO
    }

    //charge
    void SysProcessor::SetChargeTouchDock(const bool dock) {
#ifdef CAMB_PKG_DBG
        AINFO << "touch dock signal: " <<
            (dock ? "YES TOUCH" : "NO");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_adc()->
                mutable_charge_touch_dock()->
                set_value(dock);
        } else {
            AWARN << "empty ppi info!";
        }

        ImplementHub::InferMachine::EventOnDock(dock);
        PushRandomEvents(E_EVT_TOUCH_DOCK, dock);
    }

    void SysProcessor::SetChargeOnDock(const bool dock) {
#ifdef CAMB_PKG_DBG
        AINFO << "charging on dock: " <<
            (dock ? "YES DOCK" : "NO");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_adc()->
                mutable_charge_status_charging()->
                set_value(dock);
        } else {
            AWARN << "empty ppi info!";
        }

        ImplementHub::InferMachine::EventOnDock(dock);
        PushRandomEvents(E_EVT_CHARGE_ON_DOCK, dock);
    }

    void SysProcessor::SetChargeFinished(const bool dock) {
#ifdef CAMB_PKG_DBG
        AINFO << "charge finished: " <<
            (dock ? "YES FINISHED" : "NO");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_adc()->
                mutable_charge_status_finish()->
                set_value(dock);
        } else {
            AWARN << "empty ppi info!";
        }

        ImplementHub::InferMachine::EventChargeFull(dock);
        PushRandomEvents(E_EVT_FINISH_CHARGE, dock);
    }

    //mid-scan
    void SysProcessor::SetMidScanDetect(const bool ms) {
        //true: carpet, false: normal
#ifdef CAMB_PKG_DBG
        auto time_now = std::chrono::steady_clock::now();
        auto pass = std::chrono::duration<double, std::milli>(
                        time_now - time_mid_scan_last_).count();
        time_mid_scan_last_ = time_now;

        AWARN << "middle scan detect: " << ms <<
            ", host elapse: " << pass << "ms";
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_adc()->
                mutable_middle_scan_current()->
                set_value(ms);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    //super_sonic???

    //low power & dock
    void SysProcessor::SetLowPowerDocking(const bool lp) {
#ifdef CAMB_PKG_DBG
        AWARN << "lower power docking: " << lp;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_adc()->
                mutable_low_power_charge()->
                set_value(lp);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    void SysProcessor::SetLowPowerPoweroff(const bool lp) {
#ifdef CAMB_PKG_DBG
        AWARN << "lower power to be power off: " << lp;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_adc()->
                mutable_low_power_off()->
                set_value(lp);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    //wheel current, periodically 2
    void SysProcessor::SetWheelLeftCurrent(const float current) {
#ifdef CAMB_PKG_DBG
        auto time_now = std::chrono::steady_clock::now();
        auto pass = std::chrono::duration<double, std::milli>(
                        time_now - time_whl_current_last_).count();
        time_whl_current_last_ = time_now;
        auto off = pass - (float)(mcu_param_->
            whl_current().freq().value()
            * MCU_SEND_CYCLE_MS); //ms

        if (std::abs(off) >= MCU_SEND_TOLERANCE_MS) {
            AINFO << "[II] left wheel current: " << current <<
                ", host elapse: " << pass << "ms" <<
                ", dest: " << (mcu_param_->
                whl_current().freq().value()
                * MCU_SEND_CYCLE_MS) << "ms" <<
                ", Δ: " << off << "ms";
        }
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_adc()->
                mutable_leftw_current_ma()->
                set_value(current);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    void SysProcessor::SetWheelRightCurrent(const float current) {
#ifdef CAMB_PKG_DBG
        //AINFO << "right wheel current: " << current;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_adc()->
                mutable_rightw_current_ma()->
                set_value(current);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    //tof distance in mm, periodically 1
    void SysProcessor::SetTofDistance(uint32_t tof) {
#ifdef CAMB_PKG_DBG
        auto time_now = std::chrono::steady_clock::now();
        auto pass = std::chrono::duration<double, std::milli>(
                        time_now - time_tof_last_).count();
        time_tof_last_ = time_now;
        auto off = pass - (float)(mcu_param_->
            tof_param().freq().value()
            * MCU_SEND_CYCLE_MS); //ms

        if (std::abs(off) >= MCU_SEND_TOLERANCE_MS) {
            AERROR << "[I] tof distance: " << tof << "mm" <<
                ", host elapse: " << pass << "ms" <<
                ", dest: " << (mcu_param_->
                tof_param().freq().value()
                * MCU_SEND_CYCLE_MS) << "ms" <<
                ", Δ: " << off << "ms";
        }
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_infra()->
                mutable_tof_distance()->
                set_value(tof);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    //dock signal, periodically 3
    void SysProcessor::SetDockLeftSignal(uint32_t dock) {
#ifdef CAMB_PKG_DBG
        auto time_now = std::chrono::steady_clock::now();
        auto pass = std::chrono::duration<double, std::milli>(
                        time_now - time_dock_sig_last_).count();
        time_dock_sig_last_ = time_now;
        auto off = pass - (float)(mcu_param_->
            dock_signal().freq().value()
            * MCU_SEND_CYCLE_MS); //ms

        if (std::abs(off) >= MCU_SEND_TOLERANCE_MS) {
            AINFO << "[III] left dock signal: " << dock <<
                ", host elapse: " << pass << "ms" <<
                ", dest: " << (mcu_param_->
                dock_signal().freq().value()
                * MCU_SEND_CYCLE_MS) << "ms" <<
                ", Δ: " << off << "ms";
        }
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_infra()->
                mutable_dock_l()->
                set_value(dock);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    void SysProcessor::SetDockMidLeftSignal(uint32_t dock) {
#ifdef CAMB_PKG_DBG
        //AINFO << "middle left dock signal: " << dock;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_infra()->
                mutable_dock_ml()->
                set_value(dock);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    void SysProcessor::SetDockMidRightSignal(uint32_t dock) {
#ifdef CAMB_PKG_DBG
        //AINFO << "middle right dock signal: " << dock;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_infra()->
                mutable_dock_mr()->
                set_value(dock);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    void SysProcessor::SetDockRightFrontSignal(uint32_t dock) {
#ifdef CAMB_PKG_DBG
        //AINFO << "right front dock signal: " << dock;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_infra()->
                mutable_dock_front_r()->
                set_value(dock);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    void SysProcessor::SetDockRightSignal(uint32_t dock) {
#ifdef CAMB_PKG_DBG
        //AINFO << "right dock signal: " << dock;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_infra()->
                mutable_dock_r()->
                set_value(dock);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    void SysProcessor::SetDockLeftFrontSignal(uint32_t dock) {
#ifdef CAMB_PKG_DBG
        //AINFO << "left front dock signal: " << dock;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_infra()->
                mutable_dock_front_l()->
                set_value(dock);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    void SysProcessor::SetDockRearMidLeftSignal(uint32_t dock) {
#ifdef CAMB_PKG_DBG
        //AINFO << "left middle rear dock signal: " << dock;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_infra()->
                mutable_dock_rear_ml()->
                set_value(dock);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    void SysProcessor::SetDockRearMidRightSignal(uint32_t dock) {
#ifdef CAMB_PKG_DBG
        //AINFO << "right middle rear dock signal: " << dock;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_infra()->
                mutable_dock_rear_mr()->
                set_value(dock);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    //battery
    void SysProcessor::SetBatteryLevel(const uint32_t bat) {
#ifdef CAMB_PKG_DBG
        AINFO << "battery level(%), (last: " <<
            GetMessagePool()->vola_info_->brief_stat().battery() <<
            " -> current: " << bat << ")";
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_battery()->
                set_value(bat);
        } else {
            AWARN << "empty ppi info!";
        }

        if (GetMessagePool()->vola_info_) {
            GetMessagePool()->vola_info_->mutable_brief_stat()->
                set_battery(bat);
        } else {
            AWARN << "empty vola info!";
        }

        if (GetMessagePool()->mode_stat_) {
            GetMessagePool()->mode_stat_->set_battery(bat);
        } else {
            AWARN << "empty mode stat!";
        }

        //infer machine & remote side update
        UpdataBattery(bat);

        //update SQL automatically;
        if (GetMessagePool()->db_notify_hook_)
            GetMessagePool()->db_notify_hook_();
    }

    void SysProcessor::SetBatteryTemp(const uint32_t temp) {
#ifdef CAMB_PKG_DBG
        AINFO << "battery temperature: " <<
            temp << R"(℃)";
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_bat_temp()->
                set_value(temp);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    //mcu online
    void SysProcessor::SetOnlineResult(const uint32_t res) {
#ifdef CAMB_PKG_DBG
        AINFO << "mcu online result: " <<
            (res ? "NG" : "OK");
#endif
        if (res == 0) {
            chs_online_ = true;
        } else if (res == 1) {
            AWARN << "mcu online faile, can't recognize message";
        } else if (res == 2) {
            AWARN << "mcu online faile, protocol version not support";
        } else if (res == 3) {
            AWARN << "mcu online faile, device type not support";
        }

        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_online_result()->
                set_value(res);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    void SysProcessor::SetOnlineVersion(const uint32_t version) {
#ifdef CAMB_PKG_DBG
        AINFO << "mcu online version: " << version;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_proto_ver()->
                set_value(version);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    void SysProcessor::SetOnlineHeartBeat(const uint32_t hb) {
#ifdef CAMB_PKG_DBG
        AINFO << "mcu online heartbeat: " <<
            (hb ? "NG" : "OK");
#endif
        if (hb == 0) {
            chs_online_ = true;
        } else if (hb == 1) {
            AWARN << "mcu heart beat fail: " << hb;
            chs_online_ = false;
        }

        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_heart_beat_result()->
                set_value(hb);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    //mcu info
    void SysProcessor::SetMcuSn(const std::string sn) {
        AINFO << "mcu sn: " << sn;

        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_mcu_sn()->
                set_value(sn);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    void SysProcessor::SetMcuVersion(const std::string version) {
        AINFO << "mcu version: " << version;

        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_mcu_ver()->
                set_value(version);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    void SysProcessor::SetMcuCompileTime(const std::string time) {
        AINFO << "compile time: " << time;

        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_mcu_comp_time()->
                set_value(time);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    void SysProcessor::SetMcuSensorEnStat(
            const bool wall_en,
            const bool drop_en,
            const bool tof_en,
            const bool whls_pwr,
            const bool fan_pwr,
            const bool mbrush_pwr,
            const bool sbrush_pwr,
            const bool pump_pwr,
            const bool charge_en,
            const bool bot_pwr,
            const bool pump_en,
            const bool pallet_en) {
#ifdef CAMB_PKG_DBG
        AINFO << "wall:" << (wall_en ? "YES" : "NO") <<
            ", drop: " << (drop_en ? "YES" : "NO");
        AINFO << "tof: " << (tof_en ? "YES" : "NO") <<
            ", wheels power: " << (whls_pwr ? "YES" : "NO");
        AINFO << "fan power: " << (fan_pwr ? "YES" : "NO") <<
            ", mbrush power: " << (mbrush_pwr ? "YES" : "NO");
        AINFO << "sbrush power: " << (sbrush_pwr ? "YES" : "NO") <<
            ", pump power: " << (pump_pwr ? "YES" : "NO");
        AINFO << "charge: " << (charge_en ? "YES" : "NO") <<
            ", robot power: " << (bot_pwr ? "YES" : "NO");
        AINFO << "pump: " << (pump_en ? "YES" : "NO") <<
            ", pallet: " << (pallet_en ? "YES" : "NO");
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_sensor_en_stat()->
                mutable_wall_en()->set_value(wall_en);
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_sensor_en_stat()->
                mutable_drop_en()->set_value(drop_en);
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_sensor_en_stat()->
                mutable_tof_en()->set_value(tof_en);
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_sensor_en_stat()->
                mutable_wheels_power()->set_value(whls_pwr);
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_sensor_en_stat()->
                mutable_fan_power()->set_value(fan_pwr);
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_sensor_en_stat()->
                mutable_main_brush_power()->set_value(mbrush_pwr);
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_sensor_en_stat()->
                mutable_side_brush_power()->set_value(sbrush_pwr);
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_sensor_en_stat()->
                mutable_pump_motor_power()->set_value(pump_pwr);
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_sensor_en_stat()->
                mutable_charge_en()->set_value(charge_en);
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_sensor_en_stat()->
                mutable_robot_power()->set_value(bot_pwr);
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_sensor_en_stat()->
                mutable_pump_en()->set_value(pump_en);
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_sensor_en_stat()->
                mutable_pallet_en()->set_value(pallet_en);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    //ctrl result
    void SysProcessor::SetCtrlSlaveResult(const uint32_t res,
            const uint32_t cmd) {
#ifdef CAMB_PKG_DBG
        //AINFO << "control slave cmd: 0x0" << std::hex << cmd <<
            //", ctrl result: " << (res == 0 ? "OK" : "NG");
#endif

        if (res != 0) {
            AWARN << "control slave cmd: " << std::hex << cmd <<
                ", result: " << res << ", control fail!";
        }

        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_ctrl_chass_result()->
                mutable_ctrl_result()->
                set_value(res);
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_ctrl_chass_result()->
                mutable_ctrl_ack_type()->
                set_value(cmd);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    //flip camera result
    void SysProcessor::SetFlipCameraStatus(const uint32_t stat) {
#ifdef CAMB_PKG_DBG
        std::string str;
        if (stat == 0)
            str = "MID";
        if (stat == 1)
            str = "FOLD";
        if (stat == 2)
            str = "OPEN";
        AINFO << "flip camera status: " << str;
#endif
        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_camera_flip()->
                set_value(stat);
        } else {
            AWARN << "empty ppi info!";
        }
    }

    //error code
    void SysProcessor::SetSlaveErrorCode(const uint32_t error) {
#ifdef CAMB_PKG_DBG
        AWARN << "slave report error: " << error;
#endif
        //TODO
        // 
    }

    //slave time, in: 0.1ms, periodically 8
    void SysProcessor::SetSlaveSysTime(const uint32_t time) {
#ifdef CAMB_PKG_DBG
        auto time_now = std::chrono::steady_clock::now();
        auto pass = std::chrono::duration<double, std::milli>(
                        time_now - time_slave_time_ms_last_).count();

        uint32_t slave_sys_pass = 0;
        if (slave_sysms_last_ != 0)
            slave_sys_pass = time - slave_sysms_last_;

        auto off = pass - (float)(mcu_param_->
            slave_sys().freq().value()
            * MCU_SEND_CYCLE_MS); //ms
        time_slave_time_ms_last_ = time_now;

        if (std::abs(off) >= MCU_SEND_TOLERANCE_MS) {
            AINFO << "[VIII] slave time: " << time / 10000 <<
                "s+" << std::setw(3) << std::setfill('0') <<
                time % 10000 / 10 <<
                "." << time % 10 << "ms" <<
                ", last: " << slave_sysms_last_ / 10000 <<
                "s_" << std::setw(3) << std::setfill('0') <<
                slave_sysms_last_ % 10000 / 10 <<
                "." << slave_sysms_last_ % 10 << "ms" <<
                ", pass: " << slave_sys_pass / 10 << "ms" <<
                ". host elapse: " << pass << "ms" <<
                ", dest: " << (mcu_param_->
                slave_sys().freq().value()
                * MCU_SEND_CYCLE_MS) << "ms" <<
                ", Δ: " << off << "ms";
        }
#endif
        slave_sysms_last_ = time;

        if (GetMessagePool()->pph_info_) {
            GetMessagePool()->pph_info_->mutable_pp_extra()->
                mutable_slave_10th1_ms()->
                set_value(time);
        } else {
            AWARN << "empty ppi info!";
        }
    }

} //namespace brain
} //namespace camb
} //namespace mstf
