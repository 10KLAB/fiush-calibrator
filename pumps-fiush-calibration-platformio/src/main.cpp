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

  _10klab::scale::SetUpScale();
  delay(100);
  _10klab::pumps::PumpsInitialization();

  _10klab::screen::ScreenSetup();

  _10klab::screen::PrintScreen(0, 0, "Fiush", true);
  _10klab::screen::PrintScreen(0, 1, "Calibrator", false);
  delay(1000);
  _10klab::screen::PrintScreen(0, 0, "Wifi setup", true);

  _10klab::connection_manager::ConenctWifi();

  _10klab::screen::PrintScreen(0, 0, "Wifi connected!", true);
  delay(1000);
  
  Serial.println("started");

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
    if (skip_one_screen) {
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
      switch (page - 1) {
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
  float data_x[12] = {0};
  float data_y[12] = {0, 1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 1200};
  // float data_x[4] = {0};
  // float data_y[4] = {0, 1, 50, 1200};
  const int data_length = sizeof(data_y) / sizeof(float);
  int step_counter = 1;
  Serial.println("retirar objetos y presionar boton");
  _10klab::screen::PrintScreen(0, 0, "Remove Items", true);
  _10klab::screen::PrintScreen(0, 1, "and press Select", false);

  _10klab::linealization::Coefficients GetCoefficients;

  UpdateButtonsState();
  WaitingForButtonSelect();
  _10klab::screen::PrintScreen(0, 1, "...", true);
  Serial.println("tarando");
  UpdateButtonsState();

  _10klab::scale::Tare();
  delay(500);
  data_x[0] = _10klab::scale::GetRaw(10);
  _10klab::scale::Tare();

  Serial.println("peso 0: " + String(data_x[0]));
  Serial.println("poner peso 1 y presionar boton");

  _10klab::screen::PrintScreen(
      0, 0, "Put weight:" + String(int(data_y[1])) + "g", true);
  _10klab::screen::PrintScreen(0, 1, "and press Select", false);

  while (step_counter < data_length) {
    if (button_select.getSingleDebouncedPress()) {
      delay(300);
      data_x[step_counter] = _10klab::scale::GetRaw(10);

      Serial.println("retire el peso y presione el boton");
      _10klab::screen::PrintScreen(0, 0, "Remove Items", true);
      _10klab::screen::PrintScreen(0, 1, "and press Select", false);
      WaitingForButtonSelect();
      _10klab::screen::PrintScreen(0, 1, "...", true);
      Serial.println("peso: " + String(data_x[step_counter]));
      delay(250);
      _10klab::scale::Tare();
      delay(250);
      step_counter++;
      Serial.println("poner peso #: " + String(step_counter));
      _10klab::screen::PrintScreen(
          0, 0, "Put weight:" + String(int(data_y[step_counter])) + "g", true);
      _10klab::screen::PrintScreen(0, 1, "and press Select", false);
    }

    if (button_change.getSingleDebouncedPress()) {
      _10klab::screen::PrintScreen(0, 0, "Calibration", true);
      _10klab::screen::PrintScreen(0, 1, "aborted", false);
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
  _10klab::screen::PrintScreen(0, 1, "completed", false);
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
  _10klab::screen::PrintScreen(0, 1, "mode", false);
  Serial.println("exit");
  delay(2000);
}

float SelectThreshold(){
  bool selected = false;
  float threshold = 0.1;
  int page = 0;

  _10klab::screen::PrintScreen(0, 0, "Select", true);
  _10klab::screen::PrintScreen(0, 1, "Threshold", false);
  delay(2000);
  _10klab::screen::PrintScreen(0, 0, "Threshold", true);
  _10klab::screen::PrintScreen(0, 1, "10%", false);
  page = 1;

  while(!selected){
    if (button_change.getSingleDebouncedPress()) {
      switch(page){
        case 0:
        _10klab::screen::PrintScreen(0, 0, "Threshold", true);
        _10klab::screen::PrintScreen(0, 1, "10%", false);
        threshold = 0.1;
        UpdateButtonsState();
        break;

        case 1:
        _10klab::screen::PrintScreen(0, 0, "Threshold", true);
        _10klab::screen::PrintScreen(0, 1, "20%", false);
        threshold = 0.2;
        UpdateButtonsState();
        break;

        case 2:
        _10klab::screen::PrintScreen(0, 0, "Threshold", true);
        _10klab::screen::PrintScreen(0, 1, "30%", false);
        threshold = 0.3;
        UpdateButtonsState();
        break;

        case 3:
        _10klab::screen::PrintScreen(0, 0, "Threshold", true);
        _10klab::screen::PrintScreen(0, 1, "40%", false);
        threshold = 0.4;
        UpdateButtonsState();
        break;

        case 4:
        _10klab::screen::PrintScreen(0, 0, "Threshold", true);
        _10klab::screen::PrintScreen(0, 1, "50%", false);
        threshold = 0.5;
        UpdateButtonsState();
        break;

        case 5:
        _10klab::screen::PrintScreen(0, 0, "Threshold", true);
        _10klab::screen::PrintScreen(0, 1, "60%", false);
        threshold = 0.6;
        UpdateButtonsState();
        break;

        case 6:
        _10klab::screen::PrintScreen(0, 0, "Threshold", true);
        _10klab::screen::PrintScreen(0, 1, "70%", false);
        threshold = 0.7;
        UpdateButtonsState();
        break;

        case 7:
        _10klab::screen::PrintScreen(0, 0, "Threshold", true);
        _10klab::screen::PrintScreen(0, 1, "80%", false);
        threshold = 0.8;
        UpdateButtonsState();
        break;

        case 8:
        _10klab::screen::PrintScreen(0, 0, "Threshold", true);
        _10klab::screen::PrintScreen(0, 1, "90%", false);
        threshold = 0.9;
        UpdateButtonsState();
        break;
    }
      page++;
      if(page > 8){
        page = 0;
      }
    }
    if (button_select.getSingleDebouncedPress()) {
      Serial.println("selected: " + String(page) + " Threshold = " + String(threshold));
      _10klab::screen::PrintScreen(0, 0, "Threshold", true);
      _10klab::screen::PrintScreen(0, 1, "Selected: " + String(int(threshold*100)) + "%", false);
      delay(2000);
      selected = true;
    }

  }
  return threshold;
}

void PumpsCaracterizationMode() {
  _10klab::tcp_client::PumpParameters IncomingParameters;
  const int error_data = 99;
  const int max_recipient_capacity = 5;
  const int prime_amount = 500;
  float prime_measure = 0;
  const int unloading_pump_id = 0;
  // float acumulative_amount = 0;
  float measure = 0;

  IncomingParameters.pumpId = error_data;
  IncomingParameters.processFinished = false;

  float threshold = SelectThreshold();

  _10klab::screen::PrintScreen(0, 0, "Connecting...", true);
  _10klab::udp_client::UDPInitializer();
  String server_ip = _10klab::udp_client::InitialConnection();
  _10klab::screen::PrintScreen(0, 0, "Connected!", true);
  delay(1000);

  while (!IncomingParameters.processFinished) {
    IncomingParameters = _10klab::tcp_client::IncomingParameters(server_ip);
    Serial.println(IncomingParameters.pumpId);

    if (IncomingParameters.processFinished) {
      _10klab::tcp_client::SendAnswer(server_ip, true, 0);
    }

    if (IncomingParameters.pumpId != error_data &&
        !IncomingParameters.processFinished) {
      Serial.println("data arrived, start process");
      _10klab::screen::PrintScreen(
          0, 0,
          "Pump:" + String(IncomingParameters.pumpId + 1) + " " +
              String(IncomingParameters.pump_progress) + "%",
          true);
      _10klab::screen::PrintScreen(
          0, 1, "Overall:" + String(IncomingParameters.test_progress) + "%",
          false);

      ////////////////////////////////UNLOADING////////////////////

      if (measure >= max_recipient_capacity) {
        Serial.println("unloading start");

        float unloading_measure = max_recipient_capacity * 2;
        _10klab::pumps::SinglePumpActivation(unloading_pump_id);
        // delay(1000);
        // Serial.println("RETIRAR");
        while (unloading_measure > max_recipient_capacity) {
          unloading_measure = _10klab::scale::GetUnits(10);
          Serial.println("unloading measure = " + String(unloading_measure));
          delay(300);
        }
        // delay(2000);
        _10klab::pumps::SinglePumpDeactivation(unloading_pump_id);
        Serial.println("unloading end");
      }

      ///////////////////////////// PRIME///////////////////

      if (IncomingParameters.prime_pump) {
        Serial.println("prime startr");
        prime_measure = 0;
        // _10klab::scale::Tare();
        // delay(500);
        float stable_measure = 99;
        while (stable_measure > 0.3 || stable_measure < -0.3) {
          _10klab::scale::Tare();
          stable_measure = _10klab::scale::StableMeasure(false);
          delay(300);
          Serial.println("on w8");
        }

        bool half_prime = false;
        _10klab::pumps::SinglePumpActivation(IncomingParameters.pumpId);
        while (prime_measure < prime_amount) {
          prime_measure = _10klab::scale::GetUnits(10);
          Serial.println("on prime");
          delay(300);
          if(prime_measure >= (prime_amount/2) && !half_prime){
            _10klab::pumps::SinglePumpDeactivation(IncomingParameters.pumpId);
            delay(1500);
            _10klab::pumps::SinglePumpActivation(IncomingParameters.pumpId);
            half_prime = true;
          }
        }

        _10klab::pumps::SinglePumpDeactivation(IncomingParameters.pumpId);
        // delay(2000);
        // prime_measure = _10klab::scale::GetUnits(10);
        prime_measure = _10klab::scale::StableMeasure(false);

        //////////////////////////////
        Serial.println("Prime measure before unloading = " +
                       String(prime_measure));
        if (prime_measure >= max_recipient_capacity) {
          Serial.println("unloading prime startr");

          float unloading_measure = max_recipient_capacity * 2;
          _10klab::pumps::SinglePumpActivation(unloading_pump_id);
          unsigned long timeout = millis();
          const int max_time = 15000;

          while (unloading_measure > max_recipient_capacity) {
            unloading_measure = _10klab::scale::GetUnits(10);
            Serial.println("unloading prime measure = " +
                           String(unloading_measure));
            if(millis() > timeout + max_time) {
              _10klab::pumps::SinglePumpDeactivation(unloading_pump_id);
              _10klab::scale::Tare();

            }
            delay(300);
            if(unloading_measure <= max_recipient_capacity){
              Serial.println("on check");
              _10klab::pumps::SinglePumpDeactivation(unloading_pump_id);
              unloading_measure = _10klab::scale::StableMeasure(false);
              
              if(unloading_measure > max_recipient_capacity){
                _10klab::pumps::SinglePumpActivation(unloading_pump_id);
              }
            }
          }

          Serial.println("unloading prime end");

        }
        Serial.println("prime end");
      }

      bool verification = false;
      while (!verification) {
        delay(500);

        
          float stable_measure = 99;
          while (stable_measure > 0.3 || stable_measure < -0.3) {
            _10klab::scale::Tare();
            stable_measure = _10klab::scale::StableMeasure(false);
            if(stable_measure < 0.3 && stable_measure > -0.3){
              delay(1000);
              stable_measure = _10klab::scale::StableMeasure(false);
            }
          }
        
        _10klab::tcp_client::SendAnswer(server_ip, false, 0);
        _10klab::pumps::PriorityOrder(
            IncomingParameters.pumpId, IncomingParameters.pulses,
            IncomingParameters.priority, IncomingParameters.rotation,
            IncomingParameters.ka, IncomingParameters.kb, 98, 98, 98, 0, 1, 0,
            98, 98, 98, 0, 1, 0, 98, 98, 98, 0, 1, 0, 98, 98, 98, 0, 1, 0, 98,
            98, 98, 0, 1, 0);

        // delay(5000);
        // measure = _10klab::scale::StableMeasure(true);
        measure = _10klab::scale::StableMeasure2(IncomingParameters.pulses, threshold);
        if (measure == -1) {
          _10klab::pumps::SinglePumpActivation(unloading_pump_id);
          int unload_delay = IncomingParameters.pulses * 1.5;
          delay(unload_delay);
          _10klab::pumps::SinglePumpDeactivation(unloading_pump_id);
          delay(3000);
        }
        if (measure != -1) {
          verification = true;
        }
      }

      Serial.println("measure: " + String(measure));
      IncomingParameters.pumpId = error_data;

      _10klab::tcp_client::SendAnswer(server_ip, true, measure);
    }
  }

  if (measure >= max_recipient_capacity) {
    Serial.println("End unloading start");

    float unloading_measure = max_recipient_capacity * 2;
    _10klab::pumps::SinglePumpActivation(unloading_pump_id);
    while (unloading_measure > max_recipient_capacity) {
      unloading_measure = _10klab::scale::GetUnits(10);
      Serial.println("unloading measure = " + String(unloading_measure));
      delay(300);
    }
    _10klab::pumps::SinglePumpDeactivation(unloading_pump_id);
    Serial.println("End unloading end");
  }

  _10klab::screen::PrintScreen(0, 0, "Caracterization", true);
  _10klab::screen::PrintScreen(0, 1, "completed!", false);
  Serial.println("exit");
  delay(2000);
}

void UpdateButtonsState() {
  delay(500);
  button_change.getSingleDebouncedPress();
  button_select.getSingleDebouncedPress();
}

// void PumpsCaracterizationMode()
// {
//   _10klab::tcp_client::PumpParameters IncomingParameters;
//   const int error_data = 99;
//   const int max_recipient_capacity = 20;
//   const int prime_amount = 50;
//   float prime_measure = 0;
//   const int unloading_pump_id = 0;
//   float acumulative_amount = 0;

//   IncomingParameters.pumpId = error_data;
//   IncomingParameters.processFinished = false;
//   _10klab::screen::PrintScreen(0, 0, "Connecting...", true);
//   _10klab::udp_client::UDPInitializer();
//   String server_ip = _10klab::udp_client::InitialConnection();
//   _10klab::screen::PrintScreen(0, 0, "Connected!", true);
//   delay(1000);

//   while (!IncomingParameters.processFinished)
//   {
//     IncomingParameters = _10klab::tcp_client::IncomingParameters(server_ip);
//     Serial.println(IncomingParameters.pumpId);

//     if (IncomingParameters.processFinished)
//     {
//       _10klab::tcp_client::SendAnswer(server_ip, true, 0);
//     }

//     if (IncomingParameters.pumpId != error_data &&
//         !IncomingParameters.processFinished)
//     {
//       Serial.println("data arrived, start process");
//       _10klab::screen::PrintScreen(
//           0, 0,
//           "Pump:" + String(IncomingParameters.pumpId + 1) + " " +
//               String(IncomingParameters.pump_progress) + "%",
//           true);
//       _10klab::screen::PrintScreen(
//           0, 1, "Overall:" + String(IncomingParameters.test_progress) + "%",
//           false);

//       Serial.println("acumulated = " + String(acumulative_amount));

//       if (acumulative_amount >= max_recipient_capacity)
//       {

//         float unloading_measure = max_recipient_capacity * 2;
//         _10klab::pumps::SinglePumpActivation(unloading_pump_id);
//         while (unloading_measure > max_recipient_capacity)
//         {
//           unloading_measure = _10klab::scale::GetUnits(10);
//           Serial.println("unloading measure = " + String(unloading_measure));
//           delay(300);
//         }
//         _10klab::pumps::SinglePumpDeactivation(unloading_pump_id);
//         prime_measure = 0;
//         delay(500);
//         acumulative_amount = _10klab::scale::GetUnits(10);
//         while(acumulative_amount < 0){
//           acumulative_amount = _10klab::scale::GetUnits(10);
//         }
//         delay(500);
//       }
//       Serial.println("acumulated after unload = " +
//       String(acumulative_amount));

//       if (IncomingParameters.prime_pump)
//       {
//         prime_measure = 0;
//         _10klab::scale::Tare();
//         delay(500);
//         _10klab::pumps::SinglePumpActivation(IncomingParameters.pumpId);
//         while (prime_measure < prime_amount)
//         {
//           prime_measure = _10klab::scale::GetUnits(10);
//           delay(300);
//         }
//         delay(500);
//         prime_measure = _10klab::scale::GetUnits(10);
//         _10klab::pumps::SinglePumpDeactivation(IncomingParameters.pumpId);
//         acumulative_amount = acumulative_amount + prime_measure;
//       }

//       // Serial.println("acumulated = " + String(acumulative_amount));

//       // if (acumulative_amount >= max_recipient_capacity)
//       // {
//       //   Serial.println("unloading!!");
//       //   float unloading_measure = max_recipient_capacity * 2;
//       //   _10klab::pumps::SinglePumpActivation(unloading_pump_id);
//       //   while (unloading_measure > max_recipient_capacity)
//       //   {
//       //     unloading_measure = _10klab::scale::GetUnits(10);
//       //     Serial.println("unloading measure = " +
//       String(unloading_measure));
//       //     delay(300);
//       //   }
//       //   _10klab::pumps::SinglePumpDeactivation(unloading_pump_id);
//       //   prime_measure = 0;
//       //   delay(300);
//       //   acumulative_amount = _10klab::scale::GetUnits(10);
//       // }

//       _10klab::scale::Tare();
//       // delay(2000);
//       _10klab::pumps::PriorityOrder(
//           IncomingParameters.pumpId, IncomingParameters.pulses,
//           IncomingParameters.priority, IncomingParameters.rotation,
//           IncomingParameters.ka, IncomingParameters.kb, 98, 98, 98, 0, 1, 0,
//           98, 98, 98, 0, 1, 0, 98, 98, 98, 0, 1, 0, 98, 98, 98, 0, 1, 0, 98,
//           98, 98, 0, 1, 0);

//       delay(1000);
//       float measure = _10klab::scale::StableMeasure();
//       acumulative_amount = acumulative_amount + measure;
//       Serial.println("measure: " + String(measure));
//       IncomingParameters.pumpId = error_data;

//       _10klab::tcp_client::SendAnswer(server_ip, true, measure);
//     }
//   }

//   // if (acumulative_amount >= max_recipient_capacity)
//   // {
//   //   Serial.println("unloading!!");
//   //   float unloading_measure = max_recipient_capacity * 2;
//   //   _10klab::pumps::SinglePumpActivation(unloading_pump_id);
//   //   while (unloading_measure > max_recipient_capacity)
//   //   {
//   //     unloading_measure = _10klab::scale::GetUnits(10);
//   //     Serial.println("unloading measure = " + String(unloading_measure));
//   //     delay(300);
//   //   }
//   //   _10klab::pumps::SinglePumpDeactivation(unloading_pump_id);
//   //   prime_measure = 0;
//   //   delay(500);
//   //   acumulative_amount = _10klab::scale::GetUnits(10);
//   //   while(acumulative_amount < 0){
//   //     acumulative_amount = _10klab::scale::GetUnits(10);
//   //   }
//   // }
//   _10klab::screen::PrintScreen(0, 0, "Caracterization", true);
//   _10klab::screen::PrintScreen(0, 1, "completed!", false);
//   Serial.println("exit");
//   delay(2000);
// }