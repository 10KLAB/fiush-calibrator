#include "HX711.h"
#include "linealization.h"
#include <Arduino.h>
#include <EEPROM.h>
#include "Pushbutton.h"
#define BUTTON_ONE 16
#define BUTTON_TWO 17

#define EEPROM_SIZE 8

Pushbutton button_one(BUTTON_ONE);
Pushbutton button_two(BUTTON_TWO);

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 18;
const int LOADCELL_SCK_PIN = 19;
HX711 scale;
_10klab::linealization::Coefficients GetCoefficients;

float ka = 0;
float kb = 0;



void SetCoefficients();
void CalibrationMode();
void SaveCoefficients(float ka, float kb);
void ReadCoefficients();
void SelectionMenu();

void setup() {
  Serial.begin(115200);

  EEPROM.begin(EEPROM_SIZE);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(800);
  // scale.tare();               // reset the scale to 0
  // CalibrationMode();
  // SetCoefficients();
  SelectionMenu();
}

void loop() {
  // if (button.getSingleDebouncedPress()) {
  // Serial.println("tarando");
  // scale.tare();
  // Serial.println("poner peso");
  // delay(500);
  delay(2000);
  // Serial.println("tarando");
  // scale.tare();
  // Serial.println("poner peso");
  // delay(2000);

  // float average = (scale.get_units(10));
  // float new_average = (average * GetCoefficients.ka) + GetCoefficients.kb;
  // Serial.println("peso: " + String(new_average, 5) +
  //                "  ka= " + String(GetCoefficients.ka) +
  //                "kb= " + String(GetCoefficients.kb));
  // // }
}

void WeightingForButtonA() {
  while (!button_one.getSingleDebouncedPress()) {
    delay(1);
  }
}

void CalibrationMode() {
  float data_x[8] = {0};
  float data_y[8] = {0, 3, 6, 9, 53, 106, 457, 1059};
  const int data_length = sizeof(data_y) / sizeof(float);
  int step_counter = 1;
  Serial.println("retirar objetos y presionar boton");

  WeightingForButtonA();

  scale.tare();
  delay(300);
  data_x[0] = scale.get_units(10);
  scale.tare();

  Serial.println("peso: " + String(data_x[0]));
  Serial.println("poner peso 1 y presionar boton");

  while (step_counter < data_length) {
    if (button_one.getSingleDebouncedPress()) {
      delay(300);
      data_x[step_counter] = scale.get_units(10);
      Serial.println("retire el peso y presione el boton");
      WeightingForButtonA();
      Serial.println("peso: " + String(data_x[step_counter]));
      scale.tare();
      step_counter++;
      Serial.println("poner peso #: " + String(step_counter));
    }
      if(button_two.getSingleDebouncedPress()){
        Serial.println("exit");
        return;
      }
  }
  GetCoefficients =
      _10klab::linealization::GetCoefficients(data_x, data_y, data_length);

  float ka = GetCoefficients.ka;
  float kb = GetCoefficients.kb;

  SaveCoefficients(ka, kb);

  Serial.println("ka= " + String(ka));
  Serial.println("kb= " + String(kb));
  Serial.println("terminado");
}

void ManualMode(){

}


void SelectionMenu() {
  static int page = 0;
  static bool selected = false;

  Serial.println("Page 0");
  while (true) {
    if (button_one.getSingleDebouncedPress()) {
      page++;
      Serial.println("Page: " + String(page));
      if(page > 2){
        page=0;
      }
    }
    if (button_two.getSingleDebouncedPress()) {
      Serial.println("selected: " + String(page));
      // page = 0;
      selected = true;
    }
    if (selected) {
      switch (page) {
      case 0:
        Serial.println("Pump linealization Mode");
        break;

      case 1:
        Serial.println("Manual Mode");
        ReadCoefficients();
        break;

      case 2:
        Serial.println("Calibration Mode");
        CalibrationMode();
        break;

      default:
        Serial.println("out of range");
        break;
      }
      page = 0;
      selected = false;
    }
  }
}

void ReadCoefficients(){
  const int ka_integer_address = 0;
  const int ka_fractional_address = 1;
  const int kb_integer_address = 2;
  const int kb_fractional_address = 3;
  const int float_to_integer_factor = 100;

  float ka_integer = EEPROM.read(ka_integer_address);
  float ka_fraction = EEPROM.read(ka_fractional_address);
  ka = ka_integer + ka_fraction / float_to_integer_factor;

  float kb_integer = EEPROM.read(kb_integer_address);
  float kb_fraction = EEPROM.read(kb_fractional_address);
  kb = kb_integer + kb_fraction / float_to_integer_factor;

  Serial.println(ka);
  Serial.print(kb);
}

void SaveCoefficients(float ka, float kb){

  const int ka_integer_address = 0;
  const int ka_fractional_address = 1;
  const int kb_integer_address = 2;
  const int kb_fractional_address = 3;
  const int float_to_integer_factor = 100;

  int integer_ka = ka;
  int fractional_ka = (ka - integer_ka) * float_to_integer_factor;

  int integer_kb = kb;
  int fractional_kb = (kb - integer_kb) * float_to_integer_factor;

  Serial.println("integer ka= " + String(integer_ka) + " fraction ka: " + String(fractional_ka));
  Serial.println("integer kb= " + String(integer_kb) + "fraction kb: " + String(fractional_kb));

  EEPROM.write(ka_integer_address, integer_ka);
  EEPROM.write(ka_fractional_address, fractional_ka);
 
  EEPROM.write(kb_integer_address, integer_kb);
  EEPROM.write(kb_fractional_address, fractional_kb);

  EEPROM.commit();
  Serial.println("data saved");
}





// void SetCoefficients() {

//   float data_x[8] = {0};
//   float data_y[8] = {0, 3, 6, 9, 53, 106, 457, 1059};
//   const int data_length = sizeof(data_y) / sizeof(float);

//   Serial.println("iniciando calibracion...");
//   delay(5000);

//   for (int i = 0; i < data_length; i++) {
//     // delay(2000);
//     scale.tare();
//     Serial.println("poner peso #: " + String(i + 1));
//     delay(3000);
//     data_x[i] = scale.get_units(10);
//     Serial.println("peso: " + String(data_x[i]));
//     Serial.println("retirar");
//     delay(3000);
//   }

//   GetCoefficients =
//       _10klab::linealization::GetCoefficients(data_x, data_y, data_length);

//   float ka = GetCoefficients.ka;
//   float kb = GetCoefficients.kb;

//   Serial.println("ka= " + String(ka));
//   Serial.println("kb= " + String(kb));
// }