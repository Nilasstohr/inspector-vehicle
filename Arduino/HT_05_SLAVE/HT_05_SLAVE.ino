#include <SoftwareSerial.h>

int readSize =0;
char buffer[100];

int readSizeHT05_slave =0;
char bufferHT05_slave[100];
SoftwareSerial HT05_slave (21,20);

bool master = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  HT05_slave.begin(38400);
  delay(2000);
}

void loop() {
  Serial.println("listning...");
  if(hasMessageHT05_slave()){
      sendMessage();
  }
  delay(1000); 
}

void sendMessage() {
  //Serial.print("received command: ");
  int readSize;
  readSize = readSizeHT05_slave;
  for(int i=0; i<readSize; i++){
      Serial.print(bufferHT05_slave[i]);
  }
   Serial.println();
}
bool hasMessage() {
  readSize = 0;
  Serial.print("have you send a message?");
  if(Serial.available()){
    Serial.println("->yes");
    while (Serial.available()){
      char c = Serial.read();
      if(c=='\n'){
        Serial.println("your message received");
        return true;
      }
      buffer[readSize]=c;
      readSize++;
    }
  }
  Serial.println("->no");
  readSize = 0;
  return false;
}

bool hasMessageHT05_slave() {
  readSizeHT05_slave = 0;
  //Serial.print("has HT05_slave send a message?");
  if(HT05_slave.available()){
    //Serial.println("->yes");
    while (HT05_slave.available()){
      char c = HT05_slave.read();
      if(c=='\n'){
        Serial.println("HT05_slave response:");
        //printMessage();
        return true;
      }
      bufferHT05_slave[readSizeHT05_slave]=c;
      readSizeHT05_slave++;
    }
  }
  //Serial.println("->no");
  readSizeHT05_slave = 0;
  return false;
}
