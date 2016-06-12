#ifndef Pedometer_H
#define Pedometer_H

#include <arduino.h>
#include <EEPROM.h>

//EEPROM Add. for data storage
#define addStp 28 //address to store number of steps
#define addCal 32 //address to store number of calories 
#define addKm  36 //address to store number of km 

//Refer http://www.analog.com/library/analogdialogue/archives/44-06/pedometer.html

struct Pedometer_Data
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
    
    Pedometer_Data();
    //ISR_Update_StepCount is used for incrementing StepCount
    void Update_Stride_Length();
    void Update_Speed();
    void Update_Calories();
    void Update_KM();
    void RESET();
    void STORE();
    
};

#endif
