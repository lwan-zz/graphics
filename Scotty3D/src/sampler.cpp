#include "sampler.h"

using namespace std;

namespace CMU462 {

// Uniform Sampler2D Implementation //

Vector2D UniformGridSampler2D::get_sample() const {
  std::random_device rd;
  std::default_random_engine generator(rd());
  std::uniform_real_distribution<double> uni(0.0,1.0);

  double x = uni(generator);
  double y = uni(generator);
  return Vector2D(x, y);
}

// Uniform Hemisphere Sampler3D Implementation //

Vector3D UniformHemisphereSampler3D::get_sample() const {
  double Xi1 = (double)(std::rand()) / RAND_MAX;
  double Xi2 = (double)(std::rand()) / RAND_MAX;

  double theta = acos(Xi1);
  double phi = 2.0 * PI * Xi2;

  double xs = sinf(theta) * cosf(phi);
  double ys = sinf(theta) * sinf(phi);
  double zs = cosf(theta);

  return Vector3D(xs, ys, zs);
}

Vector3D CosineWeightedHemisphereSampler3D::get_sample() const {
  float f;
  return get_sample(&f);
}

Vector3D CosineWeightedHemisphereSampler3D::get_sample(float *pdf) const {
  double Xi1 = (double)(std::rand()) / RAND_MAX;
  double Xi2 = (double)(std::rand()) / RAND_MAX;

  double theta = acos(1 - 2 * Xi1) / 2;
  double phi = 2.0 * PI * Xi2;

  double x = sin(theta) * cos(phi);
  double y = sin(theta) * sin(phi);
  double z = cos(theta);

  *pdf = z / PI;

  return Vector3D(x, y, z);
}

}  // namespace CMU462
