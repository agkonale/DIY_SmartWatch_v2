#ifndef DRV2605_H               
#define DRV2605_H

#include <arduino.h>
#include <Wire.h>
#include <Adafruit_DRV2605.h>

void DRV2605_Setup(Adafruit_DRV2605 &drv);

//To play a particular sequence of effects (1-116) and wait for some delay
//Refer datasheet for effect ids
//param 2 :effect_id array
//param 3: number of effects to be played (1-7)
//param 4: delay_interval (in ms)
void Play_Effects(Adafruit_DRV2605 &drv, uint8_t* effect_id,uint8_t num,uint8_t delay_interval);

//For Alarm
//Customisable via EEPROM
void Vibrate_A(Adafruit_DRV2605 &drv,uint8_t* buf,uint8_t num);

//Vibe alert for incoming Call
//Fixed Pattern
void Vibrate_C(Adafruit_DRV2605 &drv);

//Vibe alert for recieved Message
//Fixed Pattern
void Vibrate_M(Adafruit_DRV2605 &drv);

#endif
