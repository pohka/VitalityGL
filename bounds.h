#ifndef BOUNDS_H
#define BOUNDS_H

#include "oak/core/point.h"

namespace game
{
  class Bounds
  {
    float x;
    float y;
    float w;
    float h;
    Point* origin;

    public:
      Bounds(float x, float y, float w, float h);
      ~Bounds();

      float minX();
      float minY();
      float maxX();
      float maxY();
      float height();
      float width();
      bool intersects(Bounds& b);
      Point* getOrigin();
  };
}

#endif