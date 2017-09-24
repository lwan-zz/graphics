#include "software_renderer.h"

#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>

#include "triangulation.h"

using namespace std;

namespace CMU462 {


// Implements SoftwareRenderer //

void SoftwareRendererImp::draw_svg( SVG& svg ) {

  // set top level transformation
  transformation = canvas_to_screen;

  // draw all elements
  for ( size_t i = 0; i < svg.elements.size(); ++i ) {
    draw_element(svg.elements[i]);
  }

  // draw canvas outline
  Vector2D a = transform(Vector2D(    0    ,     0    )); a.x--; a.y--;
  Vector2D b = transform(Vector2D(svg.width,     0    )); b.x++; b.y--;
  Vector2D c = transform(Vector2D(    0    ,svg.height)); c.x--; c.y++;
  Vector2D d = transform(Vector2D(svg.width,svg.height)); d.x++; d.y++;

  rasterize_line(a.x, a.y, b.x, b.y, Color::Black);
  rasterize_line(a.x, a.y, c.x, c.y, Color::Black);
  rasterize_line(d.x, d.y, b.x, b.y, Color::Black);
  rasterize_line(d.x, d.y, c.x, c.y, Color::Black);

  // resolve and send to render target
  resolve();

}

void SoftwareRendererImp::set_sample_rate( size_t sample_rate ) {

  // Task 4: 
  // You may want to modify this for supersampling support
  this->sample_rate = sample_rate;
  cout << "sample rate: " << sample_rate << endl;

  this->big_buffer.resize(4 * this->w * sample_rate * this->h * sample_rate);
  cout << "resized big buffer to: " << this->big_buffer.size() << endl;
  this->set_render_target(&(this->big_buffer[0]), this->w * sample_rate, this->h * sample_rate);
}

void SoftwareRendererImp::set_render_target( unsigned char* render_target,
                                             size_t width, size_t height ) {

  // Task 4: 
  // You may want to modify this for supersampling support
  cout << "new width, height: " << width << " " << height << endl;

  if (!(this->start_flag)) {
    cout << "setting initial width, height" << endl;
    start_flag = true;
    this->w = width;
    this->h = height;
    this->display_target = render_target;
  }

  this->render_target = render_target;
  this->target_w = width;
  this->target_h = height;

}

void SoftwareRendererImp::draw_element( SVGElement* element ) {

  // Task 5 (part 1):
  // Modify this to implement the transformation stack

  switch(element->type) {
    case POINT:
      draw_point(static_cast<Point&>(*element));
      break;
    case LINE:
      draw_line(static_cast<Line&>(*element));
      break;
    case POLYLINE:
      draw_polyline(static_cast<Polyline&>(*element));
      break;
    case RECT:
      draw_rect(static_cast<Rect&>(*element));
      break;
    case POLYGON:
      draw_polygon(static_cast<Polygon&>(*element));
      break;
    case ELLIPSE:
      draw_ellipse(static_cast<Ellipse&>(*element));
      break;
    case IMAGE:
      draw_image(static_cast<Image&>(*element));
      break;
    case GROUP:
      draw_group(static_cast<Group&>(*element));
      break;
    default:
      break;
  }

}


// Primitive Drawing //

void SoftwareRendererImp::draw_point( Point& point ) {

  Vector2D p = transform(point.position);
  rasterize_point( p.x, p.y, point.style.fillColor );

}

void SoftwareRendererImp::draw_line( Line& line ) { 

  Vector2D p0 = transform(line.from);
  Vector2D p1 = transform(line.to);
  rasterize_line( p0.x, p0.y, p1.x, p1.y, line.style.strokeColor );

}

void SoftwareRendererImp::draw_polyline( Polyline& polyline ) {

  Color c = polyline.style.strokeColor;

  if( c.a != 0 ) {
    int nPoints = polyline.points.size();
    for( int i = 0; i < nPoints - 1; i++ ) {
      Vector2D p0 = transform(polyline.points[(i+0) % nPoints]);
      Vector2D p1 = transform(polyline.points[(i+1) % nPoints]);
      rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    }
  }
}

void SoftwareRendererImp::draw_rect( Rect& rect ) {

  Color c;
  
  // draw as two triangles
  float x = rect.position.x;
  float y = rect.position.y;
  float w = rect.dimension.x;
  float h = rect.dimension.y;

  Vector2D p0 = transform(Vector2D(   x   ,   y   ));
  Vector2D p1 = transform(Vector2D( x + w ,   y   ));
  Vector2D p2 = transform(Vector2D(   x   , y + h ));
  Vector2D p3 = transform(Vector2D( x + w , y + h ));
  
  // draw fill
  c = rect.style.fillColor;
  if (c.a != 0 ) {
    rasterize_triangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, c );
    rasterize_triangle( p2.x, p2.y, p1.x, p1.y, p3.x, p3.y, c );
  }

  // draw outline
  c = rect.style.strokeColor;
  if( c.a != 0 ) {
    rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    rasterize_line( p1.x, p1.y, p3.x, p3.y, c );
    rasterize_line( p3.x, p3.y, p2.x, p2.y, c );
    rasterize_line( p2.x, p2.y, p0.x, p0.y, c );
  }

}

void SoftwareRendererImp::draw_polygon( Polygon& polygon ) {

  Color c;

  // draw fill
  c = polygon.style.fillColor;
  if( c.a != 0 ) {

    // triangulate
    vector<Vector2D> triangles;
    triangulate( polygon, triangles );

    // draw as triangles
    for (size_t i = 0; i < triangles.size(); i += 3) {
      Vector2D p0 = transform(triangles[i + 0]);
      Vector2D p1 = transform(triangles[i + 1]);
      Vector2D p2 = transform(triangles[i + 2]);
      rasterize_triangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, c );
    }
  }

  // draw outline
  c = polygon.style.strokeColor;
  if( c.a != 0 ) {
    int nPoints = polygon.points.size();
    for( int i = 0; i < nPoints; i++ ) {
      Vector2D p0 = transform(polygon.points[(i+0) % nPoints]);
      Vector2D p1 = transform(polygon.points[(i+1) % nPoints]);
      rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    }
  }
}

void SoftwareRendererImp::draw_ellipse( Ellipse& ellipse ) {

  // Extra credit 

}

void SoftwareRendererImp::draw_image( Image& image ) {

  Vector2D p0 = transform(image.position);
  Vector2D p1 = transform(image.position + image.dimension);

  rasterize_image( p0.x, p0.y, p1.x, p1.y, image.tex );
}

void SoftwareRendererImp::draw_group( Group& group ) {

  for ( size_t i = 0; i < group.elements.size(); ++i ) {
    draw_element(group.elements[i]);
  }

}

// Rasterization //

// The input arguments in the rasterization functions 
// below are all defined in screen space coordinates

void SoftwareRendererImp::rasterize_point( float x, float y, Color color ) {

  // fill in the nearest pixel
  int sx = (int) floor(x);
  int sy = (int) floor(y);

  // check bounds
  if ( sx < 0 || sx >= target_w ) return;
  if ( sy < 0 || sy >= target_h ) return;

  // fill sample - NOT doing alpha blending!
  render_target[4 * (sx + sy * target_w)    ] = (uint8_t) (color.r * 255);
  render_target[4 * (sx + sy * target_w) + 1] = (uint8_t) (color.g * 255);
  render_target[4 * (sx + sy * target_w) + 2] = (uint8_t) (color.b * 255);
  render_target[4 * (sx + sy * target_w) + 3] = (uint8_t) (color.a * 255);

}

void SoftwareRendererImp::rasterize_line( float x0, float y0,
                                          float x1, float y1,
                                          Color color) {
  x0 *= this->sample_rate;
  y0 *= this->sample_rate;
  x1 *= this->sample_rate;
  y1 *= this->sample_rate;

  if (!(this->firstel_flag)) {
    cout << "line1 dims: (" << x0 << ", " << y0 << ") (" << x1 << ", " << y1 << ")" << endl;
  }

  // Task 2: 
  // Implement line rasterization
  x0 = floor(x0);
  y0 = floor(y0);
  x1 = floor(x1);
  y1 = floor(y1);

  float dx = x1 - x0;
  float dy = y1 - y0;
  float xval = x0;
  float yval = y0;
  float eps = 0;
  float xstep, ystep;

  if (this->sample_rate > 1) {
    color.r = 1;
    color.g = 0;
    color.b = 0;
  }

  // figure out direction of slope, ystep or xstep depending on dy, dx
  if (dx < 0) {
    dx = -dx;
    xstep = -1;
  }
  else xstep = 1;

  if (dy < 0) {
    dy = -dy;
    ystep = -1;
  }
  else ystep = 1;

  rasterize_point(xval, yval, color); // plot first point

  // vertical line
  if (dx == 0) {
    while (yval != y1) {
      yval += ystep;
      rasterize_point(xval, yval, color);
    }
  }

  float m  = abs(dy / dx); 
  if (dx > dy) {
    while (xval != x1) {
      xval += xstep;
      if (eps + m >= 0.5) {
        yval += ystep;
        eps = eps + m - 1;
      }
      else eps += m;
      rasterize_point(xval, yval, color);
    }
  }
  else {
    while (yval != y1) {
      yval += ystep;
      if (eps + (1 / m) >= 0.5) {
        xval += xstep;
        eps = eps + (1 / m) - 1;
      }
      else eps += (1 / m); 
      rasterize_point(xval, yval, color);
    }
  }
}

void SoftwareRendererImp::rasterize_triangle( float x0, float y0,
                                              float x1, float y1,
                                              float x2, float y2,
                                              Color color ) {
  // Task 3: 
  // Implement triangle rasterization
  // find max bounds of all points in triangle
  float x_min = min(x0, x1);
  float x_max = max(x0, x1);
  float y_min = min(y0, y1);
  float y_max = max(y0, y1);
 
  x_min = (min(x_min, x2));
  x_max = (max(x_max, x2));
  y_min = (min(y_min, y2));
  y_max = (max(y_max, y2));

  vector<float> x_vals = {x0, x1, x2};
  vector<float> y_vals = {y0, y1, y2};
  vector<float> x_deltas = {x1 - x0, x2 - x1, x0 - x2};
  vector<float> y_deltas = {y1 - y0, y2 - y1, y0 - y2};
  vector<float> E_vals = {0, 0, 0};

  // loop through points inside bounding box
  for(int y_idx = y_min; y_idx <= y_max; y_idx++) {
    for(int x_idx = x_min; x_idx <= x_max; x_idx++) {
      for(int vertex_idx = 0; vertex_idx != x_deltas.size(); vertex_idx++) {
        E_vals[vertex_idx] = (x_idx - x_vals[vertex_idx]) * y_deltas[vertex_idx] - 
                             (y_idx - y_vals[vertex_idx]) * x_deltas[vertex_idx];
        if(E_vals[vertex_idx] > 0) break; //early exit
      }
      if (E_vals[0] <= 0 && E_vals[1] <= 0 && E_vals[2] <= 0) rasterize_point(x_idx, y_idx, color);
    } 
  }
}

void SoftwareRendererImp::rasterize_image( float x0, float y0,
                                           float x1, float y1,
                                           Texture& tex ) {
  // Task 6: 
  // Implement image rasterization

}

// resolve samples to render target
void SoftwareRendererImp::resolve( void ) {

  // Task 4: 
  // Implement supersampling
  // You may also need to modify other functions marked with "Task 4".
  // fill sample - NOT doing alpha blending!
  cout << "resolving" << endl;
  cout << "input size:  (" << this->target_w << ", " << this->target_h << ")" << endl;
  cout << "output size: (" << this->w << ", " << this->h << ")" << endl;
  cout << "sample_rate: " << sample_rate << endl;

  //int display_h_idx = 0;
  for (int target_h_idx = 0; target_h_idx != target_h; target_h_idx += sample_rate ) {
    //
    for (int target_w_idx = 0; target_w_idx != target_w; target_w_idx += sample_rate ) {
      
      int red = 0;
      int green = 0;
      int blue = 0;
      int alpha = 0;
      //cout << "Target idx: (" << target_w_idx << ", " << target_h_idx << ")" << endl;
      
      for (int box_h_idx = target_h_idx; box_h_idx != target_h_idx + sample_rate; box_h_idx++ ) {
        for (int box_w_idx = target_w_idx; box_w_idx != target_w_idx + sample_rate; box_w_idx++ ) {
          red   += render_target[4 * (box_w_idx + box_h_idx * target_w)    ];
          green += render_target[4 * (box_w_idx + box_h_idx * target_w) + 1];
          blue  += render_target[4 * (box_w_idx + box_h_idx * target_w) + 2];
          alpha += render_target[4 * (box_w_idx + box_h_idx * target_w) + 3];
        }
      }

      red   /= pow(sample_rate, 2);
      green /= pow(sample_rate, 2);
      blue  /= pow(sample_rate, 2);
      alpha /= pow(sample_rate, 2);

      int display_w_idx = target_w_idx / sample_rate;
      int display_h_idx = target_h_idx / sample_rate;
      
      /*
      if (sample_rate > 1) {
        cout << "Target ids: (" << target_w_idx << ", " << target_h_idx << ") to (" 
             << target_w_idx + sample_rate - 1 << ", " << target_h_idx + sample_rate -1 << ")" << endl;
        cout << "Display idx: (" << display_w_idx << ", " << display_h_idx << ")" << endl;
      }
      */
      
      this->display_target[4 * (display_w_idx + display_h_idx * w)    ] = (uint8_t) (red);
      this->display_target[4 * (display_w_idx + display_h_idx * w) + 1] = (uint8_t) (green);
      this->display_target[4 * (display_w_idx + display_h_idx * w) + 2] = (uint8_t) (blue);
      this->display_target[4 * (display_w_idx + display_h_idx * w) + 3] = (uint8_t) (alpha);  

      //display_w_idx += 1;
    }

    //display_h_idx += 1;
    //cout << "Display idx: (" << display_w_idx << ", " << display_h_idx << ")" << endl;
  }


  cout << "resolved" << endl;
  return;
}


} // namespace CMU462
