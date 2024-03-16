#include <cstdio>
#include "rclcpp/rclcpp.hpp"
#include "Utilities/SerialInterface.h"
#include <Eigen/Dense>
#include <fstream>
#include <std_msgs/msg/string.hpp>
#include "Sensor/OdomRangeLog.h"
#include "Sensor/SensorRecorder.h"
#include "Sensor/SensorData.h"
#include "Localization/KalmanFilter.h"
#include "TestKalmanFilterOffLine.h"

using Eigen::MatrixXd;
using Eigen::Vector3d;

using std::placeholders::_1;

#define ROS_INFO RCUTILS_LOG_INFO
#define M_PI 3.1415926535897932384626433832795
#define RAD2DEG(x) ((x)*180./M_PI)
// teensy 4.1
#define SERIAL_DEVICE_NAME "/dev/ttyACM0"
#define RECORD_DURATION_SECONDS 5


class ReadingLaser : public rclcpp::Node {
public:
    ReadingLaser() : Node("reading_laser") {
        auto default_qos = rclcpp::QoS(rclcpp::SystemDefaultsQoS());
        serialInterface = new SerialInterface(SERIAL_DEVICE_NAME);
        kalmanFilterLive = new KalmanFilter(serialInterface);
        recorder->startRecord(RECORD_DURATION_SECONDS);
        cout << "starting run" << endl;
        if (serialInterface->hasResponse()) {
            ROS_INFO(serialInterface->getResponse()->c_str());
        }

      laserScanSubscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
                "scan", default_qos,
                std::bind(&ReadingLaser::topic_callback, this, _1));

        posePublisher_ = this->create_publisher<std_msgs::msg::String>("topic_pose_string", 10);


        //JoyStickSubscription_ = this->create_subscription<std_msgs::msg::String>(
        //        "topic_joy_stick", default_qos, std::bind(&ReadingLaser::topic_callback_joy_stick, this, _1));

        timer_ = this->create_wall_timer(std::chrono::milliseconds (1),
                                      std::bind(&ReadingLaser::timer_callback, this));

        // reset
        serialInterface->sendRequest("r");
    }
private:
    void topic_callback(const sensor_msgs::msg::LaserScan::SharedPtr scan) {
        currentScan = scan;
        scanReady=true;
    }
    void timer_callback(){
        if(scanReady) {
            scanReady = false;
            if (!hasMapBeenBuild) {
                kalmanFilterLive->build(currentScan);
                hasMapBeenBuild = true;
            }
            kalmanFilterLive->update(currentScan);

            //auto message = std_msgs::msg::String();
            //message.data = kalmanFilterLive->getPoseLastString()->c_str();
            //posePublisher_->publish(message);

            if(recorder->hasRecordTimeExceeded() || kalmanFilterLive->reachedMaxPoseStorage()){
                timer_->cancel();
                laserScanSubscription_.reset();
                recorder->endRecord();
                //kalmanFilterLive->printPoseStorage();
                cout << "ending run" << endl;
                cout << "publishing data" << endl;
                publishPoseStorage(kalmanFilterLive);
                 rclcpp::shutdown();
                return;
            }
            //timer->printElapsedTime();

        }
    }

    void publishPoseStorage(KalmanFilter * kalmanFilter){
        auto message = std_msgs::msg::String();
        int size = kalmanFilter->getLoggedNum()-1;
        for(int i=0; i<size; i++){
            cout << i << endl;
            message.data = kalmanFilter->getPoseStringByIndex(i)->c_str();
            posePublisher_->publish(message);
            this_thread::sleep_for(std::chrono::milliseconds (20));
        }
        message.data = "end";
        posePublisher_->publish(message);
    }

    void topic_callback_joy_stick(const std_msgs::msg::String::SharedPtr msg) const
    {
        RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg->data.c_str());
    }

    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr laserScanSubscription_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr JoyStickSubscription_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr posePublisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    SerialInterface * serialInterface;
    sensor_msgs::msg::LaserScan::SharedPtr currentScan;
    bool scanReady=false;
    KalmanFilter * kalmanFilterLive;
    bool hasMapBeenBuild=false;
    SensorRecorder * recorder = new SensorRecorder();
};


int main(int argc, char ** argv)
{
   new TestKalmanFilterOffLine();

    rclcpp::init(argc, argv);
    auto node = std::make_shared<ReadingLaser>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;

}
