#include <Arduino.h>
#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_

namespace _10klab {
namespace tcp_client {
// void SendJSON();
void SendAnswer(String answer);
void IncommingData(String answer);

struct PumpParameters {
    int pumpId;
    int priority;
    bool rotation;
    unsigned long pulses;
    float ka;
    float kb;
    bool processFinished;
};
struct PumpParameters IncomingParameters(String server_ip);

} // namespace tcp_client
} // namespace _10klab

#endif
