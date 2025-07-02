#include "FFTStream.h"
#include <iostream>
FFTStream::FFTStream()
{
    plan = fftw_plan_dft_1d(SIZE_FFT,
                            signal,
                            output,
                            FFTW_FORWARD,
                            FFTW_ESTIMATE);
    for (int i = 0; i < SIZE_FFT; i++)
    {
        normalizedOutputFFT[i] = 0;
        last_output[i] = 0;
    }
}
void FFTStream::seek(float normalized_time)
{
    // Asegurarse de que el valor está en el rango [0, 1]
    normalized_time = std::max(0.0f, std::min(1.0f, normalized_time));
    
    // Convertir el tiempo normalizado a sf::Time
    sf::Time seek_time = sf::seconds(normalized_time * inputFile.getDuration().asSeconds());
    
    // Usar la función de la clase base para saltar a la nueva posición
    setPlayingOffset(seek_time);
}
unsigned int FFTStream::getSampleRate() const
{
    return inputFile.getSampleRate();
}
unsigned int FFTStream::getChannelCount() const
{
    return inputFile.getChannelCount();
}
FFTStream::~FFTStream()
{
    fftw_destroy_plan(plan);
}
bool FFTStream::load(std::string path)
{
    bool ok = inputFile.openFromFile(path);
    if (!ok) return ok;
    initialize(inputFile.getChannelCount(), inputFile.getSampleRate(), inputFile.getChannelMap());
    peak = 1;
    return ok;
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
// En FFTStream.cpp

bool FFTStream::onGetData(Chunk &data)
{
    int nsamples = inputFile.read(samples, samplesToStream);
    if (nsamples < samplesToStream)
        return false;

    data.sampleCount = samplesToStream;
    data.samples = samples;

    int j = 0;
    // --- LÓGICA MEJORADA ---
    if (getChannelCount() == 2) // Si es estéreo
    {
        // Tu lógica original: toma solo el canal izquierdo
        for (int i = 0; i < samplesToStream; i += 2)
        {
            signal[j][REAL] = float(samples[i]) / 32767.0f;
            signal[j][IMAG] = 0;
            j++;
        }
    }
    else // Si es mono (o cualquier otra cosa, por seguridad)
    {
        // Tomamos las primeras 1024 muestras secuencialmente
        for (int i = 0; i < samplesToStream / 2; i++)
        {
            signal[j][REAL] = float(samples[i]) / 32767.0f;
            signal[j][IMAG] = 0;
            j++;
        }
    }

    fftw_execute(plan);

    // El resto de tu código para normalizar y calcular el espectro es correcto
    // y no necesita cambios.
    for (int i = 0; i < SIZE_FFT; i++)
    {
        double amp = sqrt(output[i][REAL] * output[i][REAL] +
                          output[i][IMAG] * output[i][IMAG]);

        peak = std::max(peak, amp);
    }
    std::lock_guard<std::mutex> lock(mtx);
    for (int i = 0; i < SIZE_FFT; i++)
    {
        double amp = sqrt(output[i][REAL] * output[i][REAL] +
                          output[i][IMAG] * output[i][IMAG]);
        original_output[i] = amp;
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
float *FFTStream::getOriginalFFT()
{
    return original_output;
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