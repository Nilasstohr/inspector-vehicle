#include <cstdio>
#include "rclcpp/rclcpp.hpp"
#include "Utilities/SerialInterface.h"
#include <Eigen/Dense>
#include <fstream>
#include <std_msgs/msg/string.hpp>
#include "Sensor/SensorRecorder.h"
#include "Sensor/SensorData.h"
#include "Localization/KalmanLocalization.h"
#include "TestKalmanFilterOffLine.h"
#include "Host/DriverInterface.h"
#include "Navigation/Navigator.h"

using Eigen::MatrixXd;
using Eigen::Vector3d;

using std::placeholders::_1;

#define ROS_INFO RCUTILS_LOG_INFO
#define M_PI 3.1415926535897932384626433832795
#define RAD2DEG(x) ((x)*180./M_PI)
// teensy 4.1
#define SERIAL_DEVICE_NAME "/dev/ttyACM0"
#define RECORD_DURATION_SECONDS 80

class ReadingLaser : public rclcpp::Node {
public:
    ReadingLaser() : Node("reading_laser") {
        auto default_qos = rclcpp::QoS(rclcpp::SystemDefaultsQoS());
        serialInterface = new SerialInterface(SERIAL_DEVICE_NAME);
        driverInterface = new DriverInterface(serialInterface);
        localization = new KalmanLocalization(driverInterface);
        navigator = new Navigator(driverInterface);
        navigationPath = new NavigationPath();
        navigationPath->addPathPoint(50,40,0);
        navigationPath->addPathPoint(60,40,0);
        navigationPath->addPathPoint(70,40,0);
        navigationPath->addPathPoint(80,40,0);
        navigationPath->addPathPoint(90,40,0);
        navigationPath->addPathPoint(100,40,0);
        navigationPath->addPathPoint(110,40,0);
        navigationPath->addPathPoint(120,40,0);
        navigationPath->addPathPoint(130,40,0);
        navigationPath->addPathPoint(140,40,0);
        navigationPath->addPathPoint(150,40,0);
        navigationPath->addPathPoint(160,40,0);
        navigationPath->addPathPoint(170,40,0);
        navigationPath->addPathPoint(180,45,0);
        navigationPath->addPathPoint(190,50,0);
        navigationPath->addPathPoint(200,54,0);
        navigator->setNavigationPath(navigationPath);
        recorder->startRecord(RECORD_DURATION_SECONDS);
        cout << "starting run" << endl;
        if (serialInterface->hasResponse()) {
            ROS_INFO(serialInterface->getResponse()->c_str());
        }

        laserScanSubscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>("scan",default_qos,
                std::bind(&ReadingLaser::topic_callback, this, _1));

        posePublisher_ = this->create_publisher<std_msgs::msg::String>("topic_pose_string", 10);

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
                localization->build(currentScan);
                hasMapBeenBuild = true;
            }
            localization->update(currentScan);
            navigator->update(localization);

            auto message = std_msgs::msg::String();
            message.data = localization->getPoseLastString()->c_str();
            posePublisher_->publish(message);

            if(recorder->hasRecordTimeExceeded() || navigator->isDistinationReached()){
                timer_->cancel();
                laserScanSubscription_.reset();
                recorder->endRecord();
                message.data = "end";
                posePublisher_->publish(message);
                cout << "ending run" << endl;
                rclcpp::shutdown();
                return;
            }
            //timer->printElapsedTime();

        }
    }



    void topic_callback_joy_stick(const std_msgs::msg::String::SharedPtr msg) const
    {
        RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg->data.c_str());
    }

    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr laserScanSubscription_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr JoyStickSubscription_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr posePublisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    sensor_msgs::msg::LaserScan::SharedPtr currentScan;

    SerialInterface * serialInterface;
    DriverInterface * driverInterface;
    KalmanLocalization * localization;
    Navigator * navigator;
    NavigationPath * navigationPath;

    bool scanReady=false;
    bool hasMapBeenBuild=false;
    SensorRecorder * recorder = new SensorRecorder();
};


int main(int argc, char ** argv)
{

    //new TestKalmanFilterOffLine();
    //SerialInterface * serialInterface = new SerialInterface(SERIAL_DEVICE_NAME);
    //DriverInterface * driverInterface = new DriverInterface(serialInterface);
    //driverInterface->setAngulaelocity(4,4);

    rclcpp::init(argc, argv);
    auto node = std::make_shared<ReadingLaser>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
