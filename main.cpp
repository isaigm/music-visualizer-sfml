#include <iostream>
#include <fftw3.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <math.h>
#include <vector>
#include <mutex>
#define WIDTH 1024
#define HEIGHT 300
#define REAL 0
#define IMAG 1

float map(float n, float x1, float x2, float y1, float y2)
{
    float m = (y2 - y1) / (x2 - x1);
    return y1 + m * (n - x1);
}
class Bar
{
public:
    Bar()
    {
        bar.setSize({2, 1});
        bar.setFillColor(sf::Color::Green);
    }
    void setHeight(float height)
    {
        bar.setSize({2, height});
    }
    void setPos(float x, float y)
    {
        bar.setPosition(x, y);
    }
    void render(sf::RenderTarget &rt)
    {
        rt.draw(bar);
    }

private:
    sf::RectangleShape bar;
};
std::mutex mtx;

std::vector<Bar> bars(512);

class FFTStream : public sf::SoundStream
{
public:
    FFTStream()
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
    ~FFTStream()
    {
        fftw_destroy_plan(plan);
    }
    void load(const sf::SoundBuffer &buffer)
    {
        // extract the audio samples from the sound buffer to our own container
        m_samples.assign(buffer.getSamples(), buffer.getSamples() + buffer.getSampleCount());

        // reset the current playing position
        m_currentSample = 0;

        // initialize the base class
        initialize(buffer.getChannelCount(), buffer.getSampleRate());
    }

private:
    virtual bool onGetData(Chunk &data)
    {

        data.samples = &m_samples[m_currentSample];

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

        if (m_currentSample + samplesToStream <= m_samples.size())
        {
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

    virtual void onSeek(sf::Time timeOffset)
    {
        m_currentSample = static_cast<std::size_t>(timeOffset.asSeconds() * getSampleRate() * getChannelCount());
    }

    std::vector<sf::Int16> m_samples;
    std::size_t m_currentSample;
    static const int samplesToStream = 2048;
    fftw_complex signal[samplesToStream / 2];
    fftw_complex output[samplesToStream / 2];
    fftw_plan plan;
    float last_output[512];
};
int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Music visualizer");
    sf::SoundBuffer sb;
    sb.loadFromFile("dynatron.wav");
    FFTStream fftStream;
    fftStream.load(sb);
    fftStream.play();

    while (window.isOpen())
    {
        sf::Event ev;
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed)
            {
                window.close();
                break;
            }
        }
        window.clear();
        std::lock_guard<std::mutex> lock(mtx);

        for (auto &bar : bars)
        {
            bar.render(window);
        }
        window.display();
    }

    return 0;
}