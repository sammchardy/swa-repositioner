#include <EEPROM.h>
#include <EEPROMAnything.h>
#include <AccelStepper.h>
#include <EightAccelStepper.h>
#include <CreatureMover.h>

#define DIR_PIN 9
#define STEP_PIN 8

EightAccelStepper stepperA(4, 28, 30, 32, 34);
EightAccelStepper stepperB(4, 23, 25, 27, 29);
// Define a stepper and the pins it will use
AccelStepper stepperQr(1, STEP_PIN, DIR_PIN);

int aSteps[3][3] = {
    {-957,   -672,   -283} // LEFT
  , {-230,     0,     312}     // MID
//  , {726,     920,     1051}    // RIGHT
  , {576,     845,     1051}    // RIGHT
};

int bSteps[3][3] = {
    {-927,     -1136,   -1336}  // LEFT
  , {186,     0,      -380}    // MID
  , {827,    381,    35}  // RIGHT
};
/*
int aSteps[3][3] = {
    {-320,   -230,   -95} // LEFT
  , {-70,     0,     106}     // MID
  , {240,     308,     350}    // RIGHT
};

int bSteps[3][3] = {
    {-310,     -380,   -450}  // LEFT
  , {62,     0,      -128}    // MID
  , {267,    128,    12}  // RIGHT
};
*/

int aMaxSpeed = 100; // 225
int bMaxSpeed = 100; // 275
int idleDistance = 0;
int acceleration = 25;
int repositionSpeed = 250;
CreatureMover creature(stepperA, aMaxSpeed, aSteps,
                       stepperB, bMaxSpeed, bSteps,
                       idleDistance, acceleration);

void setup() {
  Serial.begin(9600);
  Serial.println("setup");

  stepperA.setMaxSpeed(aMaxSpeed);    
  stepperB.setMaxSpeed(bMaxSpeed);
  stepperA.setAcceleration(50.0);
  stepperB.setAcceleration(50.0);

  stepperQr.setMaxSpeed(20000);
  stepperQr.setAcceleration(5000);
}

int integerValue = 0;
int motor = 0;
char incomingByte;
boolean negative = false;
 
void loop() {
  if (creature.getState() == CreatureMover::MOVING) {
    creature.run();
  } else if (stepperA.distanceToGo() != 0) {
      stepperA.run();
  } else if (stepperB.distanceToGo() != 0) {
      stepperB.run();
  } else if (stepperQr.distanceToGo() != 0) {
      stepperQr.run();
  } else if (Serial.available() > 0) {   // something came across serial
    integerValue = 0; 
    motor = 0;   
    negative = false;
    while(1) {            // force into a loop until '\n' is received
      incomingByte = Serial.read();
      if (incomingByte == '\n') break;   // exit the while(1), we're done receiving
      if (incomingByte == -1) continue;  // if no characters are in the buffer read() returns -1
      if (incomingByte == '-') {
        negative = true;
        continue;
      }
      if (incomingByte == ':') {
        motor = integerValue;
        integerValue = 0;
        continue;
      }
      integerValue *= 10;  // shift left 1 decimal place
      // convert ASCII to integer, add, and shift left 1 decimal place
      integerValue = ((incomingByte - 48) + integerValue);
    }
    if (negative) {
      integerValue *= -1;
    }      
    Serial.print("msg:");
    Serial.print(motor);
    Serial.print(":");
    Serial.println(integerValue);
    if (motor == 1) { 
      stepperA.setMaxSpeed(repositionSpeed);  
      stepperA.move(integerValue);   
      stepperA.setMaxSpeed(repositionSpeed);  
    } else if (motor == 2) {
      stepperB.setMaxSpeed(repositionSpeed); 
      stepperB.move(integerValue);   
      stepperB.setMaxSpeed(repositionSpeed);   
    } else if (motor == 3) {
      stepperQr.move(integerValue);   
      stepperQr.setMaxSpeed(20000);  
    } else if (motor == 4) {
      resetPosition();
    } else if (motor == 5) {
      resetQrPosition();
    } else if (motor == 6) {
      
      switch(integerValue) {
        case 0:
          creature.moveToPosition(CreatureMover::XLEFT, CreatureMover::YTOP);
          break;
        case 1:
          creature.moveToPosition(CreatureMover::XLEFT, CreatureMover::YMID);
          break;
        case 2:
          creature.moveToPosition(CreatureMover::XLEFT, CreatureMover::YBOTTOM);
          break;
        case 3:
          creature.moveToPosition(CreatureMover::XMID, CreatureMover::YTOP);
          break;
        case 4:
          creature.moveToPosition(CreatureMover::XMID, CreatureMover::YMID);
          break;
        case 5:
          creature.moveToPosition(CreatureMover::XMID, CreatureMover::YBOTTOM);
          break;
        case 6:
          creature.moveToPosition(CreatureMover::XRIGHT, CreatureMover::YTOP);
          break;
        case 7:
          creature.moveToPosition(CreatureMover::XRIGHT, CreatureMover::YMID);
          break;
        case 8:
          creature.moveToPosition(CreatureMover::XRIGHT, CreatureMover::YBOTTOM);
          break;
      }
    }
  }
}

void resetPosition() {  
    EEPROM_writeAnything(0, 0);
    EEPROM_writeAnything(2, 0);
    
    creature._loadPositions();
    Serial.println("Reset positions");
}

void resetQrPosition() {  
    EEPROM_writeAnything(4, 0);
}


