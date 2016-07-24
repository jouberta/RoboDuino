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
          _outputList[i].Value = commandValue.toInt();
          _outputList[i].ValueChange = true;
      }
    }
  }
  
}

//Loop Event Handler
void RoboDuino::doLoopEvent()
{
  unsigned int now = millis();
  for(int i = 0; i < NUM_INPUTS; i++)
  {
    if ( _inputList[i].Type == TYPE_BUTTON_INC || _inputList[i].Type == TYPE_BUTTON_DEC)
    {
      if (digitalRead(_inputList[i].PinNumber) == LOW) {
        if (now >= _lastRead[i] + INPUT_DELAY || now <= _lastRead[i] - INPUT_DELAY - 5)
        {          
          if (_inputList[i].Type == TYPE_BUTTON_INC) 
          {
            if (_outputList[_inputList[i].OutputNumber].Value <= 180) {
              _outputList[_inputList[i].OutputNumber].Value = _outputList[_inputList[i].OutputNumber].Value + 1;
              _outputList[_inputList[i].OutputNumber].ValueChange = true;
            }
          }
          if (_inputList[i].Type == TYPE_BUTTON_DEC) 
          {
            if (_outputList[_inputList[i].OutputNumber].Value >= 0) {
              _outputList[_inputList[i].OutputNumber].Value = _outputList[_inputList[i].OutputNumber].Value - 1;    
              _outputList[_inputList[i].OutputNumber].ValueChange = true;        
            }
          }
          _lastRead[i] = now;
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
        if (now >= _lastRun[i] + SERVO_DELAY || now <= _lastRead[i] - SERVO_DELAY - 5) 
        {
          _lastRun[i] = now;
          if (currentValue < _outputList[i].Value) 
          {
            _servos[servoNumber].write(currentValue + 1);
          } 
          else if (currentValue > _outputList[i].Value) 
          {
            _servos[servoNumber].write(currentValue - 1);
          }          
        }
    }
    
    if (_outputList[i].Type == TYPE_MOTOR)
    {
      if (_outputList[i].ValueChange == true)
      {
        _outputList[i].ValueChange = false;
        if (now >= _lastRun[i] + MOTOR_DELAY || now <= _lastRead[i] - MOTOR_DELAY - 5) 
        {          
          _lastRun[i] = now;
          
          if (_outputList[i].Value < 0) 
          {
            //SET PINS TO REVERSE AND SET PWM VALUE
            analogWrite(_outputList[i].PinNumber[PIN_MOTOR_PWM], (_outputList[i].Value * -1));
            digitalWrite(_outputList[i].PinNumber[PIN_MOTOR_S1], HIGH);
            digitalWrite(_outputList[i].PinNumber[PIN_MOTOR_S2], LOW);
              
          } 
          else if (_outputList[i].Value > 0) 
          {
            analogWrite(_outputList[i].PinNumber[PIN_MOTOR_PWM], _outputList[i].Value);
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
}
