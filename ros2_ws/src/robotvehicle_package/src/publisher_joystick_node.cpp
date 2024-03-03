#include <chrono>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "Utilities/SerialInterface.h"

// teensy 3.2
#define SERIAL_DEVICE_NAME "/dev/ttyACM1"
#define ROS_INFO RCUTILS_LOG_INFO

using namespace std::chrono_literals;

class JoyStickPublisher : public rclcpp::Node
{
    public:
      JoyStickPublisher()
      : Node("minimal_publisher"), count_(0)
      {
        serialInterface = new SerialInterface(SERIAL_DEVICE_NAME);
        if (serialInterface->hasResponse()) {
            ROS_INFO(serialInterface->getResponse()->c_str());
        }

        publisher_ = this->create_publisher<std_msgs::msg::String>("topic_joy_stick", 10);
        timer_ = this->create_wall_timer(
          100ms, std::bind(&JoyStickPublisher::timer_callback, this));
      }

    private:
      void timer_callback()
      {
        auto message = std_msgs::msg::String();
        if (serialInterface->hasResponse()) {
            message.data =  serialInterface->getResponse()->c_str();
            publisher_->publish(message);
            RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
        }
      }
      rclcpp::TimerBase::SharedPtr timer_;
      rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
      size_t count_;
      SerialInterface * serialInterface;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<JoyStickPublisher>());
  rclcpp::shutdown();
  return 0;
}
