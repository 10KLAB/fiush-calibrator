#include "connection_manager.h"
#include <Arduino.h>
#include <WiFiManager.h>
#include <ESPmDNS.h>



#define RESET_CREDENTIALS_PIN 22

WiFiManager wifiManager;

namespace _10klab {
namespace connection_manager {
void wifiPinDefinition() { pinMode(RESET_CREDENTIALS_PIN, INPUT); }

void ConenctWifi() {
  static bool connection_message_flag = false;

  // if (!digitalRead(RESET_CREDENTIALS_PIN))
  // {
  //     Serial.println("Credentials Reset");
  //     wifiManager.resetSettings();
  // }
  // wifiManager.resetSettings();

  if (!wifiManager.autoConnect("WiFiManagerAP", "password")) {
    Serial.println("failed to connect and hit timeout");
    // reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }
  if (connection_message_flag == false) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    MDNS.begin("calibrator-scale");
    connection_message_flag = true;
  }
}

} // namespace connection_manager
} // namespace _10klab