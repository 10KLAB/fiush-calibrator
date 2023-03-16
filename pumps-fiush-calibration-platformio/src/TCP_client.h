#include <Arduino.h>
#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_

namespace _10klab {
namespace tcp_client {
// void SendJSON();
void SendAnswer(String answer, bool step_finished, float grams);
void IncommingData(String answer);

struct PumpParameters {
    int pumpId;
    int priority;
    bool rotation;
    unsigned long pulses;
    float ka;
    float kb;
    bool prime_pump;
    bool processFinished;
    int pump_progress;
    int test_progress;
};
struct PumpParameters IncomingParameters(String server_ip);

} // namespace tcp_client
} // namespace _10klab

#endif
