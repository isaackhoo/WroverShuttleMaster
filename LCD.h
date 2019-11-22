#ifndef LCD_H
#define LCD_H

#include <SPI.h>
#include <Adafruit_GFX.h>
#include "WROVER_KIT_LCD.h"
// Do not include helper.h in LCD.
// will result in circular referencing

// lcd screen variable
extern WROVER_KIT_LCD tft;

// screen size
extern uint16_t lcdScreenHeight;
extern uint16_t lcdScreenWidth;

// scroll control
extern uint16_t scrollTopFixedArea;
extern uint16_t scrollBottomFixedArea;
extern uint16_t yStart;
extern uint16_t yArea;
extern uint16_t w_tmp, h_tmp;
extern int16_t  x1_tmp, y1_tmp;
extern int scrollPosY;
extern int scrollPosX;

// methods
extern void LcdInit();
extern int LcdDoScroll(int, int);
extern int LcdScrollText(const char*);

#endif