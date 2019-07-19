// This code controls a single vibrotactile motor on a PWM pin
// can set intensity of vibration and delay to pulse the vibration on and off at lower frequencies.
// Joss Moo-Young
#include "Arduino.h"
//#include "Timer.h"

class VibrotactileMotor
{
  public:
  // frequency of vibration (delay in ms)
  int vibrationDelay = 100;
  // intesity of vibration (0-255)
  short intensity = 0;

  private:
  short motorIntensity = 255;
  short assignedPin;
  
  int timeElapsedSinceLastPulse = 0;
  int lastTimePulsed = 0;

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
  setPin(short pinNumber)
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

  inline void setIntensity(short intensity0_255)
  {
    intensity = intensity0_255;
  }

  inline void pulseMotor(int intensity, int frequency)
  {
    
  }

  // update vibrotactile motor
  inline void motorUpdate()
  {
    // Update internal timer
    /////////////////////////
    /////////////////////////
    timeElapsedSinceLastPulse = millis() - lastTimePulsed();
    
    if(vibrationDelay < timeElapsedSinceLastPulse)
    { 
      timer = timer - vibrationDelay;
      // toggle motor output between positive and negative to indicate on or off
      motorIntensity = -motorIntensity;
    }
    
    lastTimePulsed = millis();  
    ////////////////////
    ////////////////////
    
    // if no delay or motor should vibrate then vibrate
    if(motorIntensity > 0 || vibrationDelay == 0)
    {
      analogWrite(assignedPin, motorIntensity);
    } else
    {
       analogWrite(assignedPin, 0);
    }
  }
};
