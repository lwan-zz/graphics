#include "environment_light.h"

namespace CMU462 {
namespace StaticScene {

EnvironmentLight::EnvironmentLight(const HDRImageBuffer* envMap)
    : envMap(envMap) {
  // TODO: (PathTracer) initialize things here as needed
  //flux = L sin theta

  //sum flux / w + h = dist

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
  double phi = atan2(wi->z, -wi->x);
  double theta = acos(wi->y);

  // remap back to texture 2D
  double tex_x = floor((phi / (2. * PI)) * w);
  double tex_y = floor((theta / PI) * h);

  int x_idx = (int) tex_x;
  int y_idx = (int) tex_y;

  // TODO deal with edges... later
  Spectrum px1, px2, px3, px4;

  px1 = envMap->data[(y_idx)     * w + x_idx   ];
  px2 = envMap->data[(y_idx)     * w + x_idx + 1];
  px3 = envMap->data[(y_idx + 1) * w + x_idx   ];
  px4 = envMap->data[(y_idx + 1) * w + x_idx + 1];

  // interp x
  px1 = px1 * (1 - tex_x - x_idx) + px2 * (tex_x - x_idx);
  px3 = px3 * (1 - tex_x - x_idx) + px4 * (tex_x - x_idx);
  // interp y
  px1 = (px1 + px3);
  px1 = px1 * (1 - tex_y - y_idx) + px3 * (tex_y - y_idx);
  
  return px1;
}

Spectrum EnvironmentLight::sample_dir(const Ray& r) const {
  // TODO: (PathTracer) Implement
  int w = envMap->w;
  int h = envMap->h;

  Vector3D dir = r.d;
  // map sample to sphere
  double phi = atan2(dir.x, -dir.z);
  // negative phi no good
  phi = (phi < 0) ? phi + 2 * PI : phi;
  double theta = acos(dir.y);

  // remap back to texture 2D
  double tex_x = (phi / (2. * PI)) * w;
  double tex_y = (theta / PI) * h;

  // just get some kind of mapping working for now 

  // TODO deal with edges... later
  Spectrum px1, px2, px3, px4;

  px1 = envMap->data[floor(tex_y) * w + floor(tex_x)];
  px2 = envMap->data[floor(tex_y) * w + ceil(tex_x) ];
  px3 = envMap->data[ceil(tex_y)  * w + floor(tex_x)];
  px4 = envMap->data[ceil(tex_y)  * w + ceil(tex_x) ];

  // interp x
  px1 = px1 * (tex_x - floor(tex_x)) + px2 * (ceil(tex_x) - tex_x);
  px3 = px3 * (tex_x - floor(tex_x)) + px4 * (ceil(tex_x) - tex_x);
  // interp y
  px1 = px1 * (tex_y - floor(tex_y)) + px3 * (ceil(tex_y) - tex_y);
  
  //std::cout << "px1-4" << std::endl;
  //cout << tex_x << " " << tex_y << " " << x_idx << " " << y_idx
  //px1.print();
  //px2.print();
  //px3.print();
  //px4.print();
  //getchar();

  return px1;
}

}  // namespace StaticScene
}  // namespace CMU462
