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
    String getName(int pos1, int pos2);
    void init(String partList[NUM_PARTS][PART_PROPERTY_COUNT], String pinList[NUM_PARTS]);
  private:
    String _partList[NUM_PARTS][PART_PROPERTY_COUNT];
    String _pinList[NUM_PARTS];
    String _inputCommand;
    int _partListToTypeRelation[NUM_PARTS];
    int _servoCounter;
    Servo _servos[NUM_SERVOS];
    //Motor _motors[];
    void _doCommand();
};
#endif