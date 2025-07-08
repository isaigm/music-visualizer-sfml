#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H
#include <vector>
#include "Bar.h"

class Spectrogram
{
public:
    Spectrogram(const sf::FloatRect &bounds);

    void addLine();
    void render(sf::RenderTarget &rt);
    void restart();
    void coloredLine(float *fft, float sampleRate);

private:
    void generateColors();
    std::vector<Bar> spectrogram;
    sf::FloatRect m_bounds; 
    std::vector<sf::Color> colors;
    int maxLines = 0;
    int currLine = 0;
};
#endif