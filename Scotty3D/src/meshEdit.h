#ifndef CMU462_MESHEDIT_H
#define CMU462_MESHEDIT_H

#include "halfEdgeMesh.h"

using namespace std;

namespace CMU462 {

class MeshResampler {
 public:
  MeshResampler(){};
  ~MeshResampler() {}

  void upsample(HalfedgeMesh& mesh);
  void downsample(HalfedgeMesh& mesh);
  void resample(HalfedgeMesh& mesh);
};

struct all_elements {
  vector<HalfedgeIter> he;
  vector<VertexIter> vertex;
  vector<EdgeIter> edge;
  vector<HalfedgeIter> twin;
  vector<FaceIter> face;
  vector<HalfedgeIter> next;    
}; // struct everything_he

void collectElements(all_elements &ae, HalfedgeIter &he);
int loopCheck (HalfedgeIter &he);

}  // namespace CMU462

#endif  // CMU462_MESHEDIT_H
