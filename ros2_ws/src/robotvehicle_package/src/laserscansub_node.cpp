#include <cstdio>
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
using std::placeholders::_1;

#define ROS_INFO RCUTILS_LOG_INFO
#define M_PI 3.1415926535897932384626433832795
#define RAD2DEG(x) ((x)*180./M_PI)

class ReadingLaser : public rclcpp::Node {
public:

  ReadingLaser() : Node("reading_laser") {
      auto default_qos = rclcpp::QoS(rclcpp::SystemDefaultsQoS());
      subscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
            "scan", default_qos,
            std::bind(&ReadingLaser::topic_callback, this, _1));
  }

private:
    void topic_callback(const sensor_msgs::msg::LaserScan::SharedPtr scan) {
        int count = scan->scan_time / scan->time_increment;
        ROS_INFO("I heard a laser scan %s[%d]:", scan->header.frame_id.c_str(), count);
        ROS_INFO("angle_range, %f, %f", RAD2DEG(scan->angle_min), RAD2DEG(scan->angle_max));

        for(int i = 0; i < count; i++) {
            float degree = RAD2DEG(scan->angle_min + scan->angle_increment * i);
            ROS_INFO(": [%f, %f]", degree, scan->ranges[i]);
        }
  }

    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscription_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ReadingLaser>();
  RCLCPP_INFO(node->get_logger(), "Hello my friends");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}