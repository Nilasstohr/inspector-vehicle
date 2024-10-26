#include "rclcpp/rclcpp.hpp"
#include "Utilities/SerialInterface.h"
#include <Eigen/Dense>
#include <fstream>
#include "sensor_msgs/msg/laser_scan.hpp"
#include <std_msgs/msg/string.hpp>
#include "SensorRecorder.h"
#include "Sensor/SensorData.h"
#include "Navigation/Navigator.h"
#include "PathPlanning/GridMap.h"
#include "Configurations.h"
#include "Test/RecordHandler.h"
#include "MissionController.h"
#include "Test/SerialInterfaceStub.h"

using Eigen::MatrixXd;
using Eigen::Vector3d;

using std::placeholders::_1;

#define ROS_INFO RCUTILS_LOG_INFO
#define M_PI 3.1415926535897932384626433832795
#define RAD2DEG(x) ((x)*180./M_PI)

// teensy 4.1
#define RECORD_DURATION_SECONDS 70

class ControllerNode : public rclcpp::Node {
public:
    ControllerNode() : Node("reading_laser") {
        auto default_qos = rclcpp::QoS(rclcpp::SystemDefaultsQoS());
        SerialInterface *serialInterface = new SerialInterface(CONFIG_ROBOT_DRIVER_DEVICE_NAME);
        missionController = new MissionController(this,serialInterface);
        recorder->startRecord(RECORD_DURATION_SECONDS);
        cout << "starting run" << endl;
        laserScanSubscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>("scan",default_qos,
                std::bind(&ControllerNode::topic_callback, this, _1));
        timer_ = this->create_wall_timer(std::chrono::milliseconds (1),
                                    std::bind(&ControllerNode::timer_callback, this));

    }
private:
    void topic_callback(const sensor_msgs::msg::LaserScan::SharedPtr scan) {
        currentScan = scan;
        scanReady=true;
    }
    void timer_callback(){
        if(scanReady) {
            scanReady = false;
            missionController->getSensorData()->update(currentScan);
            missionController->update();
            if(recorder->hasRecordTimeExceeded() ||
               missionController->getNavigator()->isDestinationReached()){
                timer_->cancel();
                laserScanSubscription_.reset();
                recorder->endRecord();
                missionController->endMission();
                rclcpp::shutdown();
                return;
            }
        }
    }
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr laserScanSubscription_;
    rclcpp::TimerBase::SharedPtr timer_;
    sensor_msgs::msg::LaserScan::SharedPtr currentScan;
    MissionController * missionController;
    bool scanReady=false;
    SensorRecorder * recorder = new SensorRecorder();
};

class ControllerNodeEmulated : public rclcpp::Node {
public:
    ControllerNodeEmulated() : Node("reading_laser") {
        recordHandler = new RecordHandler();
        SerialInterfaceStub *serialInterface = new SerialInterfaceStub(
                CONFIG_ROBOT_DRIVER_DEVICE_NAME);
        missionController = new MissionController(this,serialInterface);
        timer_ = this->create_wall_timer(std::chrono::milliseconds (1),
                                         std::bind(&ControllerNodeEmulated::timer_callback, this));
    }
private:
    void timer_callback(){
        if(recordHandler->hasRecordsToProcess()){
            recordHandler->update(missionController->getSensorData());
            missionController->update();
        }else{
            timer_->cancel();
            missionController->endMission();
            rclcpp::shutdown();
            return;
        }

    }
    rclcpp::TimerBase::SharedPtr timer_;
    MissionController * missionController;
    RecordHandler * recordHandler;
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ControllerNode>();
    //auto node = std::make_shared<ControllerNodeEmulated>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
