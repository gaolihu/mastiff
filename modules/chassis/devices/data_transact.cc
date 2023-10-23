#include "modules/chassis/devices/data_transact.h"

namespace mstf {
namespace chss {
namespace device {

#define DISTANCE_BETWEEN_WHEELS 0.21818


#ifndef SET_BIT_VAL
#define SET_BIT_VAL(data, bit, val) \
    ((data) |= ((val) << (bit)))
#endif
#ifndef STT_BIT_VAL
#define STT_BIT_VAL(data, bit, v) \
    ((data) = (((data) & ~(1 << (bit))) | ((v) << (bit))))
#endif
#ifndef TST_BIT_VAL
#define TST_BIT_VAL(data, bit) \
    (((data) & (1 << (bit))) >> (bit))
#endif

    DataTransact::DataTransact() {
#ifdef CHSS_PKG_DBG
        AINFO << "DataTransact singleton construct";
#endif
    }

    DataTransact::~DataTransact() {
#ifdef CHSS_PKG_DBG
        AINFO << "DataTransact singleton de-construct!";
#endif
        for(auto it = device_parser_pair_.begin();
                it != device_parser_pair_.end(); it++) {
            AINFO << "de-construct devbase: " <<
                it->first->CheckDeviceInfo();
            //delete it->second;
        }
    }

    int DataTransact::RegisterDevice(const std::string& dev,
            const SensorIndicator& ind, DeviceBaseItf* snsr) {
        //message upstream handler
        std::function<int(const google::protobuf::Message*,
                const std::string&)> proto_proc =
            [this](const google::protobuf::Message* msg,
                    const std::string& type)->int {
                return DataRelease(const_cast<google::
                        protobuf::Message*>(msg), type);
            };
        device_parser_pair_[snsr] =
            const_cast<SensorIndicator*>(&ind);

        auto parser = reinterpret_cast<ParserBaseItf*>(snsr->
                GetDeviceParser());
        parser->SetProtobufHandle(proto_proc);

        AINFO << "Register msg publisher for: " << dev <<
            ", seq: " << device_parser_pair_.size() << "\n";

        // if(dev == "EAI-Lidar") {
        //     //TODO
        //     lidar_undistortion_ = std::make_unique<LidarUndistortion>();
        //     AINFO << "Init Lidar Undistortion Class";
        // }
        if(dev == "LD-Lidar") {
            //TODO
            //lidar_undistortion_ = std::make_unique<LidarUndistortion>();
            AINFO << "Init Lidar Undistortion Class";
        }

        return 0;
    }

    /*
    void DataTransact::RegisterDispatcher(const
            std::shared_ptr<DispactchRaw>& disp) {
#ifdef CHSS_PKG_DBG
        AINFO << "DataTransact register chassis dispatcher!";
#endif
        dispatcher_ = disp;
    }
    */

    //MCU data upstream
    int DataTransact::DataRelease(::google::protobuf::Message* msg,
            const std::string& type) {
        /*
        auto parser = GetSensorIndicator(E_DEVICE_MCU);
        if (parser == nullptr) {
            AWARN << "can't get parser for MCU";
            return -1;
        }
        //TODO
            
        */

        //find corresponding sensor and release the message
        //static bool first_laser = true;

        //bool process_imu = true;
        //bool process_odom = true;
        //bool process_hcr = true;

#ifdef CHSS_PKG_DBG
#if 0
        AWARN << "msg: " << msg->GetTypeName() <<
            ", contents:\n" << msg->DebugString();
#endif
#endif

        if (type == "McuReportComplicateData") {
            //4, imu
            auto imu_d = std::make_shared<IMUdata>();
            //if (parser->ConvertImuMsg(msg, imu_d) == 0) {
                //move assignment
                //lidar_undistortion_->ImuCallback(imu_d);
                // lidar_undistortion_->ImuMemory(imu_d);
                //dispatcher_->DataDispatchImu(imu_d);
            //} else {
                //process_imu = false;
            //}

            //7, odom
            auto odom_d = std::make_shared<OdomData>();
            //if (parser->ConvertOdomMsg(msg, odom_d) == 0) {
                //lidar_undistortion_->OdomCallback(odom_d);
                // lidar_undistortion_->OdomMemory(odom_d);
                //dispatcher_->DataDispatchOdom(odom_d);
            //} else {
                //process_odom = false;
            //}

            //1, tof
            //2, wheel current
            //3, dock sig
            //5, euler
            //6, wheel pulse
            //8, sys time
            //9, infra wall
            //10, wheel speed
            auto hcr_d = std::make_shared<HfChassisRaw>();
            //if (parser->ConvertHcrMsg(msg, hcr_d) == 0) {
                //dispatcher_->DataDispatchHcr(hcr_d);
            //} else {
                //process_hcr = false;
            //}

            /*
            if (!process_imu && !process_odom && !process_hcr) {
                ChassisMixData mix;
                if (parser->ConvertMixComopMsg(msg, mix) == 0) {
                    auto mmsg = std::make_shared<ChassisMixData>(mix);
                    //dispatcher_->DataDispatchMix(mmsg);
                }
            }
            */
        } else if (type == "ventura::common_msgs::sensor_msgs::PointCloud") {
            //lidar data
            /*
            auto l_parser = GetSensorIndicator(E_DEVICE_LIDAR);
            if (l_parser == nullptr) {
                AERROR << "can't get parser for lidar";
                return -1;
            }
            */

            if (proto_publisher_) {
                proto_publisher_(msg, type);
            }

            //if (l_parser->ConvertLaserMsg(msg, lsr) == 0) {
                // lidar_undistortion_->LaserMemory(lsr);
                //dispatcher_->DataDispatchLaser(std::make_shared<LaserData>(
                //lidar_undistortion_->ScanCallback(lsr)));
            //} else {
                //AERROR << "convert laser message error!";
            //}
        } else {
#ifdef CHSS_PKG_DBG
            if (type == "McuReportSimpleData") {
#if 1
                McuReportSimpleData* msd =
                    dynamic_cast<McuReportSimpleData*>(msg);
                if (msd->has_sdata()) {
                    if (msd->sdata().has_battery()) {
                        AINFO << "MCU report simple data(0x0401), battery";
                    } else if (msd->sdata().has_drop_stat()) {
                        AINFO << "MCU report simple data(0x0402), drop";
                    } else if (msd->sdata().has_bump_stat()) {
                        AINFO << "MCU report simple data(0x0403), bump";
                    } else if (msd->sdata().has_liftoff_stat()) {
                        AINFO << "MCU report simple data(0x0404), lift";
                    } else if (msd->sdata().has_dust_box()) {
                        AINFO << "MCU report simple data(0x0405), dust box";
                    } else if (msd->sdata().has_sonic_sig()) {
                        AINFO << "MCU report simple data(0x040e), sonic singnal";
                    } else if (msd->sdata().has_touch_dock_sig()) {
                        AINFO << "MCU report simple data(0x040a), dock signal";
                    } else if (msd->sdata().has_pallet_stat()) {
                        AINFO << "MCU report simple data(0x040f), pallte mop";
                    } else if (msd->sdata().has_batter_temp()) {
                        AINFO << "MCU report simple data(0x0411), battery temp";
                    } else if (msd->sdata().has_toggle_key()) {
                        AINFO << "MCU report simple data(0x040d), toggle key";
                    } else if (msd->sdata().has_no_water()) {
                        AINFO << "MCU report simple data(0x040d), no water";
                    } else if (msd->sdata().has_water_full()) {
                        AINFO << "MCU report simple data(dx0402), water full";
                    } else if (msd->sdata().has_error_code()) {
                        AINFO << "MCU report simple data(0x0409), error code";
                    } else if (msd->sdata().has_flip_cam_sig()) {
                        AINFO << "MCU report simple data(0x0410), camera flip";
                    } else {
                        AWARN << "MCU report simple data NULL?!";
                    }
                } else if (msd->has_water_tank()) {
                    AINFO << "MCU report simple data(0x0406), water tank";
                } else if (msd->has_key_type()) {
                    AINFO << "MCU report simple data(0x0407), key event";
                } else if (msd->has_wall_trig()) {
                    AINFO << "MCU report simple data(0x0408), wall triger";
                } else if (msd->has_low_power()) {
                    AINFO << "MCU report simple data(0x040b), low power";
                } else if (msd->has_middle_brush_current()) {
                    AINFO << "MCU report simple data(0x040c), m-brush current";
                } else {
                    AWARN << "MCU report simple data NULL:\n" << msd->DebugString();
                    //ignore
                    return -1;
                }
#endif
            } else if (type == "McuAckOnLineInfo") {
                AINFO << "MCU ack online info(0x8001):\n";
            } else if (type == "McuAckHeartBeat") {
                AINFO << "MCU ack heart beat(0x8002):\n";
            } else if (type == "McuAckSlaveDeviceInfo") {
                AINFO << "MCU ack slave device info(0x8101):\n";
            } else if (type == "McuAckSensorEnStat") {
                AINFO << "MCU ack sensor en stat(0x8102):\n";
            } else if (type == "McuAckCheckSensorInfo") {
                AINFO << "MCU ack check sensor info(0x8103):\n";
            } else if (type == "CtrlChassResult") {
                AINFO << "MCU ack control chassis result(0x8200):\n";
            } else if (type == "McuReportFactoryData") {
                 
                //TODO
                AINFO << "MCU report factory test:\n" <<
                    msg->DebugString();
            } else {
#ifdef CHSS_PKG_DBG
                AWARN << "unknown message type: " << type <<
                    ", contents:\n" << msg->DebugString();
                return -1;
#endif
            }
#endif
            if (type != "McuReportFactoryData") {
                //random mix data
                auto mix = std::make_shared<ChassisMixData>();
                //parser->ConvertMixMsg(msg, mix);

                //dispatcher_->DataDispatchMix(mix);
            } else {
                auto fac_d = std::make_shared<ChassisFacotryData>();
                //if (parser->ConvertFacMsg(msg, fac_d) == 0) {
                    //dispatcher_->DataDispatchFac(fac_d);
                //}

#if 0
                //6, image
                CameraCaptureFrame img_d;
                if (parser->ConvertImageMsg(msg, img_d) == 0) {
                    auto mmsg = std::make_shared<CameraCaptureFrame>(img_d);
                    dispatcher_->DataDispatchImg(mmsg);
                }
#endif
            }
        }

        return 0;
    }

    //downstream
    int DataTransact::RecvChassCtrl(const
            std::shared_ptr<ChassisCtrl>& ctrl) {
#ifdef CHSS_PKG_DBG
#if 0
        //too much noise
        AINFO << "receive chassis control:\n" <<
            ctrl->DebugString();
#endif
#endif

        int ret = -1;

        //1, ctrl wheel
        if (ctrl->has_speed_ctrl())
            if ((ret = CtrlMotor(ctrl->speed_ctrl())) != 0)
                AWARN << "wheel control problem!";

        //2, ctrl LED
        if (ctrl->has_mcu_ctrl())
            if ((ret = CtrlMcu(ctrl->mcu_ctrl())) != 0)
                AWARN << "mcu control problem!";

        //3, ctrl pump
        if (ctrl->has_soc_ctrl())
            if ((ret = CtrlSoc(ctrl->soc_ctrl())) != 0)
                AWARN << "soc control problem!";

        return ret;
    }

    int DataTransact::CtrlMotor(const SpeedCtrl& ctl_msg) {
        auto parser = GetSensorIndicator(E_DEVICE_MOTOR);
        if (parser != nullptr) {
            DownToServoData servo_data;
            servo_data.mutable_diff_speed()->
                set_linear(ctl_msg.linear());
            servo_data.mutable_diff_speed()->
                set_palstance(ctl_msg.angular());
            if (parser->WriteServoMessage(servo_data) < 0) {
                AWARN << "write servo speed msg error!";
                return -1;
            }
        } else {
            AWARN << "no parser for Motor, try MCU";
            parser = GetSensorIndicator(E_DEVICE_MCU);
            if (parser != nullptr) {
                //control speed by MCU
                //TODO
            } else {
                AERROR << "can't get parser for MCU";
                return -1;
            }
        }
        return 0;
    }

    int DataTransact::CtrlMcu(const McuPeripheralCtrl& mcu) {
        //TODO
        return 0;
    }

    int DataTransact::CtrlSoc(const DownToSocData& soc) {
        auto parser = GetSensorIndicatorByMain(EE_DEV_MAIN_SOC);
        if (parser == nullptr) {
            AWARN << "can't get parser for SOC";
            return -1;
        }

        if (parser->WriteSocMessage(soc) < 0) {
            AWARN << "parser for soc writing error!";
            return -1;
        }

        return 0;
    }

} //namespace device
} //namespace chss
} //namespace mstf
