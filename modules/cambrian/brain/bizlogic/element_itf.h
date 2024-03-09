#pragma once

#include "cyber/cyber.h"

#include "behaviortree_cpp/behavior_tree.h"
#include "behaviortree_cpp/bt_factory.h"

#include "modules/cambrian/proto/cambrian_arch.pb.h"
#include "modules/cambrian/proto/cambrian_data.pb.h"

namespace mstf {
namespace camb {
namespace brain {

    using namespace BT;

    using namespace /*mstf::camb::*/proto;
    using namespace /*mstf::*/chss::proto;

    class ElementItf {
        public:
            //bt factory register
            virtual void RegisterBtNodes(BehaviorTreeFactory*) = 0;

            //data channels
            virtual NodeStatus ElmClearCtrl() = 0;
            virtual NodeStatus ElmReleaseCtrl() = 0;

            //chassis ITFs
            virtual NodeStatus ElmChsCheckOnline() {
                return NodeStatus::FAILURE;
            };
    };


} //namespace brain
} //namespace camb
} //namespace mstf
