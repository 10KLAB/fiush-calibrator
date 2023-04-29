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

void EraseCredentials(){
  wifiManager.resetSettings();
  delay(300);
  ESP.restart();
}

void Disconnect(){
  wifiManager.disconnect();
}

void ReconnectWifi(){
  static String saved_SSID = wifiManager.getWiFiSSID();
  static String saved_PASS = wifiManager.getWiFiPass();
  const char* SSID = saved_SSID.c_str();
  const char* PASS = saved_PASS.c_str();
  Serial.println("on reconnect... SSID: " + String(SSID) + " Pass: " + String(PASS));

  if(WiFi.status() != WL_CONNECTED){
    Serial.println("disconnected");
  WiFi.begin(SSID, PASS);
  }
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
    timeout++;
    if(timeout >= 10){
      wifiManager.disconnect();
      delay(3000);
      WiFi.begin(SSID, PASS);
      timeout = 0;
    }

  }
}

void ConenctWifi() {
  static bool connection_message_flag = false;
  MDNS.begin("fiush-calibrator");
  //holi

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
    MDNS.begin("fiush-calibrator");
    connection_message_flag = true;
  }
    ReconnectWifi();
}



String myIp(){
  String ip = WiFi.localIP().toString();
  return ip;
}


} // namespace connection_manager
} // namespace _10klab