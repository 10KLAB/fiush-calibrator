

namespace _10klab {
namespace linealization {

struct Coefficients {
  float ka = 1;
  float kb = 0;
};
struct Coefficients LinearCoefficients;

struct Coefficients GetCoefficients(const float data_x[], const int length){
  LinearCoefficients.ka = data_x[0];
  LinearCoefficients.kb = length;
  return LinearCoefficients;
}

} // namespace linealization
} // namespace _10klab