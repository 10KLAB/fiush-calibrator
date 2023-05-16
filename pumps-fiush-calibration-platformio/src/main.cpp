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
const String version = "1.0.0";

Pushbutton button_change(BUTTON_CHANGE);
Pushbutton button_select(BUTTON_SELECT);

String host_ip = "";

void UpdateButtonsState();
void CalibrationMode();
void ManualMode();
void PumpsCaracterizationMode();
void SelectionMenu();
void DispensationTimeout(String message_top, String message_bottom);

void setup() {
  Serial.begin(115200);
  _10klab::scale::SetUpScale();
  delay(100);
  Serial.println("hi");
  _10klab::pumps::PumpsInitialization();
  Serial.println("hi2");
  _10klab::screen::ScreenSetup();
  delay(1000);

  _10klab::screen::PrintScreen(0, 0, "Fiush", true);
  _10klab::screen::PrintScreen(0, 1, "Calibrator" + version, false);
  _10klab::screen::PrintScreen(0, 0, "Wifi setup", true);
  // DispensationTimeout();
  delay(2000);
  _10klab::connection_manager::ConenctWifi();

  _10klab::screen::PrintScreen(0, 0, "Wifi connected!", true);
  delay(1000);

  Serial.println("started");

  SelectionMenu();
  // PumpsCaracterizationMode();
}

void loop() {
  // delay(2000);

  delay(1000);
}

void SelectionMenu() {
  // This function is a menu that allows the user to select from four different
  // options:
  //   * Pump characterization mode
  //   * Manual mode
  //   * Calibration mode
  //   * Clear WiFi credentials

  int page = 0;
  static bool selected = false;
  static bool skip_one_screen = true;
  static bool start_image = true;

  _10klab::screen::PrintScreen(0, 0, "Pump", true);
  _10klab::screen::PrintScreen(0, 1, "Caracterization", false);
  // page = 1;
  // delay(500);

  Serial.println("Page 0");
  // While the user has not selected an option:
  while (true) {
    if (skip_one_screen) {
      page++;
      skip_one_screen = false;
    }

    // If the user presses the change button or the start image flag is true:
    if (button_change.getSingleDebouncedPress() || start_image) {
      start_image = false;
      if (page > 3) {
        page = 0;
        Serial.println("Page: " + String(page));
      }
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

      // if (page > 4) {
      //   page = 0;
      //   Serial.println("Page: " + String(page));
      // }
    }

    // If the user presses the select button:
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
        _10klab::screen::PrintScreen(0, 0, "Erasing", true);
        _10klab::screen::PrintScreen(0, 1, "Credentials", false);
        delay(2000);
        _10klab::connection_manager::EraseCredentials();

      default:
        Serial.println("menu out of range");
        _10klab::screen::PrintScreen(0, 0, "Menu", true);
        _10klab::screen::PrintScreen(0, 1, "Out range", false);
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
  // This function calibrates the scale by measuring the weight of known weights and then fitting a linear equation to the data.

  // Initialize the arrays to store the measured weights and the known weights.
  float data_x[12] = {0};
  float data_y[12] = {0, 1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 1200};
  // float data_y[4] = {0, 1, 2, 5};
  // float data_x[4] = {0};
  // float data_y[4] = {0, 1, 50, 1200};

  // Get the length of the arrays.
  const int data_length = sizeof(data_y) / sizeof(float);
  int step_counter = 1;

  Serial.println("retirar objetos y presionar boton");

  // Print a message to the screen asking the user to remove all objects from the scale.
  _10klab::screen::PrintScreen(0, 0, "Remove Items", true);
  _10klab::screen::PrintScreen(0, 1, "and press Select", false);

  _10klab::linealization::Coefficients GetCoefficients;

  UpdateButtonsState();
  // Wait for the user to press the select button.
  WaitingForButtonSelect();

  _10klab::screen::PrintScreen(0, 1, "...", true);
  Serial.println("tarando");
  UpdateButtonsState();

  // _10klab::scale::Tare();

  // Tare the scale.
  _10klab::scale::CalibratorMeasure(true);
  // delay(500);
  data_x[0] = _10klab::scale::CalibratorMeasure(false);
  // _10klab::scale::Tare();

  // Tare the scale.
  _10klab::scale::CalibratorMeasure(true);
  // Print a message to the screen indicating the weight of the first known weight.
  Serial.println("peso 0: " + String(data_x[0]));
  Serial.println("poner peso 1 y presionar boton");

  _10klab::screen::PrintScreen(
      0, 0, "Put weight:" + String(int(data_y[1])) + "g", true);
  _10klab::screen::PrintScreen(0, 1, "and press Select", false);

  // While the user has not placed all of the known weights on the scale:
  while (step_counter < data_length) {
    // If the user presses the select button:
    if (button_select.getSingleDebouncedPress()) {
      // delay(300);
      // data_x[step_counter] = _10klab::scale::GetRaw(10);
      _10klab::screen::PrintScreen(0, 1, "...", true);
      // Get the weight of the current known weight.
      data_x[step_counter] = _10klab::scale::CalibratorMeasure(false);

      // Print a message to the screen asking the user to remove the current known weight from the scale.
      Serial.println("retire el peso y presione el boton");
      _10klab::screen::PrintScreen(0, 0, "Remove Items", true);
      _10klab::screen::PrintScreen(0, 1, "and press Select", false);
      WaitingForButtonSelect();
      _10klab::screen::PrintScreen(0, 1, "...", true);
      Serial.println("peso: " + String(data_x[step_counter]));
      // delay(250);
      // _10klab::scale::Tare();
      _10klab::scale::CalibratorMeasure(true);
      // delay(250);
      step_counter++;

      // Print a message to the screen asking the user to place the next known weight on the scale.
      Serial.println("poner peso #: " + String(step_counter));
      _10klab::screen::PrintScreen(
          0, 0, "Put weight:" + String(int(data_y[step_counter])) + "g", true);
      _10klab::screen::PrintScreen(0, 1, "and press Select", false);
    }

    // If the user presses the change button:
    if (button_change.getSingleDebouncedPress()) {
      // Print a message to the screen indicating that the calibration was aborted.
      _10klab::screen::PrintScreen(0, 0, "Calibration", true);
      _10klab::screen::PrintScreen(0, 1, "aborted", false);
      delay(2000);
      return;
    }
  }

  //calculate the coefficients with the actual data
  GetCoefficients =
      _10klab::linealization::GetCoefficients(data_x, data_y, data_length);

  float ka = GetCoefficients.ka;
  float kb = GetCoefficients.kb;

  //Save the coefficients in to EEPROM

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
  // This function enters manual mode, where the user can see the current weight of the object on the scale.

  // Tare the scale.
  _10klab::scale::Tare();
  unsigned long current_time = 0;
  const int refresh_time = 500;
  UpdateButtonsState();

  // While the user has not pressed the select button:
  while (!button_select.getSingleDebouncedPress()) {
    // If the current time is greater than or equal to the refresh time:
    if (millis() >= current_time + refresh_time) {
      // Get the weight of the object on the scale.
      float value = _10klab::scale::GetUnits(10);
      // Print the weight of the object on the scale to the screen.
      _10klab::screen::PrintScreen(1, 0, String(value) + " grams", true);

      Serial.print("peso= " + String(value));
      current_time = millis();
    }
  }
  // Print a message to the screen indicating that the user is leaving manual mode.
  _10klab::screen::PrintScreen(0, 0, "Leaving Manual", true);
  _10klab::screen::PrintScreen(0, 1, "mode", false);
  Serial.println("exit");
  delay(2000);
}

float SelectThreshold() {
  bool selected = false;
  float threshold = 0.1;
  int page = 0;

  _10klab::screen::PrintScreen(0, 0, "Select", true);
  _10klab::screen::PrintScreen(0, 1, "Threshold", false);
  delay(2000);
  _10klab::screen::PrintScreen(0, 0, "Threshold", true);
  _10klab::screen::PrintScreen(0, 1, "10%", false);
  page = 1;

  while (!selected) {
    if (button_change.getSingleDebouncedPress()) {
      switch (page) {
      case 0:
        _10klab::screen::PrintScreen(0, 0, "Threshold", true);
        _10klab::screen::PrintScreen(0, 1, "10%", false);
        threshold = 0.1;
        UpdateButtonsState();
        break;

      case 1:
        _10klab::screen::PrintScreen(0, 0, "Threshold", true);
        _10klab::screen::PrintScreen(0, 1, "12%", false);
        threshold = 0.12;
        UpdateButtonsState();
        break;

      case 2:
        _10klab::screen::PrintScreen(0, 0, "Threshold", true);
        _10klab::screen::PrintScreen(0, 1, "14%", false);
        threshold = 0.14;
        UpdateButtonsState();
        break;

      case 3:
        _10klab::screen::PrintScreen(0, 0, "Threshold", true);
        _10klab::screen::PrintScreen(0, 1, "16%", false);
        threshold = 0.16;
        UpdateButtonsState();
        break;

      case 4:
        _10klab::screen::PrintScreen(0, 0, "Threshold", true);
        _10klab::screen::PrintScreen(0, 1, "18%", false);
        threshold = 0.18;
        UpdateButtonsState();
        break;

      case 5:
        _10klab::screen::PrintScreen(0, 0, "Threshold", true);
        _10klab::screen::PrintScreen(0, 1, "20%", false);
        threshold = 0.2;
        UpdateButtonsState();
        break;

      case 6:
        _10klab::screen::PrintScreen(0, 0, "Threshold", true);
        _10klab::screen::PrintScreen(0, 1, "22%", false);
        threshold = 0.22;
        UpdateButtonsState();
        break;

      case 7:
        _10klab::screen::PrintScreen(0, 0, "Threshold", true);
        _10klab::screen::PrintScreen(0, 1, "24%", false);
        threshold = 0.24;
        UpdateButtonsState();
        break;

      case 8:
        _10klab::screen::PrintScreen(0, 0, "Threshold", true);
        _10klab::screen::PrintScreen(0, 1, "26%", false);
        threshold = 0.26;
        UpdateButtonsState();
        break;
      }
      page++;
      if (page > 8) {
        page = 0;
      }
    }
    if (button_select.getSingleDebouncedPress()) {
      Serial.println("selected: " + String(page) +
                     " Threshold = " + String(threshold));
      _10klab::screen::PrintScreen(0, 0, "Threshold", true);
      _10klab::screen::PrintScreen(
          0, 1, "Selected: " + String(int(threshold * 100)) + "%", false);
      delay(2000);
      selected = true;
    }
  }
  return threshold;
}

void DispensationTimeout(String message_top, String message_bottom) {
  const int alarm_output = 11;

  _10klab::screen::PrintScreen(0, 0, message_top, true);
  _10klab::screen::PrintScreen(0, 1, message_bottom, false);

  UpdateButtonsState();
  _10klab::pumps::DispensationAlarm(alarm_output, true);
  Serial.println("alarma dispensado");
  while (!button_select.getSingleDebouncedPress()) {
    _10klab::pumps::DispensationAlarm(alarm_output, false);
    delay(50);
  }
  Serial.println("alarma dispensado end");

  _10klab::screen::PrintScreen(0, 0, "Ready", true);
  // _10klab::screen::PrintScreen(0, 1, "connection", false);
  delay(2000);

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

  // Print a message to the screen indicating that the device is connecting to the server.
  _10klab::screen::PrintScreen(0, 0, "Connecting...", true);

  // Initialize the UDP client.
  _10klab::udp_client::UDPInitializer();
  // Get the IP address of the server.
  String server_ip = _10klab::udp_client::InitialConnection();
  _10klab::screen::PrintScreen(0, 0, "Connected!", true);
  delay(1000);

  // While the server is not finished processing the request:
  while (!IncomingParameters.processFinished) {
    // Get the current parameters from the server.
    IncomingParameters = _10klab::tcp_client::IncomingParameters(server_ip);
    Serial.println(IncomingParameters.pumpId);

    // If the server is finished processing the request:
    if (IncomingParameters.processFinished) {
      _10klab::tcp_client::SendAnswer(server_ip, true, 0);
    }

    // If the pump ID is not -1 and the server is not finished processing the request:
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
      // If the measured weight is greater than or equal to the maximum capacity of the recipient:
      if (measure >= max_recipient_capacity) {
        Serial.println("unloading start");

        float unloading_measure = max_recipient_capacity * 2;
        // Set the pump to the unloading mode.
        _10klab::pumps::SinglePumpActivation(unloading_pump_id);
        // delay(1000);
        // Serial.println("RETIRAR");
        unsigned long timeout = millis();
        int timeout_time = 35000;
        while ((unloading_measure > max_recipient_capacity) ||
               (millis() > timeout + timeout_time)) {
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
        unsigned long current_time_prime = millis();
        const int timeout_prime = 90000;
        while (prime_measure < prime_amount) {
          if(millis() >= current_time_prime + timeout_prime) {
            _10klab::pumps::SinglePumpDeactivation(IncomingParameters.pumpId);
            DispensationTimeout("Check pump: "+String(IncomingParameters.pumpId+1), "connection");
            while(prime_measure > max_recipient_capacity*5){
              prime_measure = _10klab::scale::GetUnits(10);
              _10klab::pumps::SinglePumpActivation(unloading_pump_id);
            }
            _10klab::pumps::SinglePumpDeactivation(unloading_pump_id);
            _10klab::pumps::SinglePumpActivation(IncomingParameters.pumpId);

            current_time_prime = millis();
          }
          prime_measure = _10klab::scale::GetUnits(10);
          Serial.println("on prime");
          delay(300);
          if (prime_measure >= (prime_amount / 2) && !half_prime) {
            _10klab::pumps::SinglePumpDeactivation(IncomingParameters.pumpId);
            delay(500);
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
        // If the measured weight is greater than or equal to the maximum capacity of the recipient:
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
            if (millis() > timeout + max_time) {
              _10klab::pumps::SinglePumpDeactivation(unloading_pump_id);
              _10klab::scale::Tare();
            }
            delay(300);
            if (unloading_measure <= max_recipient_capacity) {
              Serial.println("on check");
              _10klab::pumps::SinglePumpDeactivation(unloading_pump_id);
              unloading_measure = _10klab::scale::StableMeasure(false);

              if (unloading_measure > max_recipient_capacity) {
                _10klab::pumps::SinglePumpActivation(unloading_pump_id);
              }
            }
          }

          Serial.println("unloading prime end");
        }
        delay(1000);
        Serial.println("prime end");
      }

      bool verification = false;
      int verification_cicle_counter = 0;
      const int verification_cicles = 2;
      const int alarm_output = 11;
      bool pump_dispensation_alert = false;

      // While the verification has not been successful:
      while (!verification) {
        delay(500);

        float stable_measure = 99;
        // While the weight is not stable:
        while (stable_measure > 0.3 || stable_measure < -0.3) {
          // Reset the scale.
          _10klab::scale::Tare(); 
          stable_measure = _10klab::scale::StableMeasure(false);
          if (stable_measure < 0.3 && stable_measure > -0.3) {
            delay(1000);
            stable_measure = _10klab::scale::StableMeasure(false);
          }
        }

        _10klab::tcp_client::SendAnswer(server_ip, false, 0);
        pump_dispensation_alert = _10klab::pumps::PriorityOrder(
            IncomingParameters.pumpId, IncomingParameters.pulses,
            IncomingParameters.priority, IncomingParameters.rotation,
            IncomingParameters.ka, IncomingParameters.kb, 98, 98, 98, 0, 1, 0,
            98, 98, 98, 0, 1, 0, 98, 98, 98, 0, 1, 0, 98, 98, 98, 0, 1, 0, 98,
            98, 98, 0, 1, 0);

        if(pump_dispensation_alert){
          DispensationTimeout("Check: pump "+String(IncomingParameters.pumpId+1), "yellow cable");
          _10klab::pumps::SinglePumpActivation(unloading_pump_id);
          delay(20000);
          _10klab::pumps::SinglePumpDeactivation(unloading_pump_id);
          measure = -1;

        }
        else{
          measure = _10klab::scale::StableMeasure2(IncomingParameters.pulses,
                                                 threshold);
        }
        // delay(5000);
        // measure = _10klab::scale::StableMeasure(true);
        // measure = _10klab::scale::StableMeasure2(IncomingParameters.pulses,
        //                                          threshold);
        // If the weight is not equal to the desired amount:
        if (measure == -1) {
          _10klab::pumps::SinglePumpActivation(unloading_pump_id);
          // Delay for the amount of time it takes to unload the recipient.
          int unload_delay = IncomingParameters.pulses * 1.5;
          delay(unload_delay);
          _10klab::pumps::SinglePumpDeactivation(unloading_pump_id);
          delay(3000);
        }
        // If the weight is equal to the desired amount:
        if (measure != -1) {
          verification = true;
          // Send a message to the server with the measure
          _10klab::tcp_client::SendAnswer(server_ip, true, measure);
        }

        Serial.println("alarm counter = " + String(verification_cicle_counter));
        // If the number of verification cycles has been exceeded:
        if (verification_cicle_counter >= verification_cicles) {
          _10klab::pumps::AlarmActivation(alarm_output);
        }
        verification_cicle_counter++;
      }
      _10klab::pumps::AlarmDeactivation(alarm_output);
      // develop branch
      ////////////////////////////////////////////////////////////////////
      // for(int i = 0; i < 10; i++){
      //   _10klab::tcp_client::SendAnswer(server_ip, false, 0);
      //   delay(1000);
      // }

      // Serial.println("measure: " + String(measure));
      // IncomingParameters.pumpId = error_data;
      // static float test_data = 0;
      // static int timer = 500;
      // delay(timer);

      // _10klab::tcp_client::SendAnswer(server_ip, true, timer);
      // test_data++;
      // // timer+=500;

      // static bool disconnect = false;
      // static int time_disconnect = 1000;
      // if(disconnect==false){
      //   _10klab::connection_manager::Disconnect();
      //   // time_disconnect = timer + 500;
      //   // timer = 0;
      //   // delay(2000);
      //   // disconnect = true;
      // }
      ////////////////////////////////////////////////////////////
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
