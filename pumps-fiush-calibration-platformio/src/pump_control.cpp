#include <Arduino.h>
#include <Wire.h>
#include "PCF8574.h"
#include "ShiftRegister74HC595.h"
#include "pump_control.h"


ShiftRegister74HC595<3> auxiliarOutputs(19, 5, 18); // 19-Data, 5-SHCP, 18-STCP estaba en 4

PCF8574 PCF1_input(0x20);  // read encoder 0-7
PCF8574 PCF1_output(0x24); // set pump direction 0-7
PCF8574 PCF2_input(0x26);  // read encoder 8-15
PCF8574 PCF2_output(0x27); // set pump direction 8-15
PCF8574 PCF3_input(0x23);  // read encoder 16-23
PCF8574 PCF3_output(0x21); // set pump direction 16-23

bool last_read_encoder[24] = {true};
bool now_read_encoder[24] = {true};
int encoder_counter[24] = {0};

namespace _10klab {
namespace pumps {
#define ENABLE_PUMPS 23

void AllPCFLow();

void InitializationPCF8574(){
  PCF1_input.begin();
  PCF1_output.begin();
  PCF2_input.begin();
  PCF2_output.begin();
  PCF3_input.begin();
  PCF3_output.begin();
}

void PumpsInitialization(){
  pinMode(ENABLE_PUMPS, OUTPUT);
  InitializationPCF8574();
  AllPCFLow();
  digitalWrite(ENABLE_PUMPS, LOW);
}

void AllPCFLow() {
  Serial.print("en low");
  for (int i = 0; i < 8; i++) {
    PCF1_output.write(i, LOW);
  }
  for (int i = 0; i < 8; i++) {
    PCF2_output.write(i, LOW);
  }
  for (int i = 0; i < 8; i++) {
    PCF3_output.write(i, LOW);
  }
}

void AllPCFHigh()
{
  Serial.print("en high");
  for (int i = 0; i < 8; i++)
  {
    PCF1_output.write(i, HIGH);
  }
    for (int i = 0; i < 8; i++)
  {
    PCF2_output.write(i, HIGH);
  }
    for (int i = 0; i < 8; i++)
  {
    PCF3_output.write(i, HIGH);
  }
}

void ResetEncoders() {
  for (int i = 0; i < 24; i++) {
    encoder_counter[i] = 0;
  }
}

void InitialEncodersRead() {
  for (int i = 0; i < sizeof(now_read_encoder); i++) {
    if (i < 8) {
      now_read_encoder[i] = PCF1_input.read(i);
      last_read_encoder[i] = now_read_encoder[i];
    } else if (i >= 8 && i < 16) {
      now_read_encoder[i] = PCF2_input.read(i - 8);
      last_read_encoder[i] = now_read_encoder[i];
    } else if (i >= 16 && i < 24) {
      now_read_encoder[i] = PCF3_input.read(i - 16);
      last_read_encoder[i] = now_read_encoder[i];
    }
  }
}

void UpdateEncoders() {

  for (int i = 0; i < 24; i++) {
    if (i < 8) {
      now_read_encoder[i] = PCF1_input.read(i);
      if (now_read_encoder[i] != last_read_encoder[i]) {
        encoder_counter[i]++;
        last_read_encoder[i] = now_read_encoder[i];
      }
    }

    if (i >= 8 && i < 16) {
      now_read_encoder[i] = PCF2_input.read(i - 8);
      if (now_read_encoder[i] != last_read_encoder[i]) {
        encoder_counter[i]++;
        last_read_encoder[i] = now_read_encoder[i];
      }
    }

    if (i >= 16 && i < 24) {
      now_read_encoder[i] = PCF3_input.read(i - 16);
      if (now_read_encoder[i] != last_read_encoder[i]) {
        encoder_counter[i]++;
        last_read_encoder[i] = now_read_encoder[i];
      }
    }
  }
}

void TurnOffOnePCFOutput(int PCF_output){
    if (PCF_output < 8)
    {
      PCF1_output.write(PCF_output, LOW);
    }
    else if (PCF_output >= 8 && PCF_output < 16)
    {
      PCF2_output.write(PCF_output-8, LOW);
    }
    else if (PCF_output >= 16 && PCF_output < 24)
    {
      PCF3_output.write(PCF_output-16, LOW);
    }
}

int MinillitersToPulses(int ml, float k1, float k2){
  int pulses = 0;
  pulses = k1 * ml + k2;
  return pulses;
}

void PriorityOrder( int pump1, int ml1, int priority1, bool rotation1, float ka1, float kb1,
                    int pump2, int ml2, int priority2, bool rotation2, float ka2, float kb2,
                    int pump3, int ml3, int priority3, bool rotation3, float ka3, float kb3,
                    int pump4, int ml4, int priority4, bool rotation4, float ka4, float kb4,
                    int pump5, int ml5, int priority5, bool rotation5, float ka5, float kb5,
                    int pump6, int ml6, int priority6, bool rotation6, float ka6, float kb6                                                
                    ) {

  static int pump_number = 6;
  const int real_pumps[pump_number] = {pump1, pump2, pump3, pump4, pump5, pump6};
  const int pumps[pump_number] = {pump1, pump2, pump3, pump4, pump5, pump6};
  bool pump_direction[pump_number] = {rotation1, rotation2, rotation3, rotation4, rotation5, rotation6};
  int minilliters[pump_number] = {ml1, ml2, ml3, ml4, ml5, ml6};
  float calibration_ka[pump_number] = {ka1, ka2, ka3, ka4, ka5, ka6};
  float calibration_kb[pump_number] = {kb1, kb2, kb3, kb4, kb5, kb6};
  bool process_finished = false;
  bool pump_finished[pump_number] = {false, false, false, false, false, false};
  int actual_priority = 0;
  int pumps_priority[pump_number] = {priority1, priority2, priority3, priority4, priority5, priority6};
  int priority_counter[pump_number] = {0, 0, 0, 0, 0, 0};
  int pump_finished_counter = 0;
  int same_priority[pump_number] = {0, 0, 0, 0, 0, 0};


  for(int i = 0; i < pump_number; i++){
    minilliters[i] = MinillitersToPulses(minilliters[i], calibration_ka[i], calibration_kb[i]);
  }

  for (int i = 0; i < pump_number; i++) {
    Serial.println(pumps[i]);
  }

  digitalWrite(ENABLE_PUMPS, HIGH);
  AllPCFLow();
  ResetEncoders();
  InitialEncodersRead();

  for (int i = 0; i < pump_number; i++) {
    if (real_pumps[i] == 98) {
      pump_finished[i] = true;
      // minilliters[i] = -1;
    }
  }

  for (int i = 0; i < pump_number; i++) {
    for (int j = 0; j < pump_number; j++) {
      if (pumps_priority[j] == i) {
        same_priority[i]++;
      }
    }
  }
  Serial.println("    same priority");
  for(int i = 0; i < pump_number; i++) {
    Serial.println(same_priority[i]);
  }
  delay(2000);

  for (int i = 0; i < pump_number; i++) {
    if (same_priority[i] == 0) {
      same_priority[i] = 99;
    }
    Serial.println(same_priority[i]);
  }

  for (int i = 0; i < pump_number; i++) {
    if (pump_direction[i] == 1) {
      if (real_pumps[i] < 8) {
        PCF1_output.write(i, HIGH);
      }
      if (real_pumps[i] >= 8 && real_pumps[i] < 16) {
        PCF2_output.write(real_pumps[i] - 8, HIGH);
      }
      if (real_pumps[i] >= 16 && real_pumps[i] < 24) {
        PCF3_output.write(real_pumps[i] - 16, HIGH);
      }
    }
  }

  Serial.println("bombas");
  for (int i = 0; i < pump_number; i++) {
    Serial.println(pumps[i]);
  }
  Serial.println("ml");
  for (int i = 0; i < pump_number; i++) {
    Serial.println(minilliters[i]);
  }
  Serial.println("prioridad");
  for (int i = 0; i < pump_number; i++) {
    Serial.println(pumps_priority[i]);
  }
  Serial.println("/////////////");


  InitialEncodersRead();

  while (process_finished == false) {

    // static unsigned long current_time22 = 0;
    // if(millis() >= current_time22 + 300){
    //   Serial.println(actual_priority);
    //   current_time22 = millis();
    // } 

    // _10klab::screen::ScrollText(scroll_speed);
    process_finished = true;
    for (int i = 0; i < pump_number; i++) {
      // Serial.println("pump finished: " + String(i) + " = " + String(pump_finished[i]));
      if (pump_finished[i] == true) {
        process_finished = true;
      } else {
        process_finished = false;
        i = pump_number;
      }
    }
    // Serial.println("/////////////////////////////////////");

    for (int i = actual_priority; i < pump_number; i++) {
      if (same_priority[i] == priority_counter[i]) {
        // Serial.println("iguales");
        actual_priority++;
        // delay(3000);
      }
    }

    for (int i = 0; i < pump_number; i++) {
        // Serial.println("actual priority = " + String(actual_priority) + " pumps_priority[i] " + String(pumps_priority[i]));
      if (actual_priority == pumps_priority[i]) {
        if (pump_finished[i] == false) {
          // digitalWrite(real_pumps[i], LOW);
          // digitalWrite(real_pumps[pumps[i]], LOW);//mosfet
          // digitalWrite(real_pumps[pumps[i]], HIGH);//RELAY
          // digitalWrite(pumps[i], HIGH);
          auxiliarOutputs.set(real_pumps[i], HIGH);
          Serial.println("bomba encendida " + String(real_pumps[i]) +
                         " i = " + String(i));
        }
      }
    }

    UpdateEncoders();
    // ScrollText(scroll_speed2);

    // for (int i = 0; i < 8; i++) {
    //   Serial.println(encoder_counter[i]);
    // }
    // ingrediente 1
    if (encoder_counter[pumps[0]] >= minilliters[0]) {
      if (pump_finished[0] == false) {
        // digitalWrite(real_pumps[pumps[0]], HIGH);//mosfet
        // digitalWrite(real_pumps[pumps[0]], LOW);//RELAY
        auxiliarOutputs.set(real_pumps[0], LOW);
        TurnOffOnePCFOutput(pumps[0]);
        pump_finished_counter++;
        for (int i = 0; i < pump_number; i++) {
          if (pumps_priority[0] == i) {
            priority_counter[i]++;
          }
        }
        pump_finished[0] = true;
      }
    }
    // ingrediente 2
    if (encoder_counter[pumps[1]] >= minilliters[1]) {
      if (pump_finished[1] == false) {
        // digitalWrite(real_pumps[pumps[1]], HIGH);
        // digitalWrite(real_pumps[pumps[1]], LOW);
        auxiliarOutputs.set(real_pumps[1], LOW);
        TurnOffOnePCFOutput(pumps[1]);
        pump_finished_counter++;
        for (int i = 0; i < pump_number; i++) {
          if (pumps_priority[1] == i) {
            priority_counter[i]++;
          }
        }
        pump_finished[1] = true;
      }
    }
    // ingrediente 3
    if (encoder_counter[pumps[2]] >= minilliters[2]) {
      if (pump_finished[2] == false) {
        // digitalWrite(real_pumps[pumps[2]], HIGH);
        // digitalWrite(real_pumps[pumps[2]], LOW);
        auxiliarOutputs.set(real_pumps[2], LOW);
        TurnOffOnePCFOutput(pumps[2]);
        pump_finished_counter++;
        for (int i = 0; i < pump_number; i++) {
          if (pumps_priority[2] == i) {
            priority_counter[i]++;
          }
        }
        pump_finished[2] = true;
      }
    }
    // ingrediente 4
    if (encoder_counter[pumps[3]] >= minilliters[3]) {
      if (pump_finished[3] == false) {
        // digitalWrite(real_pumps[pumps[2]], HIGH);
        // digitalWrite(real_pumps[pumps[2]], LOW);
        auxiliarOutputs.set(real_pumps[3], LOW);
        TurnOffOnePCFOutput(pumps[3]);
        pump_finished_counter++;
        for (int i = 0; i < pump_number; i++) {
          if (pumps_priority[3] == i) {
            priority_counter[i]++;
          }
        }
        pump_finished[3] = true;
      }
    }

    //ingrediente 5
    if (encoder_counter[pumps[4]] >= minilliters[4]) {
      if (pump_finished[4] == false) {
        // digitalWrite(real_pumps[pumps[2]], HIGH);
        // digitalWrite(real_pumps[pumps[2]], LOW);
        auxiliarOutputs.set(real_pumps[4], LOW);
        TurnOffOnePCFOutput(pumps[4]);
        pump_finished_counter++;
        for (int i = 0; i < pump_number; i++) {
          if (pumps_priority[4] == i) {
            priority_counter[i]++;
          }
        }
        pump_finished[4] = true;
      }
    }

    //ingrediente 6
    if (encoder_counter[pumps[5]] >= minilliters[5]) {
      if (pump_finished[5] == false) {
        // digitalWrite(real_pumps[pumps[2]], HIGH);
        // digitalWrite(real_pumps[pumps[2]], LOW);
        auxiliarOutputs.set(real_pumps[5], LOW);
        TurnOffOnePCFOutput(pumps[5]);
        pump_finished_counter++;
        for (int i = 0; i < pump_number; i++) {
          if (pumps_priority[5] == i) {
            priority_counter[i]++;
          }
        }
        pump_finished[5] = true;
      }
    }
    // process_finished = true;
    // for (int i = actual_priority; i < pump_number; i++) {
    //   if (same_priority[i] == priority_counter[i]) {
    //     Serial.println("iguales");
    //     actual_priority++;
    //     // delay(3000);
    //   }
    // }    
  }
  digitalWrite(ENABLE_PUMPS, LOW);
  AllPCFLow();
}

} // namespace pumps
} // namespace _10klab