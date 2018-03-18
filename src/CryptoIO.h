#ifndef CRYPTOIO_H
#define CRYPTOIO_H

#include <Arduino.h>
extern "C" {
  #include "user_interface.h"
}

#define CRYPTOIO_DIGITAL 0
#define CRYPTOIO_ANALOG 1
#define CRYPTOIO_DS 2
#define CRYPTOIO_SERVO 3
#define CRYPTOIO_LED 4

// GPIO2 (D4) = i2s output ws (latch - pin 12)
// GPIO15(D8) = i2s output clock (serial clock in - pin 11)
// GPIO3 (RX) = i2s output data (serial data in - pin 14)

class CryptoIO
{
  public:
  CryptoIO();
  static void init(uint32_t frequency, uint8_t pins);
  static void begin(uint32_t frequency, uint8_t pins);
  static void digitalWrite(uint8_t pin, uint8_t value);
  static void analogWrite(uint8_t pin, uint8_t value);
  static void DSWrite(uint8_t pin, uint8_t value); //first order DeltaSigma
  static void servoWrite(uint8_t pin, uint8_t value);
  static void LEDWrite(uint8_t pin, uint8_t value);
  static uint8_t getValue(uint8_t pin);
  static uint8_t getMode(uint8_t pin);
  static void run();
  
  private:
  static uint8_t linear_brightness_curve[256];
 
  static void timer_call(void *arguments);
  static os_timer_t timer;  

  static uint8_t pin_mode[32];
  static uint32_t pin_value[32];
  static uint32_t pin_counter[32];
  
  static uint32_t operating_frequency;
  static uint32_t pin_bit_mask;
  static uint8_t pin_count;
  
};


unsigned int sinceTimer(const unsigned int millis_value);
void resetTimer(unsigned int &millis_value);

#endif
