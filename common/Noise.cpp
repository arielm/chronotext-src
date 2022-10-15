#include "common/Noise.h"

#include "chr/math/Utils.h"

using namespace chr;

static constexpr int YWRAPB  = 4;
static constexpr int YWRAP = 1 << YWRAPB;
static constexpr int ZWRAPB = 8;
static constexpr int ZWRAP = 1 << ZWRAPB;
static constexpr int PERLIN_SIZE = 4095;

Noise::Noise(int octaves, float falloff)
:
octaves(octaves),
falloff(falloff)
{
    perlin = new float[PERLIN_SIZE + 1];

    JavaRandom rnd;
    generate(rnd);
}

Noise::Noise(int64_t seed, int octaves, float falloff)
:
octaves(octaves),
falloff(falloff)
{
    perlin = new float[PERLIN_SIZE + 1];
    
    JavaRandom rnd(seed);
    generate(rnd);
}

Noise::~Noise()
{
    delete[] perlin;
}

void Noise::setOctaves(int octaves)
{
    Noise::octaves = octaves;
}

void Noise::setFalloff(float falloff)
{
    Noise::falloff = falloff;
}

void Noise::reseed(int64_t seed)
{
    JavaRandom rnd(seed);
    generate(rnd);
}

float Noise::get(float x) const
{
    if (x < 0) x = -x;
    
    int xi = (int) x;
    float xf = x - xi;
    
    float r = 0.0f;
    float ampl = 0.5f;
    
    int i = 0;
    float n1;
    
    while (true)
    {
        n1 = perlin[xi & PERLIN_SIZE];
        n1 += math::ease(xf) * (perlin[(xi + 1) & PERLIN_SIZE] - n1);
        
        r += n1 * ampl;
        
        i++;
        if (i == octaves)
        {
            break;
        }
        
        ampl *= falloff;
        xf *= 2.0f;
        xi <<= 1;
        
        if (xf >= 1.0f)
        {
            xi++;
            xf -= 1.0f;
        }
    }
    
    return r;
}

float Noise::get(float x, float y) const
{
    if (x < 0) x = -x;
    if (y < 0) y = -y;
    
    int xi = (int) x, yi = (int) y;
    float xf = x - xi, yf = y - yi;
    
    float r = 0.0f;
    float ampl = 0.5f;
    
    int i = 0, of;
    float rxf, n1, n2;
    
    while (true)
    {
        of = xi + (yi << YWRAPB);
        
        rxf = math::ease(xf);
        n1 = perlin[of & PERLIN_SIZE];
        n1 += rxf * (perlin[(of + 1) & PERLIN_SIZE] - n1);
        n2 = perlin[(of + YWRAP) & PERLIN_SIZE];
        n2 += rxf * (perlin[(of + (YWRAP + 1)) & PERLIN_SIZE] - n2);
        n1 += math::ease(yf) * (n2 - n1);
        
        r += n1 * ampl;
        
        i++;
        if (i == octaves)
        {
            break;
        }
        
        ampl *= falloff;
        xf *= 2.0f;
        xi <<= 1;
        yf *= 2.0f;
        yi <<= 1;
        
        if (xf >= 1.0f)
        {
            xi++;
            xf -= 1.0f;
        }
        if (yf >= 1.0f)
        {
            yi++;
            yf -= 1.0f;
        }
    }
    
    return r;
}

float Noise::get(float x, float y, float z) const
{
    if (x < 0) x = -x;
    if (y < 0) y = -y;
    if (z < 0) z = -z;
    
    int xi = (int) x, yi = (int) y, zi = (int) z;
    float xf = x - xi, yf = y - yi, zf = z - zi;
    
    float r = 0.0f;
    float ampl = 0.5f;
    
    int i = 0, of;
    float rxf, ryf, n1, n2, n3;
    
    while (true)
    {
        of = xi + (yi << YWRAPB) + (zi << ZWRAPB);
        
        rxf = math::ease(xf);
        ryf = math::ease(yf);
        n1 = perlin[of & PERLIN_SIZE];
        n1 += rxf * (perlin[(of + 1) & PERLIN_SIZE] - n1);
        n2 = perlin[(of + YWRAP) & PERLIN_SIZE];
        n2 += rxf * (perlin[(of + (YWRAP + 1)) & PERLIN_SIZE] - n2);
        n1 += ryf * (n2 - n1);
        of += ZWRAP;
        n2 = perlin[of & PERLIN_SIZE];
        n2 += rxf * (perlin[(of + 1) & PERLIN_SIZE] - n2);
        n3 = perlin[(of + YWRAP) & PERLIN_SIZE];
        n3 += rxf * (perlin[(of + (YWRAP + 1)) & PERLIN_SIZE] - n3);
        n2 += ryf * (n3 - n2);
        n1 += math::ease(zf) * (n2 - n1);
        
        r += n1 * ampl;
        
        i++;
        if (i == octaves)
        {
            break;
        }
        
        ampl *= falloff;
        xi <<= 1;
        xf *= 2.0f;
        yi <<= 1;
        yf *= 2.0f;
        zi <<= 1;
        zf *= 2.0f;
        
        if (xf >= 1.0f)
        {
            xi++;
            xf -= 1.0f;
        }
        if (yf >= 1.0f)
        {
            yi++;
            yf -= 1.0f;
        }
        if (zf >= 1.0f)
        {
            zi++;
            zf -= 1.0f;
        }
    }
    
    return r;
}

void Noise::generate(JavaRandom &rnd)
{
    for (int i = 0; i <= PERLIN_SIZE; i++)
    {
        perlin[i] = rnd.nextFloat();
    }
}
