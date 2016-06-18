#include "User_Biodata.h"

USER_BIODATA::USER_BIODATA()
{
    Height=EEPROM.read(addHeight);
    Age=EEPROM.read(addAge);
    Gender=EEPROM.read(addGender);
    Weight=EEPROM.read(addWeight);
    
    for (uint8_t i = 0; i < 10; i++) 
    {
        Name[10-i]=EEPROM.read(addName+i);
    }
}

void USER_BIODATA::Store_BIODATA()
{
    EEPROM.write(addHeight,Height);
    EEPROM.write(addAge,Age);
    EEPROM.write(addGender,Gender);
    EEPROM.write(addWeight,Weight);
    for (uint8_t i = 0; i < 10; i++) 
    {
        EEPROM.write(addName+i,Name[10-i]);
    }
}

void USER_BIODATA::Set_BIODATA(char Name[10],uint8_t Height, uint8_t Age,bool Gender,uint8_t Weight)
{
    Height=Height;
    Age=Age;
    Gender=Gender;
    Weight=Weight;
    Name=Name;
}
