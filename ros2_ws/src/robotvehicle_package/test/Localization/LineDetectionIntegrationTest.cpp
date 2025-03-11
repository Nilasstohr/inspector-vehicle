#include <gtest/gtest.h>
#include "rclcpp/rclcpp.hpp"
#include "Configurations.h"
#include "Localization/Incremental.h"
#include "TestUtilities/RecordHandler.h"
#include "Host/DriverInterface.h"
#include "TestUtilities/SerialInterfaceStub.h"
#include "Sensor/SensorData.h"
#include "TestConfigurations.h"
#include "Localization/BatchGradientDecent.h"
#include "Utilities/String.h"
int argcTest;
char **argvTest;

Lines getLinesFromMapFile() {
    Lines lines;
    std::vector<std::string> lineSplit;
    std::vector<std::string> coordinateSplit;
    ifstream file;
    file.open(TEST_CONFIG_RESOURCE_FILE_MAP_LINES);
    if(file.is_open()) {
        string textLine;
        while(getline(file,textLine)) {
            String::split(lineSplit,textLine,' ');
            Line line;
            for(auto &lineItem:lineSplit) {
                String::split(coordinateSplit,lineItem,';');
                if(coordinateSplit.size()!=2) {
                    EXPECT_TRUE(false) << "Expected coordinate to consist of 2 item but got " << coordinateSplit.size();
                }
                double x = atof(coordinateSplit.at(0).c_str());
                double y = atof(coordinateSplit.at(1).c_str());
                line.addRecPoint(x,y);
                coordinateSplit.clear();
            }
            lineSplit.clear();
            lines.addLine(&line);
        }
    }else {
        EXPECT_TRUE(false) << "Could not open file " << TEST_CONFIG_RESOURCE_FILE_MAP_LINES;
    }
    return lines;
}
bool compareValues(double alfa1,double alfa2,double r1,double r2) {
    double alfaTolerances =0.3;
    double rTolerances =5;
    if(alfa1<alfa2 +alfaTolerances && alfa1>alfa2-alfaTolerances &&
        r1<r2 +rTolerances && r1>r2-rTolerances) {
        return true;
        }
    return false;
}

std::map<int,Lines> getCollinearLines() {
    std::map<int,Lines> collinearMap;
    Lines lines = getLinesFromMapFile();
    int mapCount = 0;
    lines.printLineParameters();
    for(int i=0;i<lines.size();i++) {
        Line * targetLine = lines.getLine(i);
        if(!targetLine->hasBeenCollinearMatched()) {
            std::cout << "--------> Target line is: " << targetLine->getAlfa() << " " << targetLine->getR() << std::endl;
            collinearMap.insert(std::pair(mapCount,Lines()));
            targetLine->setCollinearMatched();
            collinearMap.at(mapCount).addLine(targetLine);
            for(int j=0;j<lines.size();j++) {
                Line * compareLine = lines.getLine(j);
                if(!compareLine->hasBeenCollinearMatched()) {
                    std::cout << "Compare line is: " << compareLine->getAlfa() << " " << compareLine->getR();
                    if(compareValues(compareLine->getAlfa(),targetLine->getAlfa(),
                        compareLine->getR(),targetLine->getR())) {
                        compareLine->setCollinearMatched();
                        std::cout << " [Matched] " << std::endl;
                        collinearMap.at(mapCount).addLine(compareLine);
                        }else {
                            std::cout << std::endl;
                        }
                }
            }
            mapCount++;
        }
    }
    std::cout << std::endl;
    std::cout << "Final Result: ";
    std::cout << std::endl;

    for(auto it = collinearMap.begin(); it != collinearMap.end(); ++it)
    {
        std::cout <<"Collinear lines: "<< it->first << std::endl;
        it->second.printLinePoints();
    }
    return collinearMap;
}
TEST(line_least_square_test,test_1)
{
    Line line;
    line.addRecPoint(199,0);
    line.addRecPoint(198.9966,1.1541);
    line.addRecPoint(198.786624,2.3060);
    line.addRecPoint(198.7696,3.4788);
    line.addRecPoint(198.9459,4.6362);
    line.updateSlopeFormLeastSquare();
    double toleranceM = 0.1;
    double toleranceB = 2;
    double expectedM = -7.55;
    double expectedB = 1504.94;
    if(line.getM() > expectedM + toleranceM ||  line.getM() < expectedM - toleranceM) {
        EXPECT_TRUE(false) << "Expected slope to be " << expectedM << " +- " << toleranceM
        << " but got " << line.getM() << std::endl;
    }
    if(line.getB() > expectedB + toleranceB ||  line.getB() < expectedB - toleranceB) {
        EXPECT_TRUE(false) << "Expected intercept to be " << expectedB << " +- " << toleranceB
        << " but got " << line.getB() << std::endl;
    }
}

TEST(line_detection_integration_test,test_2)
{
    int MAX_LINES_EXPECTED = 10;
    auto *serial_interface = new SerialInterface(CONFIG_ROBOT_DRIVER_DEVICE_NAME);
    auto *driver_interface = new DriverInterface(serial_interface);
    auto * sensor_data = new SensorData(driver_interface);
    auto *recordHandler = new RecordHandler();
    auto * incremental = new Incremental(CONFIG_ESP);
    while(recordHandler->hasRecordsToProcess()) {
        recordHandler->update(sensor_data);
        incremental->update(sensor_data->getScanPolarForm(),
            sensor_data->getScanPolarForm()->size());
        int linesDetected = incremental->size();
        cout<<"linesDetected: "<<linesDetected<<endl;
        if(linesDetected>MAX_LINES_EXPECTED) {
            EXPECT_TRUE(false) << "expected less that " << MAX_LINES_EXPECTED << " lines but got " << linesDetected;
        }
    }
}

TEST(line_detection_integration_test,test_3)
{
    Lines lines = getLinesFromMapFile();
    int expectedLiens =19;
    int actualLines = lines.size();
    if(actualLines!=expectedLiens) {
        EXPECT_TRUE(false) << "Expected" << expectedLiens <<" lines but got " << actualLines;
    }
}


TEST(line_detection_integration_test,test_4)
{
    std::map<int,Lines> collinearMap = getCollinearLines();
    for(auto it = collinearMap.begin(); it != collinearMap.end(); ++it)
    {
        Lines lines = it->second;
        cout <<endl;
        cout <<endl;
        cout << "********************* Line merge:" << lines.size() << "**********************: " << endl;
        cout << "----------lines in batch-----" << endl;
        lines.printLineParameters();
        const double m = lines.getLine(0)->getM();
        const double b = lines.getLine(0)->getB();
        BatchGradientDecent batchGradientDecent(0.00001,m,b);
        if(lines.size()>1) {
            cout << "-----batch gradient decent: -------" << endl;
            cout << "First line m and b:" << m << " " << b << endl;
            Line lineMerge;
            lineMerge.addRecPoints(lines.getLine(0));
            for(int i=1;i<lines.size();i++) {
                Line *line = lines.getLine(i);
                batchGradientDecent.update(line,1000);
                lineMerge.addRecPoints(line);
            }
            lineMerge.updateOriginLineNormal();
            Line bgdLine;
            bgdLine.setM(batchGradientDecent.getM());
            bgdLine.setB(batchGradientDecent.getB());
            bgdLine.updateOriginalLineNormalFromSlopeForm();
            lines.addLine(&bgdLine);
            cout << "BGD line: " << bgdLine.getAlfa() << " " << bgdLine.getR() << " " << bgdLine.getM() << " " << bgdLine.getB()<< endl;
            cout << "Merge line: " << lineMerge.getAlfa() << " " << lineMerge.getR() << " " << lineMerge.getM() << " " << lineMerge.getB()<< endl;

            //lines.printLineParameters();
        }
    }
}


int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}