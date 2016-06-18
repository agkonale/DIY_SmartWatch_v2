#include "ADXL345_Fn.h"

void SETUP_ADXL345_NORMAL_MODE(ADXL345 &Acc)
{
    detachInterrupt(1);
    Acc.setIntSingleTapEnabled(0);
    
    //+-4g(0x0-0x3: 2g-16g)
    Acc.setRange(0x1);
    
    //DC
    Acc.setInactivityAC(0);
    //scaled at 62.5 mg/LSB
    Acc.setInactivityThreshold(THRESHOLD_INACT);
    //1sec/LSB
    Acc.setInactivityTime(DUR_INACT);
    ///Enable all axes for Activity monitoring
    Acc.setInactivityXEnabled(1);
    Acc.setInactivityXEnabled(1);
    Acc.setInactivityXEnabled(1);
    
    //scaled at 62.5 mg/LSB
    Acc.setTapThreshold(THRESHOLD_1);
    //scaled at 625 us/LSB
    Acc.setTapDuration(DUR_ACT_1);
    //Enable Z axis
    Acc.setTapAxisXEnabled(0);
    Acc.setTapAxisYEnabled(0);
    Acc.setTapAxisZEnabled(1);
    
    //12.5 Hz 0x7-0xA: 12.5 Hz-100Hz
    Acc.setRate(0x7);
    
    Acc.setLowPowerEnabled(1); 
    //0x0 - 0x3, indicating 8/4/2/1Hz   
    //1Hz  
    Acc.setWakeupFrequency(0x3); 
    
    //INT1
    Acc.setIntSingleTapPin(8);
    Acc.setIntSingleTapEnabled(1);
}

void SETUP_ADXL345_PEDOMETER_MODE(ADXL345 &Acc)
{
    detachInterrupt(1);
    Acc.setIntSingleTapEnabled(0);
    
    //+-8g(0x0-0x3: 2g-16g)
    Acc.setRange(0x2);
    
    Acc.setInactivityXEnabled(0);
    Acc.setInactivityXEnabled(0);
    Acc.setInactivityXEnabled(0);

    //scaled at 62.5 mg/LSB
    Acc.setTapThreshold(THRESHOLD_2);
    //scaled at 625 us/LSB
    Acc.setTapDuration(DUR_ACT_2);
    Acc.setTapAxisXEnabled(1);
    Acc.setTapAxisYEnabled(1);
    Acc.setTapAxisZEnabled(1);

    //12.5 Hz 0x7-0xA: 12.5 Hz-100Hz
    //50 Hz
    Acc.setRate(0x9);
    
    //INT1
    Acc.setIntSingleTapPin(8);
    Acc.setIntSingleTapEnabled(1);
      
}


 
