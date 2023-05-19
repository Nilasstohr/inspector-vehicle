#define MAX_COUNTS 15000
#define COUNTS_PR_REV 3200

// motor driver

// motor 1
//#define _ENC1_A  22
//#define _ENC1_B  23
#define _INA1 17 // a1 (gray)
#define _INB1 16  // b1 (lilla)
#define _PWM1 14  // pwm1
#define _EN1DIAG1 18 // en1
#define _CS1 15 // cs1

// motor 2
/*
#define LEFT_A  31
#define LEFT_B  32
#define _INA2 27 // a2 (gray)
#define _INB2 26 // b2 (lilla)
#define _PWM2 12 // pwm2
#define _EN2DIAG2 28 // en2
#define _CS2 25 // cs2
*/

#define LEFT_A  22
#define LEFT_B  23
#define _INA2 17 // a2 (gray)
#define _INB2 16 // b2 (lilla)
#define _PWM2 14 // pwm2
#define _EN2DIAG2 18 // en2
#define _CS2 15 // cs2


#define PWM_FREQ 18310.55
#define MAX_SPEED_PWM (int)pow(2,13)
#define MIN_SPEED_PWM 500

#define VELOC_REF 15


unsigned int INTERVAL_MICROS = 100;


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

float b0_sensor = 0.0591907;
float b1_sensor = 0.0591907;
float a1_sensor = -0.88161859;
double feedForward_sensor=0;
double feedBack_sensor=0;
double output_sensor=0;

float b0_control = 0.92;
float b1_control = -0.79;
float a1_control = -1;
double feedForward_control=0;
double feedBack_control=0;
double feedThroug_control=0;
double output_control=0;
double error_control;

bool sampleReady=false;

void setup() {
  init();
}

void loop() { 
	runProgram1();
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
	counts =0;
	deltaTBufferCount =0;
	deltaT   = 0;
	lastT    = 0;
	finish = false;
	timer->begin(ISR_SAMPLE,INTERVAL_MICROS);
	double w=0;
	while(1){
		if(finish){
      forwardM2(0);
			break;
		}
		else if(sampleReady){
			w = radPrSekFromDeltaT(output_sensor);
			if(isinf(w)){
				w = 0;
			}

      //  b0 = 0.92, b1 = -0.79; a1 = -1;
			error_control = VELOC_REF-w;
      feedThroug_control = b0_control*error_control;
			output_control  = feedThroug_control + feedForward_control - feedBack_control;
      forwardM2(output_control);

      if(output_control < MIN_SPEED_PWM)
        output_control =MIN_SPEED_PWM;
      else if(output_control>MAX_SPEED_PWM){
        output_control = MAX_SPEED_PWM;
      }
			/*
			Serial.print("w= ");
			Serial.print(w);
			Serial.print(" e= ");
			Serial.print(error_control);
      Serial.print(" ft= ");
      Serial.print(feedThroug_control);
			Serial.print(" ff= ");
			Serial.print(feedForward_control);
			Serial.print(" fb= ");
			Serial.print(feedBack_control);
			Serial.print(" o= ");
			Serial.print(output_control);
      */
			feedForward_control = b1_control*error_control;
			feedBack_control    = a1_control*output_control;
			sampleReady = false;
      /*
      Serial.print(" ff(next)= ");
      Serial.print(feedForward_control);
      Serial.print(" fb(next)= ");
      Serial.println(feedBack_control);
      */
      
		}
		delayMicroseconds(1);
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
		output_sensor  = b0_sensor*deltaT + feedForward_sensor - feedBack_sensor;
		deltaTBufferFiltered[deltaTBufferCount]=output_sensor;
		deltaTBuffer[deltaTBufferCount]=deltaT;
		sampleReady = true;
		feedForward_sensor = b1_sensor*output_sensor;
		feedBack_sensor    = a1_sensor*output_sensor;
		deltaTBufferCount++;
		//Serial.println(deltaTBuffer[deltaTBufferCount]);
		//Serial.print(deltaTBuffer[deltaTBufferCount]);
    //Serial.println(deltaTBufferCount);
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
void forwardM2(int speed){
  analogWrite(_PWM2,speed); 
}
void init(){
	analogWriteResolution(13);
	//analogReadResolution(10);
	Serial.begin(115200);

	// motors
  /*
	pinMode(_INA1,OUTPUT);
	pinMode(_INB1,OUTPUT);
	pinMode(_PWM1,OUTPUT);
	pinMode(_EN1DIAG1,INPUT);
	pinMode(_CS1,INPUT);
	digitalWrite(_INA1,HIGH);
	digitalWrite(_INB1,LOW);
  analogWriteFrequency(_PWM1,PWM_FREQ);
  */
  
  pinMode(_INA2,OUTPUT);
	pinMode(_INB2,OUTPUT);
	pinMode(_PWM2,OUTPUT);
	pinMode(_EN2DIAG2,INPUT);
	pinMode(_CS2,INPUT);
	digitalWrite(_INA2,HIGH);
	digitalWrite(_INB2,LOW);
  analogWriteFrequency(_PWM2,PWM_FREQ);
	
	// encoders
	pinMode(LEFT_A, INPUT);
	digitalWrite(LEFT_A, LOW);

	pinMode(LEFT_B, INPUT);
	digitalWrite(LEFT_B, LOW);

	attachInterrupt(LEFT_A,ISR_A, CHANGE);
	attachInterrupt(LEFT_B,ISR_B, CHANGE);

	timer = new IntervalTimer;

}
