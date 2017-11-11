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
  bool intersect = false;
  double F_THRESH = 0.000001;

  // get triangle vertices
  Vector3D p0 = this->mesh->positions[this->v1];
  Vector3D p1 = this->mesh->positions[this->v2];
  Vector3D p2 = this->mesh->positions[this->v3];

  // make vectors
  Vector3D s = r.o - p0;
  Vector3D e1 = p1 - p0;
  Vector3D e2 = p2 - p0;

  Vector3D h = cross(r.d, e2);
  double a = dot(e1, h);
  if (abs(a) < F_THRESH) {return false;}

  double f = 1/a;
  Vector3D s = r.o - p0;
  double u  = f * dot(s, h);

  if (u < 0.0 || u > 1.0) {return false;}

  Vector3D q = cross(s, e1);
  double v = f * dot(r.d, q);
  if(v < 0.0 || u + v > 1.0) {return false;}

  double t = f *dot(e2, q);
  if (t > F_THRESH) {
   
    Vector3D intersection  = r.o + r.d * t;
    return true;
  }

  else {return false;}

  //cout << "vec1: " << vec1.x << " " << vec1.y << " " << vec1.z << endl;  

  //cout << ":w: " << p0.x << " " << p0.y << " " << p0.z << endl;
  //cout << "no isect" << endl;
  //Vector3D f_debug = p0 + vec1.x * (p1 - p0) + vec1.y * (p2 - p0);

  //cout << "fval: " << f_debug.x << " " << f_debug.y << " " << f_debug.z << endl;
  //cout << "norm p1-p0: " << (p1-p0).norm() << endl;
  //cout << "norm p2-p0: " << (p2-p0).norm() << endl;
}

bool Triangle::intersect(const Ray& r, Intersection* isect) const {
  // TODO (PathTracer):
  // implement ray-triangle intersection. When an intersection takes
  // place, the Intersection data should be updated accordingly
  //Vector2D e1_d = Vector2D()

  // get triangle points
  bool intersect = this->intersect(r); 

  

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
