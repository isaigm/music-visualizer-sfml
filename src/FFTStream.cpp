#include "FFTStream.h"
#include <iostream>
FFTStream::FFTStream()
{
    plan = fftw_plan_dft_1d(samplesToStream / 2,
                            signal,
                            output,
                            FFTW_FORWARD,
                            FFTW_ESTIMATE);

}
FFTStream::~FFTStream()
{
    fftw_destroy_plan(plan);
}
void FFTStream::setCtx(float *normalizedOutput)
{
    normalizedOutputFFT = normalizedOutput;
    for (int i = 0; i < 512; i++)
    {
        normalizedOutputFFT[i] = 0;
        last_output[i] = 0;
    }

}
void FFTStream::load(const sf::SoundBuffer &buffer)
{
    m_samples.assign(buffer.getSamples(), buffer.getSamples() + buffer.getSampleCount());
    m_currentSample = 0;
    initialize(buffer.getChannelCount(), buffer.getSampleRate());
    duration = buffer.getDuration().asSeconds();
}
float FFTStream::getDuration()
{
    return duration;
}
bool FFTStream::onGetData(Chunk &data)
{

    data.samples = &m_samples[m_currentSample];

    if (m_currentSample + samplesToStream <= m_samples.size())
    {
        int j = 0;
        for (int i = m_currentSample; i < m_currentSample + samplesToStream; i+=2)
        {
            signal[j][REAL] = 0.5f * (float(m_samples[i]) / 32767.0f + float(m_samples[i + 1]) / 32767.0f);
            signal[j][IMAG] = 0;
            j++;
        }
        double peak = 0;
        for (int i = 0; i < 512; i++)
        {
            double amp = sqrt(output[i][REAL] * output[i][REAL] +
                              output[i][IMAG] * output[i][IMAG]);
            peak = std::max(peak, amp);
        }
        fftw_execute(plan);

        std::lock_guard<std::mutex> lock(mtx);
        for (int i = 0; i < 512; i++)
        {
            double amp = sqrt(output[i][REAL] * output[i][REAL] +
                              output[i][IMAG] * output[i][IMAG]);

            float avg = (amp + last_output[i]) / 2;

            normalizedOutputFFT[i] = 20 * log10(avg / peak);
            last_output[i] = avg;
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
