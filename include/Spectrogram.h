#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H
#include <vector>
#include "Bar.h"

class Spectrogram
{
public:
    Spectrogram(int);
    void render(sf::RenderTarget &rt);
    void restart();
    void addLine(float y);
    void coloredLine(float *);
private:
    std::vector<Bar> spectrogram;
    std::vector<sf::Color> colors;
    int maxLines = 0;
    int currLine = 0;
};
#endif