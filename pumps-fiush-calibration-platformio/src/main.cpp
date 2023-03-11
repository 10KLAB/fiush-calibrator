#include "Pushbutton.h"
#include "TCP_client.h"
#include "UDP_client.h"
#include "connection_manager.h"
#include "eeprom-manager.h"
#include "linealization.h"
#include "scale.h"
#include <Arduino.h>

#define BUTTON_ONE 16
#define BUTTON_TWO 4

Pushbutton button_one(BUTTON_ONE);
Pushbutton button_two(BUTTON_TWO);

String host_ip = "";

void CalibrationMode();
void ManualMode();
void PumpsCaracterizationMode();
void SelectionMenu();

void setup() {
  Serial.begin(115200);
  // _10klab::eeprom::SetupEEPROM();
  // delay(100);
  // _10klab::eeprom::SaveCoefficients(0.84, 0.24);
  // _10klab::scale::SetUpScale();
  _10klab::connection_manager::ConenctWifi();

  Serial.println("holi");

  // EEPROM.write(0, 5);
  // int a = EEPROM.read(0);
  // Serial.println(a);

  SelectionMenu();
}

void loop() {
  // delay(2000);

  delay(1000);
}

void SelectionMenu() {
  static int page = 0;
  static bool selected = false;

  Serial.println("Page 0");
  while (true) {
    if (button_one.getSingleDebouncedPress()) {
      page++;
      Serial.println("Page: " + String(page));
      if (page > 3) {
        page = 0;
        Serial.println("Page: " + String(page));
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
        Serial.println("Pump caracterization Mode");
          PumpsCaracterizationMode();
        break;

      case 1:
        Serial.println("Manual Mode");
        ManualMode();
        break;

      case 2:
        Serial.println("Calibration Mode");
        CalibrationMode();
        break;

      case 3:
        Serial.println("borrando credenciales");
        _10klab::connection_manager::EraseCredentials();

      default:
        Serial.println("out of range");
        break;
      }
      page = 0;
      selected = false;
    }
  }
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

  _10klab::linealization::Coefficients GetCoefficients;

  WeightingForButtonA();
  Serial.println("tarando");

  _10klab::scale::Tare();
  delay(300);
  data_x[0] = _10klab::scale::GetRaw(10);
  _10klab::scale::Tare();

  Serial.println("peso: " + String(data_x[0]));
  Serial.println("poner peso 1 y presionar boton");

  while (step_counter < data_length) {
    if (button_one.getSingleDebouncedPress()) {
      delay(300);
      data_x[step_counter] = _10klab::scale::GetRaw(10);
      Serial.println("retire el peso y presione el boton");
      WeightingForButtonA();
      Serial.println("peso: " + String(data_x[step_counter]));
      delay(10);
      _10klab::scale::Tare();
      delay(10);
      step_counter++;
      Serial.println("poner peso #: " + String(step_counter));
    }

    if (button_two.getSingleDebouncedPress()) {
      Serial.println("exit");
      return;
    }
  }
  GetCoefficients =
      _10klab::linealization::GetCoefficients(data_x, data_y, data_length);

  float ka = GetCoefficients.ka;
  float kb = GetCoefficients.kb;

  _10klab::eeprom::SaveCoefficients(ka, kb);
  _10klab::scale::UpdateCoefficients();

  Serial.println("ka= " + String(ka));
  Serial.println("kb= " + String(kb));
  Serial.println("terminado");
}

void ManualMode() {
  _10klab::scale::Tare();
  unsigned long current_time = 0;
  const int refresh_time = 500;
  while (!button_two.getSingleDebouncedPress() &&
         !button_two.getSingleDebouncedRelease()) {

    if (millis() >= current_time + refresh_time) {
      float value = _10klab::scale::GetUnits(10);
      Serial.print("peso= " + String(value));
      current_time = millis();
    }
  }
  Serial.println("exit");
}

void PumpsCaracterizationMode() {
  _10klab::tcp_client::PumpParameters IncomingParameters;
  const int error_data = 99;

  IncomingParameters.pumpId = error_data;
  IncomingParameters.processFinished = false;
  _10klab::udp_client::UDPInitializer();
  String server_ip = _10klab::udp_client::InitialConnection();

  while (!IncomingParameters.processFinished) { 
    IncomingParameters = _10klab::tcp_client::IncomingParameters(server_ip);
    Serial.println(IncomingParameters.pumpId);

    if (IncomingParameters.pumpId != error_data) {
      Serial.println("data arrived, start process");
      IncomingParameters.pumpId = error_data;
      _10klab::tcp_client::SendAnswer(server_ip);
      delay(5000);
    }
    // delay(500);
  }
}
