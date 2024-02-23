#pragma once

#include "cyber/common/log.h"
#include "cyber/common/util.h"
#include "cyber/common/macros.h"

#include "modules/chassis/proto/chss_io.pb.h"
#include "modules/chassis/proto/chassis_config.pb.h"

#include "modules/chassis/devices/device_base_itf.h"
#include "modules/chassis/parser/parser_base_itf.h"

namespace mstf {
namespace chss {
namespace device {

    using namespace /*mstf::chss::*/proto;
    using namespace /*mstf::chss::*/parser;

    class DevParseLink {
        public:
            DECLARE_SINGLETON(DevParseLink)
            virtual ~DevParseLink();

        public:
            void  Finish() {
#ifdef CHSS_PKG_DBG
                AINFO << "singleton DevParseLink finish";
#endif
                delete this;
            }

            int RegisterDevice(const
                    SensorIndicator&,
                    DeviceBaseItf*);

            int DeviceInit();
            int DeviceStart();
            int DeviceStop();
            int DeviceResume();
            int DeviceClose();

        private:
            int DeviceInit(const SensorIndicator*);
            int DeviceStart(const SensorIndicator*);
            int DeviceStop(const SensorIndicator*);
            int DeviceResume(const SensorIndicator*);
            int DeviceClose(const SensorIndicator*);

            int HandleChsMovement(const ChsMovementCtrl&);
            int HandleChsPeriphAdc(const ChsPeriphAdcCtrl&);
            int HandleChsPeriphPwm(const ChsPeriphPwmCtrl&);
            int HandleChsPeriphGpio(const ChsPeriphGpioCtrl&);
            int HandleChsPeriphInfra(const ChsPeriphInfraCtrl&);
            int HandleChsSocMisc(const ChsSocMiscCtrl&);
            int HandleChsUpdate(const ChsFirmWareUpdate&);

#if 1
        private:
            inline DeviceBaseItf* _GetDevBaseByType(
                    EE_DEVICE_TYPE type) const {
                for (auto it = ind_dev_pair_.begin();
                        it != ind_dev_pair_.end(); it++) {
                    auto ind_ptr = it->first;
                    if (ind_ptr->type() == type)
                        return it->second;
                }
                //AERROR << "no device base for type: " << type;
                return nullptr;
            }

#endif

        private:
            //device & indicator pair
            std::unordered_map<const SensorIndicator*,
                DeviceBaseItf*> ind_dev_pair_;
    };

} //namespace device
} //namespace chss
} //namespace mstf
