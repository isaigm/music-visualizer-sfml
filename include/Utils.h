#ifndef UTILS_H
#define UTILS_H
#define WIDTH 1920
#define HEIGHT 1080
#define SIZE_FFT 1024
#include <mutex>
extern std::mutex mtx;

static float map(float n, float x1, float x2, float y1, float y2)
{
    float m = (y2 - y1) / (x2 - x1);
    return y1 + m * (n - x1);
}
static float lerp(float a, float b, float t)
{
    return t *(b - a) + a;
}
#endif