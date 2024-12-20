#define MAX_COUNTS 15000
#define COUNTS_PR_REV 3200

// motor driver

// motor 1
#define _ENC1_A  22
#define _ENC1_B  23
#define _INA1 17 // a1 (gray)
#define _INB1 16  // b1 (lilla)
#define _PWM1 14  // pwm1
#define _EN1DIAG1 18 // en1
#define _CS1 15 // cs1

// motor 2
#define _ENC2_A  31
#define _ENC2_B  32
#define _INA2 27 // a2 (gray)
#define _INB2 26 // b2 (lilla)
#define _PWM2 12 // pwm2
#define _EN2DIAG2 28 // en2
#define _CS2 25 // cs2


#define PWM_FREQ 18310.55
#define MAX_SPEED_PWM (int)pow(2,13)
#define MIN_SPEED_PWM 500

#define VELOC_REF 10

unsigned int INTERVAL_MICROS = 100;

IntervalTimer *timer;

volatile uint32_t deltaT_Temp=0;
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


class TransposedIIRFilter{
  float b0;
  float b1;
  float a1;
  double feedForward;
  double feedBack;
  double error;
  double output;
  
  public:
    TransposedIIRFilter(float b0, float b1, float a1):
      b0(b0),
      b1(b1),
      a1(a1),
      feedForward(0),
      feedBack(0),
      error(0),
      output(0)
    {}

    
    double update(double input) {
      output      = b0*input + feedForward - feedBack;
      /*
      Serial.print(" input= ");
      Serial.print(input);
      Serial.print(" ft= ");
      Serial.print(feedForward);
      Serial.print(" fb= ");
      Serial.print(feedBack);
      Serial.print(" o= ");
      Serial.print(output);
      */
      feedForward = b1*input;
      feedBack    = a1*output;
      return output;
    }
};

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
      digitalWrite(inA,HIGH);
      digitalWrite(inB,LOW);
      analogWriteFrequency(pwm,pwmFreq);
    }

    void forward(int speed){
      analogWrite(pwm,speed); 
    }
};

class DeltaT{
  volatile uint32_t deltaT=0;
  volatile uint32_t lastT=0;
  volatile uint32_t currentT=0;
  volatile signed int counts =0;
  public:
  DeltaT(){
    reset();
  }
  void update(){
      currentT = micros();
      deltaT   = currentT - lastT;
	    lastT    = currentT;
  }
  uint32_t getTime(){
    return deltaT;
  }
  void decrease(){
    counts--;
  }
  void increase(){
    counts++;
  }
  void reset(){
     deltaT=0;
     lastT=0;
     currentT=0;
     counts =0;
  }
};

TransposedIIRFilter *piControllerLeft;
TransposedIIRFilter *piControllerRight;
TransposedIIRFilter *velocityFilterLeft;
TransposedIIRFilter *velocityFilterRight;
MotorDriver * motorLeft;
MotorDriver * motorRight;
DeltaT * deltaTLeft;
DeltaT * deltaTRight;

void setup() {
	analogWriteResolution(13);
	//analogReadResolution(10);
	Serial.begin(115200);

  piControllerLeft = new TransposedIIRFilter(b0_control,b1_control,a1_control);
  piControllerRight = new TransposedIIRFilter(b0_control,b1_control,a1_control);
  
  velocityFilterLeft = new TransposedIIRFilter(b0_sensor,b1_sensor,a1_sensor);
  velocityFilterRight = new TransposedIIRFilter(b0_sensor,b1_sensor,a1_sensor);
  
  motorLeft  = new MotorDriver(_PWM2,_INA2,_INB2,_EN2DIAG2,_CS2,PWM_FREQ);
  motorRight = new MotorDriver(_PWM1,_INA1,_INB1,_EN1DIAG1,_CS1,PWM_FREQ);
  deltaTLeft  = new DeltaT();
  deltaTRight = new DeltaT();
  
  pinMode(_ENC1_A, INPUT);
	digitalWrite(_ENC1_A, LOW);

	pinMode(_ENC1_B, INPUT);
	digitalWrite(_ENC1_B, LOW);

	attachInterrupt(_ENC1_A,ISR_A_LEFT, CHANGE);
	attachInterrupt(_ENC1_B,ISR_B_LEFT, CHANGE);

  pinMode(_ENC2_A, INPUT);
	digitalWrite(_ENC2_A, LOW);

	pinMode(_ENC2_B, INPUT);
	digitalWrite(_ENC2_B, LOW);

	attachInterrupt(_ENC2_A,ISR_A_RIGHT, CHANGE);
	attachInterrupt(_ENC2_B,ISR_B_RIGHT, CHANGE);

	timer = new IntervalTimer;

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
	reset();
	finish = false;
	timer->begin(ISR_SAMPLE,INTERVAL_MICROS);
	double w=0;
	while(1){
		if(finish){
      motorLeft->forward(0);
      motorRight->forward(0);
			break;
		}
		else if(sampleReady){
			w = radPrSekFromDeltaT(output_sensor);
			if(isinf(w)){
				w = 0;
			}
      output_control = piController->update(VELOC_REF-w);
      forwardM2(output_control);

      if(output_control < MIN_SPEED_PWM)
        output_control =MIN_SPEED_PWM;
      else if(output_control>MAX_SPEED_PWM){
        output_control = MAX_SPEED_PWM;
      }
   		sampleReady = false;
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
    // get sample and filter
		deltaT_Temp = deltaT;
		output_sensor  = velocityFilter->update(deltaT_Temp);
    // store data
		deltaTBufferFiltered[deltaTBufferCount]=output_sensor;
		deltaTBuffer[deltaTBufferCount]=deltaT_Temp;
		deltaTBufferCount++;
		//Serial.println(deltaTBuffer[deltaTBufferCount]);
		//Serial.print(deltaTBuffer[deltaTBufferCount]);
    //Serial.println(deltaTBufferCount);
	  sampleReady = true;
	}else{
		timer->end();
		finish = true;
	}

}
void ISR_A_LEFT(){
	deltaTLeft->update();
	uint8_t B = digitalRead(_ENC2_B);
	uint8_t A = digitalRead(_ENC2_A);
	if((A&&B)||(!A&&!B))
    deltaTLeft->decrease();
	else
		deltaTLeft->increase();

}
void ISR_B_LEFT(){
	deltaTLeft->update();
	uint8_t B = digitalRead(_ENC2_B);
	uint8_t A = digitalRead(_ENC2_A);
	if((B&&A)||(!B&&!A))
		deltaTLeft->increase();
	else
		deltaTLeft->decrease();
}
void ISR_A_RIGHT(){
	deltaTRight->update();
	uint8_t B = digitalRead(_ENC1_B);
	uint8_t A = digitalRead(_ENC1_A);
	if((A&&B)||(!A&&!B))
    deltaTRight->decrease();
	else
		deltaTRight->increase();

}
void ISR_B_RIGHT(){
	deltaTRight->update();
	uint8_t B = digitalRead(_ENC1_B);
	uint8_t A = digitalRead(_ENC1_A);
	if((B&&A)||(!B&&!A))
		deltaTRight->increase();
	else
		deltaTRight->decrease();
}
double radPrSekFromDeltaT(uint32_t deltaT) {
	double s;
	s =  (double)(deltaT/pow(10,6));
	return (double)(radian_pr_count/s);
}

void reset(){
  deltaTLeft->reset();
  deltaTRight->reset();
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
