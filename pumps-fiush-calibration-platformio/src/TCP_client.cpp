#include "TCP_client.h"
#include "ArduinoJson.h"
#include <Arduino.h>
#include <WiFi.h>

namespace _10klab {
namespace tcp_client {
WiFiClient client;
const uint16_t serverPort = 8000;

void SendAnswer(String answer, bool step_finished, float grams) {

  const char *host_ip = answer.c_str();

  const uint16_t output_buffer_size = 100;
  StaticJsonDocument<output_buffer_size> output_doc;
  output_doc["stepFinished"] = step_finished;
  output_doc["grams"] = grams;
  uint8_t output_json[output_buffer_size] = {0};
  serializeJson(output_doc, output_json);

  

    // Conéctate al servidor
    if (client.connect(host_ip, 8000)) {
      client.write(output_json, output_buffer_size);
    } else {
      Serial.println("Error al conectarse al servidor");
    }

    // Cierra la conexión con el servidor
    client.stop();

  
}

struct PumpParameters IncomingParameters(String server_ip) {
  const char *host_ip = server_ip.c_str();
  const uint16_t input_buffer_size = 500;
  uint8_t input_buffer[input_buffer_size] = {0};
  StaticJsonDocument<input_buffer_size> input_doc;

  const uint16_t output_buffer_size = 50;
  StaticJsonDocument<output_buffer_size> output_doc;
  output_doc["stepFinished"] = false;
  uint8_t output_json[output_buffer_size] = {0};
  serializeJson(output_doc, output_json);

  const int error_data = 99;

  if (client.connect(host_ip, 8000)) {
    client.write(output_json, output_buffer_size);
    delay(100);
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
      }
      client.stop();
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
    Serial.println("Error al conectarse al servidor");
  }
  return {.pumpId = error_data};
}

// void IncommingData(String answer) {
//   const char *host_ip = answer.c_str();
//   const uint8_t input_buffer_size = 100;
//   uint8_t input_buffer[input_buffer_size] = {0};
//   StaticJsonDocument<input_buffer_size> input_doc;

//   if (client.connect(host_ip, 8000)) {
//     // client.println("Hola servidor");
//     while (client.connected() || client.available()) {
//       memset(input_buffer, 0, input_buffer_size);

//       if (client.read(input_buffer, input_buffer_size) > 0) {
//         Serial.print("Server to client: ");
//         Serial.println((char *)input_buffer);
//       }
//       DeserializationError error = deserializeJson(input_doc, input_buffer);
//       if (error) {
//         Serial.print(F("deserializeJson() failed: "));
//         Serial.println(error.f_str());
//       }
//       int pump = input_doc["pump"];
//       int priority = input_doc["priority"];
//       bool rotation = input_doc["rotation"];
//       unsigned long pulses = input_doc["pulses"];
//       float ka = input_doc["ka"];
//       float kb = input_doc["kb"];
//       Serial.println("pump: " + String(pump));
//       Serial.println("priority: " + String(priority));
//       Serial.println("rotation: " + String(rotation));
//       Serial.println("pulses: " + String(pulses));
//       Serial.println("ka: " + String(ka));
//       Serial.println("kb: " + String(kb));
//     }
//   } else {
//     Serial.println("Error al conectarse al servidor");
//   }
//   client.stop();
// }

} // namespace tcp_client
} // namespace _10klab
