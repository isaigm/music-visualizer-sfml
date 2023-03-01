#ifndef FFTSTREAM_H
#define FFTSTREAM_H
#include <SFML/Audio.hpp>
#include <fftw3.h>
#include <math.h>
#include "Utils.h"
#include "Bar.h"
#define REAL 0
#define IMAG 1
static float map(float n, float x1, float x2, float y1, float y2)
{
    float m = (y2 - y1) / (x2 - x1);
    return y1 + m * (n - x1);
}
class FFTStream : public sf::SoundStream
{
public:
    FFTStream();
    ~FFTStream();
    void setCtx(float *);
    float getDuration();
    void load(const sf::SoundBuffer &buffer);

private:
    static const int samplesToStream = 2048;
    virtual bool onGetData(Chunk &data);
    virtual void onSeek(sf::Time timeOffset);
    std::vector<sf::Int16> m_samples;
    std::size_t m_currentSample;
    fftw_complex signal[samplesToStream / 2];
    fftw_complex output[samplesToStream / 2];
    float last_output[512];
    float *normalizedOutputFFT;
    fftw_plan plan;
    float duration = 0;
};
#endif
