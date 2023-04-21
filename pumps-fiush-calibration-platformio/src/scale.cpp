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
  _10klab::eeprom::SetupEEPROM();
  loadCell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  loadCell.set_scale(750);
  Tare();
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
  // d
  // dacWrite
}

float GetRaw(int samples) {
  float average = loadCell.get_units(samples);
  // Serial.println("en GetUnits  " + String(average));
  // float value = (average*ReadCoefficients.ka) + ReadCoefficients.kb;
  return average;
}

float GetUnits(int samples) {
  float average = loadCell.get_units(samples);
  float ka = ReadCoefficients.ka;
  float kb = ReadCoefficients.kb;
  float value = (average * ka) + kb;
  // Serial.println("average = " + String(average) + " ka: " + String(ka) + " kb: " + String(kb));
  // Serial.println(value);
  return value;
}
float StableMeasure(bool enable_negatives) {
  float prev_weight = 0;
  float weight = GetUnits(10);
  unsigned int previous_time = millis();
  const int timeout = 5000;

  // if (enable_negatives) {

  //   while ((weight - prev_weight) > 0.5 || (weight - prev_weight) < -3 ||
  //          weight < 0) {

  //     if (weight >= 0) {

  //       prev_weight = weight;
  //       delay(600);
  //       weight = GetUnits(10);
  //       Serial.println("w8 for correct measure " + String(weight) + " // " +
  //                      String(weight - prev_weight));
  //     } else {
  //       weight = GetUnits(10);
  //       prev_weight = 0;
  //       delay(600);
  //       Serial.println("w8 for correct measure " + String(weight));
  //     }
  //   }
  // } else {
  //   while ((weight - prev_weight) > 0.5) {
  //     prev_weight = weight;
  //     delay(600);
  //     weight = GetUnits(10);
  //     Serial.println("w8 for correct measure unloading" + String(weight) +
  //                    " // " + String(weight - prev_weight));

  //   if(millis() > previous_time + timeout){
  //     Serial.println("tare");
  //     Tare();
  //     previous_time = millis();
  //   }
  //   }
  // }

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

float StableMeasure2(int pulses, float input_threshold) {
  const int delay_time = 300;
  const int samples = 2;
  const int average_samples = 5;
  float treshold = 1;
  float previous_measure[samples] = {0};
  float weigth = GetUnits(average_samples);
  static float previous_weigth = 1;
  static int previous_pulses = 1;

  unsigned int previous_time = millis();
  const int timeout = 7000;

  // float previous_weight_treshold = previous_weigth + 50;
    // float previous_weight_treshold = previous_weigth * 50;
  bool end_flag = false;

  


  // if(weigth < 0){
  //   Tare();
  //   delay(delay_time);
  //   weigth = GetUnits(average_samples); 
  // }


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
      treshold = pulses_diff * (1 + input_threshold);
      Serial.println("Weigth = " + String(weigth) +  " Input TH= " + String(input_threshold) + " Treshhold = " + String(treshold) + " diff= " + String(weight_diff));

    for (int i = 0; i < samples; i++) {
      previous_measure[i] = GetUnits(average_samples);
      delay(delay_time);


      if((previous_measure[i]/previous_weigth) > treshold || (weigth - previous_measure[i] < -0.5)){
        end_flag = false;
        if(weight_diff > treshold){
          Serial.println("out of range");
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





// float StableMeasure2(int pulses) {
//   const int delay_time = 300;
//   const int samples = 2;
//   const int average_samples = 5;
//   const float treshold = 0.5;
//   float previous_measure[samples] = {0};
//   float weigth = GetUnits(average_samples);
//   static float previous_weigth = weigth;
//   static int previous_pulses = 0;

//   unsigned int previous_time = millis();
//   const int timeout = 7000;

//   float previous_weight_treshold = previous_weigth + 50;
//     // float previous_weight_treshold = previous_weigth * 50;
//   bool end_flag = false;


//   // if(weigth < 0){
//   //   Tare();
//   //   delay(delay_time);
//   //   weigth = GetUnits(average_samples); 
//   // }

// if(weigth < 3){
//   // previous_weigth = weigth;
//   previous_weight_treshold = 3;
// }

// if(previous_weigth <= 0){
//   previous_weigth = weigth;
// }
// Serial.println("previous_weigth = " + String(previous_weigth) + " weigth = " + String(weigth) + " diff = " + String(weigth-previous_weigth));

// Serial.println("treshold = " + String(previous_weigth + previous_weight_treshold));

//   while (!end_flag) {

//     for (int i = 0; i < samples; i++) {
//       previous_measure[i] = GetUnits(average_samples);
//       delay(delay_time);
//     }

//     weigth = GetUnits(average_samples);
//     end_flag = true;

//     for (int i = 0; i < samples; i++) {
//       Serial.println("Weight = " + String(weigth) + " previous " + String(i) + " = " + String(previous_measure[i]) + " diff = " + String(weigth - previous_measure[i]));

//       if ((weigth - previous_measure[i]) > treshold || (weigth - previous_measure[i]) < -0.5 || weigth > (previous_weight_treshold)) {
//         end_flag = false;
//         if(weigth > (previous_weigth*4)){
//           Serial.println("out of range");
//           return -1;
//         }

//         if(millis() > previous_time + timeout){
//           Serial.println("timeout");
//           return -1;
//         }
//       }

//     }
//     delay(delay_time);
//     if(end_flag){
//       delay(1000);
//       float verify_weigth = GetUnits(average_samples);
//       if ((verify_weigth - weigth) > 0.5 || (verify_weigth - weigth) < -0.5){
//         end_flag = false;
//       }
//     }
//   }
//   previous_weigth = weigth;
//   return weigth;
// }