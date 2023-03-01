#include "linealization.h"
#include <Arduino.h>

#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 18;
const int LOADCELL_SCK_PIN = 19;
HX711 scale;
_10klab::linealization::Coefficients GetCoefficients;


void SetCoefficients();

void setup() { 
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(800);
  scale.tare();               // reset the scale to 0

  SetCoefficients();

 }

void loop() { 
    float average = (scale.get_units(10));
    float new_average = (average*GetCoefficients.ka) + GetCoefficients.kb; 
    Serial.println("peso:" + String(new_average,5) + "ka= " + String(GetCoefficients.ka) + "kb= " + String(GetCoefficients.kb));
    delay(2000);
   }


void SetCoefficients() {

  float data_x[8] = {0};
  float data_y[8] = {0, 3, 6, 9, 53, 106, 457, 1059};
  const int data_length = sizeof(data_y) / sizeof(float);

  Serial.println("iniciando calibracion...");
  delay(5000);

  for(int i = 0; i < data_length; i++){
    // delay(2000);
    scale.tare();
    Serial.println("poner peso #: " + String(i+1));
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
