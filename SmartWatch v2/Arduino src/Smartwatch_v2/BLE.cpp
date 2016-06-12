#include "BLE.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    //Alarm timing storage
    //2 Alarms/day
    //EEPROM addresses
    //0-3   sun
    //4-7   mon
    //8-11  tue
    //12-15 wed
    //16-19 thu
    //20-23 fri
    //24-27 sat

 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t alarm_day = 1;
uint8_t alarm_hour = 0;
uint8_t alarm_minute = 0;

byte TRANSACTION_POINTER = TR_MODE_IDLE;
byte TR_COMMAND = CMD_TYPE_NONE;

#define TIME_BUFFER_MAX 4
char timeParsingIndex = 0;
char timeBuffer[4] ;

#define ALARM_TIME_BUFFER_MAX 3
char alarmTimeParsingIndex = 0;
char alarmTimeBuffer[3] ;

///////////////////////////////////
//----- BT, Data parsing functions
///////////////////////////////////


// Parsing packet according to current mode
boolean receiveBluetoothData() 
{
    bool isTransactionEnded = false;
    
    while(!isTransactionEnded) 
    {
        if(Serial.available()) 
        {
            byte c = Serial.read();

            switch(TRANSACTION_POINTER)
            {
                case TR_MODE_IDLE:
                parseStartSignal(c);
                break;

                case TR_MODE_WAIT_CMD:
                parseCommand(c);
                break;

                case TR_MODE_WAIT_TIME:
                parseTime(c);
                break;

                case TR_MODE_WAIT_ALARM_TIME:
                parseAlarmTime(c);
                break;

                case TR_MODE_WAIT_COMPLETE:
                isTransactionEnded = parseEndSignal(c);
                break;
              
            }          
      }  
      
      else 
      {
          isTransactionEnded = true;
      }
      
    }  // End of while()
    return true;
}  // End of receiveBluetoothData()




void parseStartSignal(byte c) 
{
    if(c == TRANSACTION_START_BYTE) 
    {
        TRANSACTION_POINTER = TR_MODE_WAIT_CMD;
        TR_COMMAND = CMD_TYPE_NONE;
    }
}

void parseCommand(byte c) 
{
  
    if(c == CMD_TYPE_SET_TIME) 
    {
        TRANSACTION_POINTER = TR_MODE_WAIT_TIME;
        TR_COMMAND = c;
    }

    else if(c == CMD_TYPE_SET_ALARM_TIME) 
    {
        TRANSACTION_POINTER = TR_MODE_WAIT_ALARM_TIME;
        TR_COMMAND = c;
    }

    else if(c== CMD_TYPE_SET_ALARM_1 || c == CMD_TYPE_SET_ALARM_2 || c== CMD_TYPE_ALARM_ON || c== CMD_TYPE_ALARM_OFF) 
    {
        TRANSACTION_POINTER = TR_MODE_WAIT_COMPLETE;
        TR_COMMAND = c;
        processTransaction();
    }

    else 
    {
        TRANSACTION_POINTER = TR_MODE_IDLE;
        TR_COMMAND = CMD_TYPE_NONE;
    }
}



void parseTime(byte c) 
{

  if(TR_COMMAND == CMD_TYPE_SET_TIME) 
  {       
      if(timeParsingIndex >= 0 && timeParsingIndex < TIME_BUFFER_MAX) 
    {
        timeBuffer[timeParsingIndex] = (int)c;
        timeParsingIndex++;
    }
  
    else
    {
        processTransaction();
        TRANSACTION_POINTER = TR_MODE_WAIT_COMPLETE;
    }
  }
}

void parseAlarmTime(byte c) 
{

  if(TR_COMMAND == CMD_TYPE_SET_ALARM_TIME) 
  {       
      if(alarmTimeParsingIndex >= 0 && alarmTimeParsingIndex < ALARM_TIME_BUFFER_MAX) 
    {
        alarmTimeBuffer[alarmTimeParsingIndex] = (int)c;
        alarmTimeParsingIndex++;
    }
  
    else
    {
        processTransaction();
        TRANSACTION_POINTER = TR_MODE_WAIT_COMPLETE;
    }
  }
}



boolean parseEndSignal(byte c) 
{
    if(c == TRANSACTION_END_BYTE) 
    {
        TRANSACTION_POINTER = TR_MODE_IDLE;
        return true;
    }
    return false;
}


void processTransaction() {

    switch(TR_COMMAND)
    {
        case CMD_TYPE_SET_TIME:
        SetTime(timeBuffer);
        timeParsingIndex = 0;
        break;

        case CMD_TYPE_SET_ALARM_TIME:
        alarm_day=alarmTimeBuffer[0];
        alarm_hour=alarmTimeBuffer[1];
        alarm_minute=alarmTimeBuffer[2];
        alarmTimeParsingIndex = 0;
        break;

        case CMD_TYPE_SET_ALARM_1:
        ds1337_set_alarm_1_at_day(alarm_day,alarm_hour,alarm_minute);
        EEPROM.write((alarm_day-1)*4, alarm_hour);
        EEPROM.write((alarm_day-1)*4+1, alarm_minute);
        break;

        case  CMD_TYPE_SET_ALARM_2:
        ds1337_set_alarm_2_at_day(alarm_day,alarm_hour,alarm_minute);
        EEPROM.write((alarm_day-1)*4+2, alarm_hour);
        EEPROM.write((alarm_day-1)*4+3, alarm_minute);
        break;

        case CMD_TYPE_ALARM_ON:
        ds1337_clear_status();
        ds1337_set_control(DS1337_ALARM_ON);
        break;

        case CMD_TYPE_ALARM_OFF:
        ds1337_set_control(0);
        break;

        default:
        break;
   
    }
    
}
 
