//
//  PathHelper.h
//  TextNoise
//
//  Created by Ariel Malka on 4/4/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#pragma once

#include "chr/path/FollowablePath2D.h"
#include "chr/io/BinaryInputStream.h"
#include "chr/io/BinaryOutputStream.h"

class PathHelper
{
    public:
        static void read(chr::path::FollowablePath2D &path, chr::io::BinaryInputStream &stream);
        static void write(const chr::path::FollowablePath2D &path, chr::io::BinaryOutputStream &stream);
        static glm::vec2 getDimensions(const chr::path::FollowablePath2D &path);
};
