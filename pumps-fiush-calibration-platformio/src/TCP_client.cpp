#include "TCP_client.h"
#include "ArduinoJson.h"
#include <Arduino.h>
#include <WiFi.h>
#include "screen_lcd.h"
#include "connection_manager.h"

namespace _10klab {
namespace tcp_client {
WiFiClient client;
const uint16_t serverPort = 8000;

void SetTimeout(){
    static bool start_flag = true;
    if(start_flag){
      client.setTimeout(5);
      start_flag = false;
    }
}

void SendAnswer(String answer, bool step_finished, float grams) {
  SetTimeout();

  const char *host_ip = answer.c_str();

  const uint16_t output_buffer_size = 100;
  StaticJsonDocument<output_buffer_size> output_doc;
  output_doc["stepFinished"] = step_finished;
  output_doc["grams"] = grams;
  uint8_t output_json[output_buffer_size] = {0};
  serializeJson(output_doc, output_json);

  // Conéctate al servidor
  bool retry_send = false;
  while (!retry_send) {

    if (client.connect(host_ip, 8000)) {
      client.write(output_json, output_buffer_size);
      retry_send = true;
      client.stop();
    } else {
      _10klab::screen::PrintScreen(0, 0, "Connection", true);
      _10klab::screen::PrintScreen(0, 1, "problem", false);
      Serial.println("Error al conectarse al servidor sender");
      _10klab::connection_manager::ReconnectWifi();
    }
  }

  // Cierra la conexión con el servidor
}

struct PumpParameters IncomingParameters(String server_ip) {
  const char *host_ip = server_ip.c_str();
  const uint16_t input_buffer_size = 500;
  uint8_t input_buffer[input_buffer_size] = {0};
  StaticJsonDocument<input_buffer_size> input_doc;
  SetTimeout();


  const uint16_t output_buffer_size = 50;
  StaticJsonDocument<output_buffer_size> output_doc;
  output_doc["stepFinished"] = false;
  uint8_t output_json[output_buffer_size] = {0};
  serializeJson(output_doc, output_json);

  const int error_data = 99;

  bool retry_incoming = false;
  while (!retry_incoming) {


    if (client.connect(host_ip, 8000)) {
      client.write(output_json, output_buffer_size);
      delay(1000);
      while (client.connected() || client.available()) {
        memset(input_buffer, 0, input_buffer_size);

        if (client.read(input_buffer, input_buffer_size) > 0) {
          Serial.print("Server to client: ");
          Serial.println((char *)input_buffer);
        }
        DeserializationError error = deserializeJson(input_doc, input_buffer);
        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return {.pumpId = error_data};
        } else {
          client.stop();
          retry_incoming = true;
        }
        bool processFinished = input_doc["processFinished"];
        int pumpId = input_doc["pump"];
        int priority = input_doc["priority"];
        bool rotation = input_doc["rotation"];
        unsigned long pulses = input_doc["pulses"];
        float ka = input_doc["ka"];
        float kb = input_doc["kb"];
        bool prime_pump = input_doc["primePump"];
        int pump_progress = input_doc["pumpProgress"];
        int test_progress = input_doc["testProgress"];

        return {.pumpId = pumpId,
                .priority = priority,
                .rotation = rotation,
                .pulses = pulses,
                .ka = ka,
                .kb = kb,
                .prime_pump = prime_pump,
                .processFinished = processFinished,
                .pump_progress = pump_progress,
                .test_progress = test_progress};
      }

    } else {
      Serial.println("Error al conectarse al servidor incoming");
      _10klab::screen::PrintScreen(0, 0, "Connection", true);
      _10klab::screen::PrintScreen(0, 1, "problem", false);
      _10klab::connection_manager::ReconnectWifi();
    }
  }
  return {.pumpId = error_data};
}


} // namespace tcp_client
} // namespace _10klab
