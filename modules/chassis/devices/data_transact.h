#pragma once

#include <unordered_map>

//#include "Eigen/Core"
//#include "Eigen/Dense"

#include "cyber/common/log.h"
#include "cyber/common/util.h"
#include "cyber/common/macros.h"

#include "modules/chassis/proto/input_output_chs.pb.h"

#include "modules/chassis/devices/device_base_itf.h"
#include "modules/chassis/parser/parser_base_itf.h"

namespace mstf {
namespace chss {
namespace device {

    using namespace /*mstf::chss::*/proto;
    using namespace /*mstf::chss::*/parser;

    using ProtoMessagePublish = std::function<int(Message*, const std::string&)>;

    class DataTransact {
        public:
            DECLARE_SINGLETON(DataTransact)
            virtual ~DataTransact();

        public:
            void  Finish() {
                delete this;
            }

            int RegisterDevice(const std::string&,
                    const SensorIndicator&, DeviceBaseItf*);
            int RecvChassCtrl(const std::shared_ptr<ChassisCtrl>&);
            inline void RegisterPublisher(const ProtoMessagePublish& p) {
                proto_publisher_ = p;
            }

        private:
            //upstream
            int DataRelease(google::protobuf::Message*,
                    const std::string&);

            //downstream
            int CtrlMotor(const SpeedCtrl&);
            int CtrlMcu(const McuPeripheralCtrl&);
            int CtrlSoc(const DownToSocData&);

            //helper for iterate sensor & parser pairs
            ParserBaseItf* GetSensorIndicator(EE_DEVICE_TYPE type) const {
                for (auto it = device_parser_pair_.begin();
                        it != device_parser_pair_.end(); it++) {
                    auto ind_ptr = it->second;
                    if (ind_ptr->type() == type)
                        return reinterpret_cast<ParserBaseItf*>(
                                it->first->GetDeviceParser());
                }
                return nullptr;
            }

            ParserBaseItf* GetSensorIndicatorByMain(EE_DEV_MAIN type) const {
                for (auto it = device_parser_pair_.begin();
                        it != device_parser_pair_.end(); it++) {
                    auto ind_ptr = it->second;
                    if (ind_ptr->dev_main() == type)
                        return reinterpret_cast<ParserBaseItf*>(
                                it->first->GetDeviceParser());
                }
                return nullptr;
            }

        private:
            //std::shared_ptr<DispactchRaw> dispatcher_ = nullptr;
            std::unordered_map<DeviceBaseItf*, SensorIndicator*> device_parser_pair_;
            //std::unique_ptr<LidarUndistortion> lidar_undistortion_ = nullptr;
            ProtoMessagePublish proto_publisher_ = nullptr;
    };

} //namespace device
} //namespace chss
} //namespace mstf
