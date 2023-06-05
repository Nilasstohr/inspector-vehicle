#include <Arduino.h>
#include "src/TransposedIIRFilter.h"
#include "src/QuadratureEncoders.h"
#include "src/VehiclePins.h"
#include "src/VehicleParameters.h"

#define MAX_COUNTS 35000
#define COUNTS_PR_REV 3200
// motor 1
#define _INA1 17 // a1 (gray)
#define _INB1 16  // b1 (lilla)
#define _PWM1 14  // pwm1
#define _EN1DIAG1 18 // en1
#define _CS1 15 // cs1
// motor 2

#define _INA2 27 // a2 (gray)
#define _INB2 26 // b2 (lilla)
#define _PWM2 12 // pwm2
#define _EN2DIAG2 28 // en2
#define _CS2 25 // cs2

#define PWM_FREQ 18310.55
#define MAX_SPEED_PWM (int)pow(2,13)
#define MIN_SPEED_PWM 500

#define VELOC_REF 10
#define RADIANS_PR_COUNT  (double)(2*M_PI/COUNTS_PR_REV)

// sensor sampling
volatile uint32_t deltaTTempLeft=0;
volatile uint32_t deltaTTempRight=0;
volatile uint32_t deltaTLeftBuffer[MAX_COUNTS];
volatile uint32_t deltaTRightBuffer[MAX_COUNTS];
volatile uint32_t deltaTBufferCount =0;
char message[100];

// filter coeffiences
float b0_control = 0.92;
float b1_control = -0.79;
float a1_control = -1;

// helper classes
class MotorDriver{
  int pwm;
  int inA;
  int inB;
  int diag;
  int cs;
  double pwmFreq;
  public:
    MotorDriver(int pwm,int inA,int inB,int diag,int cs,double pwmFreq):
      pwm(pwm),
      inA(inA),
      inB(inB),
      diag(diag),
      cs(cs),
      pwmFreq(pwmFreq)
    {
      pinMode(inA,OUTPUT);
      pinMode(inB,OUTPUT);
      pinMode(pwm,OUTPUT);
      pinMode(diag,INPUT);
      pinMode(cs,INPUT);
      digitalWrite(inA,LOW);
      digitalWrite(inB,HIGH);
      analogWriteFrequency(pwm,pwmFreq);
    }

    void forward(int speed){
      analogWrite(pwm,speed);
    }
};

TransposedIIRFilter *piControllerLeft;
TransposedIIRFilter *piControllerRight;
MotorDriver * motorLeft;
MotorDriver * motorRight;
QuadratureEncoders * sensors;

void runProgram1();
void start();
double radPrSekFromDeltaT(uint32_t deltaT);
void outputResult();
void reset();
void init();

extern "C" int main(void){
	init();
	start();
	//runProgram1();
  //analogWrite(_PWM1,0);
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
	reset();
	sensors->startSampleTimer();
	//timer->begin(ISR_SAMPLE,INTERVAL_MICROS);
	double wLeft=0;
	double wRight=0;

	while(1){
		 if(sensors->isSampleReady()){
			if(deltaTBufferCount<MAX_COUNTS){
				deltaTTempLeft =
						sensors->encoder(QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_left)
									 ->read<uint32_t>(QuadratureEncoderReadTypes::time_interval_micros_filtered);
				deltaTTempRight =
						sensors->encoder(QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_right)
											 ->read<uint32_t>(QuadratureEncoderReadTypes::time_interval_micros_filtered);
				// store data
				deltaTLeftBuffer[deltaTBufferCount] = deltaTTempLeft;
				deltaTRightBuffer[deltaTBufferCount]= deltaTTempRight;
				deltaTBufferCount++;

				wLeft  = radPrSekFromDeltaT(deltaTTempLeft);
				wRight = radPrSekFromDeltaT(deltaTTempRight);


				// update controller for left
				if(isinf(wLeft)){
					wLeft = 0;
				}
				wLeft = piControllerLeft->update(VELOC_REF- wLeft);
				if(wLeft < MIN_SPEED_PWM)
					wLeft =MIN_SPEED_PWM;
				else if(wLeft>MAX_SPEED_PWM){
					wLeft = MAX_SPEED_PWM;
				}
				// update controller for right
				if(isinf(wRight)){
					wRight = 0;
				}
				wRight = piControllerRight->update(VELOC_REF- wRight);
				if(wRight < MIN_SPEED_PWM)
					wRight =MIN_SPEED_PWM;
				else if(wRight>MAX_SPEED_PWM){
					wRight = MAX_SPEED_PWM;
				}

				motorRight->forward(wRight);
				motorLeft->forward(wLeft);

				sensors->clearSampleReady();
			}else{
				sensors->stopSampleTimer();
				motorLeft->forward(0);
				motorRight->forward(0);
				break;
			}
		}
		delayMicroseconds(1);
	}
	outputResult();
}
double radPrSekFromDeltaT(uint32_t deltaT) {
	double s;
	s =  (double)(deltaT/pow(10,6));
	return (double)(RADIANS_PR_COUNT/s);
}
void reset(){
  piControllerLeft->reset();
  piControllerRight->reset();
  sensors->reset();
}
void outputResult(){
	uint32_t sumDeltaT = 0;
		double sumOmega =0;
		for(int i=0; i<MAX_COUNTS; i++){
			sumDeltaT = sumDeltaT + deltaTLeftBuffer[i];
			sumOmega =   sumOmega +   radPrSekFromDeltaT(deltaTLeftBuffer[i]);
			if(i==MAX_COUNTS-1){

				double meanDeltaT = sumDeltaT/MAX_COUNTS;
				double meanOmega =  sumOmega/MAX_COUNTS;
				double varDeltaT = 0;
				double varOmega = 0;
				for(int n = 0; n < MAX_COUNTS; n++ )
				{
				  //Serial.print(n);
				  //Serial.print(deltaTLeftBuffer[n]);

				  Serial.print(" ");
				  Serial.print(radPrSekFromDeltaT(deltaTLeftBuffer[n]));

				  Serial.print(" ");
				  Serial.println(radPrSekFromDeltaT(deltaTRightBuffer[n]));

				  //Serial.print(" ");
				  //Serial.println(radPrSekFromDeltaT(deltaTLeftBuffer[n])/(2*M_PI)*60);

				  //Serial.print("  mean deltaT=");
				  //Serial.print(meanDeltaT);

				  //Serial.print("  mean omega=");
				  //Serial.println(meanOmega);
				  varDeltaT += pow( deltaTLeftBuffer[n] - meanDeltaT, 2);
				  varOmega += pow( radPrSekFromDeltaT(deltaTLeftBuffer[n]) - meanOmega, 2);
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
void init(){
	analogWriteResolution(13);
	Serial.begin(115200);

	piControllerLeft = new TransposedIIRFilter(b0_control,b1_control,a1_control);
	piControllerRight = new TransposedIIRFilter(b0_control,b1_control,a1_control);

	TransposedIIRFilter *sensorFilterLeft = new TransposedIIRFilter(
			VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B0,
			VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B1,
			VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_A1);

	TransposedIIRFilter *sensorFilterRight = new TransposedIIRFilter(
			VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B0,
			VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B1,
			VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_A1);

	motorLeft  = new MotorDriver(_PWM2,_INA2,_INB2,_EN2DIAG2,_CS2,PWM_FREQ);
	motorRight = new MotorDriver(_PWM1,_INA1,_INB1,_EN1DIAG1,_CS1,PWM_FREQ);


	QuadratureEncorderParameters *encorderParametersLeft = new QuadratureEncorderParameters(
			VEHICLE_PIN_QUADRAENCODER_LEFT_CHANNEL_A,
			VEHICLE_PIN_QUADRAENCODER_LEFT_CHANNEL_B,
			VEHICLE_WHEEL_RADIUS_CM,
			VEHICLE_MOTOR_ENCODER_COUNT_PR_REV
			);

	QuadratureEncorderParameters *encorderParametersRight = new QuadratureEncorderParameters(
			VEHICLE_PIN_QUADRAENCODER_RIGHT_CHANNEL_A,
			VEHICLE_PIN_QUADRAENCODER_RIGHT_CHANNEL_B,
			VEHICLE_WHEEL_RADIUS_CM,
			VEHICLE_MOTOR_ENCODER_COUNT_PR_REV
			);

	sensors =  new QuadratureEncoders(
			encorderParametersLeft,
			sensorFilterLeft,
			encorderParametersRight,
			sensorFilterRight,
			VEHICLE_SAMPLE_TIMER_INTERVAL_MICROS);
	sensors->setupEncoders();
}
