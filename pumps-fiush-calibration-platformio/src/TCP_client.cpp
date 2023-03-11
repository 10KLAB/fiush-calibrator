#include "TCP_client.h"
#include "ArduinoJson.h"
#include <Arduino.h>
#include <WiFi.h>

namespace _10klab {
namespace tcp_client {
WiFiClient client;
const uint16_t serverPort = 8000;

void SendAnswer(String answer) {

  const char *host_ip = answer.c_str();

    // Conéctate al servidor
    if (client.connect(host_ip, 8000)) {
      client.print("pump finished");
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
  const int error_data = 99;

  if (client.connect(host_ip, 8000)) {
    client.println("Data arrived");
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

      return {.pumpId = pumpId,
              .priority = priority,
              .rotation = rotation,
              .pulses = pulses,
              .ka = ka,
              .kb = kb,
              .processFinished = processFinished};
    }

  } else {
    Serial.println("Error al conectarse al servidor");
  }
  return {.pumpId = error_data};
}

void IncommingData(String answer) {
  const char *host_ip = answer.c_str();
  const uint8_t input_buffer_size = 100;
  uint8_t input_buffer[input_buffer_size] = {0};
  StaticJsonDocument<input_buffer_size> input_doc;

  if (client.connect(host_ip, 8000)) {
    // client.println("Hola servidor");
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
      }
      int pump = input_doc["pump"];
      int priority = input_doc["priority"];
      bool rotation = input_doc["rotation"];
      unsigned long pulses = input_doc["pulses"];
      float ka = input_doc["ka"];
      float kb = input_doc["kb"];
      Serial.println("pump: " + String(pump));
      Serial.println("priority: " + String(priority));
      Serial.println("rotation: " + String(rotation));
      Serial.println("pulses: " + String(pulses));
      Serial.println("ka: " + String(ka));
      Serial.println("kb: " + String(kb));
    }
  } else {
    Serial.println("Error al conectarse al servidor");
  }
  client.stop();
}

} // namespace tcp_client
} // namespace _10klab
