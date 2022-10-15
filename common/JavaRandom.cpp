#include "common/JavaRandom.h"

#include "chr/utils/Utils.h"
#include "chr/Platform.h"
#include "chr/Log.h"

using namespace std;
using namespace chr;

JavaRandom::JavaRandom()
{
    setSeed(utils::millisSinceEpoch());
}

JavaRandom::JavaRandom(int64_t seed)
{
    setSeed(seed);
}

void JavaRandom::setSeed(int64_t seed)
{
    JavaRandom::seed = (seed ^ 0x5DEECE66DLL) & ((1LL << 48) - 1);
}

int32_t JavaRandom::next(int32_t bits)
{
    seed = (seed * 0x5DEECE66DLL + 0xBLL) & ((1LL << 48) - 1);
    return int32_t(seed >> (48 - bits));
}

int32_t JavaRandom::nextInt()
{
    return next(32);
}

int32_t JavaRandom::nextInt(int32_t n)
{
    if (n <= 0)
    {
        LOGF << "n must be positive" << endl;
    }
    
    if ((n & -n) == n) // i.e., n is a power of 2
    {
        return int32_t((n * (int64_t)next(31)) >> 31);
    }
    
    int32_t bits, val;
    do
    {
        bits = next(31);
        val = bits % n;
    }
    while (bits - val + (n - 1) < 0);
    
    return val;
}

int64_t JavaRandom::nextLong()
{
    return ((int64_t)next(32) << 32) + next(32);
}

bool JavaRandom::nextBool()
{
    return next(1) != 0;
}

float JavaRandom::nextFloat()
{
    return next(24) / float(1 << 24);
}

double JavaRandom::nextDouble()
{
    return (((int64_t)next(26) << 27) + next(27)) / double(1LL << 53);
}
