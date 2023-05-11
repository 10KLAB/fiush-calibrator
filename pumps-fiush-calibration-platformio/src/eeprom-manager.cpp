#include "eeprom-manager.h"
#include <Arduino.h>
#include <EEPROM.h>


#define EEPROM_SIZE 8

namespace _10klab {
namespace eeprom {

// This function initializes the EEPROM.
void SetupEEPROM() { EEPROM.begin(EEPROM_SIZE); }

// This function reads the coefficients from the EEPROM.
struct MemoryCoefficients ReadCoefficients() {
  Serial.println("en Readcoefficients()");

  // Get the addresses of the coefficients in the EEPROM.
  const int ka_integer_address = 0;
  const int ka_fractional_address = 1;
  const int kb_integer_address = 2;
  const int kb_fractional_address = 3;
  const int float_to_integer_factor = 100;

  // Read the integer and fractional parts of the coefficients from the EEPROM.
  float ka_integer = EEPROM.read(ka_integer_address);
  float ka_fraction = EEPROM.read(ka_fractional_address);
  float ka_value = ka_integer + ka_fraction / float_to_integer_factor;

  float kb_integer = EEPROM.read(kb_integer_address);
  float kb_fraction = EEPROM.read(kb_fractional_address);
  float kb_value = kb_integer + kb_fraction / float_to_integer_factor;

  Serial.println(ka_value);
  Serial.println(kb_value);

  // Return a struct containing the coefficients.
  return {.ka = ka_value, .kb = kb_value};
}

// This function saves the coefficients to the EEPROM.
void SaveCoefficients(float ka, float kb) {

  // Get the addresses of the coefficients in the EEPROM.
  const int ka_integer_address = 0;
  const int ka_fractional_address = 1;
  const int kb_integer_address = 2;
  const int kb_fractional_address = 3;
  const int float_to_integer_factor = 100;

  // Convert the coefficients to integers and fractions.
  int integer_ka = ka;
  int fractional_ka = ((ka + 0.006) - integer_ka) * float_to_integer_factor;

  int integer_kb = kb;
  int fractional_kb = ((kb + 0.03) - integer_kb) * float_to_integer_factor;

  Serial.println("integer ka= " + String(integer_ka) +
                 " fraction ka: " + String(fractional_ka));
  Serial.println("integer kb= " + String(integer_kb) +
                 "fraction kb: " + String(fractional_kb));

  // Write the coefficients to the EEPROM.
  EEPROM.write(ka_integer_address, integer_ka);
  EEPROM.write(ka_fractional_address, fractional_ka);

  EEPROM.write(kb_integer_address, integer_kb);
  EEPROM.write(kb_fractional_address, fractional_kb);
  
  // Commit the changes to the EEPROM.
  EEPROM.commit();
}

} // namespace eeprom
} // namespace _10klab