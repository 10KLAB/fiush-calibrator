#include "HX711.h"
#include "linealization.h"
#include "scale.h"
#include "eeprom-manager.h"
#include <Arduino.h>


#define LOADCELL_DOUT_PIN 32
#define LOADCELL_SCK_PIN 33

namespace _10klab {
namespace scale {
HX711 loadCell;
_10klab::linealization::Coefficients GetCoefficients;
_10klab::eeprom::MemoryCoefficients ReadCoefficients;

void Tare();

void SetUpScale() {
    _10klab::eeprom::SetupEEPROM();
  loadCell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  loadCell.set_scale(800);
  Tare();
  ReadCoefficients = _10klab::eeprom::ReadCoefficients();
}

void UpdateCoefficients(){
    ReadCoefficients = _10klab::eeprom::ReadCoefficients();
}

void Tare() { 
    loadCell.tare();
    // d
    // dacWrite
}

float GetRaw(int samples) { 
    float average = loadCell.get_units(samples);
    // Serial.println("en GetUnits  " + String(average));
    // float value = (average*ReadCoefficients.ka) + ReadCoefficients.kb;
    return average;
}

float GetUnits(int samples){
    float average = loadCell.get_units(samples);
    float ka = ReadCoefficients.ka;
    float kb = ReadCoefficients.kb;
    float value = (average*ka) + kb;
    Serial.println("ka: " + String(ka) + " kb: " + String(kb));
    return value;

}
float StableMeasure(){
  float prev_weight = 0;
  float weight = GetUnits(10);

  while((weight - prev_weight) > 0.5 || (weight - prev_weight) < 0){
    prev_weight = weight;
    delay(250);
    weight = GetUnits(10);
    Serial.println(weight-prev_weight);
  }
  return weight;
}

} // namespace scale
} // namespace _10klab