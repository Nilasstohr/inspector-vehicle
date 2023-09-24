#include <SoftwareSerial.h>

const byte rxPin = 2;
const byte txPin = 3;

int readSize =0;
char buffer[100];

int readSizeHT05_master =0;
char bufferHT05_master[100];
SoftwareSerial HT05_master (9, 10);

int readSizeHT05_slave =0;
char bufferHT05_slave[100];
SoftwareSerial HT05_slave (7,8);

bool master = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  HT05_master.begin(38400);
  HT05_slave.begin(38400);
  delay(2000);

  HT05_master.print("hello world\r\n");
  delay(1000);
  if(hasMessageHT05_slave()){
      sendMessage(false);
  }
  
  //executeAT("AT+ROLE?\r\n",false);
  //executeAT("AT+BIND=2014,03,198906\r\n",true); 
  //executeAT("AT+BIND?\r\n",true); 
/*
  Serial.println("---------------------- master ---------------------");
  commom(true);
  executeAT("AT+ROLE=1\r\n",true);
  executeAT("AT+CMODE=0\r\n",true);
  Serial.println("---------------------- slave  ---------------------") ;
  commom(false);
  executeAT("AT+ROLE=0\r\n",false);


  Serial.println("---------------------- Binding ---------------------") ;
  executeAT("AT+BIND=2014,3,198906\r\n",true); 
  //executeAT("AT+LINK=2014,3,198906\r\n",true);
  delay(30000);
  executeAT("AT+STATE?\r\n",true);
*/
  
  /*
  if(master==true){ // ADDR:2014:3:198906
    // master
    Serial.println("-----------MASTER------------");
    Serial.print("AT+ROLE=1\r\n");
    HT05_master.print( "AT+ROLE=1\r\n");
    delay(200);
    if(hasMessageHT05_master()){
      sendMessage();
    }
  }else{
    // slave
    Serial.println("-----------SLAVE------------");
    Serial.print("AT+ROLE=0\r\n");
    HT05_master.print( "AT+ROLE=0\r\n");
    delay(200);
    if(hasMessageHT05_master()){
      sendMessage();
    }
  }
*/
}

void loop() {
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);  
  //if(hasMessageHT05_master()){
  //  sendMessage();
 // }
  /*
  if(hasMessage()){
    send2HT05_master();
  }
  delay(3000);
  if(hasMessageHT05_master()){
    sendMessage();
  }
  */
  
 
  
}


void commom(bool master){
  // commom 
  executeAT("AT\r\n",master);
  //executeAT("AT+RESET\r\n",master);
  executeAT("AT+ORGL\r\n",master);
  executeAT("AT+ADDR?\r\n",master);
  //executeAT("AT+ROLE?\r\n",master);
}

void executeAT(String cmd,bool master){
  Serial.print(cmd);
  if(master){
    Serial.println("to master");
    HT05_master.print(cmd);
  }
  else{
    Serial.println("to slave");
    HT05_slave.print(cmd);
  }
  delay(1000);
  if(master){
    if(hasMessageHT05_master()){
      sendMessage(master);
    }
  }else{
    if(hasMessageHT05_slave()){
      sendMessage(master);
    }
  }
  Serial.println();
}

void send2HT05_master() {
  //Serial.print("received command: ");
  for(int i=0; i<readSize; i++){
    HT05_master.print(buffer[i]);
  }
}

void sendMessage(bool master) {
  //Serial.print("received command: ");
  int readSize;
  if(master)
    readSize = readSizeHT05_master;
  else
    readSize = readSizeHT05_slave;
    
  for(int i=0; i<readSize; i++){
    if(master)
      Serial.print(bufferHT05_master[i]);
    else
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

bool hasMessageHT05_master() {
  readSizeHT05_master = 0;
  //Serial.print("has HT05_master send a message?");
  if(HT05_master.available()){
    //Serial.println("->yes");
    while (HT05_master.available()){
      char c = HT05_master.read();
      if(c=='\n'){
        Serial.println("HT05_master response:");
        //printMessage();
        return true;
      }
      bufferHT05_master[readSizeHT05_master]=c;
      readSizeHT05_master++;
    }
  }
  //Serial.println("->no");
  readSizeHT05_master = 0;
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
