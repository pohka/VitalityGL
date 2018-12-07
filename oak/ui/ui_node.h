#ifndef UI_NODE_H
#define UI_NODE_H

#include "../core/types.h"

namespace oak
{
  namespace ui
  {
    struct UINode
    {
      uchar nodeType;
      float x = 0;
      float y = 0;
      ushort w;
      ushort h;
    };
  }
}

#endif