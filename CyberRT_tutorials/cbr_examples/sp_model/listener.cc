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

#if 0
#else
#include "modules/chassis/proto/input_output_chs.pb.h"
#endif

void MessageCallback(
#if 0
    const std::shared_ptr<apollo::cyber::cbr_examples::proto::Chatter>& msg) {
  AINFO << "Received message seq-> " << msg->seq();
  AINFO << "msgcontent->" << msg->content();
#else
    //const std::shared_ptr<ventura::common_msgs::geometry_msgs::Twist>& msg) {
    const std::shared_ptr<ventura::common_msgs::sensor_msgs::Imu>& msg) {
  AINFO << "msgcontent->" << msg->DebugString();
#endif
}

int main(int argc, char* argv[]) {
  // init cyber framework
  apollo::cyber::Init(argv[0]);
  // create listener node
  auto listener_node = apollo::cyber::CreateNode("listener");
  // create listener
  auto listener =
#if 0
      listener_node->CreateReader<apollo::cyber::cbr_examples::proto::Chatter>(
          "channel/chatter", MessageCallback);
#else
      //listener_node->CreateReader<ventura::common_msgs::geometry_msgs::Twist>(
      listener_node->CreateReader<ventura::common_msgs::sensor_msgs::Imu>(
          "imu", MessageCallback);
#endif
  apollo::cyber::WaitForShutdown();
  return 0;
}
