char message[100];
#define MAX_COUNTS 3200

unsigned int INTERVAL_MICROS = 10000;
#define LEFT_A  22
#define LEFT_B  23
#define MAX_COUNTS 3200

volatile signed int counts =0;
volatile uint32_t deltaT=0;
volatile uint32_t lastT=0;
volatile uint32_t currentT=0;

volatile uint32_t deltaTBuffer[MAX_COUNTS+200];
volatile uint32_t buftterCounter[MAX_COUNTS+200];
volatile signed int countsbuffer[MAX_COUNTS+200];
volatile uint32_t deltaTBufferCount =0;


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  analogWriteResolution(16);
	analogReadResolution(10);
	Serial.begin(115200);

  analogWriteFrequency(getPwmPin(),this->motorDriverPins->getPwmFrequency());

	pinMode(LEFT_A, INPUT);
	digitalWrite(LEFT_A, LOW);

	pinMode(LEFT_B, INPUT);
	digitalWrite(LEFT_B, LOW);

	attachInterrupt(LEFT_A,ISR_A, CHANGE);
	attachInterrupt(LEFT_B,ISR_B, CHANGE);


	timer = new IntervalTimer;
	//timer->begin(ISR_SAMPLE,INTERVAL_MICROS);
}
void run(){
  counts =0;
  deltaT=0;
  lastT=0;
  currentT=0;
  encoderTest();
  for (int i=0; i<=MAX_COUNTS; i++) {
    Serial.print(currentT[i]);
    if(i==SIZE)
      break;
    Serial.print(',');
  }
  Serial.print(";");
}
void encoderTest(){
  motors->forward(15000);
	while(1){
		if(counts > MAX_COUNTS){
			detachInterrupt(LEFT_A);
			detachInterrupt(LEFT_B);
			timer->end();
			motors->forward(0);
			break;
		}
		delayMicroseconds(1);
	}

	for(int i=0; i<3200; i++){
		Serial.print(countsbuffer[i]);
		Serial.print(" ");
		Serial.println(deltaTBuffer[i]);
	}
}
void ISR_SAMPLE() {
	Serial.print("counts=");
    Serial.print(counts);
	Serial.print(" time=");
	Serial.println(deltaT);
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
	if(deltaTBufferCount<MAX_COUNTS){
		deltaTBuffer[counts]=deltaT;
		countsbuffer[counts]=counts;
	}
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
	if(counts<MAX_COUNTS){
		deltaTBuffer[counts]=deltaT;
		countsbuffer[counts]=counts;
	}
}

void loop() {
  if(Serial.available()){
    int i=0;
    while (Serial.available()) {
        message[i] = Serial.read();
        i++;
    }
    String s = message;
    if(s.equals("start")){
      digitalWrite(LED_BUILTIN, HIGH);
      run();
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}