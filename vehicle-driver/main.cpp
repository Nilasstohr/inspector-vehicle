#include <Arduino.h>
#include <TestClass.h>
#include <src/TestQuadratureEncorder.h>
#include <src/Logger.h>
#include <src/VehicleTestRunner.h>

void run();
void isr();

bool runTests = false;

extern "C" int main(void)
{

	TestClass *t = new TestClass();
	t->TestLoop();
	//Logger::warning(__FUNCTION__, "This is a warning message. 6");

}


void run(){
	pinMode(12, INPUT);
    digitalWrite(12, LOW);
	attachInterrupt(12,isr, CHANGE);
	Logger::verbose(__FUNCTION__,":pins 11 set");
}

void isr(){
	Logger::verbose(__FUNCTION__,"11");
}
