#include "CryptoIO.h"

#include <i2s.h>
#include <i2s_reg.h>

  os_timer_t CryptoIO::timer; 
  uint8_t CryptoIO::pin_mode[32]={};
  uint32_t CryptoIO::pin_value[32]={};
  uint32_t CryptoIO::pin_counter[32]={};

  uint32_t CryptoIO::operating_frequency=20000;
  uint32_t CryptoIO::pin_bit_mask=0x0;
  uint8_t CryptoIO::pin_count=32;

  uint8_t CryptoIO::linear_brightness_curve[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   1,   1,   1,   1,   1,   1,
    1,   1,   1,   1,   1,   1,   1,   1, 
    1,   1,   1,   1,   1,   1,   1,   1,
    1,   1,   2,   2,   2,   2,   2,   2, 
    2,   2,   2,   2,   2,   2,   2,   2,
    2,   3,   3,   3,   3,   3,   3,   3, 
    3,   3,   3,   3,   3,   4,   4,   4,
    4,   4,   4,   4,   4,   4,   5,   5, 
    5,   5,   5,   5,   5,   5,   6,   6,
    6,   6,   6,   6,   6,   7,   7,   7, 
    7,   7,   8,   8,   8,   8,   8,   9,
    9,   9,   9,   9,  10,  10,  10,  10, 
   11,  11,  11,  11,  12,  12,  12,  12,
   13,  13,  13,  14,  14,  14,  15,  15, 
   15,  16,  16,  16,  17,  17,  18,  18,
   18,  19,  19,  20,  20,  21,  21,  22, 
   22,  23,  23,  24,  24,  25,  25,  26,
   26,  27,  28,  28,  29,  30,  30,  31, 
   32,  32,  33,  34,  35,  35,  36,  37,
   38,  39,  40,  40,  41,  42,  43,  44, 
   45,  46,  47,  48,  49,  51,  52,  53,
   54,  55,  56,  58,  59,  60,  62,  63, 
   64,  66,  67,  69,  70,  72,  73,  75,
   77,  78,  80,  82,  84,  86,  88,  90, 
   91,  94,  96,  98, 100, 102, 104, 107,
  109, 111, 114, 116, 119, 122, 124, 127, 
  130, 133, 136, 139, 142, 145, 148, 151,
  155, 158, 161, 165, 169, 172, 176, 180, 
  184, 188, 192, 196, 201, 205, 210, 214,
  219, 224, 229, 234, 239, 244, 250, 255
};

CryptoIO::CryptoIO()
{
}

void CryptoIO::timer_call(void *arguments)
{
  CryptoIO::run();
}

void ICACHE_RAM_ATTR CryptoIO::run()
{
  if (i2s_available())
  while (!i2s_is_full())
  {
    for (int i=0; i<pin_count; i++)
    {
      if (pin_mode[i]==CRYPTOIO_DIGITAL) continue;
      else if (pin_mode[i]==CRYPTOIO_ANALOG)
      {
        pin_counter[i]++;
        pin_counter[i]%=256;
        bitWrite(CryptoIO::pin_bit_mask, i, (pin_counter[i]<pin_value[i]));
      }
      else if (pin_mode[i]==CRYPTOIO_DS)
      {
        pin_counter[i]+=pin_value[i];
        bitWrite(CryptoIO::pin_bit_mask, i, (pin_counter[i]>=255));
        pin_counter[i]%=256;
      }
      else if (pin_mode[i]==CRYPTOIO_SERVO)
      {
        pin_counter[i]++;
        pin_counter[i]%=(operating_frequency/50);
        bitWrite(CryptoIO::pin_bit_mask, i, (pin_counter[i]<pin_value[i]));
      }
      else if (pin_mode[i]==CRYPTOIO_LED)
      {
        pin_counter[i]+=linear_brightness_curve[pin_value[i]];
        bitWrite(CryptoIO::pin_bit_mask, i, (pin_counter[i]>=255));
        pin_counter[i]%=256;
      }
      
    }
    i2s_write_sample(pin_bit_mask);
  }
}

void CryptoIO::init(uint32_t frequency, uint8_t pins)
{
  begin(frequency, pins);
}

void CryptoIO::begin(uint32_t frequency, uint8_t pins)
{
  pin_count=pins;

  for (int i=0; i<32; i++)
  {
    pin_mode[i] = CRYPTOIO_DIGITAL;
    pin_value[i] = 0;
    pin_counter[i] = 0;
  }
  
  pin_bit_mask = 0x0;
  i2s_set_callback(CryptoIO::run);
  i2s_begin();
  i2s_set_rate(frequency);
  operating_frequency = i2s_get_real_rate();
  
  //os_timer_setfn(&timer, CryptoIO::timer_call, NULL);
  //os_timer_arm(&timer, 8, true);
}

void CryptoIO::digitalWrite(uint8_t pin, uint8_t value)
{
  pin_mode[pin]=CRYPTOIO_DIGITAL;
  bitWrite(CryptoIO::pin_bit_mask, pin, value);
}

void CryptoIO::analogWrite(uint8_t pin, uint8_t value)
{
  if (value>=255 || value==0) return CryptoIO::digitalWrite(pin, value);
  pin_mode[pin]=CRYPTOIO_ANALOG;
  pin_value[pin]=value;
}

void CryptoIO::DSWrite(uint8_t pin, uint8_t value)
{
  pin_mode[pin]=CRYPTOIO_DS;
  pin_value[pin]=(value>255) ? 255:value;
}

void CryptoIO::servoWrite(uint8_t pin, uint8_t value)
{
  pin_mode[pin]=CRYPTOIO_SERVO;
  pin_value[pin]=(value>99) ? 99:value;
  pin_value[pin]=(operating_frequency*0.0011)+((operating_frequency*0.0020)/100)*pin_value[pin];
}

void CryptoIO::LEDWrite(uint8_t pin, uint8_t value)
{
  pin_mode[pin]=CRYPTOIO_LED;
  pin_value[pin]=(value>255) ? 255:value;
}

uint8_t CryptoIO::getValue(uint8_t pin)
{
  return pin_value[pin];
}

uint8_t CryptoIO::getMode(uint8_t pin)
{
  return pin_mode[pin];
}

unsigned int sinceTimer(const unsigned int millis_value)
{
  return millis()-millis_value;
}

void resetTimer(unsigned int &millis_value)
{
  millis_value = millis();
}
