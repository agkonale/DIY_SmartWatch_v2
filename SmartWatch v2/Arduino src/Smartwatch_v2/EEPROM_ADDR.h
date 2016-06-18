#ifndef EEPROM_ADDR_H
#define EEPROM_ADDR_H

/**EEPROM addresses
0-3   sun
4-7   mon
8-11  tue
12-15 wed
16-19 thu
20-23 fri
24-27 sat
**/

//EEPROM Add. for Pedometer data storage
#define addStp 28 //address to store number of steps
#define addCal 32 //address to store number of calories 
#define addKm  36 //address to store number of km 

#define addHeight 41
#define addAge    42
#define addGender 43
#define addWeight 44
#define addName   45  //10 byte char Array

#endif
