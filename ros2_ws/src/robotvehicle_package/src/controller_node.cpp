#include <cstdio>
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "SerialInterface.h"
#include "AwaitTimer.h"
#include <Eigen/Dense>
#include <fstream>
#include "OdomRangeLog.h";

using Eigen::MatrixXd;
using Eigen::Vector3d;

using std::placeholders::_1;

#define ROS_INFO RCUTILS_LOG_INFO
#define M_PI 3.1415926535897932384626433832795
#define RAD2DEG(x) ((x)*180./M_PI)

#define SERIAL_DEVICE_NAME "/dev/ttyACM0"


class ReadingLaser : public rclcpp::Node {
public:

    ReadingLaser() : Node("reading_laser") {
        auto default_qos = rclcpp::QoS(rclcpp::SystemDefaultsQoS());
        serialInterface = new SerialInterface(SERIAL_DEVICE_NAME);
        output = new std::string();
        odomStr = new std::string();
        if (serialInterface->hasResponse()) {
            ROS_INFO(serialInterface->getResponse()->c_str());
        }
        // return to main menu
        serialInterface->sendRequest("0");
        // enter manual driving mode
        serialInterface->sendRequest("1");
        subscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
                "scan", default_qos,
                std::bind(&ReadingLaser::topic_callback, this, _1));

        timer_ = this->create_wall_timer(std::chrono::milliseconds (1),
                                      std::bind(&ReadingLaser::timer_callback, this));
        // drive forward.
        serialInterface->sendRequest("y");
    }

private:
    uint64_t getSystemMillis() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    void topic_callback(const sensor_msgs::msg::LaserScan::SharedPtr scan) {
        currentScan = scan;
        scanReady=true;
        /*
        int count = scan->scan_time / scan->time_increment;
        time_cur = getSystemMillis();
        time_diff = time_cur-time_last;
        output->append(" time diff=");
        output->append(std::to_string(time_diff));
        output->append(" ms");
        ROS_INFO(+output->c_str());
        time_last = time_cur;
        output->clear();
        */
         //ROS_INFO("I heard a laser scan %s[%d]:", scan->header.frame_id.c_str(), count);
        //ROS_INFO("angle_rang         e, %f, %f", RAD2DEG(scan->angle_min), RAD2DEG(scan->angle_max));
        //ROS_INFO("----------------------------- scan received %d-----------------------------------",count);
        /*
        for(int i = 0; i < count; i++) {
            float degree = RAD2DEG(scan->angle_min + scan->angle_increment * i);
                    ROS_INFO(": [% i,%f, %f]",i, degree, scan->ranges[i]);
        }
         */
        //ROS_INFO("----------------------------- end of scan -----------------------------------");
    }
    void timer_callback(){
        if(scanReady) {
            scanReady=false;
            //ROS_INFO("scan found");
            serialInterface->sendRequest("p");
            odomStr->append(serialInterface->getResponse()->c_str());
            //ROS_INFO(odomStr->c_str());
            posLeft = std::stod(odomStr->substr(0, odomStr->find(" ")));
            posRight = std::stod(odomStr->substr(odomStr->find(" "), odomStr->size()));
            sensorLogger[logCount] = new OdomRangeLog(posLeft, posRight, currentScan);
            odomStr->clear();
            logCount++;
            //std::cout << n << std::endl;
            if (n > 2000) {
                // stop driving
                serialInterface->sendRequest("s");
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
                    json->append(std::to_string(sensorLogger[i]->getPosRight())).append("\n");
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
        std::cout << n << std::endl;
    }
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscription_;
    rclcpp::TimerBase::SharedPtr timer_;
    SerialInterface * serialInterface;
    std::string *output;
    uint64_t time_cur=0;
    uint64_t time_diff=0;
    uint64_t time_last=0;
    OdomRangeLog  * sensorLogger[5000];
    std::string *odomStr;
    double posLeft;
    double posRight;
    sensor_msgs::msg::LaserScan::SharedPtr currentScan;
    uint64_t n=0;
    uint64_t logCount =0;
    bool scanReady=false;
    std::string *json = new std::string();
};


int main(int argc, char ** argv)
{
    /*
    serialInterface = new SerialInterface(SERIAL_DEVICE_NAME);
    if (serialInterface->hasResponse()) {
        ROS_INFO(serialInterface->getResponse()->c_str());
    }
    serialInterface->sendRequest("0");
    serialInterface->sendRequest("6");
    uint64_t n=0;
    std::string *output;
    while (1) {
        try {
            serialInterface->sendRequest("p");
        }
        catch(const std::runtime_error &ex){
            serialInterface->reopen();
            ROS_INFO(ex.what());
        }
        output = serialInterface->getResponse();
        output->append(" ");
        output->append(std::to_string(n));
        ROS_INFO(output->c_str());
        n++;
        serialInterface->flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    */
    //serialInterface->close();
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ReadingLaser>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}

/*
// simulation data;
double d_sr = 1;
double d_sl = 1;
// distance between wheels in cm
double b= 27;
// angle of robot
// initial position
double phi = 0;
Vector3d p(30,30,phi);
// Q covariance constants
//kl  = 0.10;
//kr  = 0.10;
// %%%%%%%%%%%%%%%%%%% position estimate %%%%%%%%%%%%%%%%%%%%%%
double  d_phi = (d_sr - d_sl)/b;
// distance traveled
double   d_s   = (d_sr + d_sl)/2;

// kinematic model for differential driver page 270
Vector3d p_t(d_s*cos(phi + d_phi/2),
             d_s*sin(phi + d_phi/2),
             d_phi);
Vector3d p_hat = p + p_t;
std::cout << p_hat << std::endl;

//convert rad to degrees
p_hat(3) = rad2deg(p_hat(3));
% transpose to make plotting easier
p_hat = p_hat';
x = p_hat(:,1);
y = p_hat(:,2);
% plot
        figure
plot(x,y);
title('differential mobile robot trajectory')
xlabel('x [cm]')
ylabel('y [cm]')
xlim([0 50])
ylim([0 50])
grid on

     %%%%%%%%%%%% covariance of estimated pose %%%%%%%%%%%%%%%%%

                                          % covariance page 270
P_last = [0 0 0;...
0 0 0;...
0 0 0;];
Fx = [1 0 -d_s.*sin(phi + d_phi/2);...
0 1  d_s.*cos(phi + d_phi/2);...
0 0  1];
Fu = [ 1/2*cos(phi + d_phi/2)-d_s/(2*b)*sin(phi + d_phi/2) , 1/2*cos(phi + d_phi/2)+d_s/(2*b)*sin(phi + d_phi/2);...
1/2*sin(phi + d_phi/2)+d_s/(2*b)*cos(phi + d_phi/2) , 1/2*sin(phi + d_phi/2)-d_s/(2*b)*cos(phi + d_phi/2);...
1/b                                                 , -1/b];

Q = [kr*abs(d_sr) 0;...
0  kl*abs(d_sl)];

P_hat = Fx*P_last*Fx' + Fu*Q*Fu'
*/


