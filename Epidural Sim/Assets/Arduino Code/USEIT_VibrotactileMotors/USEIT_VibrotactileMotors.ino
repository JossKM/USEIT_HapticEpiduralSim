#define DEBUG // if defined, will print debug messages to the serial line.
#include "VibrotactileMotor.cpp"

VibrotactileMotor leftMotor;
VibrotactileMotor bottomMotor;
VibrotactileMotor rightMotor;

// SIGNAL MAPPING /////////////
const byte COMMAND_NULL = 0;
const byte COMMAND_LEFT = 1;
const byte COMMAND_BOTTOM = 2;
const byte COMMAND_RIGHT = 3;
const byte COMMAND_TOP = 4;

// Arduino receives bytes. so 256 different commands are available (0-255). Here is what each is defined to mean in this program.
// 0 - NULL (nothing)

//IO
const uint16_t motorCommandPacketSize = 2; // command size in bytes
uint8_t commandBuffer[motorCommandPacketSize];
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
  //set pins
  leftMotor.setPin(9);
  bottomMotor.setPin(10);
  rightMotor.setPin(11);
  
  Serial.begin(115200);

  #ifdef DEBUG
       Serial.print("Vibration Motor setup complete. Awaiting input...");
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
  }

  // update vibrotactile motors
  leftMotor.motorUpdate();
  bottomMotor.motorUpdate();
  rightMotor.motorUpdate();
 
  lastTimeMicros = currentRuntimeMicros;
}

inline void readSerialLine()
{
    while (Serial.available() > motorCommandPacketSize)
    {
      /*
      #ifdef DEBUG
           Serial.print("Packet received:" + *commandBuffer);
           for(int i = 0; i < motorCommandPacketSize; i++)
            {
              Serial.print(commandBuffer[i]);
            }
             Serial.println("");
      #endif
      */
      
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
      if(commandBuffer[0] == COMMAND_LEFT)
      {
        leftMotor.setIntensity(commandBuffer[1]);
      } else

      if(commandBuffer[0] == COMMAND_BOTTOM)
      {
        bottomMotor.setIntensity(commandBuffer[1]);
      } else

      if(commandBuffer[0] == COMMAND_RIGHT)
      {
        rightMotor.setIntensity(commandBuffer[1]);
      }
    }
    numCommandsInBuffer = 0;
}
