#include <Wire.h>
#include <LIDARLite.h>

LIDARLite lidarLite;
int cal_cnt = 0;

extern "C" int main(void)
{

	Serial.begin(9600); // Initialize serial connection to display distance readings
	lidarLite.begin(0, true); // Set configuration to default and I2C to 400 kHz
	lidarLite.configure(0); // Change this number to try out alternate configurations

	delay(100);

	int dist = 1;

	dist = lidarLite.distance();      // With bias correction
	lidarLite.reset(0x62);
//	dist = lidarLite.distance(false); // Without bias correction

	 while(1)
		 Serial.println("here");


	while(1)
	{
	  // At the beginning of every 100 readings,
	  // take a measurement with receiver bias correction
	  if ( cal_cnt == 0 ) {
	    //dist = lidarLite.distance();      // With bias correction
	  } else {
	    //dist = lidarLite.distance(false); // Without bias correction
	  }

	 // while(1)
	 // 		  Serial.println("here");

	  // Increment reading counter
	  cal_cnt++;
	  cal_cnt = cal_cnt % 100;


	  // Display distance
	  Serial.println(dist);
	  //Serial.println(" cm");

	  delay(100);
	}
}


