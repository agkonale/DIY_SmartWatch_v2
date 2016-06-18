#include "OLED.h"

//Display functions

//Setting Contrast (0-255) (0x00-0xFF)  Current consumption increases with higher contrast
//Refer SSD1306 Datasheet for more details
void setContrast(uint8_t CONTRAST) 
{
    Wire.beginTransmission(OLED_ADDR);
    Wire.write(0x00);
    Wire.write(CONTRASTREG);
    Wire.endTransmission();
    Wire.beginTransmission(OLED_ADDR);
    Wire.write(0x00);
    Wire.write(CONTRAST);
    Wire.endTransmission();
}


//For displaying device logo
void drawLOGO(U8GLIB_SSD1306_128X64 &u8g)
{
    u8g.drawBitmapP( 15, 3, 12, 64, IMG_logo_96x64);
}
