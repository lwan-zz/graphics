#include "bvh.h"

#include "CMU462/CMU462.h"
#include "static_scene/triangle.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CMU462 {
namespace StaticScene {

#define N_BINS 10

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {
  this->primitives = _primitives;
  BBox bb;

  for (size_t i = 0; i < primitives.size(); ++i) {
    bb.expand(primitives[i]->get_bbox());
  }

  root = new BVHNode(bb, 0, primitives.size());
  partition(*root, max_leaf_size);
}


BVHAccel::~BVHAccel() {
  // TODO (PathTracer):
  // Implement a proper destructor for your BVH accelerator aggregate
  delete this->root;
}

void BVHAccel::partition(BVHNode &bvhnode, size_t max_leaf_size) {
  
  // base case
  if (bvhnode.range <= max_leaf_size) {return;}

  // intialize things you care about
  int bvh_axis, bvh_split, num_bvh_left, num_bvh_right;
  BBox bvh_left_bbox, bvh_right_bbox;
  double bvh_sah = INF_D;

  // loop through xyz axes, then adjust l/r split per SAH cost, 
  // keep split with best SAH
  for (int axis = 0; axis != 3; axis++) {
    // first find the min/max limits of each axis, and divide into N_BINS
    double axis_min = INF_D;
    double axis_max = -INF_D; // set to opposites to guarantee some values 

    // find range of values for axis
    for (vector<Primitive*>::iterator primitive = this->primitives.begin() + bvhnode.start;
         primitive != this->primitives.begin() + bvhnode.start + bvhnode.range; primitive++ ) {

      double this_centroid = (*primitive)->get_bbox().centroid()[axis];
      if (this_centroid < axis_min) {axis_min = this_centroid;}
      if (this_centroid > axis_max) {axis_max = this_centroid;}
    }

    BBox left = BBox();
    BBox right = BBox();
    int num_left = 0;
    int num_right = 0;

    for (int split = 1; split != N_BINS; split++) {
      for (vector<Primitive*>::iterator primitive = this->primitives.begin() + bvhnode.start;
           primitive != this->primitives.begin() + bvhnode.start + bvhnode.range; primitive++ ) {

        double this_centroid = (*primitive)->get_bbox().centroid()[axis];
        int bucket = computeBucket(this_centroid, axis_min, axis_max);

        if (bucket <= split) {
          left.expand((*primitive)->get_bbox());
          num_left++;
        } else {
          right.expand((*primitive)->get_bbox());
          num_right++;
        }
      }

      double total_sa = left.surface_area() + right.surface_area();
      double this_sah = left.surface_area() / total_sa * num_left + 
                        right.surface_area() / total_sa * num_right;

      if (this_sah < bvh_sah) {
        bvh_axis = axis;
        bvh_split = split;
        num_bvh_left = num_left;
        num_bvh_right = num_right;
        bvh_left_bbox = left;
        bvh_right_bbox = right;
        bvh_sah = this_sah;
      }                  
    }
  }

  // sort the primitves in ascending order along a specified axis
  auto axis_val_compare = [&bvh_axis] (Primitive* a, Primitive* b) {
    return a->get_bbox().centroid()[bvh_axis] < b->get_bbox().centroid()[bvh_axis];
  };
  // sort
  std::sort(this->primitives.begin() + bvhnode.start, 
            this->primitives.begin() + bvhnode.start + bvhnode.range, 
            axis_val_compare);

  // create new left, right nodes from sorted primitives  
  bvhnode.l = new BVHNode(bvh_left_bbox, bvhnode.start, num_bvh_left - 1);
  bvhnode.r = new BVHNode(bvh_right_bbox, bvhnode.start + num_bvh_left, num_bvh_right);

  partition(*bvhnode.l, max_leaf_size);
  partition(*bvhnode.r, max_leaf_size);
}

inline int BVHAccel::computeBucket(double this_centroid, double axis_min, double axis_max) {
  return floor((double) N_BINS * (this_centroid - axis_min) / (axis_max - axis_min));
}

BBox BVHAccel::get_bbox() const { return root->bb; }

bool BVHAccel::intersect(const Ray &ray) const {
  // Implement ray - bvh aggregate intersection test. A ray intersects
  // with a BVH aggregate if and only if it intersects a primitive in
  // the BVH that is not an aggregate.
  
  Intersection isect;
  return intersect(ray, &isect);
}

bool BVHAccel::intersect(const Ray &ray, Intersection *isect) const {
  // Implement ray - bvh aggregate intersection test. A ray intersects
  // with a BVH aggregate if and only if it intersects a primitive in
  // the BVH that is not an aggregate. When an intersection does happen.
  // You should store the non-aggregate primitive in the intersection data
  // and not the BVH aggregate itself.
  
  findClosestHit(ray, *root, isect);
  if (isect->t < INF_D) { return true; }
  return false;
}

void BVHAccel::findClosestHit(const Ray &ray, BVHNode &node, Intersection *isect) const {  
  double tmin_node = -INF_D, tmax_node = INF_D;
  // if the ray doesn't intersect the box at all, skip checking the box
  if (!node.bb.intersect(ray, tmin_node, tmax_node)) { return; }

  if (node.isLeaf()) {
    // make a new intersection object because I keep segfaulting otherwise
    Intersection new_isect;
    for (int prim_idx = node.start; prim_idx != node.start + node.range; prim_idx++) {
      // check for intersection with primitive
      bool hit = primitives[prim_idx]->intersect(ray, &new_isect);
      if (hit && new_isect.t < isect->t) {
        *isect = new_isect;
      }
    }
  } else {
    double tmin_l = -INF_D, tmax_l = INF_D;
    double tmin_r = -INF_D, tmax_r = INF_D;

    node.l->bb.intersect(ray, tmin_l, tmax_l);
    node.r->bb.intersect(ray, tmin_l, tmax_r);

    BVHNode* first = (tmin_l <= tmin_r) ? node.l : node.r;
    BVHNode* second = (tmin_l <= tmin_r) ? node.r : node.l;
    // hold onto the t-val of the second node
    double second_t = (tmin_l <= tmin_r) ? tmin_l : tmin_r;

    findClosestHit(ray, *first, isect);
    if (second_t < isect->t)
      findClosestHit(ray, *second, isect);
  }  
}

}  // namespace StaticScene
}  // namespace CMU462
