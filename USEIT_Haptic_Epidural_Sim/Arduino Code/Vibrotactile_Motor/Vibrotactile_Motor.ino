#include "VibrotactileMotor.cpp"


const int ABSOLUTE_ENCODER = A0;
int encoderVal = 0;

const int VIBROTACTILE_MOTOR = 9;
VibrotactileMotor motor;

void setup()
{
  Serial.begin(115200);
  Serial.print("hi");
  pinMode(VIBROTACTILE_MOTOR, OUTPUT);
  motor = VibrotactileMotor(9);
}

void loop()
{ 
  motor.setFrequency(5);
  motor.setIntensity(255);
  motor.motorUpdate();
 
  Serial.print("encoder: ");
  Serial.print(encoderVal);
}
