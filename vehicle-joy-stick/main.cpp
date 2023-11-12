#include "src/MovingAverageFilter.h"
#include "src/QuadJoyStickDriver.h"
#include <SPI.h>
#include "libraries/RF24/printf.h"
#include "libraries/RF24/RF24.h"
#include "src/JoyStickController.h"
#include "src/JoyStickPins.h"
#include "src/JoyStickParams.h"


RF24 radio(10, 9); // CE, CSN

const byte address[6] = "00001";
char cmd;

JoyStickController * joyStickController;

IntervalTimer *sampleTimer;
void sample_timer_event();
volatile float voltageVertical;
volatile float voltageHorisontal;

extern "C" int main(void)
{
	joyStickController = new JoyStickController(
			JOYSTICK_ANALOG_RESOLUTION_BIT,
			JOYSTICK_PIN_RIGHT_ANALOG_VERTICAL,
			JOYSTICK_PIN_RIGHT_ANALOG_HORSONTAL,
			JOYSTICK_MIN_VOLTAGE,
			JOYSTICK_MAX_VOLTAGE);

	Serial.begin(9600);
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
	sampleTimer = new IntervalTimer;
	sampleTimer->begin(sample_timer_event,1000);
	char cmd = 's';
	char cmdLast = 's';
	float voltageReadVertical;
	float voltageReadHorisontal;
	while (1) {

		voltageReadVertical = voltageVertical;
		voltageReadHorisontal = voltageHorisontal;
		//Serial.print("vertical=");
		//Serial.print(voltageReadVertical);
		//Serial.print(" horisontal=");
		//Serial.println(voltageReadHorisontal);

		if(voltageReadVertical >= 4){
			cmd = 'y';
		}else if(voltageReadVertical <= 1){
			cmd = 'h';
		}else if(voltageReadHorisontal >= 4){
			cmd = 'd'; // spin left
		}else if(voltageReadHorisontal <=1){
			cmd = 'f';
		}else{
			cmd = 's';
		}
		if(cmd!=cmdLast){
			//Serial.print(cmd);
			radio.write(&cmd, sizeof(cmd));
			cmdLast=cmd;
		}
		delay(1);
	}

}

void sample_timer_event(){
	//voltageVertical = m_pQuadJoyStick->GetVerticalVoltageFilter();
	voltageVertical = joyStickController->getVerticalControl()->GetVerticalVoltageFilter();
	voltageHorisontal = joyStickController->getHorisontalControl()->GetVerticalVoltageFilter();
}

