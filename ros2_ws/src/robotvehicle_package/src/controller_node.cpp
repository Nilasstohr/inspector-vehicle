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
#include "Localization/Matching.h"
#include "Localization/Estimation.h"

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

        initKalman();

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

            // Kalman
            prediction->update(posLeft, posRight, estimation->getXt(), estimation->getPt());
            measurementPrediction->update(prediction);
            observations->update(sensorLogger[logCount]->getScan(),sensorLogger[logCount]->getScan()->size());
            matching->update(prediction, measurementPrediction, observations);
            estimation->update(matching, prediction->getXEst(), prediction->getPEst());
            // reset filter
            measurementPrediction->reset();
            observations->reset();
            matching->reset();

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
    void initKalman(){

        prediction = new PredictionDifferentialDrive();

        pEst = MatrixXd(3,3);
        double sigmaXY = 2; // cm
        double sigmaTheta = MathConversions::deg2rad(2); // degrees;
        pEst(0, 0)= pow(sigmaXY,2); pEst(0, 1)= 0; pEst(0, 2)= 0;
        pEst(1, 0)= 0; pEst(1, 1)= pow(sigmaXY,2); pEst(1, 2)= 0;
        pEst(2, 0)= 0; pEst(2, 1)= 0; pEst(2, 2)=  pow(sigmaTheta,2);

        xEst = MatrixXd(3,1);
        xEst(0,0)=40;
        xEst(1,0)=40;
        xEst(2,0)=0;

        TestMap * testMap = new TestMap();
        measurementPrediction = new MeasurementPrediction(testMap->getMap());

        double eps = 1;
        MatrixXd R(2, 2);
        R(0, 0)= pow(MathConversions::deg2rad(2),2); R(0, 1)= 0;
        R(1, 0)= 0; R(1, 1)= pow(2,2);
        observations = new Observations(eps,R);

        matching = new Matching(15);

        estimation = new Estimation(xEst,pEst);

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

    // kalman
    PredictionDifferentialDrive * prediction;
    MatrixXd pEst;
    MatrixXd xEst;
    MeasurementPrediction *measurementPrediction;
    Observations *observations;
    Matching * matching;
    Estimation *estimation;
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
    MatrixXd xEst(3,1);
    xEst(0,0)=40;
    xEst(1,0)=40;
    xEst(2,0)=0;
    //printMatrix(pEst,"pEst");
    //printVector(xEst,"xEst");

    // measurement prediction
    TestMap * testMap = new TestMap();
    MeasurementPrediction *measurementPrediction = new MeasurementPrediction(testMap->getMap());


    // for observations step
    double eps = 1;
    MatrixXd R(2, 2);
    R(0, 0)= pow(MathConversions::deg2rad(2),2); R(0, 1)= 0;
    R(1, 0)= 0; R(1, 1)= pow(2,2);
    Observations *observations = new Observations(eps,R);


    // matching
    Matching * matching = new Matching(15);

    // Estimation
    Estimation *estimation = new Estimation(xEst,pEst);

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


    std::vector<PointPolarForm> * scan;

    for(int i=0; i<27; i++){
        /*
        scan = &scan0;
        switch (i) {
            case 0:
                scan = &scan0;
                break;
            case 1:
                scan = &scan1;
                break;
            case 2:
                scan = &scan2;
                break;
            case 3:
                scan = &scan3;
                break;
            case 4:
                scan = &scan4;
                break;
            case 5:
                scan = &scan5;
                break;
            case 6:
                scan = &scan6;
                break;
            case 7:
                scan = &scan7;
                break;
            case 8:
                scan = &scan8;
                break;
            case 9:
                scan = &scan9;
                break;
            case 10:
                scan = &scan10;
                break;
            case 11:
                scan = &scan11;
                break;
            case 12:
                scan = &scan12;
                break;
            case 13:
                scan = &scan13;
                break;
            case 14:
                scan = &scan14;
                break;
            case 15:
                scan = &scan15;
                break;
            case 16:
                scan = &scan16;
                break;
            case 17:
                scan = &scan17;
                break;
            case 18:
                scan = &scan18;
                break;
            case 19:
                scan = &scan19;
                break;
            case 20:
                scan = &scan20;
                break;
            case 21:
                scan = &scan21;
                break;
            case 22:
                scan = &scan22;
                break;
            case 23:
                scan = &scan23;
                break;
            case 24:
                scan = &scan24;
                break;
            case 25:
                scan = &scan25;
                break;
            case 26:
                scan = &scan26;
                break;
        }
*/
        differentialDrive->update(sl[i],sr[i],estimation->getXt(),estimation->getPt());
        measurementPrediction->update(differentialDrive);
        observations->update(scan,scan->size());
        matching->update(differentialDrive,measurementPrediction,observations);
        cout << "(" << i+1 << ")";
        estimation->update(matching,differentialDrive->getXEst(),differentialDrive->getPEst());

        measurementPrediction->reset();
        observations->reset();
        matching->reset();
    }


    
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

