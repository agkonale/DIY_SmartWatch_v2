#include "Power_Management.h"

//Provides battery voltage (Vmax=4.43 V)
float Get_Battery_Voltage(uint8_t SENSE_PIN)
{
    // 1M, 330K divider across battery and using internal ADC ref of 1.1V
    // ((1e6+330e3)/330e3)*1.1 = Vmax = 4.43 Volts
    // 4.43/1023 = Volts per bit = 0.0043304  
    // get the battery Voltage
    int sensorValue = analogRead(SENSE_PIN);
    float battery_voltage  = sensorValue * 0.0043304;
    
    return battery_voltage;
}

//Provides Battery Lvl (0-3)
uint8_t Get_Battery_Lvl(float battery_voltage)
{
    uint8_t battery_status;
    
    if ( battery_voltage > 3.81)
    {    battery_status = 3;
    }
    
    else if ( battery_voltage < 3.81 && battery_voltage > 3.7 )
    {    battery_status = 2;
    }
    
    else if ( battery_voltage < 3.7 && battery_voltage > 3.4 )
    {    battery_status = 1;
    }
    
    else if ( battery_voltage < 3.4 )
    {    battery_status = 0;
    }
    
    return battery_status;
  
}

//  Credit :http://www.gammon.com.au
//  For Power Saving
//  Param: Sleep Interval 
//  1 second:  0b000110
//  2 seconds: 0b000111
//  4 seconds: 0b100000
//  8 seconds: 0b100001

void Sleep(byte interval)
{ 
    noInterrupts ();   // timed sequence below
    
    MCUSR = 0;                          // reset various flags
    WDTCSR |= 0b00011000;               // see docs, set WDCE, WDE
    WDTCSR =  0b01000000 | interval;    // set WDIE, and appropriate delay
    wdt_reset();
    
    byte adcsra_save = ADCSRA;
    ADCSRA = 0;  // disable ADC
    set_sleep_mode (SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
    sleep_enable();
    interrupts ();
    sleep_cpu ();            // now goes to Sleep and waits for the interrupt
    
    ADCSRA = adcsra_save;  // stop power reduction(Enable ADC)
  
}

  

