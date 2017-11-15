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

  tmin_x = (this->min.x - r.o.x) / r.d.x;
  tmax_x = (this->max.x - r.o.x) / r.d.x;
  if (tmin_x > tmax_x) { std::swap(tmin_x, tmax_x); }

  tmin_y = (this->min.y - r.o.y) / r.d.y;
  tmax_y = (this->max.y - r.o.y) / r.d.y;
  if (tmin_y > tmax_y) { std::swap(tmin_y, tmax_y); }

  if (tmin_x > tmax_y || tmin_y > tmax_x) { return false; }

  if (tmin_y > tmin_x) { tmin_x = tmin_y; }
  if (tmax_y < tmax_x) { tmax_x = tmax_y; }

  tmin_z = (this->min.z - r.o.z) / r.d.z;
  tmax_z = (this->max.z - r.o.z) / r.d.z;
  if (tmin_z > tmax_z) { std::swap(tmin_z, tmax_z); }

  if (tmin_x > tmax_z || tmin_z > tmax_x) { return false; }

  if (tmin_z > tmin_x) { tmin_x = tmin_z; }
  if (tmax_z < tmax_x) { tmax_x = tmax_z; }

  t0 = tmin_x;
  t1 = tmax_x;

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
