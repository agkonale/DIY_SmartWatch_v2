#ifndef DRV2605_H               
#define DRV2605_H

#include <arduino.h>
#include <Wire.h>
#include <Adafruit_DRV2605.h>
#include <inttypes.h>

void DRV2605_Setup(Adafruit_DRV2605* drv);

void Play_Effect (Adafruit_DRV2605* drv, uint8_t* effect_id,uint8_t num,uint8_t delay_interval);

//For Alarm
void vibrate_a(Adafruit_DRV2605* drv,uint8_t* buf,uint8_t num);

//Vibe alert for incoming Call
void vibrate_c(Adafruit_DRV2605* drv);

//Vibe alert for recieved Message
void vibrate_m(Adafruit_DRV2605* drv);

#endif
