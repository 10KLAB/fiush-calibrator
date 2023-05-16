#include "scale.h"
#include "HX711.h"
#include "eeprom-manager.h"
#include "linealization.h"
#include <Arduino.h>

#define LOADCELL_DOUT_PIN 32 // trigg
#define LOADCELL_SCK_PIN 33  // echo

namespace _10klab {
namespace scale {
HX711 loadCell;
_10klab::linealization::Coefficients GetCoefficients;
_10klab::eeprom::MemoryCoefficients ReadCoefficients;

void Tare();

void SetUpScale() {
  // Initialize the EEPROM.
  _10klab::eeprom::SetupEEPROM();
  // Initialize the load cell.
  loadCell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  // Set the scale's scale factor to 750.
  loadCell.set_scale(750);
  Tare();
  // Read the scale's coefficients from the EEPROM.
  ReadCoefficients = _10klab::eeprom::ReadCoefficients();
}

void UpdateCoefficients() {
  ReadCoefficients = _10klab::eeprom::ReadCoefficients();
}

void Tare() {
  // delay(300);
  loadCell.tare();
  // delay(300);
  Serial.println("Tare");

}

float GetRaw(int samples) {
  float average = loadCell.get_units(samples);
  // Serial.println("en GetUnits  " + String(average));
  // float value = (average*ReadCoefficients.ka) + ReadCoefficients.kb;
  return average;
}

float GetUnits(int samples) {
  // Get the raw value from the load cell.
  float average = loadCell.get_units(samples);
  // Get the scale's coefficients.
  float ka = ReadCoefficients.ka;
  float kb = ReadCoefficients.kb;
  // Calculate the weight of the object in units.
  float value = (average * ka) + kb;
  // Serial.println("average = " + String(average) + " ka: " + String(ka) + " kb: " + String(kb));
  // Serial.println(value);
  return value;
}

float StableMeasure(bool enable_negatives) {
  /*This function measures the weight of an object on the scale and returns the weight if it is stable.

  It does this by repeatedly measuring the weight of the object and checking if the weight has changed 
  by more than 0.5 units in the last 5 seconds.

  If the weight has not changed by more than 0.5 units in the last 5 seconds, then the weight is 
  considered to be stable and the function returns the weight.

  Otherwise, the function continues to measure the weight until the weight is stable.*/

  float prev_weight = 0;
  float weight = GetUnits(10);
  unsigned int previous_time = millis();
  const int timeout = 5000;


    while ((weight - prev_weight) > 0.5 || weight < -0.5) {
      prev_weight = weight;
      delay(300);
      weight = GetUnits(10);
      Serial.println("w8 for correct measure unloading " + String(weight) +
                     " // " + String(weight - prev_weight));

    if(millis() > previous_time + timeout){
      Serial.println("tare unloading");
      Tare();
      previous_time = millis();
    }
    
  }

  Serial.println("stable weight = " + String(weight));
  return weight;
}


float CalibratorMeasure(bool enable_tare) {
  float prev_weight = 0;
  float weight = GetRaw(10);
  unsigned int previous_time = millis();
  const int timeout = 10000;


    if(enable_tare){
      Tare();
    }
    while ((weight - prev_weight) > 0.1 || weight - prev_weight < -0.5 || weight < -0.1) {
      prev_weight = weight;
      delay(250);
      weight = GetRaw(10);
      Serial.println("w8 for correct measure unloading " + String(weight) +
                     " // " + String(weight - prev_weight));

    if((millis() > previous_time + timeout) && enable_tare){
      Serial.println("tare unloading");
      Tare();
      previous_time = millis();
    }
    
  }

  Serial.println("stable weight = " + String(weight));
  return weight;
}


float StableMeasure2(int pulses, float input_threshold) {
  const int delay_time = 300;
  const int samples = 2;
  const int average_samples = 5;
  float treshold_possitive = 1;
  float treshold_negative = -1;

  float previous_measure[samples] = {0};
  float weigth = GetUnits(average_samples);
  static float previous_weigth = 1;
  static int previous_pulses = 1;

  unsigned int previous_time = millis();
  const int timeout = 7000;

  // float previous_weight_treshold = previous_weigth + 50;
    // float previous_weight_treshold = previous_weigth * 50;
  bool end_flag = false;


if(previous_weigth <= 0){
  previous_weigth = weigth;
}
// Serial.println("previous_weigth = " + String(previous_weigth) + " weigth = " + String(weigth) + " diff = " + String(weigth-previous_weigth));

// Serial.println("treshold = " + String(previous_weigth + previous_weight_treshold));

  while (!end_flag) {

    weigth = GetUnits(average_samples);
    end_flag = true;

    if(weigth < 10){
      if(input_threshold < 2){
        input_threshold = 2;
      }
    }
    if(previous_pulses > pulses){
      previous_pulses = 1;
    }

      float weight_diff = weigth/previous_weigth;
      int pulses_diff = pulses / previous_pulses;
      treshold_possitive = pulses_diff * (1 + input_threshold);
      treshold_negative = pulses_diff * (1 - input_threshold);

      // Serial.println("Weigth = " + String(weigth) +  " Input TH= " + String(input_threshold) + " Treshhold = " + String(treshold_possitive) + " diff= " + String(weight_diff));
      Serial.println("Weigth = " + String(weigth) + " Treshhold(+) = " + String(treshold_possitive) + " Treshhold(-) = " + String(treshold_negative)+ " diff= " + String(weight_diff));
    for (int i = 0; i < samples; i++) {
      previous_measure[i] = GetUnits(average_samples);
      delay(delay_time);


      if((previous_measure[i]/previous_weigth) > treshold_possitive || (previous_measure[i]/previous_weigth) < treshold_negative){
        end_flag = false;
        if(weight_diff > treshold_possitive){
          Serial.println("out of range positive");
          Serial.println("Weigth = " + String(weigth) + " Treshhold(+) = " + String(treshold_possitive) + " Treshhold(-) = " + String(treshold_negative)+ " diff= " + String(weight_diff));
          return -1;
        }
        if(weight_diff < treshold_negative){
          Serial.println("out of range negative");
          Serial.println("Weigth = " + String(weigth) + " Treshhold(+) = " + String(treshold_possitive) + " Treshhold(-) = " + String(treshold_negative)+ " diff= " + String(weight_diff));
          return -1;
        }

        if(millis() > previous_time + timeout){
          Serial.println("timeout");
          return -1;
        }
      }

    }
    delay(delay_time);
    if(end_flag){
      delay(1000);
      float verify_weigth = GetUnits(average_samples);
      if ((verify_weigth - weigth) > 0.5 || (verify_weigth - weigth) < -0.5){
        end_flag = false;
      }
    }
  }
  previous_weigth = weigth;
  previous_pulses = pulses;
  return weigth;
}

} // namespace scale
} // namespace _10klab


