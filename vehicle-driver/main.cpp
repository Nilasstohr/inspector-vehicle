#include <Arduino.h>

extern "C" int main(void)
{
	pinMode(13, OUTPUT);
	while (1) {
		digitalWriteFast(13, HIGH);
		delay(200);
		digitalWriteFast(13, LOW);
		delay(200);
	}
}
