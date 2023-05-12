#include <Arduino.h>
#include "src/MotorDrivers.h"
#include "src/VehicleTestToolBox.h"
#include <src/VehicleTestRunner.h>
//new VehicleTestRunner();

void ISR_A();
void ISR_B();
void ISR_SAMPLE();
void init();
void runProgram1();
void start();
double radPrSekFromDeltaT(uint32_t deltaT);

unsigned int INTERVAL_MICROS = 100;
#define LEFT_A  22
#define LEFT_B  23
#define MAX_COUNTS 15000
#define COUNTS_PR_REV 3200

MotorDrivers *motors;
IntervalTimer *timer;

volatile signed int counts =0;
volatile uint32_t deltaT=0;
volatile uint32_t deltaT_Temp=0;
volatile uint32_t lastT=0;
volatile uint32_t currentT=0;
volatile bool finish = false;

volatile uint32_t deltaTBuffer[MAX_COUNTS];
volatile uint32_t deltaTBufferFiltered[MAX_COUNTS];

volatile uint32_t deltaTBufferCount =0;
char message[100];
double radian_pr_count =  (double)(2*M_PI/COUNTS_PR_REV);

float b0 = 0.0591907;
float b1 = 0.0591907;
float a1 = -0.88161859;
double feedForward=0;
double feedBack=0;
double output=0;

extern "C" int main(void)
{

	init();
	runProgram1();
}

void runProgram1(){

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
			  Serial.flush();
			  //Serial.print("running test...");
			  digitalWrite(LED_BUILTIN, HIGH);
			  start();
			  delay(200);
			  digitalWrite(LED_BUILTIN, LOW);
			}else{
				Serial.print("invalid input, did you send a new line?");
			}
		}

		delayMicroseconds(1);
	}
}

void start(){
	counts =0;
	deltaTBufferCount =0;
	deltaT   = 0;
	lastT    = 0;
	finish = false;
	timer->begin(ISR_SAMPLE,INTERVAL_MICROS);
	motors->forward(65000);
	//delay(1000);
	while(1){
		if(finish){
			motors->forward(0);
			break;
		}
		delayMicroseconds(500);
	}

	uint32_t sumDeltaT = 0;
	double sumOmega =0;
	for(int i=0; i<MAX_COUNTS; i++){
		sumDeltaT = sumDeltaT + deltaTBuffer[i];
		sumOmega =   sumOmega +   radPrSekFromDeltaT(deltaTBuffer[i]);
		if(i==MAX_COUNTS-1){

			double meanDeltaT = sumDeltaT/MAX_COUNTS;
			double meanOmega =  sumOmega/MAX_COUNTS;
			double varDeltaT = 0;
			double varOmega = 0;
			for(int n = 0; n < MAX_COUNTS; n++ )
			{
			  //Serial.print(n);
			  Serial.print(deltaTBuffer[n]);

			  Serial.print(" ");
			  Serial.print(radPrSekFromDeltaT(deltaTBuffer[n]));

			  Serial.print(" ");
			  Serial.print(radPrSekFromDeltaT(deltaTBufferFiltered[n]));

			  Serial.print(" ");
			  Serial.println(radPrSekFromDeltaT(deltaTBuffer[n])/(2*M_PI)*60);

			  //Serial.print("  mean deltaT=");
			  //Serial.print(meanDeltaT);

			  //Serial.print("  mean omega=");
			  //Serial.println(meanOmega);
			  varDeltaT += pow( deltaTBuffer[n] - meanDeltaT, 2);
			  varOmega += pow( radPrSekFromDeltaT(deltaTBuffer[n]) - meanOmega, 2);
			}
			double varianceDeltaT = varDeltaT /= MAX_COUNTS;
			Serial.print(" mean delta T=");
			Serial.println(meanDeltaT);
			Serial.print(" sd delta T=");
			Serial.println(sqrt(varianceDeltaT));
			double varianceOmega= varOmega /= MAX_COUNTS;
			Serial.print(" mean omega=");
			Serial.println(meanOmega);
			Serial.print(" sd omega=");
			Serial.println(sqrt(varianceOmega));
			Serial.print(" mean RPM=");
			Serial.println(meanOmega/(2*M_PI)*60);
			Serial.print(" sd RPM=");
			Serial.println(sqrt(varianceOmega)/(2*M_PI)*60);
			return;
		}
	}
}
void ISR_SAMPLE() {
	if(deltaTBufferCount<MAX_COUNTS){
		deltaT_Temp = deltaT;
		output  = b0*deltaT + feedForward - feedBack;
		//output  = deltaT;
		deltaTBufferFiltered[deltaTBufferCount]=output;
		deltaTBuffer[deltaTBufferCount]=deltaT;

		feedForward = b1*output;
		feedBack    = a1*output;
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
double radPrSekFromDeltaT(uint32_t deltaT) {
	double s;
	s =  (double)(deltaT/pow(10,6));
	return (double)(radian_pr_count/s);
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
