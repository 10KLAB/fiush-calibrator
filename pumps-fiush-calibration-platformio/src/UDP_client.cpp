#include "ArduinoJson.h"
#include <Arduino.h>
#include <WiFiUdp.h>
#include "connection_manager.h"

const int udpPort = 8000;

WiFiUDP udp;

namespace _10klab {
namespace udp_client {

void UDPInitializer() { udp.begin(udpPort); }

String InitialConnection() {
  bool connection = false;
  String host_ip = "";

  while (!connection) {
    int packetSize = udp.parsePacket();

    if (packetSize) {
      Serial.printf("Recibido %d bytes desde %s, puerto %d\n", packetSize,
                    udp.remoteIP().toString().c_str(), udp.remotePort());

      // Leer el mensaje recibido
      char message[packetSize + 1];
      udp.read(message, packetSize);
      message[packetSize] = '\0';

      String input_message = message;
    
      Serial.println(input_message);
      // Enviar una respuesta al cliente

      // udp.print("Mensaje recibido correctamente");
      // udp.print(_10klab::connection_manager::myIp());

      if(input_message == "hi there"){
        udp.beginPacket(udp.remoteIP(), udp.remotePort());
        host_ip = udp.remoteIP().toString().c_str();
        udp.print("hi sup");
        udp.endPacket();
        Serial.println(host_ip);
        Serial.println("bye");
        connection = true;
      }
    }
  }
  return host_ip;
}

} // namespace udp_client
} // namespace _10klab