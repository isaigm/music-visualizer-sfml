#include "FFTStream.h"
#include <iostream>
FFTStream::FFTStream()
{
    plan = fftw_plan_dft_1d(samplesToStream / 2,
                            signal,
                            output,
                            FFTW_FORWARD,
                            FFTW_ESTIMATE);
    for (int i = 0; i < 1024; i++)
    {
        normalizedOutputFFT[i] = 0;
        last_output[i] = 0;
    }
}
FFTStream::~FFTStream()
{
    fftw_destroy_plan(plan);
}
void FFTStream::load(std::string path)
{
    inputFile.openFromFile(path);
    initialize(inputFile.getChannelCount(), inputFile.getSampleRate(), inputFile.getChannelMap());
    peak = 1;
}
float FFTStream::getElapsedTime()
{
    float frac = 0;
    if (getStatus() == Status::Playing || getStatus() == Status::Paused)
    {
        frac = getPlayingOffset().asSeconds() / inputFile.getDuration().asSeconds();
    }
    return frac;
}
bool FFTStream::onGetData(Chunk &data)
{
    int nsamples = inputFile.read(samples, samplesToStream);
    if (nsamples < samplesToStream)
        return false;

    data.sampleCount = samplesToStream;
    data.samples = samples;
    int j = 0;
    for (int i = 0; i < samplesToStream; i += 2)
    {
        signal[j][REAL] = float(samples[i]) / 32767.0f;
        signal[j][IMAG] = 0;
        j++;
    }
   
    fftw_execute(plan);

    for (int i = 0; i < 1024; i++)
    {
        double amp = sqrt(output[i][REAL] * output[i][REAL] +
                          output[i][IMAG] * output[i][IMAG]);
        peak = std::max(peak, amp);
    }
    std::lock_guard<std::mutex> lock(mtx);
    for (int i = 0; i < 1024; i++)
    {
        double amp = sqrt(output[i][REAL] * output[i][REAL] +
                          output[i][IMAG] * output[i][IMAG]);

        float avg = (amp + last_output[i]) / 2;
        normalizedOutputFFT[i] = 20 * log10(avg / peak);
        last_output[i] = avg;
    }
    return true;
}

void FFTStream::onSeek(sf::Time timeOffset)
{
    inputFile.seek(timeOffset);
}
float *FFTStream::getCurrentFFT()
{
    return normalizedOutputFFT;
}
void FFTStream::toggle()
{
    if (getStatus() == Status::Playing)
    {
        pause();
    }
    else if (getStatus() == Status::Paused)
    {
        play();
    }
}