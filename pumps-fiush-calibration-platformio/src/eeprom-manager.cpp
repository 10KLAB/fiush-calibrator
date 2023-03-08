#include <Arduino.h>
#include "eeprom-manager.h"
#include <EEPROM.h>

#define EEPROM_SIZE 8

namespace _10klab {
namespace eeprom {

void SetupEEPROM() { 
    // sdf
    // sdfg
    EEPROM.begin(EEPROM_SIZE);
}

struct MemoryCoefficients ReadCoefficients() {
    Serial.println("en Readcoefficients()");
  const int ka_integer_address = 0;
  const int ka_fractional_address = 1;
  const int kb_integer_address = 2;
  const int kb_fractional_address = 3;
  const int float_to_integer_factor = 100;

  float ka_integer = EEPROM.read(ka_integer_address);
  float ka_fraction = EEPROM.read(ka_fractional_address);
  float ka_value = ka_integer + ka_fraction / float_to_integer_factor;

  float kb_integer = EEPROM.read(kb_integer_address);
  float kb_fraction = EEPROM.read(kb_fractional_address);
  float kb_value = kb_integer + kb_fraction / float_to_integer_factor;

  Serial.println(ka_value);
  Serial.println(kb_value);

  return {.ka = ka_value, .kb = kb_value};
}

void SaveCoefficients(float ka, float kb) {

  const int ka_integer_address = 0;
  const int ka_fractional_address = 1;
  const int kb_integer_address = 2;
  const int kb_fractional_address = 3;
  const int float_to_integer_factor = 100;

  int integer_ka = ka;
  int fractional_ka = ((ka + 0.01) - integer_ka) * float_to_integer_factor;

  int integer_kb = kb;
  int fractional_kb = ((kb + 0.01) - integer_kb) * float_to_integer_factor;

  Serial.println("integer ka= " + String(integer_ka) +
                 " fraction ka: " + String(fractional_ka));
  Serial.println("integer kb= " + String(integer_kb) +
                 "fraction kb: " + String(fractional_kb));

  EEPROM.write(ka_integer_address, integer_ka);
  EEPROM.write(ka_fractional_address, fractional_ka);

  EEPROM.write(kb_integer_address, integer_kb);
  EEPROM.write(kb_fractional_address, fractional_kb);

  EEPROM.commit();


}

} // namespace eeprom
} // namespace _10klab