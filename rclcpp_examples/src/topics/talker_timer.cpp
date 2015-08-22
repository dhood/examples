// Copyright 2015 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <chrono>
#include <iostream>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp/memory_strategies.hpp>

#include <std_msgs/msg/string.hpp>

using namespace rclcpp::memory_strategies::static_memory_strategy;
rclcpp::publisher::Publisher::SharedPtr g_chatter_pub;
static auto g_msg = std::make_shared<std_msgs::msg::String>();

void cb()
{
  static int counter = 0;
  static char buf_i_understand[100];
  //static auto msg = std::make_shared<std_msgs::msg::String>();
  //msg->data = "Hello World: " + std::to_string(counter++);
  //snprintf((char *)g_msg->data.data(), 256, "Hello, world! %d", counter++);
  snprintf(buf_i_understand, 
           sizeof(buf_i_understand), 
           "Hello, world! %d", counter++);
  g_msg->data.assign(buf_i_understand);
  //printf("Publishing: [%s]\n", msg->data.c_str());
  //printf("ahhh %08x\n", (unsigned)msg->data[0]
  std::cout << "Publishing: '" << g_msg->data.data() << "'" << std::endl;
  g_chatter_pub->publish(g_msg);
}

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  ObjectPoolBounds bounds;
  bounds.set_max_subscriptions(1).set_max_services(1).set_max_clients(1);
  bounds.set_max_executables(1).set_memory_pool_size(0);
  rclcpp::memory_strategy::MemoryStrategy::SharedPtr memory_strategy = 
    std::make_shared<StaticMemoryStrategy>(bounds);
  rclcpp::executors::SingleThreadedExecutor executor(memory_strategy);

  auto node = rclcpp::node::Node::make_shared("talker");
  g_chatter_pub = node->create_publisher<std_msgs::msg::String>
                    ("chatter", rmw_qos_profile_sensor_data);
  std::chrono::nanoseconds period_ns(500000000); // aka, one half-second
  auto timer = node->create_wall_timer
                 (period_ns, cb);
  g_msg->data.reserve(256);
  executor.add_node(node);
  executor.spin();
  //rclcpp::spin(node);
  return 0;
}
