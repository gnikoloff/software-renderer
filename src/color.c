#include <stdint.h>
#include "math.h"
#include "color.h"

// methods ported from
// https://lodev.org/cgtutor/quickcg.html

color_rgb_t hsv_to_rgb(float h, float s, float v) {
  float r, g, b; //this function works with floats between 0 and 1
  h /= 256.0;
  s /= 256.0;
  v /= 256.0;

  //if saturation is 0, the color is a shade of grey
  if(s == 0.0) r = g = b = v;
  //if saturation > 0, more complex calculations are needed
  else
  {
    float f, p, q, t;
    int i;
    h *= 6.0; //to bring hue to a number between 0 and 6, better for the calculations
    i = floor(h); //e.g. 2.7 becomes 2 and 3.01 becomes 3 or 4.9999 becomes 4
    f = h - i;//the fractional part of h

    p = v * (1.0 - s);
    q = v * (1.0 - (s * f));
    t = v * (1.0 - (s * (1.0 - f)));

    switch(i)
    {
      case 0: r=v; g=t; b=p; break;
      case 1: r=q; g=v; b=p; break;
      case 2: r=p; g=v; b=t; break;
      case 3: r=p; g=q; b=v; break;
      case 4: r=t; g=p; b=v; break;
      case 5: r=v; g=p; b=q; break;
      default: r = g = b = 0; break;
    }
  }
  color_rgb_t color_rgb;
  color_rgb.r = (int)(r * 255.0);
  color_rgb.g = (int)(g * 255.0);
  color_rgb.b = (int)(b * 255.0);
  return color_rgb;
}

uint32_t rgb_to_uint32(color_rgb_t* color) {
  return (255 << 24) + (color->r << 16) + (color->g << 8) + color->b;
}
