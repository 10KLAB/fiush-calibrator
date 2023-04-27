#include <Arduino.h>
#ifndef CONNECTION_MANAGER_H_
#define CONNECTION_MANAGER_H_

namespace _10klab {
namespace connection_manager {
void Disconnect();
void ReconnectWifi();
void EraseCredentials();
void ConenctWifi();
String myIp();
} // namespace connection_manager
} // namespace _10klab

#endif
