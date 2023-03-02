#include "HX711.h"
#include "linealization.h"
#include <Arduino.h>

#include <Pushbutton.h>
#define BUTTON_PIN 5
Pushbutton button(BUTTON_PIN);

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 18;
const int LOADCELL_SCK_PIN = 19;
HX711 scale;
_10klab::linealization::Coefficients GetCoefficients;

void SetCoefficients();

void CalibrationMode();

void setup() {
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(800);
  CalibrationMode();
  scale.tare();               // reset the scale to 0
  // SetCoefficients();
}

void loop() {
  // if (button.getSingleDebouncedPress()) {
    // Serial.println("tarando");
    // scale.tare();
    // Serial.println("poner peso");
    // delay(500);
    delay(2000);
    Serial.println("tarando");
    scale.tare(); 
    Serial.println("poner peso");
    delay(2000);

    float average = (scale.get_units(10));
    float new_average = (average * GetCoefficients.ka) + GetCoefficients.kb;
    Serial.println("peso: " + String(new_average, 5) +
                   "  ka= " + String(GetCoefficients.ka) +
                   "kb= " + String(GetCoefficients.kb));
  // }
}

void WeightingForButtonA(){
  while (!button.getSingleDebouncedPress()) {
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
    if (button.getSingleDebouncedPress()) {
      delay(300);
      data_x[step_counter] = scale.get_units(10);
      Serial.println("retire el peso y presione el boton");
      WeightingForButtonA();
      Serial.println("peso: " + String(data_x[step_counter]));
      scale.tare();
      step_counter++;
      Serial.println("poner peso #: " + String(step_counter));
    }
  }
  GetCoefficients =
      _10klab::linealization::GetCoefficients(data_x, data_y, data_length);

  float ka = GetCoefficients.ka;
  float kb = GetCoefficients.kb;

  Serial.println("ka= " + String(ka));
  Serial.println("kb= " + String(kb));
  Serial.println("terminado");
}

void SetCoefficients() {

  float data_x[8] = {0};
  float data_y[8] = {0, 3, 6, 9, 53, 106, 457, 1059};
  const int data_length = sizeof(data_y) / sizeof(float);

  Serial.println("iniciando calibracion...");
  delay(5000);

  for (int i = 0; i < data_length; i++) {
    // delay(2000);
    scale.tare();
    Serial.println("poner peso #: " + String(i + 1));
    delay(3000);
    data_x[i] = scale.get_units(10);
    Serial.println("peso: " + String(data_x[i]));
    Serial.println("retirar");
    delay(3000);
  }

  GetCoefficients =
      _10klab::linealization::GetCoefficients(data_x, data_y, data_length);

  float ka = GetCoefficients.ka;
  float kb = GetCoefficients.kb;

  Serial.println("ka= " + String(ka));
  Serial.println("kb= " + String(kb));
}
