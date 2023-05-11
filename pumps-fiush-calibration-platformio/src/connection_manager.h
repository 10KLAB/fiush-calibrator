#include <Arduino.h>
#ifndef CONNECTION_MANAGER_H_
#define CONNECTION_MANAGER_H_

namespace _10klab {
namespace connection_manager {
// This function disconnects the ESP32 from the WiFi network.
void Disconnect();
// This function reconnects the ESP32 to the WiFi network.
void ReconnectWifi();
// This function erases the WiFi credentials from the ESP32.
void EraseCredentials();
// This function connects the ESP32 to the WiFi network.
void ConenctWifi();
// This function gets the local IP address of the ESP32.
String myIp();
} // namespace connection_manager
} // namespace _10klab

#endif
