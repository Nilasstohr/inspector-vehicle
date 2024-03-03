#include <cstdio>
#include "rclcpp/rclcpp.hpp"
#include "Utilities/SerialInterface.h"
#include "Utilities/AwaitTimer.h"
#include <Eigen/Dense>
#include <fstream>
#include <std_msgs/msg/string.hpp>
#include "Sensor/OdomRangeLog.h"
#include "Sensor/PointRectForm.h"
#include "Sensor/SensorRecorder.h"
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
#define RECORD_DURATION_SECONDS 10


class ReadingLaser : public rclcpp::Node {
public:
    ReadingLaser() : Node("reading_laser") {
        auto default_qos = rclcpp::QoS(rclcpp::SystemDefaultsQoS());
        serialInterface = new SerialInterface(SERIAL_DEVICE_NAME);
        odomStr = new std::string();
        kalmanFilterLive = new KalmanFilter();
        recorder->startRecord(RECORD_DURATION_SECONDS);
        cout << "starting run" << endl;
        if (serialInterface->hasResponse()) {
            ROS_INFO(serialInterface->getResponse()->c_str());
        }

      laserScanSubscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
                "scan", default_qos,
                std::bind(&ReadingLaser::topic_callback, this, _1));

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
            serialInterface->sendRequest("p");
            odomStr->append(serialInterface->getResponse()->c_str());
            //cout << "wheel dis: " << odomStr->c_str() << endl;
            posLeft = std::stod(odomStr->substr(0, odomStr->find(" ")));
            posRight = std::stod(odomStr->substr(odomStr->find(" "), odomStr->size()));
            //cout << "wd: "<< posLeft  << " " << posRight << endl;
            //timer->reset();
            recorder->writeNewRecord(posLeft,posRight);
            odomStr->clear();
            int scanPointsNum = currentScan->scan_time / currentScan->time_increment;
            for(int i = 0; i < scanPointsNum; i++) {
                angle = (currentScan->angle_min + currentScan->angle_increment * i)+M_PI;
                distance = currentScan->ranges[i]*100;
                if(!isinf(distance)){
                    //scanPolarForm->push_back(PointPolarForm(angle,distance));
                    recorder->writeScanPoint(angle,distance);
                }
            }
            if(recorder->hasRecordTimeExceeded()){
                timer_->cancel();
                laserScanSubscription_.reset();
                recorder->endRecord();
                timer_->cancel();
                cout << "ending run" << endl;
                rclcpp::shutdown();
                return;
            }
            //timer->printElapsedTime();
            /*
            if (!hasMapBeenBuild) {
                kalmanFilterLive->build(scanPolarForm);
                hasMapBeenBuild = true;
            }

            kalmanFilterLive->update(posLeft,posRight,scanPolarForm, true);
            scanPolarForm->clear();
            if(kalmanFilterLive->reachedMaxPoseStorage()){
                kalmanFilterLive->printPoseStorage();
                timer_->cancel();
            }
            */
        }
    }

    void topic_callback_joy_stick(const std_msgs::msg::String::SharedPtr msg) const
    {
        RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg->data.c_str());
    }
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr laserScanSubscription_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr JoyStickSubscription_;
    rclcpp::TimerBase::SharedPtr timer_;
    SerialInterface * serialInterface;
    OdomRangeLog  * sensorLogger[160000];
    std::vector<PointPolarForm> * scanPolarForm = new std::vector<PointPolarForm>;
    float angle;
    float distance;
    std::string *odomStr;
    double posLeft;
    double posRight;
    sensor_msgs::msg::LaserScan::SharedPtr currentScan;
    uint64_t n=0;
    uint64_t logCount =0;
    bool scanReady=false;
    std::string *json = new std::string();
    KalmanFilter * kalmanFilterLive;
    bool hasMapBeenBuild=false;
    AwaitTimer *timer = new AwaitTimer();
    SensorRecorder * recorder = new SensorRecorder();
};


int main(int argc, char ** argv)
{
   //new TestKalmanFilterOffLine();

    rclcpp::init(argc, argv);
    auto node = std::make_shared<ReadingLaser>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}

/*
  void timer_callback(){
        if(scanReady) {
            scanReady = false;
            //ROS_INFO("scan found");
            serialInterface->sendRequest("p");
            odomStr->append(serialInterface->getResponse()->c_str());
            //ROS_INFO(odomStr->c_str());
            posLeft = std::stod(odomStr->substr(0, odomStr->find(" ")));
            posRight = std::stod(odomStr->substr(odomStr->find(" "), odomStr->size()));
            sensorLogger[logCount] = new OdomRangeLog(posLeft, posRight, currentScan);

            if (!hasMapBeenBuild) {
                kalmanFilterLive->build(sensorLogger[logCount]->getScan());
                hasMapBeenBuild=true;
                return;
            }

            kalmanFilterLive->update( sensorLogger[logCount]->getPosLeft(),
                                      sensorLogger[logCount]->getPosRight(),
                                      sensorLogger[logCount]->getScan(),true);
             sensorLogger[logCount]->setPose(
                    kalmanFilterLive->getX(),kalmanFilterLive->getY(),kalmanFilterLive->getTheta());
             odomStr->clear();
            logCount++;
            //std::cout << n << std::endl;
            if (n > 140000) {
                // stop driving
                serialInterface->sendRequest("r");
                timer_->cancel();
                json->append("\n{").append("\n");
                json->append("\"data\":[").append("\n");
                for (int i = 0; i < logCount; i++) {
                    json->append("{").append("\n");
                    // odom
                    json->append("\"odom\":");
                    json->append("{").append("\n");
                    json->append("\"posLeft\":");
                    json->append(std::to_string(sensorLogger[i]->getPosLeft())).append(",\n");
                    json->append("\"posRight\":");
                    json->append(std::to_string(sensorLogger[i]->getPosRight())).append(",\n");
                    json->append("\"x\":");
                    json->append(std::to_string(sensorLogger[i]->getX())).append(",\n");
                    json->append("\"y\":");
                    json->append(std::to_string(sensorLogger[i]->getY())).append(",\n");
                    json->append("\"theta\":");
                    json->append(std::to_string(sensorLogger[i]->getTheta())).append("\n");
                    json->append("},").append("\n");
                    // laser scan
                    json->append("\"scan\":{").append("\n");
                    json->append("\"points\":[").append("\n");
                    int lenScans = sensorLogger[i]->getScan()->size() - 1;
                    for (int j = 0; j < lenScans; j++) {
                        json->append("{").append("\n");
                        json->append("\"angle\":");
                        json->append(std::to_string(sensorLogger[i]->getScan()->at(j).getAngle())).append(",\n");
                        json->append("\"distance\":");
                        json->append(std::to_string(sensorLogger[i]->getScan()->at(j).getDistance())).append("\n");
                        json->append("}");
                        if (j < lenScans - 1) {
                            json->append(",");
                        }
                        json->append("\n");
                    }
                    json->append("]").append("\n");
                    json->append("}").append("\n");
                    json->append("}");
                    if (i < logCount - 1) {
                        json->append(",");
                    }
                    json->append("\n");
                }
                json->append("]").append("\n");
                json->append("}");
                //ROS_INFO(json->c_str());
                std::ofstream out("log.json");
                out << json->c_str();
                out.close();
                json->clear();
            }
        }
        n++;
        //std::cout << n << std::endl;
    }
 */