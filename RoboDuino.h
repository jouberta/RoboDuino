/*
  RoboDuino.h - 
  Created by Nobody in particular, Today.
  Released into the public domain.
*/
#ifndef RoboDunio_h
#define RoboDunio_h

#include <Arduino.h>
#include "constants.h"
#include <Servo.h>

struct PartList
{
  int Type;
  String CommandIdentifier;
  int PinNumber;
};

class RoboDuino
{
  public:
    //Constructor
    RoboDuino();
    //SerialEventHandler
    void doSerialEvent(char inputChar);
    //Loop Event Handler
    void doLoopEvent();
    void init(PartList partList[NUM_PARTS]);
  private:
    PartList _partList[NUM_PARTS];
    String _inputCommand;
    int _partListToTypeRelation[NUM_PARTS];
    int _servoCounter;
    Servo _servos[NUM_SERVOS];
    //Motor _motors[];
    void _doCommand();
};
#endif
