/*
  RoboDuino.cpp - 
  Created by Nobody in particular, Today.
  Released into the public domain.
*/

#include <Arduino.h>
#include "RoboDuino.h"


//Constructor
RoboDuino::RoboDuino()
{ 
}

//setup function
void RoboDuino::init(PartList partList[NUM_PARTS]) 
{
  // Set all the counters to zero
  _servoCounter = 0;

  
  for (int i = 0; i < NUM_PARTS; i++) 
  {
      //set the parts list
      _partList[i] = partList[i];
  }
  
  // FACTORY STUFFS
  for (int i = 0; i < NUM_PARTS; i++)
  {
    //Make the interface
    if (_partList[i].Type == TYPE_SERVO) 
    {
      _servos[_servoCounter].attach(_partList[i].PinNumber);
      _partListToTypeRelation[i] = _servoCounter;
      _servoCounter++;
    } 
    else if(_partList[i].Type== TYPE_AOUT || _partList[i].Type== TYPE_DOUT)
    {
      pinMode(_partList[i].PinNumber, OUTPUT);
      _partListToTypeRelation[i] = i;
    }
    else if(_partList[i].Type== TYPE_MOTOR) 
    {
      //interfaceInstances[i] = new Servo();
      //_motors[i].attach(_pinList[i][PIN_SERVO]); 
    }
  }
}

//SerialEventHandler
void RoboDuino::doSerialEvent(char inputChar)
{  
    // add it to the inputString:
    _inputCommand += inputChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    // TODO: make new line configurable
    if (inputChar == '\n') {
      _doCommand();
      _inputCommand = "";
    }
}

//SerialEventHandler
void RoboDuino::_doCommand()
{
  int seperatorPosition = _inputCommand.indexOf(":");
  int valueLength = _inputCommand.length() - (2 + seperatorPosition);
  
  String cmdID = _inputCommand.substring(0, seperatorPosition); // read the first letter of the command, this indicates the command type - M/motor S/servo X/switch Q/query
  String commandValue = _inputCommand.substring(seperatorPosition + 1, _inputCommand.length());

  for(int i = 0; i < NUM_PARTS; i++)
  {
    if (_partList[i].CommandIdentifier == cmdID)
    {
      if (_partList[i].Type == TYPE_SERVO)
      {
          int servoNumber = _partListToTypeRelation[i];
          _servos[servoNumber].write(commandValue.toInt());
      }
      else if(_partList[i].Type == TYPE_DOUT)
      {
          int partNumber = _partListToTypeRelation[i];
          digitalWrite(_partList[partNumber].PinNumber, commandValue.toInt());
      }
      else if(_partList[i].Type == TYPE_AOUT)
      {
          int partNumber = _partListToTypeRelation[i];
          analogWrite(_partList[partNumber].PinNumber, commandValue.toInt());
      }
    }
  }
  
}

//Loop Event Handler
void RoboDuino::doLoopEvent()
{
  
}
