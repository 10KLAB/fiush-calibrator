#include <Arduino.h>
#ifndef LINEALIZATION_H_
#define LINEALIZATION_H_

namespace _10klab {
namespace linealization {

struct Coefficients {
  float ka = 1;
  float kb = 0;
};
struct Coefficients GetCoefficients(const float data_x[], int length);

} // namespace linealization
} // namespace _10klab

#endif