#include "HPath.h"

#include "chr/utils/Utils.h"
#include "chr/math/Utils.h"

using namespace std;
using namespace chr;

HPath::HPath(float width, float segmentLength)
:
width(width),
segmentLength(segmentLength),
modY(0),
d(0)
{
    int capacity = max<int>(1, width / segmentLength);
    
    points.reserve(capacity);
    lengths.reserve(capacity);
}

int HPath::size() const
{
    return points.size();
}

bool HPath::empty() const
{
    return points.empty();
}

void HPath::begin()
{
    points.clear();
    lengths.clear();
}

void HPath::end()
{}

float HPath::getLength() const
{
    if (!empty())
    {
        return lengths.back();
    }
    else
    {
        return 0;
    }
}

void HPath::add(float z, float sx, float sy)
{
    if (!empty())
    {
        float dz = z - points.back().z;
        
        float length =sqrtf(segmentLength * segmentLength + dz * dz);
        lengths.push_back(lengths.back() + length);
    }
    else
    {
        lengths.push_back(0);
    }
    
    points.emplace_back(z, sx, sy);
}

HPath::Value HPath::pos2Value(float pos) const
{
    float length = getLength();
    
    if (length > 0)
    {
        float offset = 0;

        if ((pos < 0) || (pos >= length))
        {
            offset = width * floorf(pos / length);
            pos = math::boundf(pos, length);
        }
        
        auto index = utils::search(lengths, pos, 1, size());
        auto &p0 = points[index];
        auto &p1 = points[index + 1];
        
        float ratio = (pos - lengths[index]) / (lengths[index + 1] - lengths[index]);
        
        Value value;
        value.x = offset + segmentLength * (index + ratio);
        value.z = p0.z + (p1.z - p0.z) * ratio;
        value.sx = p0.sx + (p1.sx - p0.sx) * ratio;
        value.sy = p0.sy + (p1.sy - p0.sy) * ratio;
        
        return value;
    }
    else
    {
        return Value();
    }
}

float HPath::x2Pos(float x) const
{
    float length = getLength();
    
    if (length > 0)
    {
        float offset = 0;
        
        if ((x < 0) || (x >= width))
        {
            offset = length * floorf(x / width);
            x = math::boundf(x, width);
        }
        
        int index = int(x / segmentLength);
        float ratio = x / segmentLength - index;
        
        return offset + lengths[index] + (lengths[index + 1] - lengths[index]) * ratio;
    }
    else
    {
        return 0;
    }
}
