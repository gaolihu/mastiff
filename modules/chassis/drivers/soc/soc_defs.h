/*
 * @Date: 2023-11-03 20:29:02
 * @LastEditors: xianweijing
 * @FilePath: /aventurier_framework/modules/chassis/drivers/soc/soc_defs.h
 * @Description: Copyright (c) 2023 ShenZhen Aventurier Co. Ltd All rights reserved.
 */

#pragma once

#include "modules/chassis/proto/chassis_config.pb.h"
#include "modules/chassis/proto/input_output_chs.pb.h"

using namespace google;
using namespace protobuf;

namespace mstf {
namespace chss {
namespace driver {
using namespace /*mstf::chss::*/ proto;
using SocDataListener = std::function<void(const Message &)>;
}  // namespace driver
}  // namespace chss
}  // namespace mstf
