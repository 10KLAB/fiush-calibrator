#include "screen_lcd.h"

namespace _10klab {
namespace screen {
LiquidCrystal_I2C lcd(0x22, 16, 2);

void ScreenSetup() {
  lcd.init();
  lcd.backlight();
}
void PrintScreen(int column, int row, String text, bool clear){

  if(clear){
    lcd.clear();
  }
    lcd.setCursor(column, row);
    lcd.print(text);
}

void ClearScreen(){
    lcd.clear();
}

} // namespace screen
} // namespace _10klab