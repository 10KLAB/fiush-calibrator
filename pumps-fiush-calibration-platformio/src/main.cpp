#include <Arduino.h>
#include "linealization.h"

float data_x[9] = {120, 160, 170, 185, 190, 197, 220, 230, 240};
float data_y[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};





void setup(){
  Serial.begin(115200);

_10klab::linealization::Coefficients GetCoefficients;

float data_x[3] = {1, 2, 3};
float data_y[3] = {3, 5, 6.5};
const int data_length = sizeof(data_x) / sizeof(float);

GetCoefficients = _10klab::linealization::GetCoefficients(data_x, data_y, data_length);

float ka=GetCoefficients.ka;
float kb=GetCoefficients.kb;

Serial.println("ka= " + String(ka));
Serial.println("kb= " + String(kb));

}

void loop(){
delay(100);
}







struct Coefficients {
  float ka = 1;
  float kb = 0;
};

struct Coefficients LinearCoefficients;

struct Coefficients UpdateCoefficients(){
  LinearCoefficients.ka = 2;
  LinearCoefficients.kb = 3;
  return LinearCoefficients;
}

// int CalculateN();
// float CalculateA(int length);
// float CalculateB(int length);
// float CalculateC(int length);
// float CalculateD(int length);
// float CalculateM(float a, float b, float c, float d);
// float CalculateE(int length);
// float CalculateF(int length, float m);
// float CalculateI(int length, float e, float f);
// // void Linealization();

// void setup() {
//   Serial.begin(115200);
//   // UpdateCoefficients();
// }

// void loop() { delay(100); }

// void Linealization() {


//   int n = CalculateN();
//   float a = CalculateA(n);
//   float b = CalculateB(n);
//   float c = CalculateC(n);
//   float d = CalculateD(n);
//   float m = CalculateM(a, b, c, d);
//   float e = CalculateE(n);
//   float f = CalculateF(n, m);
//   float i = CalculateI(n, e, f);

//   Serial.println("n= " + String(n));
//   Serial.println("a= " + String(a));
//   Serial.println("b= " + String(b));
//   Serial.println("c= " + String(c));
//   Serial.println("d= " + String(d));
//   Serial.println("m= " + String(m));
//   Serial.println("e= " + String(e));
//   Serial.println("f= " + String(f));
//   Serial.println("i= " + String(i));

//   // y = m*x + i
// }

// float CalculateI(int length, float e, float f) { return (e - f) / length; }

// float CalculateF(int length, float m) {
//   float f = 0;

//   for (int i = 0; i < length; i++) {
//     f = f + data_x[i];
//   }

//   return m * f;
// }

// float CalculateE(int length) {
//   float e = 0;

//   for (int i = 0; i < length; i++) {
//     e = e + data_y[i];
//   }
//   return e;
// }

// float CalculateM(float a, float b, float c, float d) {
//   return (a - b) / (c - d);
// }

// float CalculateD(int length) {
//   float d = 0;

//   for (int i = 0; i < length; i++) {
//     d = d + data_x[i];
//   }
//   return pow(d, 2);
// }

// float CalculateC(int length) {
//   float c = 0;

//   for (int i = 0; i < length; i++) {
//     c = c + (pow(data_x[i], 2));
//   }

//   return length * c;
// }

// float CalculateB(int length) {
//   float b_x = 0;
//   float b_y = 0;

//   for (int i = 0; i < length; i++) {
//     b_x = b_x + data_x[i];
//     b_y = b_y + data_y[i];
//   }

//   return b_x * b_y;
// }

// float CalculateA(int length) {
//   float a = 0;

//   for (int i = 0; i < length; i++) {
//     a = a + (data_x[i] * data_y[i]);
//     // Serial.println(a);
//   }
//   a = a * length;

//   return (a);
// }

// int CalculateN() {
//   // calculate the length of the array
//   return sizeof(data_x) / sizeof(float);
// }
