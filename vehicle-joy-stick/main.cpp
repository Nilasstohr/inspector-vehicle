#include "src/MovingAverageFilter.h"
#include "src/QuadJoyStickDriver.h"
#include <SPI.h>
#include "libraries/RF24/printf.h"
#include "libraries/RF24/RF24.h"


RF24 radio(10, 9); // CE, CSN

const byte address[6] = "00001";

MovingAverageFilter *m_pMovingAverageFilter = new MovingAverageFilter(100,4);

QuadJoyStickDriver *m_pQuadJoyStick        = new QuadJoyStickDriver(
    m_pMovingAverageFilter,
    16,
    A8,
    0,
    0,
    4
  );

IntervalTimer *sampleTimer;
void sample_timer_event();
volatile float voltage;

extern "C" int main(void)
{
	sampleTimer = new IntervalTimer;
	sampleTimer->begin(sample_timer_event,1000);
	Serial.begin(115200);
	m_pQuadJoyStick->init();
	char cmd = 's';
	char cmdLast = 's';
	float voltageRead;

	radio.begin();
	radio.openWritingPipe(address);
	radio.setPALevel(RF24_PA_MIN);
	radio.stopListening();
	delay(1000);
	if(!radio.isChipConnected()){
		while(1){
		  Serial.println("chip is no connected");
		  delay(1000);
		}
	}
	while(1){
		radio.write(&cmd, sizeof(cmd));
		delay(1000);
	}


	String cmdStr = "";
	while (1) {
		voltageRead = voltage;
		if(voltageRead >= 4){
			cmd = 'h';
		}else if(voltageRead <= 1){
			cmd = 'd';
		}else{
			cmd = 's';
		}
		if(cmd!=cmdLast){
			Serial.print(cmd);
			radio.write(&cmd, sizeof(cmd));
			cmdLast=cmd;
		}
		delay(1);
	}
}

void sample_timer_event(){
	voltage = m_pQuadJoyStick->GetThrotleVoltageFilter();
}

