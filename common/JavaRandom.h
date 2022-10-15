#pragma once

/*
 * BASED ON THE FOLLOWING Random.java IMPLEMENTATION:
 * http://developer.classpath.org/doc/java/util/Random-source.html
 */

#include <cstdint>

class JavaRandom
{
public:
    JavaRandom();
    JavaRandom(int64_t seed);
    
    void setSeed(int64_t seed);

    int32_t next(int32_t bits);
    int32_t nextInt();
    int32_t nextInt(int32_t n);
    
    int64_t nextLong();
    bool nextBool();
    float nextFloat();
    double nextDouble();

protected:
    int64_t seed;
};
