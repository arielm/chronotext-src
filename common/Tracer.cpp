#include "common/Tracer.h"

#include "chr/path/SplinePath.h"

using namespace std;
using namespace chr;
using namespace path;

Tracer::Tracer(float chunkLength, float tol)
:
chunkLength(chunkLength),
tol(tol)
{}

void Tracer::begin()
{
  rawTrace.clear();
  path.clear();
}

void Tracer::end()
{}

void Tracer::update(const glm::vec2 &point)
{
  rawTrace.add(point);
  trace(rawTrace);
}

void Tracer::trace(const FollowablePath2D &input)
{
  if (chunkLength > 0)
  {
    float length = input.getLength();
    
    if (length > 0)
    {
      int n = length / chunkLength;
      
      if (n > 0)
      {
        SplinePath<glm::vec2> spline;
        spline
          .setType(SplinePath<glm::vec2>::TYPE_BSPLINE)
          .setSamplingTolerance(tol);

        float pos = 0;
        
        spline.add(input.getPoints().front().position);
        spline.add(input.getPoints().front().position);
        
        for (int i = 0; i < n - 1; i++)
        {
          pos += chunkLength;
          spline.add(input.offsetToPosition(pos));
        }
        
        spline.add(input.getPoints().back().position);
        
        // ---
        
        path.clear();
        path.add(spline.getPolyline());
      }
    }
  }
}
