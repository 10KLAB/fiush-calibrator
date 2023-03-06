#include <Arduino.h>
#ifndef EEPROM_MANAGER_H_
#define EEPROM_MANAGER_H_

namespace _10klab {
namespace eeprom {

struct MemoryCoefficients {
  float ka;
  float kb;
};
struct MemoryCoefficients ReadCoefficients();

void SetupEEPROM();
void SaveCoefficients(float ka, float kb);

} // namespace eeprom
} // namespace _10klab

#endif
