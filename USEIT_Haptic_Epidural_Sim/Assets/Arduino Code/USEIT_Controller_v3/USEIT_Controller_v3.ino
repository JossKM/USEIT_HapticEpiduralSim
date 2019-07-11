//#define DEBUG // if defined, will print debug messages to the serial line.

// SIGNAL MAPPING /////////////
const byte COMMAND_NULL = 0;
const byte COMMAND_START = 1;
const byte COMMAND_STOP = 2;
const byte COMMAND_STEP = 3;

// Arduino receives bytes. so 256 different commands are available (0-255). Here is what each is defined to mean in this program.
// 0 - NULL (nothing)
// 1 - 200 STEPPER MOTOR POSITION RANGE i.e. 1 is full off and 200 is full on

// microseconds between stepper motor pulses from app (delay in microseconds)
// shorter delays may be faster, but less stable. Very long delays might also be unstable. A typical NEMA17 style stepper requires ~600-700 micros delay
const uint16_t STEP_DELAY_MICROSEC = 560;

//Motor config variables
const uint16_t MOTOR_STEPS_PER_REVOL = 800; // number of steps to 360 degree rotation (depending on the motor)
const uint16_t STEP_90 = MOTOR_STEPS_PER_REVOL / 4; // 90 degrees in steps
// motor controller pins
const byte MOTOR_STEP_PIN = 13; 
const byte MOTOR_DIR_PIN = 12; 

// motor control variables
int currentStepPosition = 0; // in steps (0-MOTOR_STEPS_PER_REVOL)
int targetStepPosition = 0; // in steps (0-MOTOR_STEPS_PER_REVOL)
int stepsToTake = 0; // 0 means target reached, sign indicates direction
int stepDirection = 1; // - is CCW, + is CW


//IO
//const int INPUT_PIN = A0;
const uint16_t motorCommandPacketSize = 2; // command size in bytes
uint8_t commandBuffer[4 * motorCommandPacketSize];
uint8_t numCommandsInBuffer = 0;

const unsigned long SERIAL_READ_DELAY_MICROSEC = 16000;

// time in microseconds
unsigned long lastTimeMicros = 0;
unsigned long currentRuntimeMicros = 0;
unsigned long deltaTimeMicros = 0;

void setup()
{
  motorControlSetup();
}

void loop()
{
  motorControlUpdate();
}




inline void motorControlSetup()
{
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);   
  Serial.begin(9600);

  #ifdef DEBUG
       Serial.print("Motor setup complete. Awaiting input...");
  #endif

  lastTimeMicros = micros();
}

//triggers serial reads
long serialReadDelayTimer = SERIAL_READ_DELAY_MICROSEC;

inline void motorControlUpdate()
{
   currentRuntimeMicros = micros(); 
 deltaTimeMicros = currentRuntimeMicros - lastTimeMicros; // overflow is handled because deltaTimeMicros is unsigned and so subtracting a large number will leave it small and positive as expected
 
 serialReadDelayTimer -= deltaTimeMicros;
 
  if(serialReadDelayTimer <= 0)
  {
    readSerialLine();
    processPackets(); 
    serialReadDelayTimer = SERIAL_READ_DELAY_MICROSEC;              
                  /*// test code
                  if(currentRuntimeMicros < 1000000)
                      stepperMotorCommandNonBlocking(0);
                      else
                
                  if(currentRuntimeMicros < 2000000)
                      stepperMotorCommandNonBlocking(255);
                      else
                  
                  if(currentRuntimeMicros < 3000000)
                    stepperMotorCommandNonBlocking(0);
                    else
                
                  if(currentRuntimeMicros < 4000000)
                    stepperMotorCommandNonBlocking(255);
                    else
                    
                  if(currentRuntimeMicros < 5000000)
                    stepperMotorCommandNonBlocking(0);
                    else
                      
                  if(currentRuntimeMicros < 6000000)
                    stepperMotorCommandNonBlocking(125);
                  // \test code*/
  }

  // update stepper motor signals
  stepUpdateNonBlocking();
 
  lastTimeMicros = currentRuntimeMicros;
}

inline void readSerialLine()
{
    while (Serial.available() > motorCommandPacketSize)
    {
      #ifdef DEBUG
           Serial.print("Packet received:" + *commandBuffer);
      #endif
      
     byte commandIdx = numCommandsInBuffer * motorCommandPacketSize;

     // read next motorCommandPacketSize bytes as a packet
          for(int byteIdx = 0; byteIdx < motorCommandPacketSize; byteIdx++)
          {
              commandBuffer[commandIdx + byteIdx] = Serial.read();
          }
          numCommandsInBuffer++;
    }
}

inline void processPackets()
{
    for( int commandNum = 0; commandNum < numCommandsInBuffer; commandNum++)
    {
      byte commandIdx = commandNum * motorCommandPacketSize;

      // step command!
      if(commandBuffer[0] == COMMAND_STEP)
      {
        stepperMotorCommandNonBlocking(commandBuffer[1]);
      }

    }
    numCommandsInBuffer = 0;
}


//triggers motor to turn over time between loop() calls
long motorStepDelayTimerMicros = 0;
int signalToSend = LOW; // for the stepper motor (signals 0 and 1)

// will perform half-steps (swap signal) on timer
inline void stepUpdateNonBlocking()
{
  if(currentStepPosition != targetStepPosition)
  {
    // if enough time has passed
    if(motorStepDelayTimerMicros <= 0)
    {
      // turn motor
      digitalWrite(MOTOR_STEP_PIN,signalToSend); // send signal!
      
      // update step position tracking every HIGH signal (will add 0 if LOW signal)
      currentStepPosition += stepDirection * signalToSend;

      #ifdef DEBUG
          Serial.print("current step: ");
          Serial.print(currentStepPosition);
          Serial.print(" target step: ");
          Serial.println(targetStepPosition);
      #endif
      
      // invert signal for next call so this function sends alternating HIGH and LOW values each time it is called
      signalToSend = 1 - signalToSend;

      //reset timer
      motorStepDelayTimerMicros = STEP_DELAY_MICROSEC; // this timing is smoothed for average signals per second
    }
    
    // update timer
    motorStepDelayTimerMicros -= deltaTimeMicros;
  }
}

inline bool readyToStep() // in case you want to check at some other time
{
   return ((motorStepDelayTimerMicros - (micros() - lastTimeMicros)) <= 0);
}

inline void stepIfReady()
{
  if(readyToStep())
  {
    stepUpdateNonBlocking();
  }
}

void stepperMotorCommandNonBlocking(short inputShort)
{
  
#ifdef DEBUG
    // say what you got:
     //Serial.print("Arduino received: ");
     //Serial.println(inputShort, DEC);
#endif
   
  targetStepPosition = map(inputShort, 0, 255, 0, STEP_90);

  // determine error (i.e. how many steps to take to reach target and in what direction)
  stepsToTake = targetStepPosition - currentStepPosition;
  
 // CW direction (+)
  if(stepsToTake>0)
  {
    digitalWrite(MOTOR_DIR_PIN,HIGH);
    stepDirection = 1;
  }
 // CCW direction (-)
  else
  {
     digitalWrite(MOTOR_DIR_PIN,LOW);
     stepDirection = -1;
  }

#ifdef DEBUG
    Serial.print("moving from degrees angle ");
    Serial.print(map(currentStepPosition, 0, MOTOR_STEPS_PER_REVOL, 0, 360), DEC);
    Serial.print(" to degrees angle ");
    Serial.println(map(targetStepPosition, 0, MOTOR_STEPS_PER_REVOL, 0, 360), DEC);
#endif
}
