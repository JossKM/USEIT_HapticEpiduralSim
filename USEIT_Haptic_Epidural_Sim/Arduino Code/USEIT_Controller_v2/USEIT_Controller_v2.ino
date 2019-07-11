#define DEBUG // if defined, will print debug messages to the serial line.
//#include "Timer.h"

// input signals on the serial line to rotate the stepper motor CCW or CW.

// SIGNAL MAPPING /////////////
// Arduino receives bytes. so 256 different commands are available (0-255). Here is what each is defined to mean in this program.
// 0 - NULL (nothing)
// 1 - 200 STEPPER MOTOR POSITION RANGE i.e. 1 is full off and 200 is full on

// microseconds between stepper motor pulses (delay in microseconds)
const int stepDelay = 800;
//Motor variables
const int stepsPerRevolution = 800; // number of steps to 360 degree rotation (depending on the motor)
const int STEP_90 = stepsPerRevolution / 4; // 90 degrees in steps
int currentStepPosition = 0; // in steps
int targetStepPosition = 0; // in steps

// motor controller pins
const int stepPin = 13; 
const int dirPin = 12; 

//IO
//const int INPUT_PIN = A0;
short inputByte = 0; // for serial reading. 0 is used as a null input (no response) 2 bytes long (16 bits)
//Timer timer;
// time between serial line reads (delay in milliseconds)
//long serialReadDelay = 16;



void setup()
{
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);   
  Serial.begin(9600);
}

void loop()
{
 stepperMotorUpdate();
 readSerialLine();
 
 delay(1000); step(210); delay(1000); step(-210);
}

void readSerialLine()
{
    while (Serial.available())
    {
       inputByte = Serial.read();
        //will respond only to the last input
    }
}

void stepperMotorUpdate()
{
  // if input is not 0 (being used for "null" command)
  if (inputByte != 0)
  {
#ifdef DEBUG
    // say what you got:
     Serial.print("Arduino received: ");
     Serial.println(inputByte, HEX);
#endif

    if (inputByte <= 200)
    {
    targetStepPosition = map(inputByte, 1, 200, 0, STEP_90);
    } 

#ifdef DEBUG
    Serial.print("moving from ");
    Serial.print(map(currentStepPosition, 0, stepsPerRevolution, 0, 360), DEC);
    Serial.print(" to ");
    Serial.println(map(targetStepPosition, 0, stepsPerRevolution, 0, 360), DEC);
#endif


    // reset input
    inputByte = 0;
  } // if input

   stepTowardTarget();
}

// step the motor 1 step toward targetStepPosition
void stepTowardTarget()
{
  const int maxStepsPerFrame = 100;  
  int error = targetStepPosition - currentStepPosition;

  int numStepsToTake = min(abs(error), maxStepsPerFrame);

    
  if(error == 0)
  {
      //digitalWrite(stepPin,LOW); 
     return; // Target position reached!
  }
  
  if(error > 0) // CW
  {
      //digitalWrite(dirPin,HIGH);
       step(numStepsToTake);
  } else // CCW
  {
      //digitalWrite(dirPin,LOW);
       step(-numStepsToTake);
  }
}


void step(int steps)
{
  int numSteps = abs(steps);
 // CW
  if(steps>0)
    digitalWrite(dirPin,HIGH);
 // CCW
  else
     digitalWrite(dirPin,LOW);

  //timer.oscillate(stepPin, stepDelay, LOW, numSteps);
    
 for(int x = 0; x < numSteps; x++) {
    delayMicroseconds(stepDelay); 
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(stepDelay); 
    digitalWrite(stepPin,LOW); 
  }
  currentStepPosition += steps;
}
