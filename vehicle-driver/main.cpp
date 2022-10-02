#include <Arduino.h>
//#include <TestClass.h>
//#include <src/TestQuadratureEncorder.h>
//#include <src/Logger.h>
#include <src/VehicleTestRunner.h>
//#include <src/Experimentation.h>
//#include <vector>
//#include <map>
//#include <src/QuadratureEncoderTypes.h>
/*
using std::vector;

class Testing{
	double a;
	double b;
	double c;
	double d;
	double g;
public:
	Testing(double a,double b,double c,double d) : a(a), b(b), c(c),d(d){
		this->g = a/b*c/a*b;
	}
};

class TestingInt{
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
public:
	TestingInt(uint8_t a,uint8_t b,uint8_t c,uint8_t d) : a(a), b(b), c(c),d(d){

	}
};
*/

extern "C" int main(void)
{
	analogWriteResolution(16);
	Serial.begin(115200);
	//new Experimentation();
	new VehicleTestRunner();
	/*
	Serial.println("Starting Test");

	vector<Testing> test;
	uint32_t time1;
	uint32_t time2;

	time1 = micros();
	//micros();
	test.push_back(Testing(0.34,1.435,5.1111,7.1));
	time2 = micros();


	Serial.println(time2-time1);
	*/

}

