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
  double THRESH = 0.0000001;

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

  Vector3D e1xd = cross(e1, r.d);
  Vector3D sxe2 = cross(s, e2);
  double denom =  dot(e1xd, e2);

  // figure out if patch too small
  if (denom < THRESH) {
    //cout << "e1 x d dot e2 too small!" << endl;
    return false;
  }

  double u = -dot(sxe2, r.d) / denom;
  double v = dot(e1xd, s) / denom;
  double t = -dot(sxe2, e1) / denom;

  // check direction of normals with 
  
  if (u > 0 && v > 0 && u < 1 && v < 1 && t < r.max_t) {
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
