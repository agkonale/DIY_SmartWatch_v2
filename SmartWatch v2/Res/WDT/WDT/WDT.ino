/////////////////////////////////////////////////////////////////////////Tested OK////////////////////////////////////////////////////////////////////////////////////

#include <avr/wdt.h>
#include <avr/sleep.h>

void wake ()                            
{
  wdt_disable();  // disable watchdog
} 


ISR (WDT_vect) 
{
  wake ();
}  // end of WDT_vect


//Credit :http://www.gammon.com.au
void Sleep(byte interval){

  //u8g->sleepOn();            //Turn Display off
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
  
  ADCSRA = adcsra_save;  // stop power reduction
  
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
  Sleep(0b000111);      //2 sec
}
