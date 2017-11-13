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

  //cout << "triangle vertices: " << this->v1 << " " << this->v2 << " " << this->v3 << endl;
  //cout << "vectorsize: " << this->v.size() << endl;
  //cout << "vector: " << this->v[v1] << " " << this->v[v2] << " " << this->v[v3] << endl;
  // get triangle vertices
  
  Vector3D p0 = this->mesh->positions[this->v1];
  Vector3D p1 = this->mesh->positions[this->v2];
  Vector3D p2 = this->mesh->positions[this->v3];

  // calculate normal. looks like the attribute array is empty??
  Vector3D n = cross(p1-p0, p2-p0);
  n.print();
  //p0.print();
  //p1.print();
  //p2.print();

  // make vectors
  Vector3D s = r.o - p0;
  Vector3D e1 = p1 - p0;
  Vector3D e2 = p2 - p0;

  // might need to do better interpolation
  Vector3D normal = (this->mesh->normals[this->v1] + 
                     this->mesh->normals[this->v2] + 
                     this->mesh->normals[this->v3]) / 3;

  //cout << "normals" << endl;
  //this->mesh->normals[this->v1].print();
  //this->mesh->normals[this->v2].print();
  //this->mesh->normals[this->v3].print();

  // check ray / plane parallelism
  double n_dot_raydir = dot(normal, r.d);
  if(abs(n_dot_raydir) < 0.000001) {
    cout << "parallel ray /plane" << endl;
    return false;
  }
  cout << "normal: "; normal.print();
  cout << "p0: "; p0.print();
  
  // check if ray behind triangle
  double d = dot(normal, p0); 
  double t = (dot(normal, r.o) + d) / n_dot_raydir;
  cout << "t, d: " << t << " " << d << endl;
  //getchar();

  if (t < 0) {
    cout << "behind" << endl;
    return false;
  }

  Vector3D p =  r.o + t * r.d;
  // check if point is on the correct side of each edge of triangle

  Vector3D s0 = p1 - p0;
  Vector3D s1 = p2 - p1;
  Vector3D s2 = p0 - p2;

  vector <Vector3D> sides = {p1 - p0, p2 - p1, p0 - p2};

  for (auto& side : sides) {
    Vector3D vs = p - side;
    if (dot(normal, cross(side, vs)) < 0) {
        cout << "p on wrong side" << endl;
        return false;
    }
  }

  cout << "hit" << endl;
  r.max_t = t;
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
