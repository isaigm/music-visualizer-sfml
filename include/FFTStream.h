#ifndef FFTSTREAM_H
#define FFTSTREAM_H
#include <SFML/Audio.hpp>
#include <fftw3.h>
#include <cstdint>
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
    unsigned int getSampleRate() const;
    unsigned int getChannelCount() const;
    float *getCurrentFFT();
    float *getOriginalFFT();
    float getElapsedTime();
    bool load(std::string path);
    void toggle();
    void seek(float normalized_time);
    static const int samplesToStream = SIZE_FFT * 2;
    

private:
    virtual bool onGetData(Chunk &data);
    virtual void onSeek(sf::Time timeOffset);
    fftw_complex signal[SIZE_FFT];
    fftw_complex output[SIZE_FFT];
    float last_output[SIZE_FFT];
    float normalizedOutputFFT[SIZE_FFT];
    float original_output[SIZE_FFT];
    fftw_plan plan;
    double peak = 1;
    int16_t samples[samplesToStream];
    sf::InputSoundFile inputFile;

};
#endif
