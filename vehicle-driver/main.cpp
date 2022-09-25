#include <Arduino.h>
#include <TestClass.h>
#include <src/TestQuadratureEncorder.h>
#include <src/Logger.h>
#include <src/VehicleTestRunner.h>
#include <src/Experimentation.h>

extern "C" int main(void)
{
	analogWriteResolution(16);
	Serial.begin(115200);
	//new Experimentation();
	new VehicleTestRunner();

}
