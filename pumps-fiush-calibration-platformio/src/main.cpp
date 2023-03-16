#include "Pushbutton.h"
#include "TCP_client.h"
#include "UDP_client.h"
#include "connection_manager.h"
#include "eeprom-manager.h"
#include "linealization.h"
#include "pump_control.h"
#include "scale.h"
#include "screen_lcd.h"
#include <Arduino.h>

#define BUTTON_CHANGE 39
#define BUTTON_SELECT 36

Pushbutton button_change(BUTTON_CHANGE);
Pushbutton button_select(BUTTON_SELECT);

String host_ip = "";

void UpdateButtonsState();
void CalibrationMode();
void ManualMode();
void PumpsCaracterizationMode();
void SelectionMenu();

void setup() {
  Serial.begin(115200);
  // _10klab::eeprom::SetupEEPROM();
  // delay(100);
  // _10klab::eeprom::SaveCoefficients(0.84, 0.24);
  _10klab::scale::SetUpScale();
  // _10klab::pumps::PumpsInitialization();
  // delay(1000);
  _10klab::screen::ScreenSetup();

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
  static bool skip_one_screen = true;

  _10klab::screen::PrintScreen(0, 0, "Pump", true);
  _10klab::screen::PrintScreen(0, 1, "Caracterization", false);

  Serial.println("Page 0");
  while (true) {
    if(skip_one_screen){
      page++;
      skip_one_screen = false;
    }

    if (button_change.getSingleDebouncedPress()) {
      switch (page) {
      case 0:
        _10klab::screen::PrintScreen(0, 0, "Pump", true);
        _10klab::screen::PrintScreen(0, 1, "Caracterization", false);
        break;
      case 1:
        _10klab::screen::PrintScreen(0, 0, "Manual Mode", true);
        break;
      case 2:
        _10klab::screen::PrintScreen(0, 0, "Calibration Mode", true);
        break;
      case 3:
        _10klab::screen::PrintScreen(0, 0, "Clear WIFI", true);
        _10klab::screen::PrintScreen(0, 1, "Credentials", false);
        break;
      default:
        _10klab::screen::ClearScreen();
        break;
      }
      Serial.println("Page: " + String(page));
      page++;

      if (page > 3) {
        page = 0;
        Serial.println("Page: " + String(page));
      }
    }

    if (button_select.getSingleDebouncedPress()) {
      Serial.println("selected: " + String(page));
      // page = 0;
      selected = true;
    }

    if (selected) {
      switch (page-1) {
      case 0:
        Serial.println("Pump caracterization Mode");
        PumpsCaracterizationMode();
        page = 0;
        skip_one_screen = true;
        UpdateButtonsState();
        _10klab::screen::PrintScreen(0, 0, "Pump", true);
        _10klab::screen::PrintScreen(0, 1, "Caracterization", false);
        break;

      case 1:
        Serial.println("Manual Mode");
        ManualMode();
        page = 0;
        skip_one_screen = true;
        UpdateButtonsState();
        _10klab::screen::PrintScreen(0, 0, "Pump", true);
        _10klab::screen::PrintScreen(0, 1, "Caracterization", false);
        break;

      case 2:
        Serial.println("Calibration Mode");
        CalibrationMode();
        page = 0;
        skip_one_screen = true;
        UpdateButtonsState();
        _10klab::screen::PrintScreen(0, 0, "Pump", true);
        _10klab::screen::PrintScreen(0, 1, "Caracterization", false);
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

void WaitingForButtonSelect() {
  while (!button_select.getSingleDebouncedPress()) {
    delay(10);
  }
}

void CalibrationMode() {
  float data_x[8] = {0};
  float data_y[8] = {0, 3, 6, 9, 53, 106, 457, 1059};
  const int data_length = sizeof(data_y) / sizeof(float);
  int step_counter = 1;
  Serial.println("retirar objetos y presionar boton");
  _10klab::screen::PrintScreen(0, 0, "Remove Items", true);
  _10klab::screen::PrintScreen(0, 1 , "and press Select", false);

  _10klab::linealization::Coefficients GetCoefficients;

  UpdateButtonsState();
  WaitingForButtonSelect();
  _10klab::screen::PrintScreen(0, 1, "...", true); 
  Serial.println("tarando");
  UpdateButtonsState();

  _10klab::scale::Tare();
  delay(300);
  data_x[0] = _10klab::scale::GetRaw(10);
  _10klab::scale::Tare();

  Serial.println("peso: " + String(data_x[0]));
  Serial.println("poner peso 1 y presionar boton");

  _10klab::screen::PrintScreen(0, 0, "Put weight:" + String(int(data_y[1])) + "g", true);
  _10klab::screen::PrintScreen(0, 1 ,"and press Select", false);


  while (step_counter < data_length) {
    if (button_select.getSingleDebouncedPress()) {
      delay(300);
      data_x[step_counter] = _10klab::scale::GetRaw(10);
      
      Serial.println("retire el peso y presione el boton");
      _10klab::screen::PrintScreen(0, 0, "Remove Items", true);
      _10klab::screen::PrintScreen(0, 1 , "and press Select", false);
      WaitingForButtonSelect();
      _10klab::screen::PrintScreen(0, 1, "...", true); 
      Serial.println("peso: " + String(data_x[step_counter]));
      delay(10);
      _10klab::scale::Tare();
      delay(10);
      step_counter++;
      Serial.println("poner peso #: " + String(step_counter));
      _10klab::screen::PrintScreen(0, 0, "Put weight:" + String(int(data_y[step_counter])) + "g", true);
      _10klab::screen::PrintScreen(0, 1 ,"and press Select", false);
    }

    if (button_change.getSingleDebouncedPress()) {
      _10klab::screen::PrintScreen(0, 0, "Calibration", true);
      _10klab::screen::PrintScreen(0, 1 ,"aborted", false);
      delay(2000);
      return;
    }
  }

  GetCoefficients =
      _10klab::linealization::GetCoefficients(data_x, data_y, data_length);

  float ka = GetCoefficients.ka;
  float kb = GetCoefficients.kb;

  _10klab::eeprom::SaveCoefficients(ka, kb);
  _10klab::scale::UpdateCoefficients();

  _10klab::screen::PrintScreen(0, 0, "Calibration", true);
  _10klab::screen::PrintScreen(0, 1 ,"completed", false);
  delay(2000);

  Serial.println("ka= " + String(ka));
  Serial.println("kb= " + String(kb));
  Serial.println("terminado");
}

void ManualMode() {
  _10klab::scale::Tare();
  unsigned long current_time = 0;
  const int refresh_time = 500;
  UpdateButtonsState();

  while (!button_select.getSingleDebouncedPress()) {

    if (millis() >= current_time + refresh_time) {
      float value = _10klab::scale::GetUnits(10);
      _10klab::screen::PrintScreen(1, 0, String(value) + " grams", true);

      Serial.print("peso= " + String(value));
      current_time = millis();
    }
  }
  _10klab::screen::PrintScreen(0, 0, "Leaving Manual", true);
  _10klab::screen::PrintScreen(0, 1 ,"mode", false);
  Serial.println("exit");
  delay(2000);
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
      _10klab::pumps::PriorityOrder(
          IncomingParameters.pumpId - 1, IncomingParameters.pulses,
          IncomingParameters.priority - 1, IncomingParameters.rotation,
          IncomingParameters.ka, IncomingParameters.kb, 98, 98, 98, 0, 1, 0, 98,
          98, 98, 0, 1, 0, 98, 98, 98, 0, 1, 0, 98, 98, 98, 0, 1, 0, 98, 98, 98,
          0, 1, 0);
      IncomingParameters.pumpId = error_data;
      _10klab::tcp_client::SendAnswer(server_ip);
      // delay(5000);
    }
    // delay(100);
    // delay(500);
  }
}

void UpdateButtonsState() {
  delay(500);
  button_change.getSingleDebouncedPress();
  button_select.getSingleDebouncedPress();
}