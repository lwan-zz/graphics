#include "texture.h"
#include "color.h"

#include <assert.h>
#include <iostream>
#include <algorithm>

using namespace std;

namespace CMU462 {

inline void uint8_to_float( float dst[4], unsigned char* src ) {
  uint8_t* src_uint8 = (uint8_t *)src;
  dst[0] = src_uint8[0] / 255.f;
  dst[1] = src_uint8[1] / 255.f;
  dst[2] = src_uint8[2] / 255.f;
  dst[3] = src_uint8[3] / 255.f;
}

inline void float_to_uint8( unsigned char* dst, float src[4] ) {
  uint8_t* dst_uint8 = (uint8_t *)dst;
  dst_uint8[0] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[0])));
  dst_uint8[1] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[1])));
  dst_uint8[2] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[2])));
  dst_uint8[3] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[3])));
}

void Sampler2DImp::generate_mips(Texture& tex, int startLevel) {

  // NOTE: 
  // This starter code allocates the mip levels and generates a level 
  // map by filling each level with placeholder data in the form of a 
  // color that differs from its neighbours'. You should instead fill
  // with the correct data!

  // Task 7: Implement this

  // check start level

  cout << "generate_mips" << endl;

  if ( startLevel >= tex.mipmap.size() ) {
    std::cerr << "Invalid start level"; 
  }

  // allocate sublevels
  int baseWidth  = tex.mipmap[startLevel].width;
  int baseHeight = tex.mipmap[startLevel].height;
  int numSubLevels = (int)(log2f( (float)max(baseWidth, baseHeight)));

  numSubLevels = min(numSubLevels, kMaxMipLevels - startLevel - 1);
  tex.mipmap.resize(startLevel + numSubLevels + 1);

  cout << "sublevels: " << numSubLevels << endl;
  cout << "size of level 1: " << tex.mipmap[1].texels.size() << endl;
  cout << "startlevel: " << startLevel << endl;
  
  int width  = baseWidth;
  int height = baseHeight;
  for (int i = 1; i <= numSubLevels; i++) {
    // generate empty data structures
    MipLevel& level = tex.mipmap[startLevel + i];

    // handle odd size texture by rounding down
    width  = max( 1, width  / 2); assert(width  > 0);
    height = max( 1, height / 2); assert(height > 0);

    level.width = width;
    level.height = height;
    level.texels = vector<unsigned char>(4 * width * height);

    // go loop through pixels in mipmap previous to generate this mipmap;

  }

  // fill all 0 sub levels with interchanging colors
  //Color colors[3] = { Color(1,0,0,1), Color(0,1,0,1), Color(0,0,1,1) };
  for(size_t i = 1; i < tex.mipmap.size(); ++i) {
    // cycle through previous mipmaps, starting at 0
    
    MipLevel& mip = tex.mipmap[i];
    int mipmap_x ,mipmap_y;

    for (size_t y_idx = 0; y_idx != tex.mipmap[i-1].height; y_idx += 2) {
      for (size_t x_idx = 0; x_idx != tex.mipmap[i-1].width; x_idx += 2) {
        
        Color c = (0, 0, 0, 0);
        // perform convolution, assign value to coordinate in display_target
        float count = 0;
        for (int box_h_idx = y_idx; box_h_idx != y_idx + 2; box_h_idx++ ) {
          for (int box_w_idx = x_idx; box_w_idx != x_idx + 2; box_w_idx++ ) {
            c.r += (float) tex.mipmap[i-1].texels[4 * (box_w_idx + box_h_idx * tex.mipmap[i-1].width)    ];
            c.g += (float) tex.mipmap[i-1].texels[4 * (box_w_idx + box_h_idx * tex.mipmap[i-1].width) + 1];
            c.b += (float) tex.mipmap[i-1].texels[4 * (box_w_idx + box_h_idx * tex.mipmap[i-1].width) + 2];
            c.a += (float) tex.mipmap[i-1].texels[4 * (box_w_idx + box_h_idx * tex.mipmap[i-1].width) + 3];
            count ++;
          }
        }
        c.r = (c.r / count) / 255;
        c.g = (c.g / count) / 255;
        c.b = (c.b / count) / 255;
        c.a = (c.a / count) / 255;

        tex.mipmap[i].texels[4 * ( (x_idx / 2) + (y_idx /2) * tex.mipmap[i].width)    ] = c.r * 255;
        tex.mipmap[i].texels[4 * ( (x_idx / 2) + (y_idx /2) * tex.mipmap[i].width) + 1] = c.g * 255;
        tex.mipmap[i].texels[4 * ( (x_idx / 2) + (y_idx /2) * tex.mipmap[i].width) + 2] = c.b * 255;
        tex.mipmap[i].texels[4 * ( (x_idx / 2) + (y_idx /2) * tex.mipmap[i].width) + 3] = c.a * 255;
      }
    
    /*
    for(size_t i = 0; i < 4 * mip.width * mip.height; i += 4) {
      
      float_to_uint8( &mip.texels[i], &c.r ); // you should do this= next time, idiot. 
    */
    }
  }
  cout << "generated mipmaps" << endl;
}

Color Sampler2DImp::sample_nearest(Texture& tex, 
                                   float u, float v, 
                                   int level) {

  // Task 6: Implement nearest neighbour interpolation
  if ( level < 0 || level > kMaxMipLevels ) {
    // return magenta for invalid level
    return Color(1,0,1,1);
  }
  
  u *= tex.mipmap[level].width;
  v *= tex.mipmap[level].height;

  int pixel_u = floor(u);
  int pixel_v = floor(v);
  Color color;

  color.r = (float) tex.mipmap[level].texels[4 * (pixel_u + pixel_v * tex.mipmap[level].width)    ] / 255.0;
  color.g = (float) tex.mipmap[level].texels[4 * (pixel_u + pixel_v * tex.mipmap[level].width) + 1] / 255.0;
  color.b = (float) tex.mipmap[level].texels[4 * (pixel_u + pixel_v * tex.mipmap[level].width) + 2] / 255.0;
  color.a = (float) tex.mipmap[level].texels[4 * (pixel_u + pixel_v * tex.mipmap[level].width) + 3] / 255.0;

  return color;
}

Color Sampler2DImp::sample_bilinear(Texture& tex, 
                                    float u, float v, 
                                    int level) {
  
  // Task 6: Implement bilinear filtering
  if ( level < 0 || level > kMaxMipLevels ) {
    // return magenta for invalid level
    cout << "bad level" << endl;
    return Color(1,0,1,1);
  }
  u *= tex.mipmap[level].width;
  v *= tex.mipmap[level].height;

  int pixel_u = floor(u);
  int pixel_v = floor(v);
  Color color = (0, 0, 0, 0);

  for (int pixel_u_idx = pixel_u; pixel_u_idx != pixel_u + 2; pixel_u_idx++ ) {
    for (int pixel_v_idx = pixel_v; pixel_v_idx != pixel_v + 2; pixel_v_idx++ ) {
      color.r += (float) 0.25 * tex.mipmap[level].texels[4 * (pixel_u_idx + pixel_v_idx * tex.mipmap[level].width)    ] / 255.0;
      color.g += (float) 0.25 * tex.mipmap[level].texels[4 * (pixel_u_idx + pixel_v_idx * tex.mipmap[level].width) + 1] / 255.0;
      color.b += (float) 0.25 * tex.mipmap[level].texels[4 * (pixel_u_idx + pixel_v_idx * tex.mipmap[level].width) + 2] / 255.0;
      color.a += (float) 0.25 * tex.mipmap[level].texels[4 * (pixel_u_idx + pixel_v_idx * tex.mipmap[level].width) + 3] / 255.0;
    }
  }

  return color;
}

Color Sampler2DImp::sample_trilinear(Texture& tex, 
                                     float u, float v, 
                                     float u_scale, float v_scale) {

  // Task 7: Implement trilinear filtering

  // find mipmap depths
  float length = max(u_scale, v_scale);
  float depth = log2 (length);
  int depth1, depth2;

  if ( depth < 0 ) { 
    depth1 = 0; 
    depth2 = 1; 
  }
  else { 
    depth1 = floor(depth); 
    depth2 = ceil(depth); 
  }

  /*
  for (int depth_idx = 0; depth_idx != kMaxMipLevels; depth_idx++) {
    depth2 = depth_idx;
    pixel_size = 1 / max(tex.mipmap[depth_idx].width, tex.mipmap[depth_idx].height);
    if (pixel_size > scale_pixel) break;
  }
  */

  //cout << depth1 << ", " <<  depth2 << endl;
  //getchar();
  // scale both points to respective mipmap scales
  int u1 = floor( u * tex.mipmap[depth1].width );
  int v1 = floor( v * tex.mipmap[depth1].height );
  int u2 = floor( u * tex.mipmap[depth2].width );
  int v2 = floor( v * tex.mipmap[depth2].height ); 

  Color color1 = (0, 0, 0, 0);
  Color color2 = (0, 0, 0, 0);

  for (int u1_idx = u1; u1_idx != u1 + 2; u1_idx++ ) {
    for (int v1_idx = v1; v1_idx != v1 + 2; v1_idx++ ) {
      color1.r += (float) 0.25 * tex.mipmap[depth1].texels[4 * (u1_idx + v1_idx * tex.mipmap[depth1].width)    ] / 255.0;
      color1.g += (float) 0.25 * tex.mipmap[depth1].texels[4 * (u1_idx + v1_idx * tex.mipmap[depth1].width) + 1] / 255.0;
      color1.b += (float) 0.25 * tex.mipmap[depth1].texels[4 * (u1_idx + v1_idx * tex.mipmap[depth1].width) + 2] / 255.0;
      color1.a += (float) 0.25 * tex.mipmap[depth1].texels[4 * (u1_idx + v1_idx * tex.mipmap[depth1].width) + 3] / 255.0;
    }
  }  

  for (int u1_idx = u1; u1_idx != u1 + 2; u1_idx++ ) {
    for (int v1_idx = v1; v1_idx != v1 + 2; v1_idx++ ) {
      color2.r += (float) 0.25 * tex.mipmap[depth2].texels[4 * (u1_idx + v1_idx * tex.mipmap[depth2].width)    ] / 255.0;
      color2.g += (float) 0.25 * tex.mipmap[depth2].texels[4 * (u1_idx + v1_idx * tex.mipmap[depth2].width) + 1] / 255.0;
      color2.b += (float) 0.25 * tex.mipmap[depth2].texels[4 * (u1_idx + v1_idx * tex.mipmap[depth2].width) + 2] / 255.0;
      color2.a += (float) 0.25 * tex.mipmap[depth2].texels[4 * (u1_idx + v1_idx * tex.mipmap[depth2].width) + 3] / 255.0;
    }
  }

  Color tricolor;
  tricolor.r = ( color1.r + color2.r ) / 2;
  tricolor.g = ( color1.g + color2.g ) / 2;
  tricolor.b = ( color1.b + color2.b ) / 2;
  tricolor.a = ( color1.a + color2.a ) / 2;

  return color1; 
}

} // namespace CMU462C
