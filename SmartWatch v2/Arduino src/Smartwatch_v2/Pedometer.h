#ifndef Pedometer_H
#define Pedometer_H

#include <arduino.h>
#include <EEPROM.h>
#include "EEPROM_ADDR.h"

//Refer http://www.analog.com/library/analogdialogue/archives/44-06/pedometer.html

class Pedometer_Data
{
    uint8_t Height;  
    uint8_t Weight;
    long StepCount;
    long StepCount_Prev;
    uint8_t Steps_per_2s;
    float Stride_Length;
    float Speed;
    float KM;
    float Calories;

    public:
    //Constructor
    Pedometer_Data()
    {      
        Height=EEPROM.read(addHeight);
        Weight=EEPROM.read(addWeight);
        StepCount=0;
        StepCount_Prev=0;
        Steps_per_2s=0;
        Stride_Length=0;
        Speed=0;
        KM=0;
        Calories=0;  
    }
    
    //ISR_Update_StepCount is used for incrementing StepCount  
    void Update_StepCount();
    void Update_Steps_per_2s();
    void Update_Stride_Length();
    void Update_Speed();
    void Update_Calories();
    void Update_KM();
    void RESET();
    void STORE();
    
};

#endif
