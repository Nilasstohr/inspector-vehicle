#include <MovingAverageFilter.h>
#include <QuadJoyStickDriver.h>
/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  This example code is in the public domain.
 */

MovingAverageFilter *m_pMovingAverageFilter = new MovingAverageFilter(100,4);

QuadJoyStickDriver *m_pQuadJoyStick        = new QuadJoyStickDriver(
    m_pMovingAverageFilter,
    16,
    A8,
    0,
    0,
    4
  );

float throtle;


typedef struct sJoyStick
 {
    float Channel;
    float Voltage;
 }JoyStick;

    JoyStick  *m_JS_1;
    uint16_t m_iResolution;
    float    m_fConv;
    uint16_t m_iVoltageMin;
    uint16_t m_iVoltageMax;


int led = 13;

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200);
  m_pQuadJoyStick->init();
  pinMode(led, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  //digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(1000);               // wait for a second
  //digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  //delay(1000);               // wait for a second
  throtle = m_pQuadJoyStick->GetThrotleVoltageFilter();
  Serial.println(throtle);
  delay(1);
}
