#ifndef BLE_H
#define BLE_H

#include <arduino.h>
#include <EEPROM.h>
#include "DS1337.h"

///////////////////////////////////////////////////////////////////
//----- Protocol

//----- Bluetooth transaction parsing
#define TR_MODE_IDLE 1
#define TR_MODE_WAIT_CMD 11
#define TR_MODE_WAIT_MESSAGE 101
#define TR_MODE_WAIT_TIME 111
#define TR_MODE_WAIT_ALARM_TIME 112
#define TR_MODE_WAIT_COMPLETE 201


#define TRANSACTION_START_BYTE 0xfc
#define TRANSACTION_END_BYTE 0xfd


#define CMD_TYPE_NONE 0x00
#define CMD_TYPE_SET_TIME 0x31
#define CMD_TYPE_SET_ALARM_TIME 0x32
#define CMD_TYPE_SET_ALARM_1 0x33
#define CMD_TYPE_SET_ALARM_2 0x34
#define CMD_TYPE_ALARM_ON 0x51
#define CMD_TYPE_ALARM_OFF 0x52
#define CMD_TYPE_SLEEP 0x53


boolean receiveBluetoothData();
void parseStartSignal(byte c);
void parseCommand(byte c);
void parseMessage(byte c);
void parseTime(byte c);
void parseAlarmTime(byte c);
boolean parseEndSignal(byte c);
void processTransaction();


#endif
