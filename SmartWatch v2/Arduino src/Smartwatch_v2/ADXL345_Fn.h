#ifndef ADXL345_FN_H
#define ADXL345_FN_H

#include <arduino.h>
#include <ADXL345.h>

//For Screen Orientation (Wake on Tilt)
//62.5 mg/LSB
#define THRESHOLD_1 14   
//625 us/LSB
#define DUR_ACT_1 16

//For Step Counting
#define THRESHOLD_2 5
#define DUR_ACT_2 3     

//To enter sleep mode(Acc)
#define THRESHOLD_INACT 4
//1 sec/LSB
#define DUR_INACT 5 

void SETUP_ADXL345_NORMAL_MODE(ADXL345 &Acc);
void SETUP_ADXL345_PEDOMETER_MODE(ADXL345 &Acc);


#endif
