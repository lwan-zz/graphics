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

void collectElements(vector<HalfedgeIter>& he_vec, vector<VertexIter>& he_vertex, 
                     vector<EdgeIter>& he_edge, vector<HalfedgeIter>& he_twin, 
                     vector<FaceIter>& he_face);

}  // namespace CMU462

#endif  // CMU462_MESHEDIT_H
