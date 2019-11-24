#pragma once

#ifndef LCD_H
#define LCD_H

// Do not include helper.h in LCD.
// will result in circular referencing

// public methods
extern void LcdInit();
extern int LcdDoScroll(int, int);
extern int LcdScrollText(char *);

#endif