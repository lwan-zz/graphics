#include <float.h>
#include <assert.h>
#include "meshEdit.h"
#include "mutablePriorityQueue.h"
#include "error_dialog.h"

namespace CMU462 {
void collectElements(all_elements &ae, HalfedgeIter &he) {
  // Take references to empty vector objects and add all elements in a closed-
  // loop walk around the face, given a pointer to he on face.
  HalfedgeIter he_start = he;
  do {
    ae.he.push_back(he);
    ae.face.push_back(he->face());
    ae.vertex.push_back(he->vertex());
    ae.edge.push_back(he->edge());
    ae.twin.push_back(he->twin());
    ae.next.push_back(he->next());
    he = he->next();
  } while (he != he_start);  

}

int loopCheck (HalfedgeIter &he) {
  HalfedgeIter temp_he = he;
  int count = 0;
  cout << "checkloop he @ " << elementAddress(he) << endl;
  do {
    count ++;
    cout << elementAddress(temp_he) << endl;
    temp_he = temp_he->next();

  } while ( temp_he != he);

  cout << count << " halfedges in loop" << endl;
  return count;
}

VertexIter HalfedgeMesh::splitEdge(EdgeIter e0) {
  // TODO: (meshEdit)
  // This method should split the given edge and return an iterator to the
  // newly inserted vertex. The halfedge of this vertex should point along
  // the edge that was split, rather than the new edges.
  if (e0->halfedge()->isBoundary() || e0->halfedge()->face()->isBoundary() ||
      e0->halfedge()->twin()->face()->isBoundary()) {
      showError("Cannot split edge: on (boundary");
      return e0->halfedge()->vertex();
  }

  all_elements ae1;
  all_elements ae2;
  
  // Collect all features that will be affected:
  HalfedgeIter he1 = e0->halfedge();
  HalfedgeIter he2 = he1->twin();

  collectElements(ae1, he1);
  collectElements(ae2, he2);
  
  // Allocate new elements (vertex, faces, halfedges)
  all_elements ae3; // loop3
  all_elements ae4; // loop4

  int num_he = 3;
  
  // Add new halfedges to ae3, ae4
  for (int ii=0; ii!=num_he; ii++) {
    HalfedgeIter new_he = newHalfedge();
    ae3.he.push_back(new_he);
    ae4.he.push_back(new_he);
  }
  // Allocate new vertex, edges, faces
  VertexIter v5 = newVertex();
  FaceIter face3 = newFace();
  FaceIter face4 = newFace();
  EdgeIter edge14 = newEdge();
  EdgeIter edge23 = newEdge();
  EdgeIter edge34 = newEdge();

  //cout << elementAddress(ae3.he[0]->next()) << " " << elementAddress(ae3.he[1]->next()) << endl;
  //getchar();

  // set vertex position
  v5->position = (ae1.vertex[0]->position + ae1.vertex[1]->position) / 2;
  cout << v5->position[0] << " " << v5->position[1] << " " << v5->position[2] << endl;
      
  // First set halfedge-element associations
  ae1.he[0]->setNeighbors(ae1.he[1], ae2.he[0], ae1.vertex[0], ae1.edge[0], ae1.face[0]);
  ae1.he[1]->setNeighbors(ae1.he[2], ae4.he[0], v5, edge14, ae1.face[0]);
  ae1.he[2]->setNeighbors(ae1.he[0], ae1.twin[2], ae1.vertex[2], ae1.edge[2], ae1.face[0]);

  ae2.he[0]->setNeighbors(ae2.he[1], ae1.he[0], v5, ae2.edge[0], ae2.face[0]);
  ae2.he[1]->setNeighbors(ae2.he[2], ae2.twin[1], ae2.vertex[1], ae2.edge[1], ae2.face[0]);
  ae2.he[2]->setNeighbors(ae2.he[0], ae3.he[0], ae2.vertex[2], edge23, ae2.face[0]);

  ae3.he[0]->setNeighbors(ae3.he[1], ae2.he[2], v5, edge23, face3);
  ae3.he[1]->setNeighbors(ae3.he[2], ae2.twin[2], ae2.vertex[2], ae2.edge[2], face3);
  ae3.he[2]->setNeighbors(ae3.he[0], ae4.he[1], ae2.vertex[0], edge34, face3);

  ae4.he[0]->setNeighbors(ae4.he[1], ae1.he[1], ae1.vertex[2], edge14, face4);
  ae4.he[1]->setNeighbors(ae4.he[2], ae3.he[2], v5, edge34, face4);
  ae4.he[2]->setNeighbors(ae4.he[0], ae1.twin[1], ae2.vertex[0], ae1.edge[1], face4);

  // Set outside halfedge associations // double check the face assocations. don't make sense
  ae1.twin[2]->setNeighbors(ae1.twin[2]->next(), ae1.he[2], ae1.vertex[0], ae1.edge[2], ae1.twin[2]->face());
  ae1.twin[1]->setNeighbors(ae1.twin[1]->next(), ae4.he[2], ae1.vertex[2], ae1.edge[1], ae1.twin[1]->face());
  //cout << elementAddress(ae1.vertex[2])<< " " << elementAddress(ae1.twin[1]->vertex()) << endl;
  ae2.twin[2]->setNeighbors(ae2.twin[2]->next(), ae3.he[1], ae2.vertex[0], ae2.edge[2], ae2.twin[2]->face());
  //cout << elementAddress(ae2.vertex[0])<< " " << elementAddress(ae3.he[2]->vertex()) << endl;
  ae2.twin[1]->setNeighbors(ae2.twin[1]->next(), ae2.he[1], ae2.vertex[2], ae2.edge[1], ae2.twin[1]->face());

  // Set element halfedge assocations
  // outside square edges`
  ae1.edge[2]->halfedge() = ae1.he[2];
  ae1.edge[1]->halfedge() = ae4.he[2];
  ae2.edge[2]->halfedge() = ae3.he[1];
  ae2.edge[1]->halfedge() = ae2.he[1];

  // inside "x"
  ae1.edge[0]->halfedge() = ae1.he[0];
  edge14->halfedge() = ae1.he[1];
  edge23->halfedge() = ae3.he[0];
  edge34->halfedge() = ae4.he[0];

  // set vertex assocations
  ae1.vertex[0]->halfedge() = ae1.he[0];
  ae1.vertex[2]->halfedge() = ae1.he[2];
  ae1.vertex[1]->halfedge() = ae3.he[2];
  ae2.vertex[2]->halfedge() = ae2.he[2];

  // set face assocations
  ae1.face[0]->halfedge() = ae1.he[0];
  ae2.face[0]->halfedge() = ae2.he[0];
  face3->halfedge() = ae3.he[0];
  face4->halfedge() = ae4.he[0];
  
  // check loops!!

  loopCheck(ae1.he[0]);
  loopCheck(ae2.he[0]);
  loopCheck(ae3.he[0]);
  loopCheck(ae4.he[0]);
  loopCheck(ae1.he[2]->twin());
  loopCheck(ae2.he[1]->twin());
  loopCheck(ae3.he[1]->twin());
  loopCheck(ae4.he[2]->twin());

  cout << "edge split" << endl;
  loopCheck(v5->halfedge());
  
  return v5; 
}
  /*
      if (e0->isBoundary())
      return e0->halfedge()->vertex();

    if (e0->halfedge()->face()->isBoundary() || e0->halfedge()->twin()->face()->isBoundary())
      return e0->halfedge()->vertex();

    // collect elements
    HalfedgeIter h0 = e0->halfedge();
    HalfedgeIter h1 = h0->next();
    HalfedgeIter h2 = h1->next();
    HalfedgeIter h3 = h0->twin();
    HalfedgeIter h4 = h3->next();
    HalfedgeIter h5 = h4->next();
    HalfedgeIter h6 = h5->twin();
    HalfedgeIter h7 = h4->twin();
    HalfedgeIter h8 = h2->twin();
    HalfedgeIter h9 = h1->twin();

    VertexIter v0 = h2->vertex();
    VertexIter v1 = h0->vertex();
    VertexIter v2 = h3->vertex();
    VertexIter v3 = h5->vertex();

    EdgeIter e1 = h2->edge();
    EdgeIter e2 = h4->edge();
    EdgeIter e3 = h5->edge();
    EdgeIter e4 = h1->edge();

    FaceIter f0 = h0->face();
    FaceIter f1 = h3->face();

    //allocate new elements
    HalfedgeIter h10 = newHalfedge();
    HalfedgeIter h11 = newHalfedge();
    HalfedgeIter h12 = newHalfedge();
    HalfedgeIter h13 = newHalfedge();
    HalfedgeIter h14 = newHalfedge();
    HalfedgeIter h15 = newHalfedge();

    VertexIter v4 = newVertex();

    EdgeIter e5 = newEdge();
    EdgeIter e6 = newEdge();
    EdgeIter e7 = newEdge();

    FaceIter f2 = newFace();
    FaceIter f3 = newFace();

    // reassign elements
    h0->setNeighbors(h1, h3, v1, e0, f0);
    h1->setNeighbors(h2, h15, v4, e7, f0);
    h2->setNeighbors(h0, h8, v0, e1, f0);
    h3->setNeighbors(h4, h0, v4, e0, f1);
    h4->setNeighbors(h5, h7, v1, e2, f1);
    h5->setNeighbors(h3, h10, v3, e5, f1);
    h6->setNeighbors(h6->next(), h11, v2, e3, f2);
    h7->setNeighbors(h7->next(), h4, v3, e2, f1);
    h8->setNeighbors(h8->next(), h2, v1, e1, f0);
    h9->setNeighbors(h9->next(), h14, v0, e4, f3);
    h10->setNeighbors(h11, h5, v4, e5, f2);
    h11->setNeighbors(h12, h6, v3, e3, f2);
    h12->setNeighbors(h10, h13, v2, e6, f2);
    h13->setNeighbors(h14, h12, v4, e6, f3);
    h14->setNeighbors(h15, h9, v2, e4, f3);
    h15->setNeighbors(h13, h1, v0, e7, f3);

    v0->halfedge() = h2;
    v1->halfedge() = h0;
    v2->halfedge() = h12;
    v3->halfedge() = h5;
    v4->halfedge() = h3;

    e0->halfedge() = h0;
    e1->halfedge() = h2;
    e2->halfedge() = h4;
    e3->halfedge() = h11;
    e4->halfedge() = h14;
    e5->halfedge() = h5;
    e6->halfedge() = h12;
    e7->halfedge() = h1;

    f0->halfedge() = h0;
    f1->halfedge() = h3;
    f2->halfedge() = h10;
    f3->halfedge() = h13;
    
    v4->position = 0.5f * (v1->position + v2->position);
r=    return v4;*/


VertexIter HalfedgeMesh::collapseEdge(EdgeIter e) {
  // TODO: (meshEdit)
  // This method should collapse the given edge and return an iterator to
  // the new vertex created by the collapse.
  all_elements ae1;
  all_elements ae2;
  
  // Collect all features
  HalfedgeIter he1 = e->halfedge();
  HalfedgeIter he2 = he1->twin();

  collectElements(ae1, he1);
  collectElements(ae2, he2);
    // TODO: (meshEdit)
    // This method should collapse the given edge and return an iterator to
    // the new vertex created by the collapse.
    if (e->isBoundary())
      return e->halfedge()->vertex();

    if (e->halfedge()->face()->isBoundary() || e->halfedge()->twin()->face()->isBoundary())
      return e->halfedge()->vertex();

    // collect elements
    HalfedgeIter h0 = e->halfedge();
    HalfedgeIter h1 = h0->next();
    HalfedgeIter h2 = h1->next();
    HalfedgeIter h3 = h0->twin();
    HalfedgeIter h4 = h3->next();
    HalfedgeIter h5 = h4->next();
    HalfedgeIter h6 = h5->twin();
    HalfedgeIter h7 = h4->twin();
    HalfedgeIter h8 = h2->twin();
    HalfedgeIter h9 = h1->twin();
    HalfedgeIter h10 = h9->next();
    HalfedgeIter h11 = h10->twin();
    HalfedgeIter h12 = h11->next();
    HalfedgeIter h13 = h12->twin();
    HalfedgeIter h14 = h13->next();
    HalfedgeIter h15 = h14->twin();
    HalfedgeIter h16 = h7->next();
    HalfedgeIter h17 = h16->twin();
    HalfedgeIter h18 = h17->next();
    HalfedgeIter h19 = h18->twin();
    HalfedgeIter h20 = h19->next();
    HalfedgeIter h21 = h20->twin();

    VertexIter v0 = h5->vertex();
    VertexIter v1 = h2->vertex();
    VertexIter v2 = h3->vertex();
    VertexIter v3 = h0->vertex();

    EdgeIter e1 = h5->edge();
    EdgeIter e2 = h1->edge();
    EdgeIter e3 = h2->edge();
    EdgeIter e4 = h4->edge();

    FaceIter f0 = h0->face();
    FaceIter f1 = h3->face();

    // reassign elements
    h6->setNeighbors(h6->next(), h7, v2, e1, h6->face());
    h7->setNeighbors(h7->next(), h6, v0, e1, h7->face());
    h8->setNeighbors(h8->next(), h9, v2, e2, h8->face());
    h9->setNeighbors(h9->next(), h8, v1, e2, h9->face());
    h10->setNeighbors(h10->next(), h11, v2, h10->edge(), h10->face());
    h11->setNeighbors(h11->next(), h10, h11->vertex(), h11->edge(), h11->face());
    h12->setNeighbors(h12->next(), h13, v2, h12->edge(), h12->face());
    h13->setNeighbors(h13->next(), h12, h13->vertex(), h13->edge(), h13->face());
    h14->setNeighbors(h14->next(), h15, v2, h14->edge(), h14->face());
    h15->setNeighbors(h6, h14, h15->vertex(), h15->edge(), h15->face());
    h16->setNeighbors(h16->next(), h17, v2, h16->edge(), h16->face());
    h17->setNeighbors(h17->next(), h16, h17->vertex(), h17->edge(), h17->face());
    h18->setNeighbors(h18->next(), h19, v2, h18->edge(), h18->face());
    h19->setNeighbors(h19->next(), h18, h19->vertex(), h19->edge(), h19->face());
    h20->setNeighbors(h20->next(), h21, v2, h20->edge(), h20->face());
    h21->setNeighbors(h8, h20, h21->vertex(), h21->edge(), h21->face());

    v0->halfedge() = h7;
    v1->halfedge() = h9;
    v2->halfedge() = h6;
    v2->position = 0.5f * (v2->position + v3->position);

    e1->halfedge() = h6;
    e2->halfedge() = h8;


    // delete unused elements
    deleteHalfedge(h0);
    deleteHalfedge(h1);
    deleteHalfedge(h2);
    deleteHalfedge(h3);
    deleteHalfedge(h4);
    deleteHalfedge(h5);

    deleteVertex(v3);

    deleteEdge(e);
    deleteEdge(e3);
    deleteEdge(e4);

    deleteFace(f0);
    deleteFace(f1);

    return v2;
}

VertexIter HalfedgeMesh::collapseFace(FaceIter f) {
  // TODO: (meshEdit)
  // This method should collapse the given face and return an iterator to
  // the new vertex created by the collapse.
  showError("collapseFace() not implemented.");
  return VertexIter();
}

FaceIter HalfedgeMesh::eraseVertex(VertexIter v) {
  // TODO: (meshEdit)
  // This method should replace the given vertex and all its neighboring
  // edges and faces with a single face, returning the new face.


  return FaceIter();
}

FaceIter HalfedgeMesh::eraseEdge(EdgeIter e) {
  // TODO: (meshEdit)
  // This method should erase the given edge and return an iterator to the
  // merged face.
  if (e->halfedge()->isBoundary()) {
    showError("Cannot erase boundary");
    return e->halfedge()->face();
  }
  all_elements ae1;
  all_elements ae2;
  HalfedgeIter he = e->halfedge();

  collectElements(ae1, he);
  collectElements(ae2, he->twin());

  cout << ae1.he.size() << endl;
  cout << ae2.he.size() << endl;

  FaceIter newface = newFace();

  ae1.he[1]->setNeighbors(ae1.he[2], ae1.twin[1], ae1.vertex[1], ae1.edge[1], newface);
  ae1.he[2]->setNeighbors(ae2.he[1], ae1.twin[2], ae1.vertex[2], ae1.edge[2], newface);
  ae2.he[1]->setNeighbors(ae2.he[2], ae2.twin[1], ae2.vertex[1], ae2.edge[1], newface);
  ae2.he[2]->setNeighbors(ae1.he[1], ae2.twin[2], ae2.vertex[2], ae2.edge[2], newface);

  cout << "done deleting edge" << endl;
  return newface;
}

EdgeIter HalfedgeMesh::flipEdge(EdgeIter e0) {
  // This method should flip the given edge and return an iterator to the
  // flipped edge.
  // Early exit if boundary. not dealing with that
  if (e0->halfedge()->isBoundary() || e0->halfedge()->face()->isBoundary() ||
      e0->halfedge()->twin()->face()->isBoundary()) {
      showError("Cannot flip edge: on boundary");
      return e0;
  }
  
  all_elements ae1;
  all_elements ae2;
  
  // Collect all features that will be affected:
  HalfedgeIter he1 = e0->halfedge();
  HalfedgeIter he2 = he1->twin();

  collectElements(ae1, he1);
  collectElements(ae2, he2);

  // Set halfedge associations
  ae1.he[0]->setNeighbors(ae1.he[1], ae2.he[0], ae2.vertex[2], e0, ae1.face[0]);
  ae1.he[1]->setNeighbors(ae1.he[2], ae1.twin[2], ae1.vertex[2], ae1.edge[2], ae1.face[0]);
  ae1.he[2]->setNeighbors(ae1.he[0], ae2.twin[1], ae1.vertex[0], ae2.edge[1], ae1.face[0]);
  ae2.he[0]->setNeighbors(ae2.he[1], ae1.he[0], ae1.vertex[2], e0, ae2.face[0]);
  ae2.he[1]->setNeighbors(ae2.he[2], ae2.twin[2], ae2.vertex[2], ae2.edge[2], ae2.face[0]);
  ae2.he[2]->setNeighbors(ae2.he[0], ae1.twin[1], ae2.vertex[0], ae1.edge[1], ae2.face[0]);
  
  // Set halfedge twin associations
  ae1.twin[1]->setNeighbors(ae1.twin[1]->next(), ae2.he[2], ae1.vertex[2], ae1.edge[1], ae1.twin[1]->face());
  ae1.twin[2]->setNeighbors(ae1.twin[2]->next(), ae1.he[1], ae1.vertex[0], ae1.edge[2], ae1.twin[2]->face());
  ae2.twin[1]->setNeighbors(ae2.twin[1]->next(), ae1.he[2], ae2.vertex[2], ae2.edge[1], ae2.twin[1]->face());
  ae2.twin[2]->setNeighbors(ae2.twin[2]->next(), ae2.he[1], ae2.vertex[0], ae2.edge[2], ae2.twin[2]->face());

  // Set vertices
  ae1.vertex[2]->halfedge() = ae1.he[1];
  ae1.vertex[0]->halfedge() = ae1.he[2];
  ae2.vertex[2]->halfedge() = ae2.he[1];
  ae2.vertex[0]->halfedge() = ae2.he[2];

  // Set edges
  e0->halfedge() = ae1.he[0];
  ae1.edge[1]->halfedge() = ae2.he[2];
  ae1.edge[2]->halfedge() = ae1.he[1];
  ae2.edge[1]->halfedge() = ae1.he[2];
  ae2.edge[2]->halfedge() = ae2.he[1];

  // Set faces
  ae1.face[0]->halfedge() = ae1.he[0];
  ae2.face[0]->halfedge() = ae2.he[0];

  return e0;
}

void HalfedgeMesh::subdivideQuad(bool useCatmullClark) {
  // Unlike the local mesh operations (like bevel or edge flip), we will perform
  // subdivision by splitting *all* faces into quads "simultaneously."  Rather
  // than operating directly on the halfedge data structure (which as you've
  // seen
  // is quite difficult to maintain!) we are going to do something a bit nicer:
  //
  //    1. Create a raw list of vertex positions and faces (rather than a full-
  //       blown halfedge mesh).
  //
  //    2. Build a new halfedge mesh from these lists, replacing the old one.
  //
  // Sometimes rebuilding a data structure from scratch is simpler (and even
  // more
  // efficient) than incrementally modifying the existing one.  These steps are
  // detailed below.

  // TODO Step I: Compute the vertex positions for the subdivided mesh.  Here
  // we're
  // going to do something a little bit strange: since we will have one vertex
  // in
  // the subdivided mesh for each vertex, edge, and face in the original mesh,
  // we
  // can nicely store the new vertex *positions* as attributes on vertices,
  // edges,
  // and faces of the original mesh.  These positions can then be conveniently
  // copied into the new, subdivided mesh.
  // [See subroutines for actual "TODO"s]
  if (useCatmullClark) {
    computeCatmullClarkPositions();
  } else {
    computeLinearSubdivisionPositions();
  }

  // TODO Step II: Assign a unique index (starting at 0) to each vertex, edge,
  // and
  // face in the original mesh.  These indices will be the indices of the
  // vertices
  // in the new (subdivided mesh).  They do not have to be assigned in any
  // particular
  // order, so long as no index is shared by more than one mesh element, and the
  // total number of indices is equal to V+E+F, i.e., the total number of
  // vertices
  // plus edges plus faces in the original mesh.  Basically we just need a
  // one-to-one
  // mapping between original mesh elements and subdivided mesh vertices.
  // [See subroutine for actual "TODO"s]
  assignSubdivisionIndices();

  // TODO Step III: Build a list of quads in the new (subdivided) mesh, as
  // tuples of
  // the element indices defined above.  In other words, each new quad should be
  // of
  // the form (i,j,k,l), where i,j,k and l are four of the indices stored on our
  // original mesh elements.  Note that it is essential to get the orientation
  // right
  // here: (i,j,k,l) is not the same as (l,k,j,i).  Indices of new faces should
  // circulate in the same direction as old faces (think about the right-hand
  // rule).
  // [See subroutines for actual "TODO"s]
  vector<vector<Index> > subDFaces;
  vector<Vector3D> subDVertices;
  buildSubdivisionFaceList(subDFaces);
  cout << "done subdiciisnon facelist"  << endl;
  buildSubdivisionVertexList(subDVertices);

  // TODO Step IV: Pass the list of vertices and quads to a routine that clears
  // the
  // internal data for this halfedge mesh, and builds new halfedge data from
  // scratch,
  // using the two lists.
  rebuild(subDFaces, subDVertices);
}

/**
 * Compute new vertex positions for a mesh that splits each polygon
 * into quads (by inserting a vertex at the face midpoint and each
 * of the edge midpoints).  The new vertex positions will be stored
 * in the members Vertex::newPosition, Edge::newPosition, and
 * Face::newPosition.  The values of the positions are based on
 * simple linear interpolation, e.g., the edge midpoints and face
 * centroids.
 */
void HalfedgeMesh::computeLinearSubdivisionPositions() {
  // TODO For each vertex, assign Vertex::newPosition to
  // its original position, Vertex::position.
  for (VertexIter vi = verticesBegin(); vi != verticesEnd(); vi++) {
    vi->newPosition = vi->position;
  }
  // TODO For each edge, assign the midpoint of the two original
  // positions to Edge::newPosition.
  for (EdgeIter ei = edgesBegin(); ei != edgesEnd(); ei++) {
    ei->newPosition = ei->centroid();
  }
  // TODO For each face, assign the centroid (i.e., aithmetic mean)
  // of the original vertex positions to Face::newPosition.  Note
  // that in general, NOT all faces will be triangles!
  for (FaceIter fi = facesBegin(); fi != facesEnd(); fi ++) {
    fi->newPosition = fi->centroid();
  }
}

/**
 * Compute new vertex positions for a mesh that splits each polygon
 * into quads (by inserting a vertex at the face midpoint and each
 * of the edge midpoints).  The new vertex positions will be stored
 * in the members Vertex::newPosition, Edge::newPosition, and
 * Face::newPosition.  The values of the positions are based on
 * the Catmull-Clark rules for subdivision.
 */
void HalfedgeMesh::computeCatmullClarkPositions() {
  // TODO The implementation for this routine should be
  // a lot like HalfedgeMesh::computeLinearSubdivisionPositions(),
  // except that the calculation of the positions themsevles is
  // slightly more involved, using the Catmull-Clark subdivision
  // rules. (These rules are outlined in the Developer Manual.)

  // TODO face

  // TODO edges

  // TODO vertices
  showError("computeCatmullClarkPositions() not implemented.");
}

/**
 * Assign a unique integer index to each vertex, edge, and face in
 * the mesh, starting at 0 and incrementing by 1 for each element.
 * These indices will be used as the vertex indices for a mesh
 * subdivided using Catmull-Clark (or linear) subdivision.
 */
void HalfedgeMesh::assignSubdivisionIndices() {
  // TODO Start a counter at zero; if you like, you can use the
  // "Index" type (defined in halfedgeMesh.h)
  Index meshindex = 0;
  // TODO Iterate over vertices, assigning values to Vertex::index

  // TODO Iterate over edges, assigning values to Edge::index

  // TODO Iterate over faces, assigning values to Face::index
  for (VertexIter vi = verticesBegin(); vi != verticesEnd(); vi++) {
    vi->index = meshindex;
    meshindex ++;
  }

  for (EdgeIter ei = edgesBegin(); ei != edgesEnd(); ei++) {
    ei->index = meshindex;
    meshindex ++;
  }

  for (FaceIter fi = facesBegin(); fi != facesEnd(); fi ++) {
    fi->index = meshindex;
    meshindex ++;
  }

}

/**
 * Build a flat list containing all the vertex positions for a
 * Catmull-Clark (or linear) subdivison of this mesh.  The order of
 * vertex positions in this list must be identical to the order
 * of indices assigned to Vertex::newPosition, Edge::newPosition,
 * and Face::newPosition.
 */
void HalfedgeMesh::buildSubdivisionVertexList(vector<Vector3D>& subDVertices) {
  // TODO Resize the vertex list so that it can hold all the vertices.

  // TODO Iterate over vertices, assigning Vertex::newPosition to the
  // appropriate location in the new vertex list.

  // TODO Iterate over edges, assigning Edge::newPosition to the appropriate
  // location in the new vertex list.

  // TODO Iterate over faces, assigning Face::newPosition to the appropriate
  // location in the new vertex list.
  showError("buildSubdivisionVertexList() not implemented.");
}

/**
 * Build a flat list containing all the quads in a Catmull-Clark
 * (or linear) subdivision of this mesh.  Each quad is specified
 * by a vector of four indices (i,j,k,l), which come from the
 * members Vertex::index, Edge::index, and Face::index.  Note that
 * the ordering of these indices is important because it determines
 * the orientation of the new quads; it is also important to avoid
 * "bowties."  For instance, (l,k,j,i) has the opposite orientation
 * of (i,j,k,l), and if (i,j,k,l) is a proper quad, then (i,k,j,l)
 * will look like a bowtie.
 */
void HalfedgeMesh::buildSubdivisionFaceList(vector<vector<Index> >& subDFaces) {
  // TODO This routine is perhaps the most tricky step in the construction of
  // a subdivision mesh (second, perhaps, to computing the actual Catmull-Clark
  // vertex positions).  Basically what you want to do is iterate over faces,
  // then for each for each face, append N quads to the list (where N is the
  // degree of the face).  For this routine, it may be more convenient to simply
  // append quads to the end of the list (rather than allocating it ahead of
  // time), though YMMV.  You can of course iterate around a face by starting
  // with its first halfedge and following the "next" pointer until you get
  // back to the beginning.  The tricky part is making sure you grab the right
  // indices in the right order---remember that there are indices on vertices,
  // edges, AND faces of the original mesh.  All of these should get used.  Also
  // remember that you must have FOUR indices per face, since you are making a
  // QUAD mesh!
  for (FaceIter fi = facesBegin(); fi != facesEnd() ; fi++) {
    HalfedgeIter he = fi->halfedge();

    all_elements ae;
    collectElements(ae, he);
    vector<Index> quad(4);

    for (int ii=0; ii != ae.he.size(); ii++) {
      quad[0] = ae.he[ii]->vertex()->index;
      quad[1] = ae.he[ii]->edge()->index;
      quad[2] = ae.he[ii]->face()->index;

      if (ii == 0) {
        quad[3] = ae.he.back()->edge()->index;
      } else {
        quad[3] = ae.he[ii-1]->edge()->index;
      }

    subDFaces.push_back(quad);
    }
  }

  // TODO iterate over faces
  // TODO loop around face
  // TODO build lists of four indices for each sub-quad
  // TODO append each list of four indices to face list
  showError("buildSubdivisionFaceList() not implemented.");
}

FaceIter HalfedgeMesh::bevelVertex(VertexIter v) {
  // TODO This method should replace the vertex v with a face, corresponding to
  // a bevel operation. It should return the new face.  NOTE: This method is
  // responsible for updating the *connectivity* of the mesh only---it does not
  // need to update the vertex positions.  These positions will be updated in
  // HalfedgeMesh::bevelVertexComputeNewPositions (which you also have to
  // implement!)

  showError("bevelVertex() not implemented.");
  return facesBegin();
}

FaceIter HalfedgeMesh::bevelEdge(EdgeIter e) {
  // TODO This method should replace the edge e with a face, corresponding to a
  // bevel operation. It should return the new face.  NOTE: This method is
  // responsible for updating the *connectivity* of the mesh only---it does not
  // need to update the vertex positions.  These positions will be updated in
  // HalfedgeMesh::bevelEdgeComputeNewPositions (which you also have to
  // implement!)

  showError("bevelEdge() not implemented.");
  return facesBegin();
}

FaceIter HalfedgeMesh::bevelFace(FaceIter f) {
  // TODO This method should replace the face f with an additional, inset face
  // (and ring of faces around it), corresponding to a bevel operation. It
  // should return the new face.  NOTE: This method is responsible for updating
  // the *connectivity* of the mesh only---it does not need to update the vertex
  // positions.  These positions will be updated in
  // HalfedgeMesh::bevelFaceComputeNewPositions (which you also have to
  // implement!)
  all_elements outside;
  all_elements inside;
  
  // Collect all features on face;
  HalfedgeIter he1 = f->halfedge();
  collectElements(outside, he1);
  FaceIter inside_face = newFace();

  // Allocate new elements of inside face (without connections)
  for (int ii=0; ii!=outside.he.size(); ii++) {
    HalfedgeIter temphe = newHalfedge();
    HalfedgeIter temphe_twin = newHalfedge();
    VertexIter tempvertex = newVertex();
    EdgeIter tempedge = newEdge();
    
    inside.he.push_back(temphe);
    inside.vertex.push_back(tempvertex);
    inside.edge.push_back(tempedge);
    inside.twin.push_back(temphe_twin);
    inside.face.push_back(inside_face);
  }

  // Connect elements on the inside
  // Twins are not associated yet, and new faces are not yet added. 
  for (int ii=0; ii!=inside.he.size(); ii++) {
    if (ii!= inside.he.size()-1) {
      inside.he[ii]->setNeighbors(inside.he[ii+1], inside.twin[ii], inside.vertex[ii], inside.edge[ii], inside.face[ii]);
    }
    else {
      inside.he[ii]->setNeighbors(inside.he.front(), inside.twin[ii], inside.vertex[ii], inside.edge[ii], inside.face[ii]);
    }
  }

  // Construct halfedges, edges connecting inside / outside 
  vector<HalfedgeIter> he_connectors; // Little vector for convenient access to the connecting halfedges
  for (int ii=0; ii!=inside.he.size(); ii++) {
    all_elements bevel;

    HalfedgeIter out = newHalfedge();
    HalfedgeIter in = newHalfedge();
    EdgeIter tempedge = newEdge();
    FaceIter tempface = newFace();
    he_connectors.push_back(out);
    out->setNeighbors(outside.he[ii], in, inside.vertex[ii], tempedge, tempface);

    // Can't set face of the twin for the first iteration, since face hasn't been allocated yet. 
    if (ii!=0) {
      in->setNeighbors(inside.he[ii-1]->twin(), out, outside.vertex[ii], tempedge, he_connectors[ii-1]->face());
    }
    
  }
  // Now connect the rest of the elements for the twin halfedge of the first halfedge
  he_connectors[0]->twin()->setNeighbors(inside.he.back()->twin(), he_connectors[0], outside.vertex[0], 
                                         he_connectors[0]->edge(), he_connectors.back()->face());

  // Reconnect all of the "outside" halfedges to their new neighbors, faces
  for (int ii=0; ii!=outside.he.size(); ii++) {
    if (ii < outside.he.size() - 1) {
      outside.he[ii]->setNeighbors(he_connectors[ii+1]->twin(), outside.twin[ii], outside.vertex[ii], outside.edge[ii],
                                   he_connectors[ii]->face());
    }
    else {
      outside.he[ii]->setNeighbors(he_connectors.front()->twin(), outside.twin[ii], outside.vertex[ii], outside.edge[ii],
                                   he_connectors[ii]->face()); 
    }
  }

  // Connect inside twins to neighbors
  for (int ii=0; ii!=inside.twin.size(); ii++) {
    if (ii < inside.twin.size() -1 ) {
      inside.twin[ii]->setNeighbors(he_connectors[ii], inside.he[ii], inside.vertex[ii+1], inside.edge[ii], he_connectors[ii]->face());
    }
    else {
      inside.twin[ii]->setNeighbors(he_connectors[ii], inside.he[ii], inside.vertex.front(), inside.edge[ii], he_connectors[ii]->face());
    }
  }

  // check all loops
  //for (int ii=0; ii!=he_connectors.size(); ii++) {
  //  cout << "checking bevel loop: " << ii << endl;
  //  loopCheck(he_connectors[ii]);
  //}
  //cout << "checking inside loop" << endl;
  //loopCheck(inside.he[0]);

  // Assign edges, vertices, faces to their halfedges
  for (int ii=0; ii!=inside.he.size(); ii++) {
    // Assign for inside loop
    inside.vertex[ii]->halfedge() = inside.he[ii];
    inside.edge[ii]->halfedge() = inside.he[ii];
    // Assign for outside loop
    outside.vertex[ii]->halfedge() = outside.he[ii];
    outside.edge[ii]->halfedge() = outside.he[ii];
    // Assign for added edges between outside/inside
    he_connectors[ii]->edge()->halfedge() = he_connectors[ii];
    he_connectors[ii]->face()->halfedge() = he_connectors[ii];
  }
  // Last face
  inside.face[0]->halfedge() = inside.he[0];
  // double check
  //cout << elementAddress(inside.face[0]) << " " << elementAddress(inside.he[0]->face()) << endl;

  return inside.face[0];
}


void HalfedgeMesh::bevelFaceComputeNewPositions(
    vector<Vector3D>& originalVertexPositions,
    vector<HalfedgeIter>& newHalfedges, double normalShift,
    double tangentialInset) {
  // TODO Compute new vertex positions for the vertices of the beveled face.
  //
  // These vertices can be accessed via newHalfedges[i]->vertex()->position for
  // i = 1, ..., newHalfedges.size()-1.
  //
  // The basic strategy here is to loop over the list of outgoing halfedges,
  // and use the preceding and next vertex position from the original mesh
  // (in the originalVertexPositions array) to compute an offset vertex
  // position.
  //
  // Note that there is a 1-to-1 correspondence between halfedges in
  // newHalfedges and vertex positionsret
  // in orig.  So, you can write loops of the form
  //
  // for( int i = 0; i < newHalfedges.size(); hs++ )
  // {
  //    Vector3D pi = originalVertexPositions[i]; // get the original vertex
  //    position correponding to vertex i
  // }
  //

  // Calculate plane normal by crossing two planar vectors
  // vec1
  Vector3D vec1 = originalVertexPositions[0] - originalVertexPositions[1];
  Vector3D vec2 = originalVertexPositions[2] - originalVertexPositions[1];
  Vector3D normal_vec = cross(vec2, vec1);
  normal_vec.normalize();
  //normal_vec *= 100;

  cout << "normal vec--------------------------------------------------------------" << endl;
  cout << normal_vec[0] << " " << normal_vec[1] << " " << normal_vec[2] << endl;

  /*
  for (int ii =0; ii < newHalfedges.size(); ii++) {
    cout << newHalfedges[0]->vertex()->position << " " << newHalfedges[1]->vertex()->position << " " << newHalfedges[2]->vertex()->position << endl;
    cout << originalVertexPositions[0] << " " << originalVertexPositions[1] << " " << originalVertexPositions[2] << endl;
    cout << isnan(newHalfedges[0]->vertex()->position[0]) << endl;
  }
  */

  if (newHalfedges[0]->vertex()->position[0] == 0 &&
      newHalfedges[0]->vertex()->position[1] == 0 &&
      newHalfedges[0]->vertex()->position[2] == 0 &&
      newHalfedges[1]->vertex()->position[0] == 0 &&
      newHalfedges[1]->vertex()->position[1] == 0 &&
      newHalfedges[1]->vertex()->position[2] == 0 ) {
    for (int ii = 0; ii < newHalfedges.size(); ii++) {
      newHalfedges[ii]->vertex()->position = originalVertexPositions[ii];
    }
  } 


  vector<Vector3D> unit_vecs; 
  for (int ii = 0; ii < newHalfedges.size(); ii ++) {
    int before_idx = (ii + newHalfedges.size() - 1) % newHalfedges.size();
    int after_idx = (ii + 1) % newHalfedges.size();

    Vector3D new_vertex;
    Vector3D tan_delta = (newHalfedges[ii]->vertex()->position + newHalfedges[before_idx]->vertex()->position + 
                          newHalfedges[after_idx]->vertex()->position) / 3;
    tan_delta -= newHalfedges[ii]->vertex()->position;
    tan_delta.normalize();
    tan_delta *= tangentialInset;// * 100;

    Vector3D norm_delta = normal_vec * normalShift;
    cout << "computing tangent, normal, final deltas" << endl;
    cout << normal_vec[0] << " " << normal_vec[1] << " " << normal_vec[2] << endl;
    cout << normalShift << endl;
    cout << tan_delta[0] << " " << tan_delta[1] << " " << tan_delta[2] << endl;
    cout << norm_delta[0] << " " << norm_delta[1] << " " << norm_delta[2] << endl;
    
    //new_vertex = originalVertexPositions[ii] + norm_delta + tan_delta;
    cout << new_vertex[0] << " " << new_vertex[1] << " " << new_vertex[2] << endl;
    newHalfedges[ii]->vertex()->position += (norm_delta + tan_delta);
  }

}

void HalfedgeMesh::bevelVertexComputeNewPositions(
    Vector3D originalVertexPosition, vector<HalfedgeIter>& newHalfedges,
    double tangentialInset) {
  // TODO Compute new vertex positions for the vertices of the beveled vertex.
  //
  // These vertices can be accessed via newHalfedges[i]->vertex()->position for
  // i = 1, ..., hs.size()-1.
  //
  // The basic strategy here is to loop over the list of outgoing halfedges,
  // and use the preceding and next vertex position from the original mesh
  // (in the orig array) to compute an offset vertex position.

}

void HalfedgeMesh::bevelEdgeComputeNewPositions(
    vector<Vector3D>& originalVertexPositions,
    vector<HalfedgeIter>& newHalfedges, double tangentialInset) {
  // TODO Compute new vertex positions for the vertices of the beveled edge.
  //
  // These vertices can be accessed via newHalfedges[i]->vertex()->position for
  // i = 1, ..., newHalfedges.size()-1.
  //
  // The basic strategy here is to loop over the list of outgoing halfedges,
  // and use the preceding and next vertex position from the original mesh
  // (in the orig array) to compute an offset vertex position.
  //
  // Note that there is a 1-to-1 correspondence between halfedges in
  // newHalfedges and vertex positions
  // in orig.  So, you can write loops of the form
  //
  // for( int i = 0; i < newHalfedges.size(); i++ )
  // {
  //    Vector3D pi = originalVertexPositions[i]; // get the original vertex
  //    position correponding to vertex i
  // }
  //

}

void HalfedgeMesh::splitPolygons(vector<FaceIter>& fcs) {
  for (auto f : fcs) splitPolygon(f);
}

void HalfedgeMesh::splitPolygon(FaceIter f) {
  // TODO: (meshedit) 
  // Triangulate a polygonal face
  cout << "degree" << endl;
  cout << f->degree() << endl;
  if (f->degree() != 4) {
    showError("Only splitting polygons with 4 sides, buddy. Quadrilaterals! Four sides!");
    return;
  } 
  all_elements polyface;

  //TODO: FIGURE OUT WHY SCOTTY IS CALLING SPLIT POLYGON WHEN DEGREE = 3
  
  // Collect all features on face;
  HalfedgeIter he1 = f->halfedge();
  collectElements(polyface, he1);
  
  // Allocate new elements
  FaceIter newface = newFace();
  HalfedgeIter newhe = newHalfedge();
  HalfedgeIter newhe_twin = newHalfedge();
  EdgeIter newedge = newEdge();

  // Update connectivity
  // Just handle squares for now
  newhe->setNeighbors(polyface.he[3], newhe_twin, polyface.vertex[1], newedge, polyface.face[0]);
  newhe_twin->setNeighbors(polyface.he[1], newhe, polyface.vertex[3], newedge, newface);

  polyface.he[0]->setNeighbors(newhe, polyface.twin[0], polyface.vertex[0], polyface.edge[0], polyface.face[0]);
  polyface.he[1]->setNeighbors(polyface.he[2], polyface.twin[1], polyface.vertex[1], polyface.edge[1], newface);
  polyface.he[2]->setNeighbors(newhe_twin, polyface.twin[2], polyface.vertex[2], polyface.edge[2], newface);
  polyface.he[3]->setNeighbors(polyface.he[0], polyface.twin[3], polyface.vertex[3], polyface.edge[3], polyface.face[0]);

  // Set associations for new elements, reassociate old face
  polyface.face[0]->halfedge() = polyface.he[0];
  newface->halfedge() = newhe_twin;
  newedge->halfedge() = newhe;

  for (int ii = 0; ii << polyface.he.size(); ii++) {
    polyface.edge[ii]->halfedge() = polyface.he[ii];
    polyface.vertex[ii]->halfedge() = polyface.he[ii];
  }
}

EdgeRecord::EdgeRecord(EdgeIter& _edge) : edge(_edge) {
  // TODO: (meshEdit)
  // Compute the combined quadric from the edge endpoints.
  // -> Build the 3x3 linear system whose solution minimizes the quadric error
  //    associated with these two endpoints.
  // -> Use this system to solve for the optimal position, and store it in
  //    EdgeRecord::optimalPoint.
  // -> Also store the cost associated with collapsing this edg in
  //    EdgeRecord::Cost.
}

void MeshResampler::upsample(HalfedgeMesh& mesh)
// This routine should increase the number of triangles in the mesh using Loop
// subdivision.
{
  // TODO: (meshEdit)
  // Compute new positions for all the vertices in the input mesh, using
  // the Loop subdivision rule, and store them in Vertex::newPosition.
  // -> At this point, we also want to mark each vertex as being a vertex of the
  //    original mesh.
  // -> Next, compute the updated vertex positions associated with edges, and
  //    store it in Edge::newPosition.
  // -> Next, we're going to split every edge in the mesh, in any order.  For
  //    future reference, we're also going to store some information about which
  //    subdivided edges come from splitting an edge in the original mesh, and
  //    which edges are new, by setting the flat Edge::isNew. Note that in this
  //    loop, we only want to iterate over edges of the original mesh.
  //    Otherwise, we'll end up splitting edges that we just split (and the
  //    loop will never end!)
  // -> Now flip any new edge that connects an old and new vertex.
  // -> Finally, copy the new vertex positions into final Vertex::position.

  // Each vertex and edge of the original surface can be associated with a
  // vertex in the new (subdivided) surface.
  // Therefore, our strategy for computing the subdivided vertex locations is to
  // *first* compute the new positions
  // using the connectity of the original (coarse) mesh; navigating this mesh
  // will be much easier than navigating
  // the new subdivided (fine) mesh, which has more elements to traverse.  We
  // will then assign vertex positions in
  // the new mesh based on the values we computed for the original mesh.

  // Compute updated positions for all the vertices in the original mesh, using
  // the Loop subdivision rule.

  // Next, compute the updated vertex positions associated with edges.

  // Next, we're going to split every edge in the mesh, in any order.  For
  // future
  // reference, we're also going to store some information about which
  // subdivided
  // edges come from splitting an edge in the original mesh, and which edges are
  // new.
  // In this loop, we only want to iterate over edges of the original
  // mesh---otherwise,
  // we'll end up splitting edges that we just split (and the loop will never
  // end!)

  // Finally, flip any new edge that connects an old and new vertex.

  // Copy the updated vertex positions to the subdivided mesh.
  showError("upsample() not implemented.");
}

void MeshResampler::downsample(HalfedgeMesh& mesh) {
  // TODO: (meshEdit)
  // Compute initial quadrics for each face by simply writing the plane equation
  // for the face in homogeneous coordinates. These quadrics should be stored
  // in Face::quadric
  // -> Compute an initial quadric for each vertex as the sum of the quadrics
  //    associated with the incident faces, storing it in Vertex::quadric
  // -> Build a priority queue of edges according to their quadric error cost,
  //    i.e., by building an EdgeRecord for each edge and sticking it in the
  //    queue.
  // -> Until we reach the target edge budget, collapse the best edge. Remember
  //    to remove from the queue any edge that touches the collapsing edge
  //    BEFORE it gets collapsed, and add back into the queue any edge touching
  //    the collapsed vertex AFTER it's been collapsed. Also remember to assign
  //    a quadric to the collapsed vertex, and to pop the collapsed edge off the
  //    top of the queue.
  showError("downsample() not implemented.");
}

void MeshResampler::resample(HalfedgeMesh& mesh) {
  // TODO: (meshEdit)
  // Compute the mean edge length.
  // Repeat the four main steps for 5 or 6 iterations
  // -> Split edges much longer than the target length (being careful about
  //    how the loop is written!)
  // -> Collapse edges much shorter than the target length.  Here we need to
  //    be EXTRA careful about advancing the loop, because many edges may have
  //    been destroyed by a collapse (which ones?)
  // -> Now flip each edge if it improves vertex degree
  // -> Finally, apply some tangential smoothing to the vertex positions
  showError("resample() not implemented.");
}

}  // namespace CMU462
