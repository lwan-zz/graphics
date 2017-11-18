#include "bsdf.h"

#include <algorithm>
#include <iostream>
#include <utility>


using std::min;
using std::max;
using std::swap;

namespace CMU462 {

void make_coord_space(Matrix3x3& o2w, const Vector3D& n) {
  Vector3D z = Vector3D(n.x, n.y, n.z);
  Vector3D h = z;
  if (fabs(h.x) <= fabs(h.y) && fabs(h.x) <= fabs(h.z))
    h.x = 1.0;
  else if (fabs(h.y) <= fabs(h.x) && fabs(h.y) <= fabs(h.z))
    h.y = 1.0;
  else
    h.z = 1.0;

  z.normalize();
  Vector3D y = cross(h, z);
  y.normalize();
  Vector3D x = cross(z, y);
  x.normalize();

  o2w[0] = x;
  o2w[1] = y;
  o2w[2] = z;
}

// Diffuse BSDF //
Spectrum DiffuseBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return albedo * (1.0 / PI);
}

Spectrum DiffuseBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  *wi =  sampler.get_sample(pdf);
  return f(wo, *wi);
}

// Mirror BSDF //

Spectrum MirrorBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum MirrorBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO (PathTracer):
  // Implement MirrorBSDF
  // perfect reflection, so pdf should be one?
  *pdf = 1.;
  reflect(wo, wi);
  return this->reflectance * (1.f / fabs(cos_theta(wo)));
}

// Glossy BSDF //

/*
Spectrum GlossyBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum GlossyBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  *pdf = 1.0f;
  return reflect(wo, wi, reflectance);
}
*/

// Refraction BSDF //

Spectrum RefractionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum RefractionBSDF::sample_f(const Vector3D& wo, Vector3D* wi,
                                  float* pdf) {
  // Implement RefractionBSDF
  *pdf = 1;
  refract (wo, wi, ior);
  return this->transmittance;
}

// Glass BSDF //

Spectrum GlassBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum GlassBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO (PathTracer):
  // Compute Fresnel coefficient and either reflect or refract based on it.

  *pdf = 1;
  // if not refract, then have to reflect
  if (!refract(wo, wi, ior)) {
    reflect(wo, wi);
    return reflectance * (1.f / fabs(cos_theta(wo)));
  }

  // switch to i,t notation
  double ior_i, ior_t;
  double cos_i = cos_theta(wo);
  double cos_t = cos_theta(*wi);

  if (cos_i > 0) {
    ior_i = 1;
    ior_t = ior;
  } else {
    ior_i = ior;
    ior_t = 1.;
  }

  double r_parallel = ((ior_t * cos_i) - (ior_i * cos_t)) / ((ior_t * cos_i) + (ior_i * cos_t));
  double r_perp = ((ior_i * cos_i) - (ior_t * cos_t)) / ((ior_i * cos_i) + (ior_t * cos_t));

  double fresnel = 0.5 * (pow(r_parallel, 2) + pow(r_perp, 2));

  // rng, see if it's less than fresnel ? reflect
  if (cmu_rand() < fresnel) {
    reflect(wo, wi);
    return reflectance * (1. / fabs(cos_i));
  } else {
    double df = (pow(ior_t, 2) / pow(ior_i, 2)) * (1. - fresnel) / fabs(cos_i);
    return transmittance * df;
  }

}

void BSDF::reflect(const Vector3D& wo, Vector3D* wi) {
  // TODO (PathTracer):
  // Implement reflection of wo about normal (0,0,1) and store result in wi.
  //Vector3D normal = Vector3D(0., 0., 1.);
  //Vector3D ref = wo - 2. * dot(wo, normal) * normal;
  wi->x = -wo.x;
  wi->y = -wo.y;
  wi->z = wo.z;
}

bool BSDF::refract(const Vector3D& wo, Vector3D* wi, float ior) {
  // TODO (PathTracer):
  // Use Snell's Law to refract wo surface and store result ray in wi.
  // Return false if refraction does not occur due to total internal reflection
  // and true otherwise. When dot(wo,n) is positive, then wo corresponds to a
  // ray entering the surface through vacuum.

  Vector3D normal(0, 0, 1);
  float ior_i, ior_t;
  float cos_i, cos_t, sin_t, sin_i;

  // check which direction ray is
  // wo entering surface
  cos_i = dot(normal, wo);
  if (cos > 0) {
    ior_t = ior;
    ior_i = 1.; 

  } else {
    ior_t = 1.;
    ior_i = ior;
  }
  
  // check internal reflection
  sin_i = sqrt(1 - pow(cos_i, 2));
  float cos_t_pow2 = 1 - pow((ior_i / ior_t), 2) * pow(sin_i, 2);

  if (cos_t_pow2 < 0) { return false; }

  cos_t = sqrt(cos_t_pow2);
  sin_t = (ior_i / ior_t) * sin_i;

  // convert spherical to cartesian
  wi->x = -wo.x * sin_t / sin_i;
  wi->y = -wo.y * sin_t / sin_i;
  wi->z = (cos > 0) ? -cos_t : cos_t;

  return true;
}

// Emission BSDF //

Spectrum EmissionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum EmissionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  *wi = sampler.get_sample(pdf);
  return Spectrum();
}

}  // namespace CMU462
