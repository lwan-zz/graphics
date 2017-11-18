#include "triangle.h"

#include "CMU462/CMU462.h"
#include "GL/glew.h"

namespace CMU462 {
namespace StaticScene {

Triangle::Triangle(const Mesh* mesh, vector<size_t>& v) : mesh(mesh), v(v) {}
Triangle::Triangle(const Mesh* mesh, size_t v1, size_t v2, size_t v3)
    : mesh(mesh), v1(v1), v2(v2), v3(v3) {}

BBox Triangle::get_bbox() const {
  // TODO (PathTracer):
  // compute the bounding box of the triangle
  // get max / min x, y, z

  Vector3D max, min;

  // get triangle vertices
  Vector3D p0 = this->mesh->positions[this->v1];
  Vector3D p1 = this->mesh->positions[this->v2];
  Vector3D p2 = this->mesh->positions[this->v3];  

  max.x = std::max(p0.x, std::max(p1.x, p2.x));
  max.y = std::max(p0.y, std::max(p1.y, p2.y));
  max.z = std::max(p0.z, std::max(p1.z, p2.z));

  min.x = std::min(p0.x, std::min(p1.x, p2.x));
  min.y = std::min(p0.y, std::min(p1.y, p2.y));
  min.z = std::min(p0.z, std::min(p1.z, p2.z));

  return BBox(min, max);
}

bool Triangle::intersect(const Ray& r) const {
  Intersection isect;
  return intersect(r, &isect);
}

bool Triangle::intersect(const Ray& r, Intersection* isect) const {
  // implement ray-triangle intersection. When an intersection takes
  // place, the Intersection data should be updated accordingly

  // get triangle vertices
  Vector3D p0 = this->mesh->positions[this->v1];
  Vector3D p1 = this->mesh->positions[this->v2];
  Vector3D p2 = this->mesh->positions[this->v3];

  // make vectors
  Vector3D s = r.o - p0;
  Vector3D e1 = p1 - p0;
  Vector3D e2 = p2 - p0;

  Vector3D e1xd = cross(e1, r.d);
  Vector3D sxe2 = cross(s, e2);
  double denom =  dot(e1xd, e2);

  // figure out if patch too small
  if (labs(denom) < EPS_D) { return false;}

  double u = -dot(sxe2, r.d) / denom;
  double v = dot(e1xd, s) / denom;
  double t = -dot(sxe2, e1) / denom;

  // check direction of normals with 
  if (u > 0 && v > 0 && u < 1 && u + v < 1 && t < r.max_t && t > r.min_t) {
    Vector3D normal = (1. - u - v) * this->mesh->normals[this->v1] + 
                      u * this->mesh->normals[this->v2] + 
                      v * this->mesh->normals[this->v3];

    if (dot(r.d, normal) < 0) { normal = -normal; }
    r.max_t = t;
    isect->t = t;   
    isect->n = normal;
    isect->bsdf = this->mesh->get_bsdf();
    isect->primitive = this;
    return true;
  }
 
  return false;
 
}

void Triangle::draw(const Color& c) const {
  glColor4f(c.r, c.g, c.b, c.a);
  glBegin(GL_TRIANGLES);
  glVertex3d(mesh->positions[v1].x, mesh->positions[v1].y,
             mesh->positions[v1].z);
  glVertex3d(mesh->positions[v2].x, mesh->positions[v2].y,
             mesh->positions[v2].z);
  glVertex3d(mesh->positions[v3].x, mesh->positions[v3].y,
             mesh->positions[v3].z);
  glEnd();
}

void Triangle::drawOutline(const Color& c) const {
  glColor4f(c.r, c.g, c.b, c.a);
  glBegin(GL_LINE_LOOP);
  glVertex3d(mesh->positions[v1].x, mesh->positions[v1].y,
             mesh->positions[v1].z);
  glVertex3d(mesh->positions[v2].x, mesh->positions[v2].y,
             mesh->positions[v2].z);
  glVertex3d(mesh->positions[v3].x, mesh->positions[v3].y,
             mesh->positions[v3].z);
  glEnd();
}

}  // namespace StaticScene
}  // namespace CMU462
