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

class RoboDuino
{
  public:
    //Constructor
    RoboDuino();
    //SerialEventHandler
    void doSerialEvent(char inputChar);
    //Loop Event Handler
    void doLoopEvent();
    //Internal setup function
    void init(String partList[][3], char pinList[][3]);
  private:
    String _partList[][3];
    char _pinList[][3];
    Servo _servos[];
    //Motor _motors[];
    String _inputCommand;
    void _doCommand();
    void _setup();
};
#endif
