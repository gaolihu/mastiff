#pragma once

#include "cyber/cyber.h"
#include "cyber/common/log.h"
#include "cyber/common/macros.h"
#include "cyber/message/message_traits.h"

#include "modules/chassis/proto/chss_io.pb.h"
#include "modules/chassis/proto/chassis_config.pb.h"

namespace mstf {
namespace chss {
namespace device {

    using namespace google::protobuf;

    class MsgsImplItf {
        public:
            MsgsImplItf(MsgsImplItf&) = delete;
            MsgsImplItf(const MsgsImplItf&) = delete;
            MsgsImplItf& operator=(const MsgsImplItf&) = delete;

            explicit MsgsImplItf() {
#if 0
                AINFO << "template impl base itf";
#endif
            }

            virtual ~MsgsImplItf() {
            }

            virtual void MsgsFlowoutControl(bool sw) {}
            virtual void MsgsFlowinControl(bool sw) {}

            //dispatch
            virtual int MessagePublish(const
                    std::shared_ptr<Message>& msg) { return 0; }

            //subscribe, obsolete
            virtual int MessageReceive(const
                    std::shared_ptr<Message>& msg) { return 0; }
    };

} //namespace device
} //namespace chss
} //namespace mstf
