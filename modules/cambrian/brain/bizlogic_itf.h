#pragma once

#include "modules/cambrian/brain/bizlogic/bizness_client.h"

namespace mstf {
namespace camb {
namespace brain {

    class BizlogicItf {
        public:
            BizlogicItf() {
#ifdef CAMB_PKG_DBG
                AINFO << "BizlogicItf construct";
#endif
                bz_client_ = std::make_unique<BiznessClient>();
                bz_client_->BuildMsgChannel(std::bind(
                            &BizlogicItf::DispachMessage,
                            this, std::placeholders::_1),
                        std::bind(&BizlogicItf::DispachDrvSlam,
                            this, std::placeholders::_1,
                            std::placeholders::_2),
                        std::bind(&BizlogicItf::DispachDrvNavi,
                            this, std::placeholders::_1,
                            std::placeholders::_2));
            }
            virtual ~BizlogicItf() {
#ifdef CAMB_PKG_DBG
                AINFO << "BizlogicItf de-construct";
#endif
            }

            virtual int DispachMessage(const
                    std::shared_ptr<Message>&) = 0;
            virtual int DispachDrvSlam(const
                    std::shared_ptr<DriveSlam>& rqs,
                    std::shared_ptr<SlamDriveAck>& rsp) = 0;
            virtual int DispachDrvNavi(const
                    std::shared_ptr<DriveNavigation>& rqs,
                    std::shared_ptr<NavigationAck>& rsp) = 0;

            void inline ExecRobotWork(
                    const EE_ROBOT_MODE mode,
                    const EE_ROBOT_STATUS stat,
                    const bool is_new) {
                bz_client_->ExecRobotWork(mode, stat, is_new);
            }

        private:
            //bizlogic client
            std::unique_ptr<BiznessClient> bz_client_;
    };

} //namespace brain
} //namespace camb
} //namespace mstf
