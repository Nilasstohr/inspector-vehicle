#include "DualVNH5019MotorShield.h"
/*
DualVNH5019MotorShield(
    unsigned char INA1,
    unsigned char INB1,
    unsigned char PWM1,
    unsigned char EN1DIAG1,
    unsigned char CS1,
    unsigned char INA2,
    unsigned char INB2,
    unsigned char PWM2,
    unsigned char EN2DIAG2,
    unsigned char CS2);
*/

DualVNH5019MotorShield md = DualVNH5019MotorShield(
    26, // a1
    27, // b1
    28, // pwm1
    12, // en1
    24, // cs1

    34, // a2
    35, // b2
    33, // pwm2
    39, // en2
    38 // cs2
);

void stopIfFault()
{
  if (md.getM1Fault())
  {
    Serial.println("M1 fault");
    while(1);
  }
  if (md.getM2Fault())
  {
    Serial.println("M2 fault");
    while(1);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Dual VNH5019 Motor Shield");
  md.init();
}

void loop()
{ 

  //stopIfFault();
  //md.setM2Speed(70);
  /*
  for (int i = 0; i <= 400; i++)
  { 
   
   
    Serial.print("M2 input value= ");
    Serial.print(i);
    Serial.print(" M2 current: ");
    Serial.println(md.getM2CurrentMilliamps());
    delay(200);
  }
  
  for (int i = 400; i >= -400; i--)
  {
    md.setM2Speed(i);
    stopIfFault();
    delay(2);
  }
  */

  /*
  for (int i = 0; i <= 400; i++)
  {
    md.setM1Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
    }
    delay(2);
  }
  
  for (int i = 400; i >= -400; i--)
  {
    md.setM1Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
    }
    delay(2);
  }
  
  for (int i = -400; i <= 0; i++)
  {
    md.setM1Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
    }
    delay(2);
  }
  */

  /*
  for (int i = 0; i <= 400; i++)
  {
    md.setM2Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }
    delay(2);
  }
  
  for (int i = 400; i >= -400; i--)
  {
    md.setM2Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }
    delay(2);
  }
  
  for (int i = -400; i <= 0; i++)
  {
    md.setM2Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }
    delay(2);
  }
  */
}
