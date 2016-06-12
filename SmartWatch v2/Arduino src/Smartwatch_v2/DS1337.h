#ifndef DS1337_H
#define DS1337_H

#include <arduino.h>
#include "Wire.h"

// leap year calulator expects year argument as years offset from 1970
#define LEAP_YEAR(Y)     ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )

#define decode_bcd(x)           ((x >> 4) * 10 + (x & 0x0F))

#define encode_bcd(x)           ((((x / 10) & 0x0F) << 4) + (x % 10))

static  const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};

// Device address
#define DS1337_ADDR  					          0x68

//Refer datasheet
// Registers
#define DS1337_REG_SECONDS              0x00
#define DS1337_REG_MINUTES              0x01
#define DS1337_REG_HOURS                0x02
#define DS1337_REG_DAY                  0x03
#define DS1337_REG_DATE                 0x04
#define DS1337_REG_MONTH                0x05
#define DS1337_REG_YEAR                 0x06
#define DS1337_REG_A1_SECONDS           0x07
#define DS1337_REG_A1_MINUTES           0x08
#define DS1337_REG_A1_HOUR              0x09
#define DS1337_REG_A1_DAY_DATE          0x0A
#define DS1337_REG_A2_MINUTES           0x0B
#define DS1337_REG_A2_HOUR              0x0C
#define DS1337_REG_A2_DAY_DATE          0x0D
#define DS1337_REG_CONTROL              0x0E
#define DS1337_REG_STATUS               0x0F

/**
  If set, in an hour register (DS1337_REG_HOURS, DS1337_REG_A1_HOUR,
  DS1337_REG_A2_HOUR, the hour is between 0 and 12, and the
  (!AM)/PM bit indicates AM or PM.
  If not set, the hour is between 0 and 23.
*/
#define DS1337_HOUR_12   			(0x01 << 6)

/**
  If DS1337_HOUR_12 is set:
  - If set, indicates PM
  - If not set, indicates AM
*/
#define DS1337_PM_MASK        (0x01 << 5)

/**
  For DS1337_A1_DAY_DATE and DS1337_A2_DAY_DATE,
  - If set, DAY_DATE register specifies a date (1-31).
  - If not set, DAY_DATE register specifies a day (1-7).
*/
#define DS1337_DAY_MASK      	(0x01 << 6)

// If set, the oscillator has stopped since the last time
// this bit was cleared.
#define DS1337_OSC_STOP_FLAG  			(0x01 << 7)

// If set, the A1 alarm has occurred.
#define DS1337_A1_FLAG        			(0x01)

// If set, the A2 alarm has occurred.
#define DS1337_A2_FLAG        			(0x01 << 1)

// Set to disable the oscillator
#define DS1337_OSC_DISABLE     			(0x01 << 7)

// Enables Alarm interrupts on INTA(1 Hz o/p on INTB)
#define DS1337_ALARM_ON          		0b00000011

/** Represents a time and date. */
struct time_RTC 
{
    uint8_t Second;
    uint8_t Minute;
    uint8_t Hour;
    uint8_t Wday;
    uint8_t Day;
    uint8_t Month;
    uint8_t Year;
};

void i2c_read(uint8_t addr,uint8_t REGaddr,uint8_t* buf, uint8_t num);
void i2c_write(uint8_t addr, uint8_t* buf, uint8_t num) ;
void i2c_write_1(uint8_t addr, uint8_t b);

void SetTime(char timeBuffer[4]);
void make_time(time_RTC* tm, uint32_t time_Input);
void ds1337_read_time(time_RTC* tm);
void ds1337_write_time(time_RTC* tm);

void ds1337_set_alarm_1_at_day(uint8_t Day, uint8_t Hour, uint8_t Minute);
void ds1337_set_alarm_2_at_day(uint8_t Day, uint8_t Hour, uint8_t Minute);

void ds1337_set_control(uint8_t ctrl);
void ds1337_clear_control_bits(uint8_t mask);

void ds1337_get_status(uint8_t* stat);
void ds1337_set_status(uint8_t stat);
void ds1337_clear_status();

#endif
