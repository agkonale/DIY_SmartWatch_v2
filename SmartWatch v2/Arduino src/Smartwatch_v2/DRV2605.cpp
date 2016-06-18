#include "DRV2605.h"

//Setup for DRV2605
void DRV2605_Setup(Adafruit_DRV2605 &drv)
{
    drv.begin();
    drv.selectLibrary(1);  
    
    // I2C trigger by sending 'go' command 
    // default, internal trigger when sending GO command
    drv.setMode(DRV2605_MODE_INTTRIG);   
}

//To play a particular sequence of effects (1-116) and wait for some delay
//Refer datasheet for effect ids
//param 2 :effect_id array
//param 3: number of effects to be played (1-7)
//param 4: delay_interval (in ms)

void Play_Effects (Adafruit_DRV2605 &drv, uint8_t* effect_id,uint8_t num,uint8_t delay_interval)
{

    // set the effects to play 
    for (uint8_t i = 0; i < num; i++) 
    {
        drv.setWaveform(i, effect_id[i]);
    }      
  
    //End of Waveform
    drv.setWaveform(num, 0); 
  
    // play the effect!
    drv.go();
  
    // wait a bit
    delay(delay_interval);
  
}

//For Alarm
void Vibrate_A(Adafruit_DRV2605 &drv,uint8_t* buf,uint8_t num)
{  
    Play_Effects(drv,buf,num,100);
}

//Vibe alert for incoming Call
void Vibrate_C(Adafruit_DRV2605 &drv)
{
    uint8_t buf[7]={10,11,12,13,14,15,16};
    Play_Effects(drv,buf,7,100);
}

//Vibe alert for recieved Message
void Vibrate_M(Adafruit_DRV2605 &drv)
{
    uint8_t buf[7]={10,11,12,13,14,15,16};
    Play_Effects(drv,buf,7,100);
}

