#include <Arduino.h>
#include <src/VehicleTestRunner.h>

extern "C" int main(void)
{
	analogWriteResolution(13);
	analogReadResolution(10);
	Serial.begin(115200);
	new VehicleTestRunner();
}
