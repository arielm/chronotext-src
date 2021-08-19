//
//  PathHelper.cpp
//  TextNoise
//
//  Created by Ariel Malka on 4/4/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#include "PathHelper.h"

using namespace std;
using namespace chr;
using namespace path;
using namespace io;

void PathHelper::read(FollowablePath2D &path, BinaryInputStream &stream)
{
    if (stream.good())
    {
        auto count = stream.read<uint32_t>();
        
        vector<glm::vec2> points;
        points.reserve(count);

        for (auto i = 0; i < count; i++)
        {
            points.emplace_back(stream.read<glm::vec2>());
        }
        
        path
            .begin()
            .add(points)
            .end();
    }
}

void PathHelper::write(const FollowablePath2D &path, BinaryOutputStream &stream)
{
    const auto &points = path.getPoints();

    stream.write(uint32_t(points.size()));

    for (int i = 0; i < points.size(); i++)
    {
        stream.write(points[i].position);
    }
}

glm::vec2 PathHelper::getDimensions(const FollowablePath2D &path)
{
    float minX = FLT_MAX;
    float minY = FLT_MAX;
    float maxX = FLT_MIN;
    float maxY = FLT_MIN;
    
    const auto &points = path.getPoints();
    
    for (int i = 0; i < points.size(); i++)
    {
        float x = points[i].position.x;
        float y = points[i].position.y;
        
        if (x < minX) minX = x;
        if (x > maxX) maxX = x;
        
        if (y < minY) minY = y;
        if (y > maxY) maxY = y;
    }
    
    return glm::vec2(maxX - minX, maxY - minY);
}
