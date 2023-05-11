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


/*The GetCoefficients() function first calculates the number of data points. Then, it calculates 
the coefficients a, b, c, and d. Next, it calculates the slope m of the line. Then, it calculates 
the sum of the y-coordinates and the sum of the products of the x-coordinates and the slope m. 
Finally, it calculates the y-intercept i of the line. It then prints the coefficients of 
the line and returns them.*/
struct Coefficients GetCoefficients(const float data_x[], const float data_y[],
                                    const int length) {
  // const int n = length;
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

/*The CalculateI() function first calculates the difference between the sum of the y-coordinates 
and the sum of the products of the x-coordinates and the slope. Then, it divides the difference 
by the number of data points. Finally, it returns the result.*/
float CalculateI(const int length, const float e, const float f) {
  return (e - f) / length;
}

/*The CalculateF() function first initializes the sum f to 0. Then, 
it iterates over the data points. For each data point, it adds the product 
of the x-coordinate and the slope to f. Finally, it returns f.*/
float CalculateF(const float data_x[], const int length, const float m) {
  float f = 0;

  for (int i = 0; i < length; i++) {
    f = f + data_x[i];
  }

  return m * f;
}

/*The CalculateE() function first initializes the sum e to 0. Then, 
it iterates over the data points. For each data point, it adds the 
y-coordinate to e. Finally, it returns e.*/
float CalculateE(const float data_y[], const int length) {
  float e = 0;

  for (int i = 0; i < length; i++) {
    e = e + data_y[i];
  }
  return e;
}

/*The CalculateM() function first calculates the difference between the coefficients a and b. 
Then, it calculates the difference between the coefficients c and d. Finally, 
it returns the ratio of the two differences.*/
float CalculateM(const float a, const float b, const float c, const float d) {
  return (a - b) / (c - d);
}

/*The CalculateD() function first initializes the coefficient d to 0. Then, 
it iterates over the data points. For each data point, it adds the x-coordinate to d. 
Finally, it returns the square of d.*/
float CalculateD(const float data_x[], const int length) {
  float d = 0;

  for (int i = 0; i < length; i++) {
    d = d + data_x[i];
  }
  return pow(d, 2);
}

/*The CalculateC() function first initializes the coefficient c to 0. Then, 
it iterates over the data points. For each data point, it adds the square of 
the x-coordinate to c. Finally, it multiplies c by the number of data points 
and returns it.*/
float CalculateC(const float data_x[], const int length) {
  float c = 0;

  for (int i = 0; i < length; i++) {
    c = c + (pow(data_x[i], 2));
  }

  return length * c;
}

/*The CalculateB() function first initializes the coefficients b_x and b_y to 0. Then, 
it iterates over the data points. For each data point, it adds the x-coordinate to b_x and the 
y-coordinate to b_y. Finally, it returns the product of b_x and b_y.*/
float CalculateB(const float data_x[], const float data_y[], const int length) {
  float b_x = 0;
  float b_y = 0;

  for (int i = 0; i < length; i++) {
    b_x = b_x + data_x[i];
    b_y = b_y + data_y[i];
  }

  return b_x * b_y;
}

/*
The CalculateA() function first initializes the coefficient a to 0. Then, it iterates 
over the data points. For each data point, it adds the product of the x- and y-coordinates to a. 
Finally, it multiplies a by the number of data points and returns it.
*/
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
