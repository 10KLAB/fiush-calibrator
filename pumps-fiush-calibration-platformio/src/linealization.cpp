#include "linealization.h"

#include <Arduino.h>

namespace _10klab {
namespace linealization {

float CalculateA(const float data_x[], const float data_y[], const int length);
float CalculateB(const float data_x[], const float data_y[], const int length);
float CalculateC(const float data_x[], const int length);
float CalculateD(const float data_x[], const int length);
float CalculateM(const float a, const float b, const float c, const float d);
float CalculateE(const float data_y[], const int length);
float CalculateF(const float data_x[], const int length, const float m);
float CalculateI(const int length, const float e, const float f);

struct Coefficients GetCoefficients(const float data_x[], const float data_y[],
                                    const int length) {
  const int n = length;
  const float a = CalculateA(data_x, data_y, length);
  const float b = CalculateB(data_x, data_y, length);
  const float c = CalculateC(data_x, length);
  const float d = CalculateD(data_x, length);
  const float m = CalculateM(a, b, c, d);
  const float e = CalculateE(data_y, length);
  const float f = CalculateF(data_x, length, m);
  const float i = CalculateI(length, e, f);

  // y = m*x + i
  // Serial.println("n= " + String(n));
  // Serial.println("a= " + String(a));
  // Serial.println("b= " + String(b));
  // Serial.println("c= " + String(c));
  // Serial.println("d= " + String(d));
  // Serial.println("m= " + String(m));
  // Serial.println("e= " + String(e));
  // Serial.println("f= " + String(f));
  // Serial.println("i= " + String(i));

  return {.ka = m, .kb = i};
}

float CalculateI(const int length, const float e, const float f) {
  return (e - f) / length;
}

float CalculateF(const float data_x[], const int length, const float m) {
  float f = 0;

  for (int i = 0; i < length; i++) {
    f = f + data_x[i];
  }

  return m * f;
}

float CalculateE(const float data_y[], const int length) {
  float e = 0;

  for (int i = 0; i < length; i++) {
    e = e + data_y[i];
  }
  return e;
}

float CalculateM(const float a, const float b, const float c, const float d) {
  return (a - b) / (c - d);
}

float CalculateD(const float data_x[], const int length) {
  float d = 0;

  for (int i = 0; i < length; i++) {
    d = d + data_x[i];
  }
  return pow(d, 2);
}

float CalculateC(const float data_x[], const int length) {
  float c = 0;

  for (int i = 0; i < length; i++) {
    c = c + (pow(data_x[i], 2));
  }

  return length * c;
}

float CalculateB(const float data_x[], const float data_y[], const int length) {
  float b_x = 0;
  float b_y = 0;

  for (int i = 0; i < length; i++) {
    b_x = b_x + data_x[i];
    b_y = b_y + data_y[i];
  }

  return b_x * b_y;
}

float CalculateA(const float data_x[], const float data_y[], const int length) {
  float a = 0;

  for (int i = 0; i < length; i++) {
    a = a + (data_x[i] * data_y[i]);
    // Serial.println(a);
  }
  a = a * length;

  return (a);
}

}  // namespace linealization
}  // namespace _10klab
