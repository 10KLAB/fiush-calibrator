#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#ifndef SCREEN_LCD_H_
#define SCREEN_LCD_H_

namespace _10klab {
namespace screen {
void ScreenSetup();
void PrintScreen(int column, int row, String text, bool clear);
void ClearScreen();
} // namespace screen
} // namespace _10klab
#endif