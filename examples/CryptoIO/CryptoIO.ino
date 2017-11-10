#include "CryptoIO.h"

uint32_t counter=0;
int direction=1;
unsigned int myTimer;

void setup()
{
  CryptoIO::begin(100000, 8);
  resetTimer(myTimer);
}

void loop()
{
if (sinceTimer(myTimer)>100)
{
  resetTimer(myTimer);
  counter+=direction;
  if (counter==255 || counter==0) direction*=-1;
  CryptoIO::digitalWrite(1 ,direction); //LOW - HIGH
  CryptoIO::analogWrite(2 ,counter); //0 - 255
  CryptoIO::DSWrite(3 ,counter); //0 - 255
  CryptoIO::servoWrite(4 ,(uint8_t)counter/2.55); //0 - 100
  CryptoIO::LEDWrite(5 ,counter); //0 - 255
}
delay(0);
}
