
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
    chr::CrossSketch::run(600, 600);
    return 0;
  }

#elif defined(CHR_PLATFORM_EMSCRIPTEN)

  int main(int argc, char** argv)
  {
    chr::CrossSketch::run(4);
    return 0;
  }

#endif
