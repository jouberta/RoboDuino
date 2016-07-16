/*
  RoboDuino.h - 
  Created by Nobody in particular, Today.
  Released into the public domain.
*/
#ifndef RoboDunio_h
#define RoboDunio_h

#include <Arduino.h>
#include "constants.h"
#include "UserConst.h"
#include <Servo.h>

struct Output
{
  int Type;
  String CommandIdentifier;
  int PinNumber;
  int Value;
};

struct Input
{
  int Type;
  String CommandIdentifier;
  int PinNumber;
  int OutputNumber;
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
    void init(Input inputList[NUM_INPUTS], Output outputList[NUM_OUTPUTS]);
  private:
    Output _outputList[NUM_OUTPUTS];
    Input _inputList[NUM_INPUTS];
    String _inputCommand;
    int _partListToTypeRelation[NUM_OUTPUTS];
    int _servoCounter;
    Servo _servos[NUM_SERVOS];
    //Motor _motors[];
    void _doCommand();
};
#endif
