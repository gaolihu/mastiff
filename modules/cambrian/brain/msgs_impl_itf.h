#pragma once

namespace mstf {
namespace camb {
namespace brain {

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

} //namespace brain
} //namespace camb
} //namespace mstf
