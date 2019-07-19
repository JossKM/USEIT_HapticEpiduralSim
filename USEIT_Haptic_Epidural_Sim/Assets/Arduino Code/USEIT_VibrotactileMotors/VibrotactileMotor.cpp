#define DEBUG
// This code controls a single vibrotactile motor on a PWM pin
// can set intensity of vibration and delay to pulse the vibration on and off at lower frequencies.
// Joss Moo-Young

#include "Arduino.h"
#include "Timer.h"

class VibrotactileMotor
{
  public:
  // frequency of vibration (delay in ms)
  int vibrationDelay = 100;
  // intesity of vibration (0-255)
  short intensity = 0;
  Timer t;

  private:
  short motorIntensity = 255;
  short assignedPin;

  

  int pulseEvent;
  
  int timeElapsedSinceLastPulse = 0;
  long lastTimePulsed = 0;

  public:

  VibrotactileMotor()
  {
  }
  
  VibrotactileMotor(short pinNumber)
  {
    assignedPin = pinNumber;
    pinMode(assignedPin, OUTPUT);
  }
  
  //set output pin
  void setPin(short pinNumber)
  {
    assignedPin = pinNumber;
    pinMode(assignedPin, OUTPUT);
  }

  //set frequency of pulses, in hertz (cycles/sec)
  inline void setFrequency(int freq_hertz)
  {
    //note that if you set a frequency higher than 1000Hz it the delay will become 0 and the vibration will be continuous. Higher frequencies are not counted.
    //vibrationDelay = 1000/freq_hertz / 2; 1000milliseconds for 1 sec / hertz divided by 2 because this delay is applied twice per period
    vibrationDelay = 500/freq_hertz;
  }

  inline void setIntensity(short intensity0_255 = 255)
  {
    intensity = intensity0_255;
     #ifdef DEBUG
           Serial.print("setIntensity");
           Serial.println(intensity);
     #endif
  }

  // update vibrotactile motor
  inline void motorUpdate()
  {
    
    // Update internal timer
    /////////////////////////
    /////////////////////////
    t.update();
  }
};
