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
        GetMessagePool()->HookImplementor(std::bind(
                    &SysProcessor::ParseChssMisc, this));

        visualize_ = std::make_unique<VisualClient>(
                &cc->cmb_module_conf().visual_ops());
        database_ = std::make_unique<DbaseClient>(
                &cc->cmb_module_conf().db_ops());
    }

    SysProcessor::~SysProcessor() {
#ifdef CAMB_PKG_DBG
        AINFO << "SysProcessor de-construct";
#endif
    }

    void SysProcessor::Init() {
#ifdef CAMB_PKG_DBG
        AINFO << "SysProcessor init";
#endif
        //firstly update system load
        UpdateSystemLoad();

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

    void SysProcessor::UpdateSystemLoad() {
        auto load = cyber::common::GetSysLoad();
        //GetMessagePool()->vola_info_->mutable_brief_stat()->
            //set_sys_load(std::get<0>(load));
#if 1
//#ifdef CAMB_PKG_DBG
        AINFO << "system load in last"
            " 2 mins: " << (100 * std::get<0>(load)) << "%"
            ", 5 mins: " << (100 * std::get<1>(load)) << "%"
            ", 10 mins: " << (100 * std::get<2>(load)) << "%";
#endif
    }

    void SysProcessor::ParseChssMisc() {
        auto msg = GetMessagePool()->FeedbkChassisMisc();
#ifdef CAMB_PKG_DBG
        AINFO << "paser chassis misc data!\n" <<
            msg->DebugString();
#endif
        //liquid
        if (msg->has_liquid_level()) {
            //clean tank
            if (msg->liquid_level().has_clean_tank()) {
                if (msg->liquid_level().clean_tank().has_dev())
                    CleanTankName(msg->liquid_level().clean_tank().dev().value());
                if (msg->liquid_level().clean_tank().has_en())
                    CleanTankStat(msg->liquid_level().clean_tank().en().value());
                if (msg->liquid_level().clean_tank().has_level())
                    CleanTankLevel(msg->liquid_level().clean_tank().level().value());
                if (msg->liquid_level().clean_tank().has_empty())
                    CleanTankEmpty(msg->liquid_level().clean_tank().empty().value());
                if (msg->liquid_level().clean_tank().has_low())
                    CleanTankLow(msg->liquid_level().clean_tank().low().value());
                if (msg->liquid_level().clean_tank().has_full())
                    CleanTankFull(msg->liquid_level().clean_tank().full().value());
                if (msg->liquid_level().clean_tank().has_update_freq_ms())
                    CleanTankUpdataFreqms(msg->liquid_level().clean_tank().update_freq_ms().value());
            }
            //dirty tank
            if (msg->liquid_level().has_dirty_tank()) {
                if (msg->liquid_level().dirty_tank().has_dev())
                    DirtyTankName(msg->liquid_level().dirty_tank().dev().value());
                if (msg->liquid_level().dirty_tank().has_en())
                    DirtyTankStat(msg->liquid_level().dirty_tank().en().value());
                if (msg->liquid_level().dirty_tank().has_level())
                    DirtyTankLevel(msg->liquid_level().dirty_tank().level().value());
                if (msg->liquid_level().dirty_tank().has_empty())
                    DirtyTankEmpty(msg->liquid_level().dirty_tank().empty().value());
                if (msg->liquid_level().dirty_tank().has_low())
                    DirtyTankLow(msg->liquid_level().dirty_tank().low().value());
                if (msg->liquid_level().dirty_tank().has_full())
                    DirtyTankFull(msg->liquid_level().dirty_tank().full().value());
                if (msg->liquid_level().dirty_tank().has_update_freq_ms())
                    DirtyTankUpdateFreqms(msg->liquid_level().dirty_tank().update_freq_ms().value());
            }
            //dust box
            if (msg->liquid_level().has_dust_box()) {
                if (msg->liquid_level().dust_box().has_dev())
                    DustBoxName(msg->liquid_level().dust_box().dev().value());
                if (msg->liquid_level().dust_box().has_en())
                    DustBoxStat(msg->liquid_level().dust_box().en().value());
                if (msg->liquid_level().dust_box().has_level())
                    DustBoxLevel(msg->liquid_level().dust_box().level().value());
                if (msg->liquid_level().dust_box().has_empty())
                    DustBoxEmpty(msg->liquid_level().dust_box().empty().value());
                if (msg->liquid_level().dust_box().has_low())
                    DustBoxLow(msg->liquid_level().dust_box().low().value());
                if (msg->liquid_level().dust_box().has_full())
                    DustBoxFull(msg->liquid_level().dust_box().full().value());
                if (msg->liquid_level().dust_box().has_update_freq_ms())
                    DustBoxUpdataFreqms(msg->liquid_level().dust_box().update_freq_ms().value());
            }
            //detergent tank
            if (msg->liquid_level().has_detergent_tank()) {
                if (msg->liquid_level().detergent_tank().has_dev())
                    DetergentTankName(msg->liquid_level().detergent_tank().dev().value());
                if (msg->liquid_level().detergent_tank().has_en())
                    DetergentTankStat(msg->liquid_level().detergent_tank().en().value());
                if (msg->liquid_level().detergent_tank().has_level())
                    DetergentTankLevel(msg->liquid_level().detergent_tank().level().value());
                if (msg->liquid_level().detergent_tank().has_empty())
                    DetergentTankEmpty(msg->liquid_level().detergent_tank().empty().value());
                if (msg->liquid_level().detergent_tank().has_low())
                    DetergentTankLow(msg->liquid_level().detergent_tank().low().value());
                if (msg->liquid_level().detergent_tank().has_full())
                    DetergentTankFull(msg->liquid_level().detergent_tank().full().value());
                if (msg->liquid_level().detergent_tank().has_update_freq_ms())
                    DetergentTankUpdataFreqms(msg->liquid_level().detergent_tank().update_freq_ms().value());
            }
        }

        //adc
        if (msg->has_periph_adc()) {
            //left disc brush
            if (msg->periph_adc().has_disc_brush_left()) {
                if (msg->periph_adc().disc_brush_left().has_dev())
                    DiscBrushLeftName(msg->periph_adc().disc_brush_left().dev().value());
                if (msg->periph_adc().disc_brush_left().has_en())
                    DiscBrushLeftStat(msg->periph_adc().disc_brush_left().en().value());
                if (msg->periph_adc().disc_brush_left().has_voltage_mv())
                    DiscBrushLeftVoltmv(msg->periph_adc().disc_brush_left().voltage_mv().value());
                if (msg->periph_adc().disc_brush_left().has_current_ma())
                    DiscBrushLeftCurrentma(msg->periph_adc().disc_brush_left().current_ma().value());
                if (msg->periph_adc().disc_brush_left().has_temperature())
                    DiscBrushLeftTemperature(msg->periph_adc().disc_brush_left().temperature().value());
                if (msg->periph_adc().disc_brush_left().has_update_freq_ms())
                    DiscBrushLeftUpdateFreqms(msg->periph_adc().disc_brush_left().update_freq_ms().value());
            }
            //right disc brush
            if (msg->periph_adc().has_disc_brush_right()) {
                if (msg->periph_adc().disc_brush_right().has_dev())
                    DiscBrushRightName(msg->periph_adc().disc_brush_right().dev().value());
                if (msg->periph_adc().disc_brush_right().has_en())
                    DiscBrushRightStat(msg->periph_adc().disc_brush_right().en().value());
                if (msg->periph_adc().disc_brush_right().has_voltage_mv())
                    DiscBrushRightVoltmv(msg->periph_adc().disc_brush_right().voltage_mv().value());
                if (msg->periph_adc().disc_brush_right().has_current_ma())
                    DiscBrushRightCurrentma(msg->periph_adc().disc_brush_right().current_ma().value());
                if (msg->periph_adc().disc_brush_right().has_temperature())
                    DiscBrushRightTemperature(msg->periph_adc().disc_brush_right().temperature().value());
                if (msg->periph_adc().disc_brush_right().has_update_freq_ms())
                    DiscBrushRightUpdateFreqms(msg->periph_adc().disc_brush_right().update_freq_ms().value());
            }
            //front roll brush
            if (msg->periph_adc().has_roll_brush_front()) {
                if (msg->periph_adc().roll_brush_front().has_dev())
                    RollBrushFrontName(msg->periph_adc().roll_brush_front().dev().value());
                if (msg->periph_adc().roll_brush_front().has_en())
                    RollBrushFrontStat(msg->periph_adc().roll_brush_front().en().value());
                if (msg->periph_adc().roll_brush_front().has_voltage_mv())
                    RollBrushFrontVoltmv(msg->periph_adc().roll_brush_front().voltage_mv().value());
                if (msg->periph_adc().roll_brush_front().has_current_ma())
                    RollBrushFrontCurrentma(msg->periph_adc().roll_brush_front().current_ma().value());
                if (msg->periph_adc().roll_brush_front().has_temperature())
                    RollBrushFrontTemperature(msg->periph_adc().roll_brush_front().temperature().value());
                if (msg->periph_adc().roll_brush_front().has_update_freq_ms())
                    RollBrushFrontUpdateFreqms(msg->periph_adc().roll_brush_front().update_freq_ms().value());
            }
            //rear roll brush
            if (msg->periph_adc().has_roll_brush_rear()) {
                if (msg->periph_adc().roll_brush_rear().has_dev())
                    RollBrushRearName(msg->periph_adc().roll_brush_rear().dev().value());
                if (msg->periph_adc().roll_brush_rear().has_en())
                    RollBrushRearStat(msg->periph_adc().roll_brush_rear().en().value());
                if (msg->periph_adc().roll_brush_rear().has_voltage_mv())
                    RollBrushRearVoltmv(msg->periph_adc().roll_brush_rear().voltage_mv().value());
                if (msg->periph_adc().roll_brush_rear().has_current_ma())
                    RollBrushRearCurrentma(msg->periph_adc().roll_brush_rear().current_ma().value());
                if (msg->periph_adc().roll_brush_rear().has_temperature())
                    RollBrushRearTemperature(msg->periph_adc().roll_brush_rear().temperature().value());
                if (msg->periph_adc().roll_brush_rear().has_update_freq_ms())
                    RollBrushRearUpdateFreqms(msg->periph_adc().roll_brush_rear().update_freq_ms().value());
            }
            //suck fan 1
            if (msg->periph_adc().has_suck_fan_1()) {
                if (msg->periph_adc().suck_fan_1().has_dev())
                    SuckFanName(msg->periph_adc().suck_fan_1().dev().value());
                if (msg->periph_adc().suck_fan_1().has_en())
                    SuckFanStat(msg->periph_adc().suck_fan_1().en().value());
                if (msg->periph_adc().suck_fan_1().has_voltage_mv())
                    SuckFanVoltmv(msg->periph_adc().suck_fan_1().voltage_mv().value());
                if (msg->periph_adc().suck_fan_1().has_current_ma())
                    SuckFanCurrentma(msg->periph_adc().suck_fan_1().current_ma().value());
                if (msg->periph_adc().suck_fan_1().has_temperature())
                    SuckFanTemperature(msg->periph_adc().suck_fan_1().temperature().value());
                if (msg->periph_adc().suck_fan_1().has_update_freq_ms())
                    SuckFanUpdateFreqms(msg->periph_adc().suck_fan_1().update_freq_ms().value());
            }
            //wheel left
            if (msg->periph_adc().has_wheel_left()) {
                if (msg->periph_adc().wheel_left().has_dev())
                    WheelLeftName(msg->periph_adc().wheel_left().dev().value());
                if (msg->periph_adc().wheel_left().has_en())
                    WheelLeftStat(msg->periph_adc().wheel_left().en().value());
                if (msg->periph_adc().wheel_left().has_voltage_mv())
                    WheelLeftVoltmv(msg->periph_adc().wheel_left().voltage_mv().value());
                if (msg->periph_adc().wheel_left().has_current_ma())
                    WheelLeftCurrentma(msg->periph_adc().wheel_left().current_ma().value());
                if (msg->periph_adc().wheel_left().has_temperature())
                    WheelLeftTemperature(msg->periph_adc().wheel_left().temperature().value());
                if (msg->periph_adc().wheel_left().has_update_freq_ms())
                    WheelLeftUpdateFreqms(msg->periph_adc().wheel_left().update_freq_ms().value());
            }
            //wheel right
            if (msg->periph_adc().has_wheel_right()) {
                if (msg->periph_adc().wheel_right().has_dev())
                    WheelRightName(msg->periph_adc().wheel_right().dev().value());
                if (msg->periph_adc().wheel_right().has_en())
                    WheelRightStat(msg->periph_adc().wheel_right().en().value());
                if (msg->periph_adc().wheel_right().has_voltage_mv())
                    WheelRightVoltmv(msg->periph_adc().wheel_right().voltage_mv().value());
                if (msg->periph_adc().wheel_right().has_current_ma())
                    WheelRightCurrentma(msg->periph_adc().wheel_right().current_ma().value());
                if (msg->periph_adc().wheel_right().has_temperature())
                    WheelRightTemperature(msg->periph_adc().wheel_right().temperature().value());
                if (msg->periph_adc().wheel_right().has_update_freq_ms())
                    WheelRightUpdateFreqms(msg->periph_adc().wheel_right().update_freq_ms().value());
            }
            //battery
            if (msg->periph_adc().has_battery_config()) {
                if (msg->periph_adc().battery_config().has_dev())
                    BatteryName(msg->periph_adc().battery_config().dev().value());
                if (msg->periph_adc().battery_config().has_en())
                    BatteryStat(msg->periph_adc().battery_config().en().value());
                if (msg->periph_adc().battery_config().has_voltage_mv())
                    BatteryVoltmv(msg->periph_adc().battery_config().voltage_mv().value());
                if (msg->periph_adc().battery_config().has_current_ma())
                    BatteryCurrentma(msg->periph_adc().battery_config().current_ma().value());
                if (msg->periph_adc().battery_config().has_temperature())
                    BatteryTemperature(msg->periph_adc().battery_config().temperature().value());
                if (msg->periph_adc().battery_config().has_update_freq_ms())
                    BatteryUpdateFreqms(msg->periph_adc().battery_config().update_freq_ms().value());
            }
        }
        //pwm
        if (msg->has_periph_pwm()) {
            //disc brush left
            if (msg->periph_pwm().has_disc_brush_left()) {
                if (msg->periph_pwm().disc_brush_left().has_dev())
                    PwmDiscBrushLeftName(msg->periph_pwm().disc_brush_left().dev().value());
                if (msg->periph_pwm().disc_brush_left().has_en())
                    PwmDiscBrushLeftStat(msg->periph_pwm().disc_brush_left().en().value());
                if (msg->periph_pwm().disc_brush_left().has_duty())
                    PwmDiscBrushLeftDuty(msg->periph_pwm().disc_brush_left().duty().value());
                if (msg->periph_pwm().disc_brush_left().has_cycle_ns())
                    PwmDiscBrushLeftCyclens(msg->periph_pwm().disc_brush_left().cycle_ns().value());
                if (msg->periph_pwm().disc_brush_left().has_positive_ns())
                    PwmDiscBrushLeftPositivens(msg->periph_pwm().disc_brush_left().positive_ns().value());
                if (msg->periph_pwm().disc_brush_left().has_update_freq_ms())
                    PwmDiscBrushLeftUpdateFreqms(msg->periph_pwm().disc_brush_left().update_freq_ms().value());
            }
            //disc brush right
            if (msg->periph_pwm().has_disc_brush_right()) {
                if (msg->periph_pwm().disc_brush_right().has_dev())
                    PwmDiscBrushRightName(msg->periph_pwm().disc_brush_right().dev().value());
                if (msg->periph_pwm().disc_brush_right().has_en())
                    PwmDiscBrushRightStat(msg->periph_pwm().disc_brush_right().en().value());
                if (msg->periph_pwm().disc_brush_right().has_duty())
                    PwmDiscBrushRightDuty(msg->periph_pwm().disc_brush_right().duty().value());
                if (msg->periph_pwm().disc_brush_right().has_cycle_ns())
                    PwmDiscBrushRightCyclens(msg->periph_pwm().disc_brush_right().cycle_ns().value());
                if (msg->periph_pwm().disc_brush_right().has_positive_ns())
                    PwmDiscBrushRightPositivens(msg->periph_pwm().disc_brush_right().positive_ns().value());
                if (msg->periph_pwm().disc_brush_right().has_update_freq_ms())
                    PwmDiscBrushRightUpdateFreqms(msg->periph_pwm().disc_brush_right().update_freq_ms().value());
            }
            //roll brush front
            if (msg->periph_pwm().has_roll_brush_front()) {
                if (msg->periph_pwm().roll_brush_front().has_dev())
                    PwmRollBrushFrontName(msg->periph_pwm().roll_brush_front().dev().value());
                if (msg->periph_pwm().roll_brush_front().has_en())
                    PwmRollBrushFrontStat(msg->periph_pwm().roll_brush_front().en().value());
                if (msg->periph_pwm().roll_brush_front().has_duty())
                    PwmRollBrushFrontDuty(msg->periph_pwm().roll_brush_front().duty().value());
                if (msg->periph_pwm().roll_brush_front().has_cycle_ns())
                    PwmRollBrushFrontCyclens(msg->periph_pwm().roll_brush_front().cycle_ns().value());
                if (msg->periph_pwm().roll_brush_front().has_positive_ns())
                    PwmRollBrushFrontPositivens(msg->periph_pwm().roll_brush_front().positive_ns().value());
                if (msg->periph_pwm().roll_brush_front().has_update_freq_ms())
                    PwmRollBrushFrontUpdateFreqms(msg->periph_pwm().roll_brush_front().update_freq_ms().value());
            }
            //roll brush rear
            if (msg->periph_pwm().has_roll_brush_rear()) {
                if (msg->periph_pwm().roll_brush_rear().has_dev())
                    PwmRollBrushRearName(msg->periph_pwm().roll_brush_rear().dev().value());
                if (msg->periph_pwm().roll_brush_rear().has_en())
                    PwmRollBrushRearStat(msg->periph_pwm().roll_brush_rear().en().value());
                if (msg->periph_pwm().roll_brush_rear().has_duty())
                    PwmRollBrushRearDuty(msg->periph_pwm().roll_brush_rear().duty().value());
                if (msg->periph_pwm().roll_brush_rear().has_cycle_ns())
                    PwmRollBrushRearCyclens(msg->periph_pwm().roll_brush_rear().cycle_ns().value());
                if (msg->periph_pwm().roll_brush_rear().has_positive_ns())
                    PwmRollBrushRearPositivens(msg->periph_pwm().roll_brush_rear().positive_ns().value());
                if (msg->periph_pwm().roll_brush_rear().has_update_freq_ms())
                    PwmRollBrushRearUpdateFreqms(msg->periph_pwm().roll_brush_rear().update_freq_ms().value());
            }
            //suck fan 1
            if (msg->periph_pwm().has_suck_fan_1()) {
                if (msg->periph_pwm().suck_fan_1().has_dev())
                    PwmSuckFanName(msg->periph_pwm().suck_fan_1().dev().value());
                if (msg->periph_pwm().suck_fan_1().has_en())
                    PwmSuckFanStat(msg->periph_pwm().suck_fan_1().en().value());
                if (msg->periph_pwm().suck_fan_1().has_duty())
                    PwmSuckFanDuty(msg->periph_pwm().suck_fan_1().duty().value());
                if (msg->periph_pwm().suck_fan_1().has_cycle_ns())
                    PwmSuckFanCyclens(msg->periph_pwm().suck_fan_1().cycle_ns().value());
                if (msg->periph_pwm().suck_fan_1().has_positive_ns())
                    PwmSuckFanPositivens(msg->periph_pwm().suck_fan_1().positive_ns().value());
                if (msg->periph_pwm().suck_fan_1().has_update_freq_ms())
                    PwmSuckFanUpdateFreqms(msg->periph_pwm().suck_fan_1().update_freq_ms().value());
            }
            //pwm charge batery
            if (msg->periph_pwm().has_battery_charge()) {
                if (msg->periph_pwm().battery_charge().has_dev())
                    PwmBatteryName(msg->periph_pwm().battery_charge().dev().value());
                if (msg->periph_pwm().battery_charge().has_en())
                    PwmBatteryStat(msg->periph_pwm().battery_charge().en().value());
                if (msg->periph_pwm().battery_charge().has_duty())
                    PwmBatteryDuty(msg->periph_pwm().battery_charge().duty().value());
                if (msg->periph_pwm().battery_charge().has_cycle_ns())
                    PwmBatteryCyclens(msg->periph_pwm().battery_charge().cycle_ns().value());
                if (msg->periph_pwm().battery_charge().has_positive_ns())
                    PwmBatteryPositivens(msg->periph_pwm().battery_charge().positive_ns().value());
                if (msg->periph_pwm().battery_charge().has_update_freq_ms())
                    PwmBatteryUpdateFreqms(msg->periph_pwm().battery_charge().update_freq_ms().value());
            }
        }
        //gpio
        if (msg->has_periph_gpio()) {
            //emergency stop
            if (msg->periph_gpio().has_emergency_stop()) {
                if (msg->periph_gpio().emergency_stop().has_dev())
                    GpioEmergencyStopName(msg->periph_gpio().emergency_stop().dev().value());
                if (msg->periph_gpio().emergency_stop().has_en())
                    GpioEmergencyStopStat(msg->periph_gpio().emergency_stop().en().value());
                if (msg->periph_gpio().emergency_stop().has_gear())
                    GpioEmergencyStopGear(msg->periph_gpio().emergency_stop().gear().value());
                if (msg->periph_gpio().emergency_stop().has_on_ms())
                    GpioEmergencyStopOnms(msg->periph_gpio().emergency_stop().on_ms().value());
                if (msg->periph_gpio().emergency_stop().has_off_ms())
                    GpioEmergencyStopOffms(msg->periph_gpio().emergency_stop().off_ms().value());
                if (msg->periph_gpio().emergency_stop().has_update_freq_ms())
                    GpioEmergencyStopUpdateFreqms(msg->periph_gpio().emergency_stop().update_freq_ms().value());
            }
            //emergency break
            if (msg->periph_gpio().has_emergency_break()) {
                if (msg->periph_gpio().emergency_break().has_dev())
                    GpioEmergencyBreakName(msg->periph_gpio().emergency_break().dev().value());
                if (msg->periph_gpio().emergency_break().has_en())
                    GpioEmergencyBreakStat(msg->periph_gpio().emergency_break().en().value());
                if (msg->periph_gpio().emergency_break().has_gear())
                    GpioEmergencyBreakGear(msg->periph_gpio().emergency_break().gear().value());
                if (msg->periph_gpio().emergency_break().has_on_ms())
                    GpioEmergencyBreakOnms(msg->periph_gpio().emergency_break().on_ms().value());
                if (msg->periph_gpio().emergency_break().has_off_ms())
                    GpioEmergencyBreakOffms(msg->periph_gpio().emergency_break().off_ms().value());
                if (msg->periph_gpio().emergency_break().has_update_freq_ms())
                    GpioEmergencyBreakUpdateFreqms(msg->periph_gpio().emergency_break().update_freq_ms().value());
            }
            //veichle key
            if (msg->periph_gpio().has_veichle_key()) {
                if (msg->periph_gpio().veichle_key().has_dev())
                    GpioKeyName(msg->periph_gpio().veichle_key().dev().value());
                if (msg->periph_gpio().veichle_key().has_en())
                    GpioKeyStat(msg->periph_gpio().veichle_key().en().value());
                if (msg->periph_gpio().veichle_key().has_gear())
                    GpioKeyGear(msg->periph_gpio().veichle_key().gear().value());
                if (msg->periph_gpio().veichle_key().has_on_ms())
                    GpioKeyOnms(msg->periph_gpio().veichle_key().on_ms().value());
                if (msg->periph_gpio().veichle_key().has_off_ms())
                    GpioKeyOffms(msg->periph_gpio().veichle_key().off_ms().value());
                if (msg->periph_gpio().veichle_key().has_update_freq_ms())
                    GpioKeyUpdateFreqms(msg->periph_gpio().veichle_key().update_freq_ms().value());
            }
            //gear adjust
            if (msg->periph_gpio().has_gear_adjust()) {
                if (msg->periph_gpio().gear_adjust().has_dev())
                    GpioGearName(msg->periph_gpio().gear_adjust().dev().value());
                if (msg->periph_gpio().gear_adjust().has_en())
                    GpioGearStat(msg->periph_gpio().gear_adjust().en().value());
                if (msg->periph_gpio().gear_adjust().has_gear())
                    GpioGearGear(msg->periph_gpio().gear_adjust().gear().value());
                if (msg->periph_gpio().gear_adjust().has_on_ms())
                    GpioGearOnms(msg->periph_gpio().gear_adjust().on_ms().value());
                if (msg->periph_gpio().gear_adjust().has_off_ms())
                    GpioGearOffms(msg->periph_gpio().gear_adjust().off_ms().value());
                if (msg->periph_gpio().gear_adjust().has_update_freq_ms())
                    GpioGearUpdateFreqms(msg->periph_gpio().gear_adjust().update_freq_ms().value());
            }
            //logo led
            if (msg->periph_gpio().has_logo_led()) {
                if (msg->periph_gpio().logo_led().has_dev())
                    GpioLogoLedName(msg->periph_gpio().logo_led().dev().value());
                if (msg->periph_gpio().logo_led().has_en())
                    GpioLogoLedStat(msg->periph_gpio().logo_led().en().value());
                if (msg->periph_gpio().logo_led().has_gear())
                    GpioLogoLedGear(msg->periph_gpio().logo_led().gear().value());
                if (msg->periph_gpio().logo_led().has_on_ms())
                    GpioLogoLedOnms(msg->periph_gpio().logo_led().on_ms().value());
                if (msg->periph_gpio().logo_led().has_off_ms())
                    GpioLogoLedOffms(msg->periph_gpio().logo_led().off_ms().value());
                if (msg->periph_gpio().logo_led().has_update_freq_ms())
                    GpioLogoLedUpdateFreqms(msg->periph_gpio().logo_led().update_freq_ms().value());
            }
            //turn left led
            if (msg->periph_gpio().has_turn_left_led()) {
                if (msg->periph_gpio().turn_left_led().has_dev())
                    GpioTurnLeftLedName(msg->periph_gpio().turn_left_led().dev().value());
                if (msg->periph_gpio().turn_left_led().has_en())
                    GpioTurnLeftLedStat(msg->periph_gpio().turn_left_led().en().value());
                if (msg->periph_gpio().turn_left_led().has_gear())
                    GpioTurnLeftLedGear(msg->periph_gpio().turn_left_led().gear().value());
                if (msg->periph_gpio().turn_left_led().has_on_ms())
                    GpioTurnLeftLedOnms(msg->periph_gpio().turn_left_led().on_ms().value());
                if (msg->periph_gpio().turn_left_led().has_off_ms())
                    GpioTurnLeftLedOffms(msg->periph_gpio().turn_left_led().off_ms().value());
                if (msg->periph_gpio().turn_left_led().has_update_freq_ms())
                    GpioTurnLeftLedUpdateFreqms(msg->periph_gpio().turn_left_led().update_freq_ms().value());
            }
            //turn right led
            if (msg->periph_gpio().has_turn_right_led()) {
                if (msg->periph_gpio().turn_right_led().has_dev())
                    GpioTurnRightLedName(msg->periph_gpio().turn_right_led().dev().value());
                if (msg->periph_gpio().turn_right_led().has_en())
                    GpioTurnRightLedStat(msg->periph_gpio().turn_right_led().en().value());
                if (msg->periph_gpio().turn_right_led().has_gear())
                    GpioTurnRightLedGear(msg->periph_gpio().turn_right_led().gear().value());
                if (msg->periph_gpio().turn_right_led().has_on_ms())
                    GpioTurnRightLedOnms(msg->periph_gpio().turn_right_led().on_ms().value());
                if (msg->periph_gpio().turn_right_led().has_off_ms())
                    GpioTurnRightLedOffms(msg->periph_gpio().turn_right_led().off_ms().value());
                if (msg->periph_gpio().turn_right_led().has_update_freq_ms())
                    GpioTurnRightLedUpdateFreqms(msg->periph_gpio().turn_right_led().update_freq_ms().value());
            }
            //head lamp led
            if (msg->periph_gpio().has_headlight_led()) {
                if (msg->periph_gpio().headlight_led().has_dev())
                    GpioHeadLightLedName(msg->periph_gpio().headlight_led().dev().value());
                if (msg->periph_gpio().headlight_led().has_en())
                    GpioHeadLightLedStat(msg->periph_gpio().headlight_led().en().value());
                if (msg->periph_gpio().headlight_led().has_gear())
                    GpioHeadLightLedGear(msg->periph_gpio().headlight_led().gear().value());
                if (msg->periph_gpio().headlight_led().has_on_ms())
                    GpioHeadLightLedOnms(msg->periph_gpio().headlight_led().on_ms().value());
                if (msg->periph_gpio().headlight_led().has_off_ms())
                    GpioHeadLightLedOffms(msg->periph_gpio().headlight_led().off_ms().value());
                if (msg->periph_gpio().headlight_led().has_update_freq_ms())
                    GpioHeadLightLedUpdateFreqms(msg->periph_gpio().headlight_led().update_freq_ms().value());
            }
            //drain valve
            if (msg->periph_gpio().has_drain_valve()) {
                if (msg->periph_gpio().drain_valve().has_dev())
                    GpioDrainValveName(msg->periph_gpio().drain_valve().dev().value());
                if (msg->periph_gpio().drain_valve().has_en())
                    GpioDrainValveStat(msg->periph_gpio().drain_valve().en().value());
                if (msg->periph_gpio().drain_valve().has_gear())
                    GpioDrainValveGear(msg->periph_gpio().drain_valve().gear().value());
                if (msg->periph_gpio().drain_valve().has_on_ms())
                    GpioDrainValveOnms(msg->periph_gpio().drain_valve().on_ms().value());
                if (msg->periph_gpio().drain_valve().has_off_ms())
                    GpioDrainValveOffms(msg->periph_gpio().drain_valve().off_ms().value());
                if (msg->periph_gpio().drain_valve().has_update_freq_ms())
                    GpioDrainValveUpdateFreqms(msg->periph_gpio().drain_valve().update_freq_ms().value());
            }
            //water fill
            if (msg->periph_gpio().has_water_fill_valve()) {
                if (msg->periph_gpio().water_fill_valve().has_dev())
                    GpioFillValveName(msg->periph_gpio().water_fill_valve().dev().value());
                if (msg->periph_gpio().water_fill_valve().has_en())
                    GpioFillValveStat(msg->periph_gpio().water_fill_valve().en().value());
                if (msg->periph_gpio().water_fill_valve().has_gear())
                    GpioFillValveGear(msg->periph_gpio().water_fill_valve().gear().value());
                if (msg->periph_gpio().water_fill_valve().has_on_ms())
                    GpioFillValveOnms(msg->periph_gpio().water_fill_valve().on_ms().value());
                if (msg->periph_gpio().water_fill_valve().has_off_ms())
                    GpioFillValveOffms(msg->periph_gpio().water_fill_valve().off_ms().value());
                if (msg->periph_gpio().water_fill_valve().has_update_freq_ms())
                    GpioFillValveUpdateFreqms(msg->periph_gpio().water_fill_valve().update_freq_ms().value());
            }
            //water spray
            if (msg->periph_gpio().has_water_spray_valve()) {
                if (msg->periph_gpio().water_spray_valve().has_dev())
                    GpioSprayValveName(msg->periph_gpio().water_spray_valve().dev().value());
                if (msg->periph_gpio().water_spray_valve().has_en())
                    GpioSprayValveStat(msg->periph_gpio().water_spray_valve().en().value());
                if (msg->periph_gpio().water_spray_valve().has_gear())
                    GpioSprayValveGear(msg->periph_gpio().water_spray_valve().gear().value());
                if (msg->periph_gpio().water_spray_valve().has_on_ms())
                    GpioSprayValveOnms(msg->periph_gpio().water_spray_valve().on_ms().value());
                if (msg->periph_gpio().water_spray_valve().has_off_ms())
                    GpioSprayValveOffms(msg->periph_gpio().water_spray_valve().off_ms().value());
                if (msg->periph_gpio().water_spray_valve().has_update_freq_ms())
                    GpioSprayValveUpdateFreqms(msg->periph_gpio().water_spray_valve().update_freq_ms().value());
            }
            //detergent tank
            if (msg->periph_gpio().has_detergent_valve()) {
                if (msg->periph_gpio().detergent_valve().has_dev())
                    GpioDetergentValveName(msg->periph_gpio().detergent_valve().dev().value());
                if (msg->periph_gpio().detergent_valve().has_en())
                    GpioDetergentValveStat(msg->periph_gpio().detergent_valve().en().value());
                if (msg->periph_gpio().detergent_valve().has_gear())
                    GpioDetergentValveGear(msg->periph_gpio().detergent_valve().gear().value());
                if (msg->periph_gpio().detergent_valve().has_on_ms())
                    GpioDetergentValveOnms(msg->periph_gpio().detergent_valve().on_ms().value());
                if (msg->periph_gpio().detergent_valve().has_off_ms())
                    GpioDetergentValveOffms(msg->periph_gpio().detergent_valve().off_ms().value());
                if (msg->periph_gpio().detergent_valve().has_update_freq_ms())
                    GpioDetergentValveUpdateFreqms(msg->periph_gpio().detergent_valve().update_freq_ms().value());
            }
            //water scrape
            if (msg->periph_gpio().has_water_scrape()) {
                if (msg->periph_gpio().water_scrape().has_dev())
                    GpioWaterScrapeName(msg->periph_gpio().water_scrape().dev().value());
                if (msg->periph_gpio().water_scrape().has_en())
                    GpioWaterScrapeStat(msg->periph_gpio().water_scrape().en().value());
                if (msg->periph_gpio().water_scrape().has_gear())
                    GpioWaterScrapeGear(msg->periph_gpio().water_scrape().gear().value());
                if (msg->periph_gpio().water_scrape().has_on_ms())
                    GpioWaterScrapeOnms(msg->periph_gpio().water_scrape().on_ms().value());
                if (msg->periph_gpio().water_scrape().has_off_ms())
                    GpioWaterScrapeOffms(msg->periph_gpio().water_scrape().off_ms().value());
                if (msg->periph_gpio().water_scrape().has_update_freq_ms())
                    GpioWaterScrapeUpdateFreqms(msg->periph_gpio().water_scrape().update_freq_ms().value());
            }
            //bump
            if (msg->periph_gpio().has_bump_1()) {
                if (msg->periph_gpio().bump_1().has_dev())
                    GpioBumpName(msg->periph_gpio().bump_1().dev().value());
                if (msg->periph_gpio().bump_1().has_en())
                    GpioBumpStat(msg->periph_gpio().bump_1().en().value());
                if (msg->periph_gpio().bump_1().has_gear())
                    GpioBumpGear(msg->periph_gpio().bump_1().gear().value());
                if (msg->periph_gpio().bump_1().has_on_ms())
                    GpioBumpOnms(msg->periph_gpio().bump_1().on_ms().value());
                if (msg->periph_gpio().bump_1().has_off_ms())
                    GpioBumpOffms(msg->periph_gpio().bump_1().off_ms().value());
                if (msg->periph_gpio().bump_1().has_update_freq_ms())
                    GpioBumpUpdateFreqms(msg->periph_gpio().bump_1().update_freq_ms().value());
            }
        }
        //infra red
        if (msg->has_periph_infra()) {
            //drop sensor
            if (msg->periph_infra().has_drop_sensor()) {
                if (msg->periph_infra().drop_sensor().has_dev())
                    InfraDropName(msg->periph_infra().drop_sensor().dev().value());
                if (msg->periph_infra().drop_sensor().has_en())
                    InfraDropStat(msg->periph_infra().drop_sensor().en().value());
                if (msg->periph_infra().drop_sensor().has_value())
                    InfraDropValue(msg->periph_infra().drop_sensor().value().value());
                if (msg->periph_infra().drop_sensor().has_update_freq_ms())
                    InfraDropUpdateFreqms(msg->periph_infra().drop_sensor().update_freq_ms().value());
            }
        }
        //soc
        if (msg->has_periph_soc()) {
            //camera
            if (msg->periph_soc().camera().size()) {
                for (auto x : msg->periph_soc().camera()) {
                    if (x.has_dev())
                        SocCameraDev(x.index(), x.dev().value());
                    if (x.has_en())
                        SocCameraStat(x.index(), x.en().value());
                }
            }
            //wifi
            if (msg->periph_soc().has_wifi()) {
                if (msg->periph_soc().wifi().has_mode()) {
                    SocWifiMode(msg->periph_soc().wifi().mode().value());
                }
                if (msg->periph_soc().wifi().has_ip()) {
                    SocWifiIp(msg->periph_soc().wifi().ip().value());
                }
                if (msg->periph_soc().wifi().has_mac()) {
                    SocWifiMac(msg->periph_soc().wifi().mac().value());
                }
                if (msg->periph_soc().wifi().has_dns()) {
                    SocWifiDns(msg->periph_soc().wifi().dns().value());
                }
                if (msg->periph_soc().wifi().has_info()) {
                    SocWifiInfo(msg->periph_soc().wifi().info().value());
                }
                if (msg->periph_soc().wifi().has_name()) {
                    SocWifiName(msg->periph_soc().wifi().name().value());
                }
                if (msg->periph_soc().wifi().has_bssid()) {
                    SocWifiBssid(msg->periph_soc().wifi().bssid().value());
                }
                if (msg->periph_soc().wifi().has_status()) {
                    SocWifiStat(msg->periph_soc().wifi().status().value());
                }
                if (msg->periph_soc().wifi().has_scan_list()) {
                    SocWifiScanList(msg->periph_soc().wifi().scan_list().value());
                }
                if (msg->periph_soc().wifi().has_ping_speed()) {
                    SocWifiPingms(msg->periph_soc().wifi().ping_speed().value());
                }
                if (msg->periph_soc().wifi().has_wifi_result()) {
                    SocWifiResult(msg->periph_soc().wifi().wifi_result().value());
                }
            }
            //4g
            if (msg->periph_soc().has_four_g()) {
                if (msg->periph_soc().four_g().has_dev()) {
                    Soc4GName(msg->periph_soc().four_g().dev().value());
                }
                if (msg->periph_soc().four_g().has_account()) {
                    Soc4GAccount(msg->periph_soc().four_g().account().value());
                }
            }
            //audio
            if (msg->periph_soc().has_audio()) {
                if (msg->periph_soc().audio().has_dev()) {
                    SocAudioName(msg->periph_soc().audio().dev().value());
                }
                if (msg->periph_soc().audio().has_pack()) {
                    SocAudioPack(msg->periph_soc().audio().pack().value());
                }
                if (msg->periph_soc().audio().has_volume()) {
                    SocAudioVolume(msg->periph_soc().audio().volume().value());
                }
                if (msg->periph_soc().audio().has_mute_sw()) {
                    SocAudioMuteSwitch(msg->periph_soc().audio().mute_sw().value());
                }
            }
            //lidar
            if (msg->periph_soc().has_lidar_info()) {
                if (msg->periph_soc().lidar_info().has_dev()) {
                    SocLidarName(msg->periph_soc().lidar_info().dev().value());
                }
                if (msg->periph_soc().lidar_info().has_en()) {
                    SocLidarStat(msg->periph_soc().lidar_info().en().value());
                }
                if (msg->periph_soc().lidar_info().has_update_freq_ms()) {
                    SocLidarFreq(msg->periph_soc().lidar_info().update_freq_ms().value());
                }
            }
            //imu
            if (msg->periph_soc().has_imu_info()) {
                if (msg->periph_soc().imu_info().has_dev()) {
                    SocImuName(msg->periph_soc().imu_info().dev().value());
                }
                if (msg->periph_soc().imu_info().has_en()) {
                    SocImuStat(msg->periph_soc().imu_info().en().value());
                }
                if (msg->periph_soc().imu_info().has_update_freq_ms()) {
                    SocImuFreq(msg->periph_soc().imu_info().update_freq_ms().value());
                }
            }
            //linelaser
            if (msg->periph_soc().has_llaser_info()) {
                if (msg->periph_soc().llaser_info().has_dev()) {
                    SocLlaserName(msg->periph_soc().llaser_info().dev().value());
                }
                if (msg->periph_soc().llaser_info().has_en()) {
                    SocLlaserStat(msg->periph_soc().llaser_info().en().value());
                }
                if (msg->periph_soc().llaser_info().has_update_freq_ms()) {
                    SocLlaserFreq(msg->periph_soc().llaser_info().update_freq_ms().value());
                }
            }
            //super sonic
            if (msg->periph_soc().has_super_sonic()) {
                if (msg->periph_soc().super_sonic().has_dev()) {
                    SocSsonicName(msg->periph_soc().super_sonic().dev().value());
                }
                if (msg->periph_soc().super_sonic().has_en()) {
                    SocSsonicStat(msg->periph_soc().super_sonic().en().value());
                }
                if (msg->periph_soc().super_sonic().has_update_freq_ms()) {
                    SocSsonicFreq(msg->periph_soc().super_sonic().update_freq_ms().value());
                }
            }
            //tof
            if (msg->periph_soc().has_tof_info()) {
                if (msg->periph_soc().tof_info().has_dev()) {
                    SocTofName(msg->periph_soc().tof_info().dev().value());
                }
                if (msg->periph_soc().tof_info().has_en()) {
                    SocTofStat(msg->periph_soc().tof_info().en().value());
                }
                if (msg->periph_soc().tof_info().has_update_freq_ms()) {
                    SocTofFreq(msg->periph_soc().tof_info().update_freq_ms().value());
                }
            }
            //js
            if (msg->periph_soc().has_joy_stick()) {
                if (msg->periph_soc().joy_stick().has_dev()) {
                    SocJsName(msg->periph_soc().joy_stick().dev().value());
                }
                if (msg->periph_soc().joy_stick().has_en()) {
                    SocJsStat(msg->periph_soc().joy_stick().en().value());
                }
                if (msg->periph_soc().joy_stick().has_update_freq_ms()) {
                    SocJsFreq(msg->periph_soc().joy_stick().update_freq_ms().value());
                }
            }
        }
        //respose control messages
        //TODO
        if (msg->has_response()) {
        }
    }

    ///1, Liquid info
    void SysProcessor::CleanTankName(const std::string&) {
    }

    void SysProcessor::CleanTankStat(const bool) {
    }

    void SysProcessor::CleanTankLevel(const uint32_t) {
    }

    void SysProcessor::CleanTankEmpty(const uint32_t) {
    }

    void SysProcessor::CleanTankLow(const uint32_t) {
    }

    void SysProcessor::CleanTankFull(const uint32_t) {
    }

    void SysProcessor::CleanTankUpdataFreqms(const uint32_t) {
    }

    void SysProcessor::DirtyTankName(const std::string&) {
    }

    void SysProcessor::DirtyTankStat(const bool) {
    }

    void SysProcessor::DirtyTankLevel(const uint32_t) {
    }

    void SysProcessor::DirtyTankEmpty(const uint32_t) {
    }

    void SysProcessor::DirtyTankLow(const uint32_t) {
    }

    void SysProcessor::DirtyTankFull(const uint32_t) {
    }

    void SysProcessor::DirtyTankUpdateFreqms(const uint32_t) {
    }

    void SysProcessor::DustBoxName(const std::string&) {
    }

    void SysProcessor::DustBoxStat(const bool) {
    }

    void SysProcessor::DustBoxLevel(const uint32_t) {
    }

    void SysProcessor::DustBoxEmpty(const uint32_t) {
    }

    void SysProcessor::DustBoxLow(const uint32_t) {
    }

    void SysProcessor::DustBoxFull(const uint32_t) {
    }

    void SysProcessor::DustBoxUpdataFreqms(const uint32_t) {
    }

    void SysProcessor::DetergentTankName(const std::string&) {
    }

    void SysProcessor::DetergentTankStat(const bool) {
    }

    void SysProcessor::DetergentTankLevel(const uint32_t) {
    }

    void SysProcessor::DetergentTankEmpty(const uint32_t) {
    }

    void SysProcessor::DetergentTankLow(const uint32_t) {
    }

    void SysProcessor::DetergentTankFull(const uint32_t) {
    }

    void SysProcessor::DetergentTankUpdataFreqms(const uint32_t) {
    }

    ///2, ADC info
    void SysProcessor::DiscBrushLeftName(const std::string&) {
	}

    void SysProcessor::DiscBrushLeftStat(const bool) {
	}

    void SysProcessor::DiscBrushLeftVoltmv(const uint32_t) {
	}

    void SysProcessor::DiscBrushLeftCurrentma(const uint32_t) {
	}

    void SysProcessor::DiscBrushLeftTemperature(const uint32_t) {
	}

    void SysProcessor::DiscBrushLeftUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::DiscBrushRightName(const std::string&) {
	}

    void SysProcessor::DiscBrushRightStat(const bool) {
	}

    void SysProcessor::DiscBrushRightVoltmv(const uint32_t) {
	}

    void SysProcessor::DiscBrushRightCurrentma(const uint32_t) {
	}

    void SysProcessor::DiscBrushRightTemperature(const uint32_t) {
	}

    void SysProcessor::DiscBrushRightUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::RollBrushFrontName(const std::string&) {
	}

    void SysProcessor::RollBrushFrontStat(const bool) {
	}

    void SysProcessor::RollBrushFrontVoltmv(const uint32_t) {
	}

    void SysProcessor::RollBrushFrontCurrentma(const uint32_t) {
	}

    void SysProcessor::RollBrushFrontTemperature(const uint32_t) {
	}

    void SysProcessor::RollBrushFrontUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::RollBrushRearName(const std::string&) {
	}

    void SysProcessor::RollBrushRearStat(const bool) {
	}

    void SysProcessor::RollBrushRearVoltmv(const uint32_t) {
	}

    void SysProcessor::RollBrushRearCurrentma(const uint32_t) {
	}

    void SysProcessor::RollBrushRearTemperature(const uint32_t) {
	}

    void SysProcessor::RollBrushRearUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::SuckFanName(const std::string&) {
	}

    void SysProcessor::SuckFanStat(const bool) {
	}

    void SysProcessor::SuckFanVoltmv(const uint32_t) {
	}

    void SysProcessor::SuckFanCurrentma(const uint32_t) {
	}

    void SysProcessor::SuckFanTemperature(const uint32_t) {
	}

    void SysProcessor::SuckFanUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::WheelLeftName(const std::string&) {
	}

    void SysProcessor::WheelLeftStat(const bool) {
	}

    void SysProcessor::WheelLeftVoltmv(const uint32_t) {
	}

    void SysProcessor::WheelLeftCurrentma(const uint32_t) {
	}

    void SysProcessor::WheelLeftTemperature(const uint32_t) {
	}

    void SysProcessor::WheelLeftUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::WheelRightName(const std::string&) {
	}

    void SysProcessor::WheelRightStat(const bool) {
    }

    void SysProcessor::WheelRightVoltmv(const uint32_t) {
    }

    void SysProcessor::WheelRightCurrentma(const uint32_t) {
    }

    void SysProcessor::WheelRightTemperature(const uint32_t) {
    }

    void SysProcessor::WheelRightUpdateFreqms(const uint32_t) {
    }

    void SysProcessor::BatteryName(const std::string&) {
    }

    void SysProcessor::BatteryStat(const bool) {
    }

    void SysProcessor::BatteryVoltmv(const uint32_t) {
    }

    void SysProcessor::BatteryCurrentma(const uint32_t) {
    }

    void SysProcessor::BatteryTemperature(const uint32_t) {
    }

    void SysProcessor::BatteryUpdateFreqms(const uint32_t) {
    }

    ///3, PWM info
    void SysProcessor::PwmDiscBrushLeftName(const std::string&) {
	}

    void SysProcessor::PwmDiscBrushLeftStat(const bool) {
	}

    void SysProcessor::PwmDiscBrushLeftDuty(const uint32_t) {
	}

    void SysProcessor::PwmDiscBrushLeftCyclens(const uint32_t) {
	}

    void SysProcessor::PwmDiscBrushLeftPositivens(const uint32_t) {
	}

    void SysProcessor::PwmDiscBrushLeftUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::PwmDiscBrushRightName(const std::string&) {
	}

    void SysProcessor::PwmDiscBrushRightStat(const bool) {
	}

    void SysProcessor::PwmDiscBrushRightDuty(const uint32_t) {
	}

    void SysProcessor::PwmDiscBrushRightCyclens(const uint32_t) {
	}

    void SysProcessor::PwmDiscBrushRightPositivens(const uint32_t) {
	}

    void SysProcessor::PwmDiscBrushRightUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::PwmRollBrushFrontName(const std::string&) {
	}

    void SysProcessor::PwmRollBrushFrontStat(const bool) {
	}

    void SysProcessor::PwmRollBrushFrontDuty(const uint32_t) {
	}

    void SysProcessor::PwmRollBrushFrontCyclens(const uint32_t) {
	}

    void SysProcessor::PwmRollBrushFrontPositivens(const uint32_t) {
	}

    void SysProcessor::PwmRollBrushFrontUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::PwmRollBrushRearName(const std::string&) {
	}

    void SysProcessor::PwmRollBrushRearStat(const bool) {
	}

    void SysProcessor::PwmRollBrushRearDuty(const uint32_t) {
	}

    void SysProcessor::PwmRollBrushRearCyclens(const uint32_t) {
	}

    void SysProcessor::PwmRollBrushRearPositivens(const uint32_t) {
	}

    void SysProcessor::PwmRollBrushRearUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::PwmSuckFanName(const std::string&) {
	}

    void SysProcessor::PwmSuckFanStat(const bool) {
	}

    void SysProcessor::PwmSuckFanDuty(const uint32_t) {
	}

    void SysProcessor::PwmSuckFanCyclens(const uint32_t) {
	}

    void SysProcessor::PwmSuckFanPositivens(const uint32_t) {
	}

    void SysProcessor::PwmSuckFanUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::PwmBatteryName(const std::string&) {
	}

    void SysProcessor::PwmBatteryStat(const bool) {
	}

    void SysProcessor::PwmBatteryDuty(const uint32_t) {
	}

    void SysProcessor::PwmBatteryCyclens(const uint32_t) {
	}

    void SysProcessor::PwmBatteryPositivens(const uint32_t) {
	}

    void SysProcessor::PwmBatteryUpdateFreqms(const uint32_t) {
	}

    ///4, GPIO info
    void SysProcessor::GpioEmergencyStopName(const std::string&) {
	}

    void SysProcessor::GpioEmergencyStopStat(const bool) {
	}

    void SysProcessor::GpioEmergencyStopGear(const uint32_t) {
	}

    void SysProcessor::GpioEmergencyStopOnms(const uint32_t) {
	}

    void SysProcessor::GpioEmergencyStopOffms(const uint32_t) {
	}

    void SysProcessor::GpioEmergencyStopUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::GpioEmergencyBreakName(const std::string&) {
	}

    void SysProcessor::GpioEmergencyBreakStat(const bool) {
	}

    void SysProcessor::GpioEmergencyBreakGear(const uint32_t) {
	}

    void SysProcessor::GpioEmergencyBreakOnms(const uint32_t) {
	}

    void SysProcessor::GpioEmergencyBreakOffms(const uint32_t) {
	}

    void SysProcessor::GpioEmergencyBreakUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::GpioKeyName(const std::string&) {
	}

    void SysProcessor::GpioKeyStat(const bool) {
	}

    void SysProcessor::GpioKeyGear(const uint32_t) {
	}

    void SysProcessor::GpioKeyOnms(const uint32_t) {
	}

    void SysProcessor::GpioKeyOffms(const uint32_t) {
	}

    void SysProcessor::GpioKeyUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::GpioGearName(const std::string&) {
	}

    void SysProcessor::GpioGearStat(const bool) {
	}

    void SysProcessor::GpioGearGear(const uint32_t) {
	}

    void SysProcessor::GpioGearOnms(const uint32_t) {
	}

    void SysProcessor::GpioGearOffms(const uint32_t) {
	}

    void SysProcessor::GpioGearUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::GpioLogoLedName(const std::string&) {
	}

    void SysProcessor::GpioLogoLedStat(const bool) {
	}

    void SysProcessor::GpioLogoLedGear(const uint32_t) {
	}

    void SysProcessor::GpioLogoLedOnms(const uint32_t) {
	}

    void SysProcessor::GpioLogoLedOffms(const uint32_t) {
	}

    void SysProcessor::GpioLogoLedUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::GpioTurnLeftLedName(const std::string&) {
	}

    void SysProcessor::GpioTurnLeftLedStat(const bool) {
	}

    void SysProcessor::GpioTurnLeftLedGear(const uint32_t) {
	}

    void SysProcessor::GpioTurnLeftLedOnms(const uint32_t) {
	}

    void SysProcessor::GpioTurnLeftLedOffms(const uint32_t) {
	}

    void SysProcessor::GpioTurnLeftLedUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::GpioTurnRightLedName(const std::string&) {
	}

    void SysProcessor::GpioTurnRightLedStat(const bool) {
	}

    void SysProcessor::GpioTurnRightLedGear(const uint32_t) {
	}

    void SysProcessor::GpioTurnRightLedOnms(const uint32_t) {
	}

    void SysProcessor::GpioTurnRightLedOffms(const uint32_t) {
	}

    void SysProcessor::GpioTurnRightLedUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::GpioHeadLightLedName(const std::string&) {
	}

    void SysProcessor::GpioHeadLightLedStat(const bool) {
	}

    void SysProcessor::GpioHeadLightLedGear(const uint32_t) {
	}

    void SysProcessor::GpioHeadLightLedOnms(const uint32_t) {
	}

    void SysProcessor::GpioHeadLightLedOffms(const uint32_t) {
	}

    void SysProcessor::GpioHeadLightLedUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::GpioDrainValveName(const std::string&) {
	}

    void SysProcessor::GpioDrainValveStat(const bool) {
	}

    void SysProcessor::GpioDrainValveGear(const uint32_t) {
	}

    void SysProcessor::GpioDrainValveOnms(const uint32_t) {
	}

    void SysProcessor::GpioDrainValveOffms(const uint32_t) {
	}

    void SysProcessor::GpioDrainValveUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::GpioFillValveName(const std::string&) {
	}

    void SysProcessor::GpioFillValveStat(const bool) {
	}

    void SysProcessor::GpioFillValveGear(const uint32_t) {
	}

    void SysProcessor::GpioFillValveOnms(const uint32_t) {
	}

    void SysProcessor::GpioFillValveOffms(const uint32_t) {
	}

    void SysProcessor::GpioFillValveUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::GpioSprayValveName(const std::string&) {
	}

    void SysProcessor::GpioSprayValveStat(const bool) {
	}

    void SysProcessor::GpioSprayValveGear(const uint32_t) {
	}

    void SysProcessor::GpioSprayValveOnms(const uint32_t) {
	}

    void SysProcessor::GpioSprayValveOffms(const uint32_t) {
	}

    void SysProcessor::GpioSprayValveUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::GpioDetergentValveName(const std::string&) {
	}

    void SysProcessor::GpioDetergentValveStat(const bool) {
	}

    void SysProcessor::GpioDetergentValveGear(const uint32_t) {
	}

    void SysProcessor::GpioDetergentValveOnms(const uint32_t) {
	}

    void SysProcessor::GpioDetergentValveOffms(const uint32_t) {
	}

    void SysProcessor::GpioDetergentValveUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::GpioWaterScrapeName(const std::string&) {
	}

    void SysProcessor::GpioWaterScrapeStat(const bool) {
	}

    void SysProcessor::GpioWaterScrapeGear(const uint32_t) {
	}

    void SysProcessor::GpioWaterScrapeOnms(const uint32_t) {
	}

    void SysProcessor::GpioWaterScrapeOffms(const uint32_t) {
	}

    void SysProcessor::GpioWaterScrapeUpdateFreqms(const uint32_t) {
	}

    void SysProcessor::GpioBumpName(const std::string&) {
	}

    void SysProcessor::GpioBumpStat(const bool) {
	}

    void SysProcessor::GpioBumpGear(const uint32_t) {
	}

    void SysProcessor::GpioBumpOnms(const uint32_t) {
    }

    void SysProcessor::GpioBumpOffms(const uint32_t) {
    }

    void SysProcessor::GpioBumpUpdateFreqms(const uint32_t) {
    }

    ///5, INFRA info
    void SysProcessor::InfraDropName(const std::string&) {
    }

    void SysProcessor::InfraDropStat(const bool) {
    }

    void SysProcessor::InfraDropValue(const uint32_t) {
    }

    void SysProcessor::InfraDropUpdateFreqms(const uint32_t) {
    }

    ///6, SOC info
    void SysProcessor::SocCameraDev(const uint32_t, const std::string&) {
	}

    void SysProcessor::SocCameraStat(const uint32_t, const bool) {
	}

    void SysProcessor::SocWifiMode(const std::string&) {
	}

    void SysProcessor::SocWifiIp(const std::string&) {
	}

    void SysProcessor::SocWifiMac(const std::string&) {
	}

    void SysProcessor::SocWifiDns(const std::string&) {
	}

    void SysProcessor::SocWifiInfo(const std::string&) {
	}

    void SysProcessor::SocWifiName(const std::string&) {
	}

    void SysProcessor::SocWifiBssid(const std::string&) {
	}

    void SysProcessor::SocWifiStat(const std::string&) {
	}

    void SysProcessor::SocWifiScanList(const std::string&) {
	}

    void SysProcessor::SocWifiIsOnline(const std::string&) {
	}

    void SysProcessor::SocWifiPingms(const uint32_t) {
	}

    void SysProcessor::SocWifiResult(const std::string&) {
	}

    void SysProcessor::Soc4GName(const std::string&) {
	}

    void SysProcessor::Soc4GAccount(const uint32_t) {
	}

    void SysProcessor::SocAudioName(const std::string&) {
	}

    void SysProcessor::SocAudioPack(const std::string&) {
	}

    void SysProcessor::SocAudioVolume(const uint32_t) {
	}

    void SysProcessor::SocAudioMuteSwitch(const bool) {
	}

    void SysProcessor::SocLidarName(const std::string&) {
	}

    void SysProcessor::SocLidarStat(const bool) {
	}

    void SysProcessor::SocLidarFreq(const uint32_t) {
	}

    void SysProcessor::SocImuName(const std::string&) {
	}

    void SysProcessor::SocImuStat(const bool) {
	}

    void SysProcessor::SocImuFreq(const uint32_t) {
	}

    void SysProcessor::SocLlaserName(const std::string&) {
	}

    void SysProcessor::SocLlaserStat(const bool) {
	}

    void SysProcessor::SocLlaserFreq(const uint32_t) {
	}

    void SysProcessor::SocSsonicName(const std::string&) {
	}

    void SysProcessor::SocSsonicStat(const bool) {
	}

    void SysProcessor::SocSsonicFreq(const uint32_t) {
	}

    void SysProcessor::SocTofName(const std::string&) {
	}

    void SysProcessor::SocTofStat(const bool) {
	}

    void SysProcessor::SocTofFreq(const uint32_t) {
	}

    void SysProcessor::SocJsName(const std::string&) {
	}

    void SysProcessor::SocJsStat(const bool) {
	}

    void SysProcessor::SocJsFreq(const uint32_t) {
	}


} //namespace brain
} //namespace camb
} //namespace mstf
