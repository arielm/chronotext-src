
#include "Sketch.h"

namespace chr
{
  CrossSketch* createSketch()
  {
    return new Sketch();
  }
}

#if defined(CHR_PLATFORM_DESKTOP)

int main(int argc, char** argv)
{
  chr::CrossSketch::run(0, 0, 4, 24);
  return 0;
}
#endif
