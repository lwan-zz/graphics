#include "sphere.h"

#include <cmath>

#include "../bsdf.h"
#include "../misc/sphere_drawing.h"

namespace CMU462 {
namespace StaticScene {

bool Sphere::test(const Ray& r, double& t1, double& t2) const {
  // TODO (PathTracer):
  // Implement ray - sphere intersection test.
  // Return true if there are intersections and writing the
  // smaller of the two intersection times in t1 and the larger in t2.
  cout << "spheretest" << endl;
  return false;
}

bool Sphere::intersect(const Ray& r) const {
  Intersection isect;
  return intersect(r, &isect);
}

bool Sphere::intersect(const Ray& r, Intersection* isect) const {
  // TODO (PathTracer):
  // Implement ray - sphere intersection.
  // Note again that you might want to use the the Sphere::test helper here.
  // When an intersection takes place, the Intersection data should be updated
  // correspondingly.
  // help from here: https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
  Vector3D L  = this->o - r.o;
  double tca = dot(L, r.d);

  if (tca < 0) {return false;} // misses sphere
  
  double d2 = dot(L, L) - tca * tca;
  if (d2 > this->r2) {return false;}
  double thc = sqrt(this->r2 - d2);  

  double t1 = tca - thc;
  double t2 = tca + thc;
  double t;
  // use the smaller of the intersections
  if (t2 < t1) {swap(t1, t2);}
  if (t1 < 0) {
    t1 = t2;
    if (t1 < 0) {
      return false;
    }
  }
 
  if (t1 < r.max_t) {
    r.max_t = t1;
    Vector3D normal = ((r.o + t1 * r.d) - this->o).unit();
    isect->t = t1;
    isect->n = normal;
    isect->bsdf = this->get_bsdf();
    isect->primitive = this;
    return true;
  }

  return false;
}

void Sphere::draw(const Color& c) const { Misc::draw_sphere_opengl(o, r, c); }

void Sphere::drawOutline(const Color& c) const {
  // Misc::draw_sphere_opengl(o, r, c);
}

}  // namespace StaticScene
}  // namespace CMU462
