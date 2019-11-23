#ifndef LCD_H
#define LCD_H

// Do not include helper.h in LCD.
// will result in circular referencing

// // screen size
// extern uint16_t lcdScreenHeight;
// extern uint16_t lcdScreenWidth;

// // scroll control
// extern uint16_t scrollTopFixedArea;
// extern uint16_t scrollBottomFixedArea;
// extern uint16_t yStart;
// extern uint16_t yArea;
// extern uint16_t w_tmp, h_tmp;
// extern int16_t  x1_tmp, y1_tmp;
// extern int scrollPosY;
// extern int scrollPosX;

// methods
extern void LcdInit();
extern int LcdDoScroll(int, int);
extern int LcdScrollText(const char*);

#endif