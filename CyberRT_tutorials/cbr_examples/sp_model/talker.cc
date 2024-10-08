/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include "CyberRT_tutorials/cbr_examples/proto/cbr_examples.pb.h"

#include "cyber/cyber.h"
#include "cyber/time/rate.h"
#include "cyber/time/time.h"

#if 0
#else
#include "modules/chassis/proto/chss_io.pb.h"
#endif

using apollo::cyber::Rate;
using apollo::cyber::Time;
using apollo::cyber::cbr_examples::proto::Chatter;

int main(int argc, char *argv[]) {
  // init cyber framework
  apollo::cyber::Init(argv[0]);
  // create talker node
#if 0
  auto talker_node = apollo::cyber::CreateNode("talker");
  // create talker
  auto talker = talker_node->CreateWriter<Chatter>("channel/chatter");
  Rate rate(3.0);
  uint64_t seq = 0;
  while (apollo::cyber::OK()) {
    auto msg = std::make_shared<Chatter>();
    msg->set_timestamp(Time::Now().ToNanosecond());
    msg->set_lidar_timestamp(Time::Now().ToNanosecond());
    msg->set_seq(seq);
    msg->set_content("Hello, apollo!");
    talker->Write(msg);
    AINFO << "talker sent a message! No. " << seq;
    seq++;
    rate.Sleep();
  }
#else
  auto talker_node = apollo::cyber::CreateNode("move");
  // create talker

  auto talker = talker_node->CreateWriter<ventura::common_msgs::geometry_msgs::Twist>("cmd_vel");
  Rate rate(0.1);
  uint64_t seq = 0;
  while (apollo::cyber::OK()) {
    auto msg = std::make_shared<ventura::common_msgs::geometry_msgs::Twist>();

    if (seq == 5) {
        msg->mutable_linear()->set_x(0.f);
    } else {
        msg->mutable_linear()->set_x(0.03f);
    }

    talker->Write(msg);

    AINFO << "talker sent a message! No. " << seq;
    seq++;
    rate.Sleep();
  }
#endif
  return 0;
}
