#include "Pedometer.h"


void Pedometer_Data::Update_StepCount()
{
    StepCount++;
}

void Pedometer_Data::Update_Steps_per_2s()
{
    Steps_per_2s=StepCount-StepCount_Prev;
    StepCount_Prev=StepCount;
}


void Pedometer_Data::Update_Stride_Length()
{
    switch(Steps_per_2s)
    {
        case 1:
        Stride_Length=Height/500;
        break;

        case 2:
        Stride_Length=Height/400;
        break;

        case 3:
        Stride_Length=Height/300;
        break;

        case 4:
        Stride_Length=Height/200;
        break;

        case 5:
        Stride_Length=Height/120;
        break;

        case 6:
        Stride_Length=Height/100;
        break;

        default:
        Stride_Length=Height*0.012;
        break;           
    }
  
}


void Pedometer_Data::Update_Speed()
{
    Speed=Stride_Length*Steps_per_2s/2;
}


//Depends upon : Body weight, intensity of workout, conditioning level, and metabolism
//Provides Calories burnt in 2s
void Pedometer_Data::Update_Calories()
{
    //Active Period
    if(Speed>0)
    {
        Calories+=Speed*Weight/400; 
    }
  
    //Inactive Period
    else 
    {
        Calories+=Weight/1800;
    }
}

void Pedometer_Data::Update_KM()
{
    KM+=Stride_Length*Steps_per_2s/1000;
}


void Pedometer_Data::RESET()
{
    StepCount=0;
    StepCount_Prev=0;
    Steps_per_2s=0;
    Stride_Length=0;
    Speed=0;
    KM=0;
    Calories=0;
}

void Pedometer_Data::STORE()
{
    EEPROM.put(addStp,StepCount);
    EEPROM.put(addCal,Calories);
    EEPROM.put(addKm,KM);
}

