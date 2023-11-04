/*
* Arduino Wireless Communication Tutorial
*       Example 1 - Receiver Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(10, 9); // CE, CSN

const byte address[6] = "00001";
char cmd;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  /*
  if(!radio.isChipConnected()){
    while(1){
      Serial.println("chip is no connected");
    }
  }
  */
}

void loop() {
  //Serial.println("checking if received any data:");
  if (radio.available()) {
    //Serial.println("received -");
    char text[32] = "";
    radio.read(&cmd, sizeof(cmd));
    Serial.println(cmd);
  }
  delay(1000);
}