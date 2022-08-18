#include <Arduino.h>
#include <TestClass.h>
#include <src/TestQuadratureEncorder.h>
#include <src/Logger.h>
#include <src/VehicleTestRunner.h>

void isr();

bool runTests = true;

extern "C" int main(void)
{
	//pinMode(30, INPUT);
	//digitalWrite(30, LOW);
    //attachInterrupt(30,isr, CHANGE);
	Serial.begin(115200);
	Logger::setLogLevel(Logger::VERBOSE);
	Logger::verbose(__FUNCTION__,":Starting Program");

	if(runTests){
		Logger::verbose(__FUNCTION__,":Started up in test mode");
		new VehicleTestRunner();
	}


	delay(1000);



	//TestClass *t = new TestClass();
	//t->TestLoop();
	//Logger::warning(__FUNCTION__, "This is a warning message. 6");

}

void isr(){

}
