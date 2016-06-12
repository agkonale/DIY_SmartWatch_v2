#include "DS1337.h"

/**
   \brief Read data from an I2C device.
   \param addr The address of the device from which to read.
   \param REGaddr The address of the register from which to read.
   \param buf A pointer to a buffer in which to store the data.
   \param num The number of bytes to read.
*/
void i2c_read(uint8_t addr,uint8_t REGaddr,uint8_t* buf, uint8_t num) 
{
    i2c_write_1(addr,REGaddr);
    
    Wire.requestFrom(addr, num);
    for (uint8_t i = 0; i < num; i++) 
    {
        buf[i] = Wire.read();
    }
}

/**
   \brief Write data to an I2C device.
   \param addr The address of the device to which to write.
   \param buf A pointer to a buffer from which to read the data.
   \param num The number of bytes to write.
*/
void i2c_write(uint8_t addr, uint8_t* buf, uint8_t num) 
{
    Wire.beginTransmission(addr);
    for (uint8_t i = 0; i < num; i++) 
    {
        Wire.write(buf[i]);
    }
    Wire.endTransmission();
}

/**
   \brief Write a single byte to an I2C device.
   \param addr The address of the device to which to write.
   \param b The byte to write.
*/
void i2c_write_1(uint8_t addr, uint8_t b) 
{
    Wire.beginTransmission(addr);
    Wire.write(b);
    Wire.endTransmission();
}

/**
   \defgroup ds1337_time_RTC Time Structure and Associated Methods
*/

/**
   \brief Set the values in a struct time_RTC instance.
   \param tm A pointer to a struct time_RTC instance.
   \param timeInput desired epoch time
*/

void make_time(time_RTC* tm, uint32_t timeInput) 
{

    uint8_t yr;
    uint8_t mnth, monthLength;
    uint32_t Time;
    unsigned long days;
    
    //correction for Indian Standard Time(+5:30)
    Time = timeInput+19800;   
    
    tm->Second = Time % 60;
    Time /= 60; // now it is minutes
    tm->Minute = Time % 60;
    Time /= 60; // now it is hours
    tm->Hour = Time % 24;
    Time /= 24; // now it is days
    tm->Wday = ((Time + 4) % 7) + 1;  // Sunday is day 1 
    
    yr = 0;  
    days = 0;
    while((unsigned)(days += (LEAP_YEAR(yr) ? 366 : 365)) <= Time) 
    {
        yr++;
    }
    tm->Year = yr-30; // yr is offset from 1970 //Year is offset from 2000
    
    days -= LEAP_YEAR(yr) ? 366 : 365;
    Time -= days; // now it is days in this year, starting at 0
    
    days=0;
    mnth=0;
    monthLength=0;
    
    for (mnth=0; mnth<12; mnth++) 
    {
        if (mnth==1) 
        {   // february
            if (LEAP_YEAR(yr)) 
            {
                monthLength=29;
            } 
            else 
            {
                monthLength=28;
            }
        } 
        else 
        {
            monthLength = monthDays[mnth];
        }
      
      if (Time >= monthLength) 
      {
          Time -= monthLength;
      } 
      else 
      {
          break;
      }
      
    }
    
    tm->Month = mnth + 1;  // jan is month 1  
    tm->Day = Time + 1;     // day of month
    
}

/**
   \defgroup ds1337_time Time Getter and Setter Methods
*/

/**
   \brief Read the current time.
   \param time A pointer to a struct time_t instance in which to store the time.

*/
void ds1337_read_time(time_RTC* tm) 
{ 
  
    uint8_t buf[7];
    i2c_read(DS1337_ADDR,DS1337_REG_SECONDS,buf,7);
    tm->Second = decode_bcd(buf[0]);
    tm->Minute = decode_bcd(buf[1]);
  
    if (buf[2] & DS1337_HOUR_12) 
    {
        tm->Hour = ((buf[2] >> 4) & 0x01) * 12 + ((buf[2] >> 5) & 0x01) * 12;
    } 
    else 
    {
        tm->Hour = decode_bcd(buf[2]);
    }
  
    tm->Wday = decode_bcd(buf[3]);
    tm->Day = decode_bcd(buf[4]);
    tm->Month = decode_bcd(buf[5] & 0x1F);
    tm->Year = 100 * ((buf[5] >> 7) & 0x01) + decode_bcd(buf[6]);

}

/**
   \brief Set the time.
   \param time A pointer to a struct time_t instance containing the time to set.

*/
void ds1337_write_time(time_RTC* tm) 
{
    uint8_t buf[8];
  
    buf[0] = DS1337_REG_SECONDS;
    buf[1] = encode_bcd(tm->Second);
    buf[2] = encode_bcd(tm->Minute);
    buf[3] = encode_bcd(tm->Hour); // Time always stored in 24-hour format
    buf[4] = encode_bcd(tm->Wday); 
    buf[5] = encode_bcd(tm->Day);
    buf[6] = (( tm->Year / 100) << 7) | encode_bcd(tm->Month);
    buf[7] = encode_bcd((tm->Year) % 100);
    i2c_write(DS1337_ADDR, buf, 8);
}

//param Epoch time 
void SetTime(char timeBuffer[4])
{
    uint32_t timeInput=0;
    time_RTC tm;
  
    for (int i = 0; i < 4; i++) 
    {
        timeInput = (timeInput << 8) | timeBuffer[i];
    }
    
    make_time(&tm,timeInput);
    ds1337_write_time(&tm);
}

/**
   \Remember to clear DS1337_A1_FLAG after alarm is triggered using ds1337_clear_status()
   \brief Set alarm 1 to occur at a specified day, hour and minute once per week.
   (At 0 seconds).
   \param day The day of the week at which the alarm should occur (in the range 1-7).
   \param hour The hour at which the alarm should occur.
   \param minute The minute at which the alarm should occur.

*/
void ds1337_set_alarm_1_at_day(uint8_t Day, uint8_t Hour, uint8_t Minute) 
{
    uint8_t buf[5];
    buf[0] = DS1337_REG_A1_SECONDS;
    buf[1] =0;
    buf[2] = encode_bcd(Minute);
    buf[3] = encode_bcd(Hour);
    buf[4] = DS1337_DAY_MASK | encode_bcd(Day);
    i2c_write(DS1337_ADDR, buf, 5);
}

/**
   \defgroup ds1337_alarm_2 Alarm 2 Setter Methods
*/

/**
   \Remember to clear DS1337_A2_FLAG after alarm is triggered using ds1337_clear_status()
   \brief Set alarm 2 to occur at a specified day, hour, and minute once per week.
   (At 0 seconds).
   \param day The day at which the alarm should occur (in the range 1-7).
   \param hour The hour at which the alarm should occur.
   \param minute The minute at which the alarm should occur.

*/
void ds1337_set_alarm_2_at_day(uint8_t Day, uint8_t Hour, uint8_t Minute) 
{
    uint8_t buf[4];
    buf[0] = DS1337_REG_A2_MINUTES;
    buf[1] = encode_bcd(Minute);
    buf[2] = encode_bcd(Hour);
    buf[3] = DS1337_DAY_MASK | encode_bcd(Day);
    i2c_write(DS1337_ADDR, buf, 4);
}

/**
   \defgroup ds1337_control Control Register Methods
*/

/**
   \brief Set the value of the control register.
   \param ctrl The value to set.
*/
void ds1337_set_control(uint8_t ctrl) 
{
    uint8_t buf[2];
    buf[0] = DS1337_REG_CONTROL;
    buf[1] = ctrl;
    i2c_write(DS1337_ADDR, buf, 2);
}

/**
   \brief Clear the specified bits in the control register.
   \param mask A mask specifying which bits to clear. (High bits will be cleared.)

*/
void ds1337_clear_control_bits(uint8_t mask) 
{
    ds1337_set_control(~mask);
}


/**
   \defgroup ds1337_status Status Register Methods
*/

/**
   \brief Get the value of the status register.
   \param stat A pointer to a value in which to store the value of the status register.

*/
void ds1337_get_status(uint8_t* stat) 
{
    i2c_read(DS1337_ADDR,DS1337_REG_STATUS,stat, 1);
}

/**
   \brief Set the value of the status register.
   \param stat The value to set.

*/
void ds1337_set_status(uint8_t stat) 
{
    uint8_t buf[2];
    buf[0] = DS1337_REG_STATUS;
    buf[1] = stat;
    i2c_write(DS1337_ADDR, buf, 2);
}


/**
   \brief Clear the status register.

*/
void ds1337_clear_status() 
{
    uint8_t buf[2];
    buf[0] = DS1337_REG_STATUS;
    buf[1] = 0;
    i2c_write(DS1337_ADDR, buf, 2);
}




