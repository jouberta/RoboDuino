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
  int PinNumber[NUM_OUTPUT_PINS];
  int Value;
  bool ValueChange;
};

struct Input
{
  int Type;
  String CommandIdentifier;
  int PinNumber;
  int OutputNumber;
  bool Pullup;
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
    // Initialising function
    void init(Input inputList[NUM_INPUTS], Output outputList[NUM_OUTPUTS]);
  private:
    // Array of outputs
    Output _outputList[NUM_OUTPUTS];
    // Array of servos
    Servo _servos[NUM_SERVOS];
    // Array of last run timings
    unsigned int _lastRun[NUM_OUTPUTS];
    // Array of inputs
    Input _inputList[NUM_INPUTS];
    // Array of last read timings
    unsigned int _lastRead[NUM_INPUTS];
    // Array of input states
    bool _inputStates[NUM_INPUTS];
    // Command string buffer
    String _inputCommand;
    // Part list relation
    int _partListToTypeRelation[NUM_OUTPUTS];
    int _servoCounter;
    // Command processing method
    void _doCommand();
    void _doInput(int i, unsigned int now);
};
#endif
