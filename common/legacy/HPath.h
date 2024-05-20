#pragma once

#include <vector>

class HPath
{
public:
    struct Point
    {
        float z;
        float sx;
        float sy;
        
        Point(float z, float sx, float sy)
        :
        z(z),
        sx(sx),
        sy(sy)
        {}
    };
    
    struct Value
    {
        float x;
        float z;
        float sx;
        float sy;
    };

    float width;
    float segmentLength;

    float modY;
    float d;

    std::vector<Point> points;
    std::vector<float> lengths;

    HPath(float width, float segmentLength);

    int size() const;
    bool empty() const;
    
    void begin();
    void end();
    
    float getLength() const;
    void add(float z, float sx, float sy);
    
    HPath::Value pos2Value(float pos) const;
    float x2Pos(float x) const;
};
