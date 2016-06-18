#ifndef USER_BIODATA_H
#define USER_BIODATA_H

#include <arduino.h>
#include <EEPROM.h>
#include "EEPROM_ADDR.h"

class USER_BIODATA
{  
    char Name[10];  
    uint8_t Height; //in cm
    uint8_t Age;    //in Yrs
    bool Gender;    //0 :Male,1: Female
    uint8_t Weight; //in Kg

    public:
    USER_BIODATA();
    void Store_BIODATA();
    void Set_BIODATA(char Name[10],uint8_t Height, uint8_t Age,bool Gender,uint8_t Weight);
};


#endif
