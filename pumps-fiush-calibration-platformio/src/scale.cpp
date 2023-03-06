#include "HX711.h"
#include "linealization.h"
#include "scale.h"
#include "eeprom-manager.h"
#include <Arduino.h>


#define LOADCELL_DOUT_PIN 27
#define LOADCELL_SCK_PIN 26

namespace _10klab {
namespace scale {
HX711 loadCell;
_10klab::linealization::Coefficients GetCoefficients;
_10klab::eeprom::MemoryCoefficients ReadCoefficients;

void Tare();

void SetUpScale() {
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

} // namespace scale
} // namespace _10klab