#include "viewport.h"

#include "CMU462.h"
#include <iostream>

namespace CMU462 {

void ViewportImp::set_viewbox( float x, float y, float span ) {

  // Task 5 (part 2): 
  // Set svg to normalized device coordinate transformation. Your input
  // arguments are defined as SVG canvans coordinates.
  this->x = x;
  this->y = y;
  this->span = span; 
  
  Matrix3x3 m = Matrix3x3::identity();
  //scale
  m(0, 0) = 1 / (2 * span);
  m(1, 1) = 1 / (2 * span);
  m(0, 2) = - (x - span) / (2 * span);
  m(1, 2) = - (y - span) / (2 * span);
  set_canvas_to_norm(m);
}

void ViewportImp::update_viewbox( float dx, float dy, float scale ) { 
  
  this->x -= dx;
  this->y -= dy;
  this->span *= scale;
  set_viewbox( x, y, span );
}

} // namespace CMU462