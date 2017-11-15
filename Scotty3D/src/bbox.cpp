#include "bbox.h"

#include "GL/glew.h"

#include <algorithm>
#include <iostream>

namespace CMU462 {

bool BBox::intersect(const Ray &r, double &t0, double &t1) const {
  // TODO (PathTracer):
  // Implement ray - bounding box intersection test
  // If the ray intersected the bounding box within the range given by
  // t0, t1, update t0 and t1 with the new intersection times.
  // Used this resource: https://www.scratchapixel.com/lessons/...
  // 3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
  double tmin_x, tmin_y, tmin_z;
  double tmax_x, tmax_y, tmax_z;

  // first evaluate x, y comparison
  tmin_x = (((r.sign[0] > 0) ? this->min.x : this->max.x) - r.o.x) * r.inv_d.x;
  tmax_x = (((r.sign[0] > 0) ? this->max.x : this->min.x) - r.o.x) * r.inv_d.x;
  
  tmin_y = (((r.sign[1] > 0) ? this->min.y : this->max.y) - r.o.x) * r.inv_d.y;
  tmax_y = (((r.sign[1] > 0) ? this->max.y : this->min.y) - r.o.x) * r.inv_d.y;
  
  if (tmin_x > tmax_y || tmin_y > tmax_x) { return false; }
  
  // check which t value for x, y plane intersection is closer
  if (tmin_y > tmin_x) { t0 = tmin_y; }
  if (tmax_y < tmax_x) { t1 = tmax_y; }

  tmin_z = (((r.sign[2] > 0) ? this->min.z : this->max.z) - r.o.z) * r.inv_d.z;
  tmax_z = (((r.sign[2] > 0) ? this->max.z : this->min.z) - r.o.z) * r.inv_d.z;

  // check which t values are closer again
  if (t0 > tmax_z || tmin_z > t1) { std::cout << "earlyexit2" << std::endl; return false; }
  if (tmin_z > t0) { t0 = tmin_z; }
  if (tmax_z < t1) { t1 = tmax_z; }

  return true;
}

void BBox::draw(Color c) const {
  glColor4f(c.r, c.g, c.b, c.a);

  // top
  glBegin(GL_LINE_STRIP);
  glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(max.x, max.y, max.z);
  glEnd();

  // bottom
  glBegin(GL_LINE_STRIP);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, min.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, min.y, min.z);
  glEnd();

  // side
  glBegin(GL_LINES);
  glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(min.x, min.y, max.z);
  glEnd();
}

std::ostream &operator<<(std::ostream &os, const BBox &b) {
  return os << "BBOX(" << b.min << ", " << b.max << ")";
}

}  // namespace CMU462
