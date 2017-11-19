#include "environment_light.h"

namespace CMU462 {
namespace StaticScene {

EnvironmentLight::EnvironmentLight(const HDRImageBuffer* envMap)
    : envMap(envMap) {
  // TODO: (PathTracer) initialize things here as needed
  
}

Spectrum EnvironmentLight::sample_L(const Vector3D& p, Vector3D* wi,
                                    float* distToLight, float* pdf) const {
  // TODO: (PathTracer) Implement
  Vector3D sample_vec = this->sampler.get_sample();
  int w = envMap->w;
  int h = envMap->h;

  *pdf = 1. / (4. * PI);
  *wi = sample_vec;
  *distToLight = INF_D;

  // map sample to sphere
  double phi = atan2(wi->x, -wi->z) + PI;
  double theta = acos(wi->y);

  // remap back to texture 2D
  double tex_x = floor((phi / (2. * PI)) * envMap->w);
  double tex_y = floor((theta / PI) * envMap->h);

  int x_idx = (int) tex_x;
  int y_idx = (int) tex_y;

  // TODO deal with edges... later
  Spectrum px1, px2, px3, px4;

  px1 = envMap->data[(y_idx)     * w + x_idx   ];
  px2 = envMap->data[(y_idx)     * w + x_idx + 1];
  px3 = envMap->data[(y_idx + 1) * w + x_idx   ];
  px4 = envMap->data[(y_idx + 1) * w + x_idx + 1];

  // average 1D
  px1 = (px1 + px2) * 0.5;
  px3 = (px3 + px4) * 0.5;
  // average 2D
  px1 = (px1 + px3) * 0.5;

  return px1;
}

Spectrum EnvironmentLight::sample_dir(const Ray& r) const {
  // TODO: (PathTracer) Implement
  return Spectrum();
}

}  // namespace StaticScene
}  // namespace CMU462
