#include "bitmap.h"
#include <ADXL345.h>
#include <avr/wdt.h>
#include "DS1337.h"
#include "BLE.h"
#include "DRV2605.h"
#include "Pedometer.h"
#include "DS1337.h"
#include "OLED.h"
#include "Power_Management.h"

//Watch Mode:
#define NORMAL_MODE 0        
#define PEDOMETER_MODE 1     
volatile uint8_t WATCH_MODE=NORMAL_MODE;

#define SCREEN_REFRESH_INTERVAL 500 //ms

//Page ptr:
#define HOME_PAGE             0x0
#define PEDOMETER_PAGE        0x1
#define MENU_PAGE             0x2
#define ALARM_PAGE            0x3
#define BLANK_PAGE            0xA

/////////////////////////////////////////////////////////PINS//////////////////////////////////////////////////////////////

#define ALARM_INTPIN 2                //Alarm Interrupt Pin (INT0)
#define ACC_INTPIN 3                  //For counting steps/Wake on Tilt(other applications requiring accelerometer)  (INT1) 
#define ON_OFF_INTPIN 8               //To stop Alarm vibration/To Wake up the screen (PCINT0)
#define Button1_INTPIN 9              //UI Button 1 (PCINT0)
#define Button2_INTPIN 10             //UI Button 2 (PCINT0)

#define BATTERY_SENSE_PIN A0          //select the input pin for the battery sense point
#define USB_CHARGING_STAT_PIN 4       //USB Connection status
#define BLE_CONNECTION_STAT_PIN 5     //BLE Connection status

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

volatile bool ALRM_ON_OFF = false;
volatile bool ALRM_TRIGGERED = false;

//Waking up screen via INT(button/Acc)
volatile bool Wake_via_INT=false;
//Awake time counter(1u = SCREEN_REFRESH_INTERVAL)
uint8_t count=0;
#define count_MAX 20 //10 sec

//Current page pointer
volatile uint8_t Page_ptr=HOME_PAGE;

//For ADXL345
bool NORMAL_SETUP_FLAG=false;
bool PEDOMETER_SETUP_FLAG=false;

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

//////////////////////////////////////////////////////////WDT/////////////////////////////////////////////////////////////////

void wake ()                            
{
    wdt_disable();  // disable watchdog
} 

ISR (WDT_vect) 
{
    wake ();
}  // end of WDT_vect

////////////////////////////////////////////////////////USER-BIODATA///////////////////////////////////////////////////////////

struct USER_BIODATA
{
    USER_BIODATA();
    void Store_BIODATA();
    void Set_BIODATA(char Name[10],uint8_t Height, uint8_t Age,bool Gender,uint8_t Weight);
    char Name[10];  
    uint8_t Height; //in cm
    uint8_t Age;    //in Yrs
    bool Gender;    //0 :Male,1: Female
    uint8_t Weight; //in Kg
};

 USER_BIODATA::USER_BIODATA()
 {
    Height=EEPROM.read(41);
    Age=EEPROM.read(42);
    Gender=EEPROM.read(43);
    Weight=EEPROM.read(44);
    for (uint8_t i = 0; i < 10; i++) 
    {
        Name[10-i]=EEPROM.read(45+i);
    }
}

 void USER_BIODATA::Store_BIODATA()
 {
    EEPROM.write(41,Height);
    EEPROM.write(42,Age);
    EEPROM.write(43,Gender);
    EEPROM.write(44,Weight);
    for (uint8_t i = 0; i < 10; i++) 
    {
        EEPROM.write(45+i,Name[10-i]);
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

USER_BIODATA USER;

Pedometer_Data Data;

/////////////////////////////////////////////////////Peripheral Devices///////////////////////////////////////////////////////

 // A structure to hold the time read from the DS1337 RTC
 // Sunday is day 1 
 // jan is month 1 
 time_RTC time;
 
 // A structure for controlling I2C OLED (128X64)
 U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);  // I2C / TWI 
 
 // A structure for controlling Haptic driver DRV2605
 Adafruit_DRV2605 drv;

 //ADXL345 3-axis accelerometer
 ADXL345 Acc;
 
////////////////////////////////////////////////////////ISR////////////////////////////////////////////////////////////////////
 //Keep these as small as possible!!
 
 //For UI buttons(3)
 ISR (PCINT0_vect) 
 {  
  
    if(digitalRead(ON_OFF_INTPIN)==0)
    {
        WATCH_MODE=NORMAL_MODE;
        Page_ptr=HOME_PAGE;
        Wake_via_INT=true;
        ALRM_ON_OFF=false;
    }
  
    else if(digitalRead(Button1_INTPIN)==0)
    {
      
      
    }
  
    else if(digitalRead(Button2_INTPIN)==0)
    {
      
      
    }
  
 }

 //For DS1337
 void ISR_ALARM()
 {
    ALRM_TRIGGERED=true;    
 }

 //For Acc
 void ISR_Update_StepCount()
 {
    Data.StepCount++;
 }

 void ISR_Wake_Via_Acc()
 {
    Wake_via_INT=true;
    Page_ptr=HOME_PAGE;
 }

//////////////////////////////////////////////////////////////ADXL345/////////////////////////////////////////////////////////////

 void SETUP_ADXL345_NORMAL_MODE()
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
      
      attachInterrupt (1,ISR_Wake_Via_Acc,RISING); 
      PEDOMETER_SETUP_FLAG=false; 
      NORMAL_SETUP_FLAG=true; 
 }

 void SETUP_ADXL345_PEDOMETER_MODE()
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
      
      attachInterrupt (1,ISR_Update_StepCount,RISING); 
      NORMAL_SETUP_FLAG=false;   
      PEDOMETER_SETUP_FLAG=true; 
 
 }

/////////////////////////////////////////////////////////////////Li-Po Battery///////////////////////////////////////////////////////
 
 float Battery_Voltage;
 uint8_t Battery_Lvl;
 //Charging complete status
 bool is_Charged=false;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
void setup() 
{
    //Set Baud Rate for BLE HM10
    //Chosen to match Bootloader BAUD Rate of Arduino PRO-MINI for OTA programming
    Serial.begin(57600);
  
    pinMode(ON_OFF_INTPIN, INPUT_PULLUP);
    pinMode(Button1_INTPIN, INPUT_PULLUP);
    pinMode(Button1_INTPIN, INPUT_PULLUP);
    pinMode(USB_CHARGING_STAT_PIN, INPUT_PULLUP);
    pinMode(BLE_CONNECTION_STAT_PIN, INPUT_PULLUP);  
  
    //Unused Pins (Set as Output :LOW to conserve power)
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(12, OUTPUT);
    
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    
    cli();    // switch interrupts off while messing with their settings  
    PCICR =0x01;          // Enable PCINT0 interrupt
    PCMSK0 = 0b00000111;  //Enable interrupts on Pins 8,9,10
    sei();    // turn interrupts back on
    
    //Personalise
    USER.Set_BIODATA("Abhishek  ",176,20,0,82);
  
    //Use internal precise 1v1 reference voltage for measuring battery volatge
    analogReference(INTERNAL);
    
    // Initialize the I2C bus
    Wire.begin();
  
    //Setup DRV2605
    DRV2605_Setup(&drv);
    
    //Initialize ADXL345
    Acc.initialize();
  
    //DS1337 alarm INT
    attachInterrupt (0,ISR_ALARM,LOW);
   
    //Loading stored alarms for present day
    ds1337_read_time(&time);
    uint8_t temp=time.Day;
    ds1337_set_alarm_1_at_day(temp,EEPROM.read((temp-1)*4) ,EEPROM.read((temp-1)*4+1));
    ds1337_set_alarm_2_at_day(temp,EEPROM.read((temp-1)*4+2) ,EEPROM.read((temp-1)*4+3));
  
    //Enable Alarms
    ds1337_clear_status();
    ds1337_set_control(DS1337_ALARM_ON);
  
    //Set Display font
    u8g.setFont(u8g_font_fur14r);
    
    //Display Logo
    u8g.firstPage();
    do {
      setContrast(0xFF);
      drawLOGO(&u8g);
      delay(3000);
      setContrast(0x02);
    } while ( u8g.nextPage() );
    delay(5000);

}


void loop() 
{

    if(ALRM_TRIGGERED)
    {      
        //Display Alarm Icon  
        Display(ALARM_PAGE);
         
        ALRM_TRIGGERED=false;
        //Reset alarmFlag
        ds1337_clear_status();
        ALRM_ON_OFF = true;
        
        uint8_t temp=0;
        
        //Load User Specific Alarm waveform stored in EEPROM
        uint8_t num=EEPROM.read(55);
        uint8_t buf[7];
    
    for (uint8_t i = 0; i < num; i++)
    {
        buf[i]=EEPROM.read(56+i);
    }
    
    while(ALRM_ON_OFF==true)
    {
        vibrate_a(&drv,buf,num);
        temp++;
        if(temp>20)
        {
            break;
        }
    }  
    } 
    
  
    if(WATCH_MODE==NORMAL_MODE){
  
    if(NORMAL_SETUP_FLAG==false){
        SETUP_ADXL345_NORMAL_MODE();
      }
  
    //To check if data is recieved from REMOTE
    bool isReceived = false;
    //USB charging status
    bool is_Charging=false;
    //Connection Status with Android phone
    bool is_Connected=false; 
    
    // Receive data from remote and parse
    isReceived = receiveBluetoothData();
  
    // If data doesn't arrive
    if(!isReceived){
      
      is_Charging=digitalRead(USB_CHARGING_STAT_PIN);
      is_Connected=digitalRead(BLE_CONNECTION_STAT_PIN);
  
      //Turn on watch only if screen is properly oriented or USB is connected or Wake button is pressed
      if(is_Charging or Wake_via_INT){ 
        
        if(Wake_via_INT){
        count++;
        if(count>count_MAX){
          Wake_via_INT=false;
          count=0;
        }
        } 
  
        if(Page_ptr==HOME_PAGE){
          
          Battery_Voltage=Get_Battery_Voltage(A0);
          Battery_Lvl=Get_Battery_Lvl(Battery_Voltage);
                
          uint8_t temp=time.Wday;
        
          ds1337_read_time(&time);
  
          if(temp!=time.Wday){
            //Loading stored alarms for present day
              ds1337_set_alarm_1_at_day(temp,EEPROM.read((temp-1)*4) ,EEPROM.read((temp-1)*4+1));
              ds1337_set_alarm_2_at_day(temp,EEPROM.read((temp-1)*4+2) ,EEPROM.read((temp-1)*4+3));
          } 
          
        }
  
        
        Display(Page_ptr); 
        
        delay(SCREEN_REFRESH_INTERVAL);  
           
      }
      
      else{      
        //Turn off Display
        Display(BLANK_PAGE);
        //Sleep for 2s
        //  1 second:  0b000110
        //  2 seconds: 0b000111
        //  4 seconds: 0b100000
        //  8 seconds: 0b100001
        Sleep(0b000111);      
      }  
      
    }
      
    }
  
  
  
  
  
    if(WATCH_MODE==PEDOMETER_MODE){
      
      if(PEDOMETER_SETUP_FLAG==false){
        SETUP_ADXL345_PEDOMETER_MODE();
      }
      
      //PedoMeter algorithm
      Data.Steps_per_2s=Data.StepCount-Data.StepCount_Prev;
      Data.StepCount_Prev=Data.StepCount;
      Data.Update_Stride_Length();
      Data.Update_Speed();
      Data.Update_Calories();
      Data.Update_KM(); 
  
      //Display data on screen
      Display(PEDOMETER_PAGE);
         
      //Sleep for 2s 
      Sleep(0b000111); 
  }


}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Display(uint8_t Page_ptr)
{

    if (Page_ptr == HOME_PAGE)
    {
        //display USB,Connectivity stat;  battery v,lvl ; Time ;
  
        u8g.setPrintPos(0, 28);
      
        if (time.Hour < 10)
        {
            u8g.print("0");
            u8g.print(time.Hour);
        }
        else
        {
            u8g.print(time.Hour);
        }
           
        u8g.print(":");
        if (time.Minute < 10)
        {
            u8g.print("0");
            u8g.print(time.Minute);
        }
        else
        {
            u8g.print(time.Minute);
        }
        
        u8g.setPrintPos(80, 60);
        if (time.Second < 10)
        {
            u8g.print("0");
            u8g.print(time.Second);
        }
        else
        {
            u8g.print(time.Second);
        }
  
        switch (Battery_Lvl)
        {
            case 3:
            u8g.drawBitmapP(110, 0, 2, 16, ICON_BITMAP_Batt_Lvl_3);
            break;
  
            case 2:
            u8g.drawBitmapP(110, 0, 2, 16, ICON_BITMAP_Batt_Lvl_2);
            break;
  
            case 1:
            u8g.drawBitmapP(110, 0, 2, 16, ICON_BITMAP_Batt_Lvl_1);
            break;
  
            default:
            u8g.drawBitmapP(110, 0, 2, 16, ICON_BITMAP_Batt_Lvl_0);
            break;
        }
                    
        } 
  
     else if (Page_ptr == MENU_PAGE)
     {
          
     }     
  
  
     else if (Page_ptr == ALARM_PAGE)
     {
          //u8g.drawBitmapP( 15, 3, 12, 64, IMG_alarm_96x64);       
     }    
  
  
     else if (Page_ptr == PEDOMETER_PAGE)
     {
          
     }    
   
     else if (Page_ptr == BLANK_PAGE)
     {
        u8g.firstPage();
        do  
        {       
        } 
        while ( u8g.nextPage() );
     } 
  
        
}

