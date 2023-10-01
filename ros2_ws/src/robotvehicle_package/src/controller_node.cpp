#include <cstdio>
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "SerialInterface.h"
#include "AwaitTimer.h"
#include <Eigen/Dense>
#include <fstream>
#include "Sensor/OdomRangeLog.h"
#include "Localization/PredictionDifferentialDrive.h"
#include "Sensor/PointRectForm.h"
#include "Localization/Observations.h"
#include "Utilities/MathConversions.h"
#include "Localization/MeasurementPrediction.h"
#include "Localization/TestMap.h"

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

        MatrixXd R(2, 2);
        R(0, 0)= pow(MathConversions::deg2rad(2),2); R(0, 1)= 0;
        R(1, 0)= 0; R(1, 1)= pow(2,2);
        this->R = R;
        this->observations = new Observations(eps,R);
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

        //timer_ = this->create_wall_timer(std::chrono::milliseconds (1),
        //                              std::bind(&ReadingLaser::timer_callback, this));
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
        //OdomRangeLog *log = new OdomRangeLog(0, 0, currentScan);
        //observations->update(log->getScan());
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
    MatrixXd R;
    double eps = 1;
    Observations *observations;


};


int main(int argc, char ** argv)
{
    PredictionDifferentialDrive * differentialDrive =
            new PredictionDifferentialDrive();


    // for prediction step
    MatrixXd pEst(3, 3);
    double sigmaXY = 2; // cm
    double sigmaTheta = MathConversions::deg2rad(2); // degrees;
    pEst(0, 0)= pow(sigmaXY,2); pEst(0, 1)= 0; pEst(0, 2)= 0;
    pEst(1, 0)= 0; pEst(1, 1)= pow(sigmaXY,2); pEst(1, 2)= 0;
    pEst(2, 0)= 0; pEst(2, 1)= 0; pEst(2, 2)=  pow(sigmaTheta,2);
    Vector3d xEst(40,40,0);
    //printMatrix(pEstLast,"pEst");
    //printVector(xEstLast,"xEst");

    // for observations step
    double eps = 1;
    MatrixXd R(2, 2);
    R(0, 0)= pow(MathConversions::deg2rad(2),2); R(0, 1)= 0;
    R(1, 0)= 0; R(1, 1)= pow(2,2);
    Observations *observations = new Observations(eps,R);

    TestMap * testMap = new TestMap();
    MeasurementPrediction *measurementPrediction = new MeasurementPrediction(testMap->getMap());

    double sl[27];
    double sr[27];
    sl[0]=0.00; sr[0]=0.00;
    sl[1]=0.02; sr[1]=0.02;
    sl[2]=0.12; sr[2]=0.19;
    sl[3]=0.41; sr[3]=0.56;
    sl[4]=0.93; sr[4]=1.10;
    sl[5]=1.94; sr[5]=2.08;
    sl[6]=3.29; sr[6]=3.38;
    sl[7]=4.71; sr[7]=4.73;
    sl[8]=6.49; sr[8]=6.46;
    sl[9]=8.42; sr[9]=8.36;
    sl[10]=10.16; sr[10]=10.06;
    sl[11]=12.28; sr[11]=12.16;
    sl[12]=14.03; sr[12]=13.86;
    sl[13]=16.16; sr[13]=15.97;
    sl[14]=18.26; sr[14]=18.09;
    sl[15]=20.03; sr[15]=19.90;
    sl[16]=22.03; sr[16]=21.98;
    sl[17]=23.94; sr[17]=24.02;
    sl[18]=25.57; sr[18]=25.72;
    sl[19]=27.45; sr[19]=27.65;
    sl[20]=29.04; sr[20]=29.26;
    sl[21]=30.93; sr[21]=31.16;
    sl[22]=32.82; sr[22]=33.00;
    sl[23]=34.46; sr[23]=34.58;
    sl[24]=36.39; sr[24]=36.46;
    sl[25]=38.32; sr[25]=38.30;
    sl[26]=39.99; sr[26]=39.89;

    /*
    for(int i=0; i<26; i++){
        differentialDrive->update(sl[i],sr[i],xEst,pEst);
        xEst = differentialDrive->getXEstLast();
        pEst = differentialDrive->getPEstLast();
        std::cout <<xEst(0);
        std::cout <<" ";
        std::cout <<xEst(1);
        std::cout <<std::endl;
    }
    */

    std::vector<PointPolarForm> scan;
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-179.824+180),0.8380*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-179.258+180),0.8400*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-178.692+180),0.8400*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-178.126+180),0.8400*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-177.560+180),0.8400*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-176.994+180),0.8400*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-176.428+180),0.8420*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-175.862+180),0.8420*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-175.296+180),0.8420*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-174.730+180),0.8440*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-174.164+180),0.8440*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-173.598+180),0.8460*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-173.032+180),0.8480*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-172.466+180),0.8480*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-171.900+180),0.8500*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-171.334+180),0.8520*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-170.768+180),0.8520*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-170.202+180),0.8540*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-169.636+180),0.8560*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-169.070+180),0.8580*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-168.503+180),0.8600*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-167.937+180),0.8600*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-167.371+180),0.8640*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-166.805+180),0.8660*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-166.239+180),0.8680*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-165.673+180),0.8700*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-165.107+180),0.8720*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-164.541+180),0.8740*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-163.975+180),0.8780*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-163.409+180),0.8800*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-162.843+180),0.8840*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-162.277+180),0.8860*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-161.711+180),0.8880*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-161.145+180),0.8920*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-160.579+180),0.8960*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-160.013+180),0.9000*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-159.447+180),0.9040*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-158.881+180),0.9060*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-158.315+180),0.9100*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-157.749+180),0.9140*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-157.183+180),0.9180*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-156.617+180),0.9220*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-156.051+180),0.9260*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-155.485+180),0.9320*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-154.919+180),0.9360*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-154.353+180),0.9400*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-153.786+180),0.9440*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-153.220+180),0.9500*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-152.654+180),0.9540*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-152.088+180),0.9600*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-151.522+180),0.9660*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-150.956+180),0.9720*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-150.390+180),0.9760*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-149.824+180),0.9820*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-149.258+180),0.9880*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-148.692+180),0.9940*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-148.126+180),1.0000*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-147.560+180),1.0060*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-146.994+180),1.0120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-146.428+180),1.0200*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-145.862+180),1.0280*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-145.296+180),1.0360*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-144.730+180),1.0420*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-144.164+180),1.0500*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-143.598+180),1.0580*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-143.032+180),1.0640*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-142.466+180),1.0720*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-141.900+180),1.0800*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-141.334+180),1.0900*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-140.768+180),1.1000*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-140.202+180),1.1080*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-139.069+180),1.1180*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-135.107+180),1.1160*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-133.409+180),1.0940*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-132.277+180),1.0840*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-131.711+180),1.0740*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-131.145+180),1.0660*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-130.579+180),1.0580*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-130.013+180),1.0500*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-129.447+180),1.0400*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-128.881+180),1.0320*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-128.315+180),1.0240*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-127.749+180),1.0180*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-127.183+180),1.0100*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-126.617+180),1.0040*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-126.051+180),0.9960*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-125.485+180),0.9880*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-124.919+180),0.9820*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-124.352+180),0.9760*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-123.786+180),0.9700*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-123.220+180),0.9640*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-122.654+180),0.9580*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-122.088+180),0.9520*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-121.522+180),0.9460*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-120.956+180),0.9400*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-120.390+180),0.9340*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-119.824+180),0.9300*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-119.258+180),0.9260*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-118.692+180),0.9200*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-118.126+180),0.9160*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-117.560+180),0.9120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-116.994+180),0.9060*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-116.428+180),0.9020*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-115.862+180),0.8980*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-115.296+180),0.8940*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-114.730+180),0.8900*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-114.164+180),0.8860*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-113.598+180),0.8820*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-113.032+180),0.8780*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-112.466+180),0.8740*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-111.900+180),0.8720*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-111.334+180),0.8680*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-110.768+180),0.8640*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-110.202+180),0.8620*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-109.635+180),0.8580*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-109.069+180),0.8560*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-108.503+180),0.8520*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-107.937+180),0.8500*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-107.371+180),0.8480*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-106.805+180),0.8460*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-106.239+180),0.8440*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-105.673+180),0.8400*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-105.107+180),0.8380*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-104.541+180),0.8360*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-103.975+180),0.8360*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-103.409+180),0.8340*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-102.843+180),0.8320*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-102.277+180),0.8300*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-101.711+180),0.8280*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-101.145+180),0.8260*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-100.579+180),0.8240*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-100.013+180),0.8240*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-99.447+180),0.8220*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-98.881+180),0.8200*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-98.315+180),0.8200*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-97.749+180),0.8180*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-97.183+180),0.8180*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-96.617+180),0.8180*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-96.051+180),0.8160*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-95.485+180),0.8160*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-94.918+180),0.8140*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-94.352+180),0.8140*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-93.786+180),0.8120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-93.220+180),0.8120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-92.654+180),0.8120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-92.088+180),0.8120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-91.522+180),0.8120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-90.956+180),0.8120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-90.390+180),0.8120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-89.824+180),0.8120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-89.258+180),0.8120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-88.692+180),0.8120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-88.126+180),0.8120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-87.560+180),0.8120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-86.994+180),0.8140*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-86.428+180),0.8140*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-85.862+180),0.8140*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-85.296+180),0.8140*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-84.730+180),0.8160*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-84.164+180),0.8180*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-83.598+180),0.8180*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-83.032+180),0.8180*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-82.466+180),0.8200*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-81.900+180),0.8200*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-81.334+180),0.8220*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-80.768+180),0.8240*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-80.202+180),0.8240*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-79.635+180),0.8260*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-79.069+180),0.8280*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-78.503+180),0.8300*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-77.937+180),0.8320*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-77.371+180),0.8340*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-76.805+180),0.8360*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-76.239+180),0.8380*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-75.673+180),0.8400*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-75.107+180),0.8420*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-74.541+180),0.8440*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-73.975+180),0.8460*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-73.409+180),0.8480*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-72.843+180),0.8520*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-72.277+180),0.8540*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-71.711+180),0.8560*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-71.145+180),0.8600*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-70.579+180),0.8640*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-70.013+180),0.8660*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-69.447+180),0.8700*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-68.881+180),0.8740*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-68.315+180),0.8760*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-67.749+180),0.8800*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-67.183+180),0.8840*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-60.390+180),0.7920*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-59.258+180),0.7800*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-58.692+180),0.7700*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-58.126+180),0.7580*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-57.560+180),0.7460*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-56.994+180),0.7340*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-56.428+180),0.7240*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-55.862+180),0.7140*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-55.296+180),0.7060*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-54.730+180),0.6960*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-54.164+180),0.6880*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-53.598+180),0.6780*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-53.032+180),0.6700*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-52.466+180),0.6600*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-51.900+180),0.6520*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-51.334+180),0.6460*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-50.768+180),0.6380*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-50.201+180),0.6320*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-49.635+180),0.6240*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-49.069+180),0.6160*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-48.503+180),0.6100*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-47.937+180),0.6040*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-47.371+180),0.5980*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-46.805+180),0.5920*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-46.239+180),0.5860*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-45.673+180),0.5800*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-45.107+180),0.5760*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-44.541+180),0.5680*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-43.975+180),0.5640*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-43.409+180),0.5580*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-42.843+180),0.5540*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-42.277+180),0.5480*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-41.711+180),0.5440*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-41.145+180),0.5400*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-40.579+180),0.5340*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-40.013+180),0.5300*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-39.447+180),0.5260*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-38.881+180),0.5220*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-38.315+180),0.5180*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-37.749+180),0.5140*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-37.183+180),0.5110*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-36.617+180),0.5070*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-36.051+180),0.5030*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-35.484+180),0.5000*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-34.918+180),0.4960*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-34.352+180),0.4930*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-33.786+180),0.4900*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-33.220+180),0.4870*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-32.654+180),0.4830*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-32.088+180),0.4800*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-31.522+180),0.4770*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-30.956+180),0.4740*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-30.390+180),0.4720*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-29.824+180),0.4690*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-29.258+180),0.4660*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-28.692+180),0.4640*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-28.126+180),0.4610*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-27.560+180),0.4590*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-26.994+180),0.4570*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-26.428+180),0.4550*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-25.862+180),0.4530*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-25.296+180),0.4500*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-24.730+180),0.4480*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-24.164+180),0.4460*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-23.598+180),0.4440*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-23.032+180),0.4420*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-22.466+180),0.4400*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-21.900+180),0.4380*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-21.334+180),0.4360*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-20.768+180),0.4350*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-20.201+180),0.4330*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-19.635+180),0.4310*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-19.069+180),0.4300*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-18.503+180),0.4280*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-17.937+180),0.4270*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-17.371+180),0.4260*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-16.805+180),0.4240*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-16.239+180),0.4230*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-15.673+180),0.4220*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-15.107+180),0.4210*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-14.541+180),0.4200*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-13.975+180),0.4190*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-13.409+180),0.4180*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-12.843+180),0.4160*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-12.277+180),0.4150*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-11.711+180),0.4140*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-11.145+180),0.4140*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-10.579+180),0.4130*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-10.013+180),0.4120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-9.447+180),0.4110*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-8.881+180),0.4100*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-8.315+180),0.4090*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-7.749+180),0.4090*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-7.183+180),0.4090*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-6.617+180),0.4080*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-6.051+180),0.4070*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-5.484+180),0.4070*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-4.918+180),0.4070*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-4.352+180),0.4060*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-3.786+180),0.4060*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-3.220+180),0.4060*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-2.654+180),0.4050*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-2.088+180),0.4050*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-1.522+180),0.4050*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-0.956+180),0.4040*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(-0.390+180),0.4040*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(0.176+180),0.4040*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(0.742+180),0.4040*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(1.308+180),0.4040*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(1.874+180),0.4050*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(2.440+180),0.4050*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(3.006+180),0.4040*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(3.572+180),0.4050*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(4.138+180),0.4050*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(4.704+180),0.4050*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(5.270+180),0.4050*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(5.836+180),0.4050*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(6.402+180),0.4060*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(6.968+180),0.4060*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(7.534+180),0.4070*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(8.100+180),0.4070*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(8.666+180),0.4080*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(9.233+180),0.4090*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(9.799+180),0.4090*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(10.365+180),0.4090*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(10.931+180),0.4100*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(11.497+180),0.4110*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(12.063+180),0.4120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(12.629+180),0.4130*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(13.195+180),0.4130*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(13.761+180),0.4140*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(14.327+180),0.4150*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(14.893+180),0.4170*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(15.459+180),0.4180*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(16.025+180),0.4190*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(16.591+180),0.4200*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(17.157+180),0.4210*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(17.723+180),0.4220*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(18.289+180),0.4230*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(18.855+180),0.4250*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(19.421+180),0.4260*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(19.987+180),0.4280*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(20.553+180),0.4290*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(21.119+180),0.4300*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(21.685+180),0.4320*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(22.251+180),0.4340*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(22.817+180),0.4360*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(23.383+180),0.4380*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(23.950+180),0.4390*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(24.516+180),0.4410*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(25.082+180),0.4430*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(25.648+180),0.4450*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(26.214+180),0.4470*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(26.780+180),0.4500*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(27.346+180),0.4520*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(27.912+180),0.4540*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(28.478+180),0.4570*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(29.044+180),0.4590*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(29.610+180),0.4610*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(30.176+180),0.4640*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(30.742+180),0.4670*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(31.308+180),0.4690*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(31.874+180),0.4720*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(32.440+180),0.4750*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(33.006+180),0.4780*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(33.572+180),0.4810*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(34.138+180),0.4850*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(34.704+180),0.4880*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(35.270+180),0.4920*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(35.836+180),0.4950*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(36.402+180),0.4990*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(36.968+180),0.5030*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(37.534+180),0.5070*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(38.100+180),0.5110*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(38.667+180),0.5150*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(39.233+180),0.5180*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(39.799+180),0.5220*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(40.365+180),0.5260*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(40.931+180),0.5300*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(47.723+180),0.5360*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(48.855+180),0.5320*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(49.421+180),0.5280*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(49.987+180),0.5240*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(50.553+180),0.5200*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(51.119+180),0.5160*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(51.685+180),0.5120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(52.251+180),0.5060*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(52.817+180),0.5030*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(53.383+180),0.5000*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(53.950+180),0.4970*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(54.516+180),0.4930*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(55.082+180),0.4890*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(55.648+180),0.4860*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(56.214+180),0.4820*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(56.780+180),0.4790*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(57.346+180),0.4760*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(57.912+180),0.4730*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(58.478+180),0.4700*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(59.044+180),0.4670*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(59.610+180),0.4650*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(60.176+180),0.4620*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(60.742+180),0.4590*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(61.308+180),0.4560*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(61.874+180),0.4540*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(62.440+180),0.4510*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(63.006+180),0.4490*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(63.572+180),0.4470*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(64.138+180),0.4450*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(64.704+180),0.4420*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(65.270+180),0.4400*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(65.836+180),0.4380*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(66.402+180),0.4360*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(66.968+180),0.4350*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(67.534+180),0.4330*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(68.100+180),0.4310*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(68.667+180),0.4290*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(69.233+180),0.4270*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(69.799+180),0.4250*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(70.365+180),0.4230*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(70.931+180),0.4220*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(71.497+180),0.4210*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(72.063+180),0.4190*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(72.629+180),0.4180*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(73.195+180),0.4160*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(73.761+180),0.4150*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(74.327+180),0.4140*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(74.893+180),0.4130*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(75.459+180),0.4120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(76.025+180),0.4100*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(76.591+180),0.4100*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(77.157+180),0.4090*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(77.723+180),0.4080*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(78.289+180),0.4070*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(78.855+180),0.4060*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(79.421+180),0.4050*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(79.987+180),0.4040*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(80.553+180),0.4030*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(81.119+180),0.4020*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(81.685+180),0.4020*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(82.251+180),0.4010*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(82.817+180),0.4000*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(83.384+180),0.4000*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(83.950+180),0.3990*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(84.516+180),0.3990*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(85.082+180),0.3990*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(85.648+180),0.3980*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(86.214+180),0.3980*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(86.780+180),0.3980*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(87.346+180),0.3970*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(87.912+180),0.3970*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(88.478+180),0.3960*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(89.044+180),0.3960*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(89.610+180),0.3960*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(90.176+180),0.3960*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(90.742+180),0.3960*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(91.308+180),0.3960*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(91.874+180),0.3960*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(92.440+180),0.3960*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(93.006+180),0.3960*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(93.572+180),0.3960*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(94.138+180),0.3960*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(94.704+180),0.3970*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(95.270+180),0.3970*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(95.836+180),0.3970*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(96.402+180),0.3970*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(96.968+180),0.3980*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(97.534+180),0.3980*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(98.101+180),0.3980*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(98.667+180),0.3990*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(99.233+180),0.4000*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(99.799+180),0.4000*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(100.365+180),0.4010*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(100.931+180),0.4010*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(101.497+180),0.4020*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(102.063+180),0.4030*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(102.629+180),0.4040*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(103.195+180),0.4040*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(103.761+180),0.4050*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(104.327+180),0.4070*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(104.893+180),0.4070*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(105.459+180),0.4080*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(106.025+180),0.4100*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(106.591+180),0.4100*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(107.157+180),0.4110*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(107.723+180),0.4130*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(108.289+180),0.4140*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(108.855+180),0.4150*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(109.421+180),0.4170*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(109.987+180),0.4180*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(110.553+180),0.4190*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(111.119+180),0.4210*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(111.685+180),0.4230*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(112.251+180),0.4240*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(112.817+180),0.4260*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(113.384+180),0.4280*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(113.950+180),0.4290*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(114.516+180),0.4310*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(115.082+180),0.4330*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(115.648+180),0.4350*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(116.214+180),0.4380*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(116.780+180),0.4390*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(117.346+180),0.4420*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(117.912+180),0.4440*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(118.478+180),0.4460*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(119.044+180),0.4480*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(119.610+180),0.4510*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(120.176+180),0.4540*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(120.742+180),0.4570*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(121.308+180),0.4590*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(121.874+180),0.4610*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(122.440+180),0.4640*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(123.006+180),0.4670*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(123.572+180),0.4710*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(124.138+180),0.4740*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(124.704+180),0.4770*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(125.270+180),0.4800*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(125.836+180),0.4830*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(126.402+180),0.4860*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(126.968+180),0.4910*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(127.535+180),0.4950*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(128.101+180),0.4990*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(128.667+180),0.5030*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(129.233+180),0.5070*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(129.799+180),0.5110*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(130.365+180),0.5150*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(130.931+180),0.5200*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(131.497+180),0.5240*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(132.063+180),0.5280*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(132.629+180),0.5340*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(133.195+180),0.5380*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(133.761+180),0.5440*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(134.327+180),0.5480*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(134.893+180),0.5540*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(135.459+180),0.5600*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(136.025+180),0.5660*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(136.591+180),0.5720*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(137.157+180),0.5780*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(137.723+180),0.5840*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(138.289+180),0.5900*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(138.855+180),0.5980*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(139.421+180),0.6060*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(139.987+180),0.6120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(140.553+180),0.6200*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(141.119+180),0.6280*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(141.685+180),0.6360*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(142.251+180),0.6440*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(142.818+180),0.6540*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(143.384+180),0.6640*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(143.950+180),0.6720*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(144.516+180),0.6820*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(145.082+180),0.6920*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(145.648+180),0.7020*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(146.214+180),0.7120*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(146.780+180),0.7240*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(147.346+180),0.7360*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(147.912+180),0.7480*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(148.478+180),0.7600*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(149.044+180),0.7740*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(149.610+180),0.7900*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(150.176+180),0.8040*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(150.742+180),0.8180*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(156.968+180),0.9000*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(158.667+180),0.8960*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(159.233+180),0.8920*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(159.799+180),0.8880*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(160.365+180),0.8860*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(160.931+180),0.8840*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(161.497+180),0.8820*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(162.063+180),0.8780*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(162.629+180),0.8760*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(163.195+180),0.8720*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(163.761+180),0.8700*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(164.327+180),0.8680*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(164.893+180),0.8660*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(165.459+180),0.8640*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(166.025+180),0.8620*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(166.591+180),0.8600*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(167.157+180),0.8580*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(167.723+180),0.8560*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(168.289+180),0.8560*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(168.855+180),0.8540*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(169.421+180),0.8520*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(169.987+180),0.8500*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(170.553+180),0.8500*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(171.119+180),0.8480*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(171.685+180),0.8480*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(172.252+180),0.8460*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(172.818+180),0.8440*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(173.384+180),0.8440*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(173.950+180),0.8420*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(174.516+180),0.8420*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(175.082+180),0.8420*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(175.648+180),0.8420*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(176.214+180),0.8400*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(176.780+180),0.8400*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(177.346+180),0.8400*100));
    scan.push_back(PointPolarForm(MathConversions::deg2rad(177.912+180),0.8400*100));

    /*
    for(int i=0; i<scan.size();i++){
        std::cout << i << " "<< scan.at(i).getAngle()  << " " << scan.at(i).getDistance()  << std::endl;

    }
    */

    //observations->update(&scan,scan.size());

    /*
    Line * line;
    for(int i=0; i<observations->size(); i++){
        line = observations->getLineByIndex(i);
        line->updateOriginLineNormal();
        std::cout << line->getAlfa() << " " << line->getR() << std::endl;
    }
    */

/*
    for(int i=0; i<26; i++){
        differentialDrive->update(sl[i],sr[i],xEst,pEst);
        xEst = differentialDrive->getXEstLast();
        pEst = differentialDrive->getPEstLast();

        std::cout <<xEst(0);
        std::cout <<" ";
        std::cout <<xEst(1);
        std::cout <<std::endl;
    }
*/



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


