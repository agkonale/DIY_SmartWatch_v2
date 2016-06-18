#ifndef OLED_H
#define OLED_H

#include <arduino.h>
#include <U8glib.h>
#include "Wire.h"
#include "bitmap.h"

// b0111100/b0111101 for DC LOW/HIGH
#define OLED_ADDR 0x3c  //(Default DC-LOW)
#define CONTRASTREG 0x81

void setContrast(uint8_t CONTRAST);
void drawLOGO(U8GLIB_SSD1306_128X64 &u8g);

#endif
