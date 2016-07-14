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

//SerialEventHandler
void RoboDuino::doSerialEvent(char inputChar)
{  
    // add it to the inputString:
    _inputCommand += inputChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inputChar == '\n') {
      _doCommand();
      _inputCommand = "";
    }
}

//SerialEventHandler
void RoboDuino::_doCommand()
{
  
  char cmdID = _inputCommand[0]; // read the first letter of the command, this indicates the command type - M/motor S/servo X/switch Q/query
  int commandValue = _inputCommand.substring(1, (_inputCommand.length() - 1)).toInt();

  for(int i = 0; i < NUM_PARTS; i++)
  {
    if (_partList[i][COMMAND_IDENTIFIER] == (String)cmdID)
    {
      if (_partList[i][PART_TYPE] == "servo")
      {
          _servos[i].write(commandValue);
      }
      else if(_partList[i][PART_TYPE] == "digital_output")
      {
          digitalWrite(_pinList[i].toInt(), commandValue);
      }
      else if(_partList[i][PART_TYPE] == "analogue_output")
      {
          analogWrite(_pinList[i].toInt(), commandValue);
      }
    }
  }
  
}

//Loop Event Handler
void RoboDuino::doLoopEvent()
{
  
}

//setup function
void RoboDuino::init(String partList[][PART_PROPERTY_COUNT], String pinList[]) 
{
  
  for (int i = 0; i < NUM_PARTS; i++) 
  {
    //set the parts list
    _partList[i][0] = partList[i][0];
    _partList[i][1] = partList[i][1];
    _partList[i][2] = partList[i][2];
    _partList[i][3] = partList[i][3];
    
    _pinList[i] = pinList[i];
  }
  
  // FACTORY STUFFS
  for (int i = 0; i < NUM_PARTS; i++)
  {
    /*
     * TODO: create counters for each type of part to relate its position in
     *       the list of parts and create an internal array relating the 
     *       position of a part in its list to its position in the part list
     *       e.g. _servoCounter = 0
     *       _partListToTypeRelation[]
     */
    //Make the interface
    if (_partList[i][PART_TYPE] == "servo") 
    {
      /*
      * e.g. _partListToTypeRelation[i] = _servoCounter;
      *      _servoCounter++;
      */
      _servos[i].attach(_pinList[i].toInt());
    } 
    else if(_partList[i][PART_TYPE]== "analogue_out" || _partList[i][PART_TYPE]== "digital_out") 
    {
      pinMode(_pinList[i].toInt(), OUTPUT);
    }
    else if(_partList[i][PART_TYPE]== "motor") 
    {
      //interfaceInstances[i] = new Servo();
      //_motors[i].attach(_pinList[i][PIN_SERVO]); 
    }
  }
}

String RoboDuino::getName(int pos1, int pos2)
{  
  return _partList[pos1][pos2];
}
