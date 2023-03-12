#ifndef FFTSTREAM_H
#define FFTSTREAM_H
#include <SFML/Audio.hpp>
#include <fftw3.h>
#include <math.h>
#include "Utils.h"
#include "Bar.h"
#define REAL 0
#define IMAG 1

class FFTStream : public sf::SoundStream
{
public:
    FFTStream();
    ~FFTStream();
    float *getCurrentFFT();
    float getElapsedTime();
    void load(std::string path);
    void toggle();
private:
    static const int samplesToStream = 2048;
    virtual bool onGetData(Chunk &data);
    virtual void onSeek(sf::Time timeOffset);
    fftw_complex signal[samplesToStream / 2];
    fftw_complex output[samplesToStream / 2];
    float last_output[samplesToStream / 2];
    float normalizedOutputFFT[samplesToStream / 2];
    fftw_plan plan;
    double peak = 1;
    sf::Int16 samples[samplesToStream];
    sf::InputSoundFile inputFile;

};
#endif
