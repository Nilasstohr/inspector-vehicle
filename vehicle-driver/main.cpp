#include <Arduino.h>
#include "src/TransposedIIRFilter.h"
#include "src/VehiclePins.h"
#include "src/VehicleParameters.h"
#include "src/VehicleTestToolBox.h"
#include "src/DualPiVelocityControl.h"
#include "src/PiVelocityControllers.h"

#define MAX_COUNTS 35000
#define COUNTS_PR_REV 3200
// motor 1
#define MAX_SPEED_PWM (int)pow(2,13)
#define MIN_SPEED_PWM 500

#define VELOC_REF 5
#define RADIANS_PR_COUNT  (double)(2*M_PI/COUNTS_PR_REV)

// sensor sampling
volatile uint32_t deltaTTempLeft=0;
volatile uint32_t deltaTTempRight=0;
volatile uint32_t deltaTLeftBuffer[MAX_COUNTS];
volatile uint32_t deltaTRightBuffer[MAX_COUNTS];
volatile uint32_t deltaTBufferCount =0;
char message[100];

DualPiVelocityControl * dualVelocityController;

void runProgram1();
void start();
void velocityContorl();
double radPrSekFromDeltaT(uint32_t deltaT);
void outputResult();
void reset();
void init();

extern "C" int main(void){
	init();
	velocityContorl();
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
	/*
	reset();
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

				wLeft  = sensors->encoder(QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_left)
						->getParameters()->calculateAngularVelocity(deltaTTempLeft);
				//radPrSekFromDeltaT(deltaTTempLeft);


				wRight = sensors->encoder(QuadratureEncoders::QuadratureEncoderSide::quadrature_encoder_right)
								->getParameters()->calculateAngularVelocity(deltaTTempRight);
						//radPrSekFromDeltaT(deltaTTempRight);


				// update controller for left
				if(isinf(wLeft)){
					wLeft = 0;
				}
				wLeft = controlFilters->left()->update(VELOC_REF- wLeft);
				if(wLeft < MIN_SPEED_PWM)
					wLeft =MIN_SPEED_PWM;
				else if(wLeft>MAX_SPEED_PWM){
					wLeft = MAX_SPEED_PWM;
				}
				// update controller for right
				if(isinf(wRight)){
					wRight = 0;
				}

				wRight = controlFilters->right()->update(VELOC_REF- wRight);
				if(wRight < MIN_SPEED_PWM)
					wRight =MIN_SPEED_PWM;
				else if(wRight>MAX_SPEED_PWM){
					wRight = MAX_SPEED_PWM;
				}

				motors->forward(wLeft, wRight);
				sensors->clearSampleReady();
			}else{
				motors->forward(0, 0);
				break;
			}
		}
		delayMicroseconds(1);
	}
	outputResult();
	*/
}
void velocityContorl(){
	reset();
	while(1){
		if(deltaTBufferCount<MAX_COUNTS){
			dualVelocityController->update(VELOC_REF);
			// store data
			if(dualVelocityController->wasSensorReady()){
				deltaTLeftBuffer[deltaTBufferCount] = dualVelocityController->getDeltaTLeft();
				deltaTRightBuffer[deltaTBufferCount]= dualVelocityController->getDeltaTRight();
				deltaTBufferCount++;
			}
		}else{
			reset();
			break;
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
  dualVelocityController->reset();
  deltaTBufferCount=0;
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
	VehicleTestToolBox *toolBox = new VehicleTestToolBox();
	toolBox->buildMotorDrivers();
	toolBox->buildQuadratureEncoders();
	dualVelocityController = toolBox->buildDualPiVelocityControl();

}
