#include <Arduino.h>
#ifndef LINEALIZATION_H_
#define LINEALIZATION_H_

namespace _10klab {
namespace linealization {

struct Coefficients {
  float ka;
  float kb;
};
struct Coefficients GetCoefficients(const float data_x[], const float data_y[],
                                    int length);

}  // namespace linealization
}  // namespace _10klab

#endif
