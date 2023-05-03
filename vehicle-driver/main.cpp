#include <Arduino.h>
#include "src/MotorDrivers.h"
#include "src/VehicleTestToolBox.h"
//#include <src/VehicleTestRunner.h>
//new VehicleTestRunner();

void ISR_A();
void ISR_B();
void ISR_SAMPLE();
void init();
void start();

unsigned int INTERVAL_MICROS = 100;
#define LEFT_A  22
#define LEFT_B  23
#define MAX_COUNTS 15000

MotorDrivers *motors;
IntervalTimer *timer;

volatile signed int counts =0;
volatile uint32_t deltaT=0;
volatile uint32_t lastT=0;
volatile uint32_t currentT=0;
volatile bool finish = false;

volatile uint32_t deltaTBuffer[MAX_COUNTS];
volatile uint32_t deltaTBufferCount =0;
char message[100];
double radian_count =  (double)(2*M_PI/countPrRevolution);

extern "C" int main(void)
{
	init();
	while(1){
		//Serial.println("waiting for input");
		if(Serial.available()){
			int i=0;
			while (Serial.available()){
				message[i] = Serial.read();
				i++;
			}
			String s = message;
			if(s.equals("start")){
			  digitalWrite(LED_BUILTIN, HIGH);
			  start();
			  delay(200);
			  digitalWrite(LED_BUILTIN, LOW);
			}
		}

		//delay(500);
	}
}

void start(){
	counts =0;
	deltaTBufferCount =0;
	deltaT   = 0;
	lastT    = 0;
	finish = false;
	timer->begin(ISR_SAMPLE,INTERVAL_MICROS);
	motors->forward(15000);
	while(1){
		if(finish){
			motors->forward(0);

			break;
		}
		delayMicroseconds(500);
	}
	for(int i=0; i<MAX_COUNTS; i++){
		Serial.print(deltaTBuffer[i]);
		if(i==MAX_COUNTS-1){
		  break;
		}
		Serial.print(",");
	}
	Serial.print(";");

}
void ISR_SAMPLE() {
	if(deltaTBufferCount<MAX_COUNTS){
		deltaTBuffer[deltaTBufferCount]=deltaT;
		deltaTBufferCount++;
		//Serial.println(deltaTBuffer[deltaTBufferCount]);
		//Serial.print(deltaTBuffer[deltaTBufferCount]);
	}else{
		timer->end();
		finish = true;
	}

}
void ISR_A(){
	currentT = micros();
	deltaT   = currentT - lastT;
	lastT    = currentT;
	uint8_t B = digitalRead(LEFT_B);
	uint8_t A = digitalRead(LEFT_A) ;
	if((A&&B)||(!A&&!B))
		counts--;
	else
		counts++;


}
void ISR_B(){
	currentT = micros();
	deltaT   = currentT - lastT;
	lastT    = currentT;
	uint8_t B = digitalRead(LEFT_B);
	uint8_t A = digitalRead(LEFT_A);
	if((B&&A)||(!B&&!A))
		counts++;
	else
		counts--;
}
void init(){
	analogWriteResolution(16);
	analogReadResolution(10);
	Serial.begin(115200);

	VehicleTestToolBox * toolBox = new VehicleTestToolBox();

	motors = toolBox->createMotorDrivers();
	motors->forward(0);

	pinMode(LEFT_A, INPUT);
	digitalWrite(LEFT_A, LOW);

	pinMode(LEFT_B, INPUT);
	digitalWrite(LEFT_B, LOW);

	attachInterrupt(LEFT_A,ISR_A, CHANGE);
	attachInterrupt(LEFT_B,ISR_B, CHANGE);

	timer = new IntervalTimer;

}
