#include <SoftwareSerial.h>

SoftwareSerial HT05_master (9, 10);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  HT05_master.begin(38400);
  delay(2000);
}

void loop() {  
  HT05_master.print("hello world\r\n");
  delay(3000);  
}
