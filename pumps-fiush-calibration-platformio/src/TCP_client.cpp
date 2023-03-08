#include "ArduinoJson.h"
#include <Arduino.h>
#include <WiFi.h>

WiFiClient client;

const uint16_t serverPort = 8000;

namespace _10klab {
namespace tcp_client {

static unsigned int time_to_turn_on = 0;
static unsigned int delay_counter = 0;

void BlinkLedOnBoard(int delay_time, int blink_number);

void SendAnswer(String answer) {

  const char *host_ip = answer.c_str();

  while (true) {
    // Conéctate al servidor
    if (client.connect(host_ip, 8000)) {
      client.println("Hola servidor");
    } else {
      Serial.println("Error al conectarse al servidor");
    }

    // Cierra la conexión con el servidor
    client.stop();

    delay(1000);
  }
}
} // namespace tcp_client
} // namespace _10klab
