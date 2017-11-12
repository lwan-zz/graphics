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

  return BBox();
}

bool Triangle::intersect(const Ray& r) const {
  // TODO (PathTracer): implement ray-triangle intersection

  cout << "no isect" << endl;
  return false;
}

bool Triangle::intersect(const Ray& r, Intersection* isect) const {
  // TODO (PathTracer):
  // implement ray-triangle intersection. When an intersection takes
  // place, the Intersection data should be updated accordingly
  // get triangle points
  double F_THRESH = 0.000001;

  // get triangle vertices
  Vector3D p0 = this->mesh->positions[this->v1];
  Vector3D p1 = this->mesh->positions[this->v2];
  Vector3D p2 = this->mesh->positions[this->v3];

  // make vectors
  Vector3D s = r.o - p0;
  Vector3D e1 = p1 - p0;
  Vector3D e2 = p2 - p0;

  // might need to do better interpolation
  Vector3D normal = (this->mesh->normals[this->v1] + 
                     this->mesh->normals[this->v2] + 
                     this->mesh->normals[this->v3]) / 3;

  // check ray, plane parallelism
  double dot_dir = dot(normal, r.d);
  if (abs(dot_dir) < F_THRESH) {return false;}
  double d = dot(normal, p0);
  double t = (dot(r.o, normal) + d) / dot_dir;

  // check if t within bounds
  if (t > r.max_t || t < r.min_t) {return false;}
  r.max_t = t;

  // if point behind ray, return false
  if (t < 0) {return false;}
  Vector3D intersect = r.o + t * r.d;

  // check which side intersection is on
  Vector3D p1_p0 = p1 - p0;
  Vector3D intersect_p0 = intersect - p0;
  if (dot(normal, cross(p1_p0, intersect_p0)) < 0) {return false;}

  Vector3D p2_p1 = p2 - p1;
  Vector3D intersect_p1 = intersect - p1;
  if (dot(normal, cross(p2_p1, intersect_p1)) < 0) {return false;}  

  Vector3D p0_p2 = p0 - p2;
  Vector3D intersect_p2 = intersect - p2;
  if (dot(normal, cross(p0_p2, intersect_p2)) < 0) {return false;}  

  cout << "hit" << endl;

  isect->t = t;
  isect->n = normal;
  isect->bsdf = this->mesh->get_bsdf();
  isect->primitive = this;



  return true;
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
