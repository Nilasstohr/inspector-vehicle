#include <cstdio>
#include "rclcpp/rclcpp.hpp"
#include "Utilities/SerialInterface.h"
#include <Eigen/Dense>
#include <fstream>
#include <std_msgs/msg/string.hpp>
#include "nav_msgs/msg/occupancy_grid.hpp"
#include "Sensor/SensorRecorder.h"
#include "Sensor/SensorData.h"
#include "Localization/KalmanLocalization.h"
#include "Host/DriverInterface.h"
#include "Navigation/Navigator.h"
#include "PathPlanning/TestSearchAlgoritms.h"
#include "PathPlanning/GridMap.h"
#include "Configurations.h"
#include "Test/PlayBackTesting.h"

using Eigen::MatrixXd;
using Eigen::Vector3d;

using std::placeholders::_1;

#define ROS_INFO RCUTILS_LOG_INFO
#define M_PI 3.1415926535897932384626433832795
#define RAD2DEG(x) ((x)*180./M_PI)

// teensy 4.1
#define SERIAL_DEVICE_NAME "/dev/ttyACM0"
#define RECORD_DURATION_SECONDS 70

class ControllerNode : public rclcpp::Node {
public:
    ControllerNode() : Node("reading_laser") {
        auto default_qos = rclcpp::QoS(rclcpp::SystemDefaultsQoS());
        serialInterface = new SerialInterface(SERIAL_DEVICE_NAME);
        driverInterface = new DriverInterface(serialInterface);
        localization = new KalmanLocalization(driverInterface);
        gripMap = new GridMap(CONFIG_GRID_VALUE_FULL_AVAILABLE,
                              CONFIG_GRID_VALUE_FULL_OCCUPIED,
                              CONFIG_GRID_VALUE_UPDATE_INTERVAL);
        navigator = new Navigator(driverInterface);
        aStar = new AStar();
        gripMap->loadGridMap();
        navigationPath = getTestPath();
        navigator->setNavigationPath(navigationPath);
        recorder->startRecord(RECORD_DURATION_SECONDS);
        cout << "starting run" << endl;
        if (serialInterface->hasResponse()) {
            ROS_INFO(serialInterface->getResponse()->c_str());
        }

        laserScanSubscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>("scan",default_qos,
                std::bind(&ControllerNode::topic_callback, this, _1));

        posePublisher_ = this->create_publisher<std_msgs::msg::String>("topic_pose_string", 10);
        gridMapPublisher_ = this->create_publisher<std_msgs::msg::String>("topic_grid_map", 10);

        timer_ = this->create_wall_timer(std::chrono::milliseconds (1),
                                    std::bind(&ControllerNode::timer_callback, this));

        // reset
        serialInterface->sendRequest("r");
    }
private:
    NavigationPath * getTestPath(){
        PathPoint *endPoint = new PathPoint();
        endPoint->set(170,83);
        aStar->update(localization->getPose(),
                     endPoint, gripMap->updateMapWithObstacleSafeDistance());
        return aStar->getNavigationPath();

         //cout << "Navigation Path Found:" << aStar->pathToString() << endl;
        /*
        NavigationPath *navigationPath = new NavigationPath();
        navigationPath->addPathPoint(60,40,0);
        navigationPath->addPathPoint(77,34,0);
        navigationPath->addPathPoint(93,29,0);
        navigationPath->addPathPoint(110,27,0);
        navigationPath->addPathPoint(125,27,0);
        navigationPath->addPathPoint(140,27,0);
        navigationPath->addPathPoint(156,27,0);
        navigationPath->addPathPoint(174,26,0);
        navigationPath->addPathPoint(189,29,0);
        navigationPath->addPathPoint(200,39,0);
        navigationPath->addPathPoint(205,52,0);
        navigationPath->addPathPoint(205,65,0);
        navigationPath->addPathPoint(201,79,0);
        navigationPath->addPathPoint(190,89,0);
        navigationPath->addPathPoint(176,93,0);
        navigationPath->addPathPoint(163,96,0);
        navigationPath->addPathPoint(153,96,0);
        navigationPath->addPathPoint(143,96,0);
        navigationPath->addPathPoint(131,95,0);
        navigationPath->addPathPoint(119,90.5,0);
        navigationPath->addPathPoint(110,80,0);
        navigationPath->addPathPoint(100,70,0);
        navigationPath->addPathPoint(86,63,0);
        navigationPath->addPathPoint(70,55,0);
        navigationPath->addPathPoint(59,50,0);
        navigationPath->addPathPoint(50,45,0);
        navigationPath->addPathPoint(40,40,0);
        */

        /*
        navigationPath->addPathPoint(40,50,0);
        navigationPath->addPathPoint(40,60,0);
        navigationPath->addPathPoint(40,70,0);
        navigationPath->addPathPoint(40,80,0);
        navigationPath->addPathPoint(40,90,0);
        navigationPath->addPathPoint(50,90,0);
        navigationPath->addPathPoint(60,90,0);
        navigationPath->addPathPoint(70,90,0);
        navigationPath->addPathPoint(80,90,0);
        navigationPath->addPathPoint(90,90,0);
        navigationPath->addPathPoint(90,100,0);
        */
        //return navigationPath;
    }
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
            recorder->update(localization->getSensorDate());
            //gripMap->AStar(localization->getSensorDate()->getScanPolarForm(),localization->getPose());
            navigator->update(localization);
            //cout << *localization->getPoseLastString() << endl;
            auto message = std_msgs::msg::String();
            message.data = localization->getPoseLastString()->c_str();
            posePublisher_->publish(message);

            if(recorder->hasRecordTimeExceeded() || navigator->isDestinationReached()){
                timer_->cancel();
                laserScanSubscription_.reset();
                recorder->endRecord();
                message.data = "end";
                posePublisher_->publish(message);
                // public grid updateMapWithObstacleSafeDistance
                auto gridMapMessage = std_msgs::msg::String();
                gridMapMessage.data = gripMap->obstacleSafeDistanceMapToString()->c_str();
                gridMapPublisher_->publish(gridMapMessage);
                std::this_thread::sleep_for(200ms);
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
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr gridMapPublisher_;

    rclcpp::TimerBase::SharedPtr timer_;
    sensor_msgs::msg::LaserScan::SharedPtr currentScan;

    SerialInterface * serialInterface;
    DriverInterface * driverInterface;
    KalmanLocalization * localization;
    PlayBackTesting * testKalmanFilterOffLine;
    Navigator * navigator;
    NavigationPath * navigationPath;
    GridMap * gripMap;
    AStar * aStar;

    bool scanReady=false;
    bool hasMapBeenBuild=false;
    SensorRecorder * recorder = new SensorRecorder();
};

class ControllerNodeEmulated : public rclcpp::Node {

public:
    ControllerNodeEmulated() : Node("reading_laser") {
        playBackTesting = new PlayBackTesting();
        posePublisher_ = this->create_publisher<std_msgs::msg::String>("topic_pose_string", 10);
        gridMapPublisher_ = this->create_publisher<std_msgs::msg::String>("topic_grid_map", 10);
        cout << "starting run" << endl;
        timer_ = this->create_wall_timer(std::chrono::milliseconds (1),
                                         std::bind(&ControllerNodeEmulated::timer_callback, this));
    }
private:

    void timer_callback(){
        auto message = std_msgs::msg::String();

        //auto gridMapMessage = std_msgs::msg::String();
        //playBackTesting->getGripMap()->updateMapWithObstacleSafeDistance();
        //gridMapMessage.data = playBackTesting->getGripMap()->obstacleSafeDistanceMapToString()->c_str();
        //gridMapPublisher_->publish(gridMapMessage);

        if(playBackTesting->hasRecordsToProcess()){
            playBackTesting->update();

            //auto gridMapMessage = std_msgs::msg::String();
            //gridMapMessage.data = playBackTesting->getGripMap()->obstacleSafeDistanceMapToString()->c_str();
            //gridMapPublisher_->publish(gridMapMessage);

            message = std_msgs::msg::String();
            message.data = playBackTesting->getLocalization()->getPoseLastString()->c_str();
            posePublisher_->publish(message);
        }else{
            timer_->cancel();
            message.data = "end";
            posePublisher_->publish(message);
            publishGridAndNavigationPath();
            cout << "ending run" << endl;
            rclcpp::shutdown();
            return;
        }

    }

    void publishGridAndNavigationPath(){
        PathPoint *endPoint = new PathPoint();
        endPoint->set(150,60);
        AStar aStar;
        aStar.update(playBackTesting->getLocalization()->getPose(),
                    endPoint, playBackTesting->getGripMap()->updateMapWithObstacleSafeDistance());
        //cout << "Navigation Path Found:" << aStar->pathToString() << endl;
        string gridMapMessageString;
        gridMapMessageString.append(playBackTesting->getGripMap()->obstacleSafeDistanceMapToString()->c_str());
        gridMapMessageString.append( "\npath\n");
        gridMapMessageString.append(aStar.pathToString());
        auto gridMapMessage = std_msgs::msg::String();
        gridMapMessage.data = gridMapMessageString.c_str();
        //playBackTesting->getGripMap()->storeMap();
        gridMapPublisher_->publish(gridMapMessage);
        std::this_thread::sleep_for(200ms);
    }

    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr posePublisher_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr gridMapPublisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    PlayBackTesting * playBackTesting;
};

int main(int argc, char ** argv)
{
    //TestSearchAlgoritms();
    rclcpp::init(argc, argv);
    //auto node = std::make_shared<ControllerNode>();
    auto node = std::make_shared<ControllerNodeEmulated>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
