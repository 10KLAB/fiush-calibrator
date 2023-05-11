#include "connection_manager.h"
#include <Arduino.h>
#include <WiFiManager.h>
#include <ESPmDNS.h>
#include <WiFi.h>



#define RESET_CREDENTIALS_PIN 22

WiFiManager wifiManager;

namespace _10klab {
namespace connection_manager {
// Define the wifi pin mode as input
void wifiPinDefinition() { pinMode(RESET_CREDENTIALS_PIN, INPUT); }

// Erase the saved wifi credentials and restart the device
void EraseCredentials(){
  wifiManager.resetSettings();
  delay(300);
  ESP.restart();
}

// Disconnect from the current wifi network
void Disconnect(){
  wifiManager.disconnect();
}

// Reconnect to the saved wifi network
void ReconnectWifi(){
  static String saved_SSID = wifiManager.getWiFiSSID();
  static String saved_PASS = wifiManager.getWiFiPass();
  const char* SSID = saved_SSID.c_str();
  const char* PASS = saved_PASS.c_str();
  Serial.println("on reconnect... SSID: " + String(SSID) + " Pass: " + String(PASS));

  // Check if the device is not already connected to a wifi network
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("disconnected");
    // Attempt to connect to the saved wifi network
    WiFi.begin(SSID, PASS);
  }
  // Try to connect to the wifi network and reconnect if necessary
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
    timeout++;
    if(timeout >= 10){
      // Disconnect and reconnect after a timeout period
      wifiManager.disconnect();
      delay(3000);
      WiFi.begin(SSID, PASS);
      timeout = 0;
    }

  }
}

// Function to connect to WiFi network
void ConenctWifi() {
  static bool connection_message_flag = false;
  // Set up mDNS service
  MDNS.begin("fiush-calibrator");

  // Attempt to connect to WiFi network
  if (!wifiManager.autoConnect("fiush-calibrator", "fiush12345")) {
    // If connection fails, restart ESP and try again
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


/*
 * This function gets the local IP address of the ESP32 and returns it as a string.
 */
String myIp(){
  String ip = WiFi.localIP().toString();
  return ip;
}


} // namespace connection_manager
} // namespace _10klab