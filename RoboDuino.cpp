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
  // Set all the counters to zero
  _servoCounter = 0;
}

//setup function
void RoboDuino::init(Input inputList[NUM_INPUTS], Output outputList[NUM_OUTPUTS]) 
{    
  // FACTORY STUFFS FOR OUTPUTS
  for (int i = 0; i < NUM_OUTPUTS; i++)
  {
    //set the parts list
    _outputList[i] = outputList[i];
    //Make the interface
    if (_outputList[i].Type == TYPE_SERVO) 
    {
      _servos[_servoCounter].attach(_outputList[i].PinNumber[PIN_SERVO]);
      _partListToTypeRelation[i] = _servoCounter;
      _servoCounter++;
    } 
    else if(_outputList[i].Type== TYPE_AOUT || _outputList[i].Type== TYPE_DOUT)
    {
      int pinPosition = PIN_AOUT;
      if (_outputList[i].Type== TYPE_DOUT) {
        pinPosition = PIN_DOUT;
      }
      pinMode(_outputList[i].PinNumber[pinPosition], OUTPUT);
      _partListToTypeRelation[i] = i;
    }
    else if(_outputList[i].Type== TYPE_MOTOR) 
    {
      pinMode(_outputList[i].PinNumber[PIN_MOTOR_S1], OUTPUT);
      pinMode(_outputList[i].PinNumber[PIN_MOTOR_S1], OUTPUT);
      pinMode(_outputList[i].PinNumber[PIN_MOTOR_PWM], OUTPUT);
      _partListToTypeRelation[i] = i;
    }     
  }
  
  // FACTORY STUFFS FOR INPUTS
  for (int i = 0; i < NUM_INPUTS; i++) 
  {
    //set the parts list
    _inputList[i] = inputList[i];
    if (_inputList[i].Type == TYPE_BUTTON || 
        _inputList[i].Type == TYPE_BUTTON_INC || 
        _inputList[i].Type == TYPE_BUTTON_DEC 
    ) 
    {
      pinMode(_inputList[i].PinNumber, INPUT_PULLUP);
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
  String commandValue = _inputCommand.substring(seperatorPosition + 1, seperatorPosition + valueLength + 1);
  
  for(int i = 0; i < NUM_OUTPUTS; i++)
  {
    if (_outputList[i].CommandIdentifier == cmdID)
    {
      if (_outputList[i].Type == TYPE_SERVO)
      {
          int servoNumber = _partListToTypeRelation[i];
          _outputList[i].Value = commandValue.toInt();
      }
      else if(_outputList[i].Type == TYPE_DOUT)
      {
          int partNumber = _partListToTypeRelation[i];
          digitalWrite(_outputList[partNumber].PinNumber[PIN_DOUT], commandValue.toInt());
      }
      else if(_outputList[i].Type == TYPE_AOUT)
      {
          int partNumber = _partListToTypeRelation[i];
          analogWrite(_outputList[partNumber].PinNumber[PIN_AOUT], commandValue.toInt());
      }
      else if(_outputList[i].Type == TYPE_MOTOR)
      {
          int partNumber = _partListToTypeRelation[i];
          _outputList[partNumber].Value = commandValue.toInt();
          _outputList[partNumber].ValueChange = true;
      }
    }
  }
  
}

//Loop Event Handler
void RoboDuino::doLoopEvent()
{
  for(int i = 0; i < NUM_INPUTS; i++)
  {
    if ( _inputList[i].Type == TYPE_BUTTON_INC || _inputList[i].Type == TYPE_BUTTON_DEC)
    {
      if (digitalRead(_inputList[i].PinNumber) == LOW) {
        if (_inputList[i].Type == TYPE_BUTTON_INC) 
        {
          _outputList[_inputList[i].OutputNumber].Value = _outputList[_inputList[i].OutputNumber].Value + 1;
          _outputList[_inputList[i].OutputNumber].ValueChange = true;
        }
        if (_inputList[i].Type == TYPE_BUTTON_DEC) 
        {
          _outputList[_inputList[i].OutputNumber].Value = _outputList[_inputList[i].OutputNumber].Value - 1;    
          _outputList[_inputList[i].OutputNumber].ValueChange = true;        
        }
      }
    }
  }
  for(int i = 0; i < NUM_OUTPUTS; i++)
  {
    if (_outputList[i].Type == TYPE_SERVO)
    {
        int servoNumber = _partListToTypeRelation[i];
        int currentValue = _servos[servoNumber].read();
        if (currentValue < _outputList[i].Value) 
        {
          _servos[servoNumber].write(currentValue + 1);
          delay(SERVO_DELAY);
        } 
        else if (currentValue > _outputList[i].Value) 
        {
          _servos[servoNumber].write(currentValue - 1);    
          delay(SERVO_DELAY);
        }
    }
    
    if (_outputList[i].Type == TYPE_MOTOR)
    {
      int currentMotorValue = analogRead(_outputList[i].PinNumber[PIN_MOTOR_PWM]);
      if (_outputList[i].ValueChange == true)
      {
        _outputList[i].ValueChange = false;
        if (_outputList[i].Value < 0) 
        {
          //SET PINS TO REVERSE AND SET PWM VALUE
          analogWrite(_outputList[i].PinNumber[PIN_MOTOR_PWM], (_outputList[i].Value * -1));
          delay(SERVO_DELAY);
          digitalWrite(_outputList[i].PinNumber[PIN_MOTOR_S1], HIGH);
          digitalWrite(_outputList[i].PinNumber[PIN_MOTOR_S2], LOW);
            
        } 
        else if (_outputList[i].Value > 0) 
        {
          analogWrite(_outputList[i].PinNumber[PIN_MOTOR_PWM], _outputList[i].Value);
          delay(100);
          //SET PINS TO foward AND SET PWM VALUE
          digitalWrite(_outputList[i].PinNumber[PIN_MOTOR_S1], LOW);
          digitalWrite(_outputList[i].PinNumber[PIN_MOTOR_S2], HIGH);
        }  
        else if (_outputList[i].Value == 0) 
        {
          analogWrite(_outputList[i].PinNumber[PIN_MOTOR_PWM], LOW);          
        } 
      }
    }
  }
}
