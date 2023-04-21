#include "connection_manager.h"
#include <Arduino.h>
#include <WiFiManager.h>
#include <ESPmDNS.h>
#include <WiFi.h>



#define RESET_CREDENTIALS_PIN 22

WiFiManager wifiManager;

namespace _10klab {
namespace connection_manager {
void wifiPinDefinition() { pinMode(RESET_CREDENTIALS_PIN, INPUT); }

void ReconnectWifi(){
  static String saved_SSID = wifiManager.getWiFiSSID();
  static String saved_PASS = wifiManager.getWiFiPass();
  const char* SSID = saved_SSID.c_str();
  const char* PASS = saved_PASS.c_str();
  Serial.println("on reconnect... SSID: " + String(SSID) + " Pass: " + String(PASS));
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
}

void ConenctWifi() {
  static bool connection_message_flag = false;

  if (!wifiManager.autoConnect("fiush-calibrator", "fiush12345")) {
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
    ReconnectWifi();
}



String myIp(){
  String ip = WiFi.localIP().toString();
  return ip;
}
void EraseCredentials(){
  wifiManager.resetSettings();
  delay(300);
  ESP.restart();
}

} // namespace connection_manager
} // namespace _10klab