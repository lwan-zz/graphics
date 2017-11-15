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

  // TODO (PathTracer):
  // Construct a BVH from the given vector of primitives and maximum leaf
  // size configuration. The starter code build a BVH aggregate with a
  // single leaf node (which is also the root) that encloses all the
  // primitives.

  //  cout << "num prims: " << primitives.size() << endl;;
    cout << "max leaf size" << max_leaf_size << endl;
  //  BBox bb8 = primitives[0]->get_bbox();
  //  cout << "bbox1:" << endl;
  //  bb8.max.print();
  //  bb8.min.print();
  //
  //getchar();
  BBox bb;

  for (size_t i = 0; i < primitives.size(); ++i) {
    bb.expand(primitives[i]->get_bbox());
  }
  cout << "building bvh" << endl;
  root = new BVHNode(bb, 0, primitives.size());
  partition(*root, max_leaf_size);
}


BVHAccel::~BVHAccel() {
  // TODO (PathTracer):
  // Implement a proper destructor for your BVH accelerator aggregate
  delete root;
}

void BVHAccel::partition(BVHNode &bvhnode, size_t max_leaf_size) {
  
  // base case
  cout << bvhnode.range << endl;
  
  if (bvhnode.range <= max_leaf_size) {return;}

  // make your bins, bin counters
  //vector < BBox > bins (N_BINS, BBox());
  //vector < int > bin_count (N_BINS, 0);

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
        //cout << "bucket: " << bucket << endl;
        if (bucket <= split) {
          left.expand((*primitive)->get_bbox());
          num_left++;
        } else {
          right.expand((*primitive)->get_bbox());
          num_right++;
        }
      }
      //cout << "numleft: " << num_left << endl;
      //cout << "numright: " << num_right << endl;

      double total_sa = left.surface_area() + right.surface_area();
      double this_sah = left.surface_area() / total_sa * num_left + 
                        right.surface_area() / total_sa * num_right;

      if (this_sah < bvh_sah) {
        // lowest heuristic, so set your nos
        //cout << "found lowest sah" << endl;
        //cout << "sah: " << this_sah << endl;
        //cout << "split: " << split << endl;
        //cout << "axis: " << axis << endl;
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

  cout << "found lowest sah" << endl;
  cout << "sah: " << bvh_sah << endl;
  cout << "split: " << bvh_split << endl;
  cout << "axis: " << bvh_axis << endl;
  cout << "num_left" << num_bvh_left << endl;
  cout << "num_right" << num_bvh_right << endl;

  // sort the primitves in ascending order along a specified axis
  auto axis_val_compare = [&bvh_axis] (Primitive* a, Primitive* b) {
    return a->get_bbox().centroid()[bvh_axis] < b->get_bbox().centroid()[bvh_axis];
  };
  // sort
  std::sort(this->primitives.begin() + bvhnode.start, 
            this->primitives.begin() + bvhnode.start + bvhnode.range, 
            axis_val_compare);

  cout << "create new nodes" << endl;
  // create new left, right nodes from sorted primitives
  cout << "bvh left: "<< bvhnode.start << " to " << bvhnode.start + num_bvh_left << endl;
  cout << "bvh right: "<< bvhnode.start + num_bvh_left + 1 << " to " << bvhnode.start + num_bvh_left + num_bvh_right  << endl;

  //getchar();
  bvhnode.l = new BVHNode(bvh_left_bbox, bvhnode.start, num_bvh_left - 1);
  bvhnode.r = new BVHNode(bvh_right_bbox, bvhnode.start + num_bvh_left, num_bvh_right);

  partition(*bvhnode.l, max_leaf_size);
  partition(*bvhnode.r, max_leaf_size);
}

inline int BVHAccel::computeBucket(double this_centroid, double axis_min, double axis_max) {
  //cout << "axis_max: " << axis_max << endl;
  //cout << "axis_min: " << axis_min << endl;
  //cout << "this_centroid: " << this_centroid << endl;
  return floor((double) N_BINS * (this_centroid - axis_min) / (axis_max - axis_min));
}

BBox BVHAccel::get_bbox() const { return root->bb; }

bool BVHAccel::intersect(const Ray &ray) const {
  // TODO (PathTracer):
  // Implement ray - bvh aggregate intersection test. A ray intersects
  // with a BVH aggregate if and only if it intersects a primitive in
  // the BVH that is not an aggregate.

  bool hit = false;
  for (size_t p = 0; p < primitives.size(); ++p) {
    if (primitives[p]->intersect(ray)) hit = true;
  }

  return hit;
}

bool BVHAccel::intersect(const Ray &ray, Intersection *isect) const {
  // TODO (PathTracer):
  // Implement ray - bvh aggregate intersection test. A ray intersects
  // with a BVH aggregate if and only if it intersects a primitive in
  // the BVH that is not an aggregate. When an intersection does happen.
  // You should store the non-aggregate primitive in the intersection data
  // and not the BVH aggregate itself.

  bool hit = false;
  for (size_t p = 0; p < primitives.size(); ++p) {
    if (primitives[p]->intersect(ray, isect)) hit = true;
  }

  return hit;
}

}  // namespace StaticScene
}  // namespace CMU462
