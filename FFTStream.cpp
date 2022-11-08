#include "FFTStream.h"
#include "Utils.h"
#include <iostream>
FFTStream::FFTStream()
{
    plan = fftw_plan_dft_1d(samplesToStream / 2,
                            signal,
                            output,
                            FFTW_FORWARD,
                            FFTW_ESTIMATE);
    for (int i = 0; i < 512; i++)
    {
        last_output[i] = 0;
    }
}
FFTStream::~FFTStream()
{
    fftw_destroy_plan(plan);
}
void FFTStream::setCtx(Bar *bars)
{
    this->bars = bars;
}
void FFTStream::load(const sf::SoundBuffer &buffer)
{
    // extract the audio samples from the sound buffer to our own container
    m_samples.assign(buffer.getSamples(), buffer.getSamples() + buffer.getSampleCount());

    // reset the current playing position
    m_currentSample = 0;

    // initialize the base class
    initialize(buffer.getChannelCount(), buffer.getSampleRate());
}

bool FFTStream::onGetData(Chunk &data)
{

    data.samples = &m_samples[m_currentSample];

    if (m_currentSample + samplesToStream <= m_samples.size())
    {
        int j = 0;
        for (int i = m_currentSample; i < m_currentSample + samplesToStream; i++)
        {
            if (i % 2 == 0)
            {
                signal[j][REAL] = m_samples[i];
                signal[j][IMAG] = 0;
                j++;
            }
        }

        fftw_execute(plan);

        double peak = 0;
        for (int i = 0; i < 512; i++)
        {
            double amp = sqrt(output[i][REAL] * output[i][REAL] +
                              output[i][IMAG] * output[i][IMAG]);
            peak = std::max(peak, amp);
        }
        float x = 0;
        std::lock_guard<std::mutex> lock(mtx);
        for (int i = 0; i < 512; i++)
        {
            double amp = sqrt(output[i][REAL] * output[i][REAL] +
                              output[i][IMAG] * output[i][IMAG]);
            double norm = amp / peak;
            float avg = (norm + last_output[i]) / 2;
            float height = map(avg, 0, 1, 0, HEIGHT);
            last_output[i] = avg;
            bars[i].setHeight(height);
            bars[i].setPos(x, HEIGHT - height);
            x += 2;
        }

        data.sampleCount = samplesToStream;
        m_currentSample += samplesToStream;
        return true;
    }
    else
    {
        data.sampleCount = m_samples.size() - m_currentSample;
        m_currentSample = m_samples.size();
        return false;
    }
}

void FFTStream::onSeek(sf::Time timeOffset)
{
    m_currentSample = static_cast<std::size_t>(timeOffset.asSeconds() * getSampleRate() * getChannelCount());
}
