#include "Spectrogram.h"
#include "Utils.h"
#include <iostream>
Spectrogram::Spectrogram(int ml) : maxLines(ml)
{
    auto gen_colors = [=](std::vector<sf::Color> &colors, sf::Color startColor, sf::Color endColor, int n)
    {
        float t = 0;
        float dt = 1 / float(n);
        for (int i = 0; i < n; i++)
        {
            int r = lerp(startColor.r, endColor.r, t);
            int g = lerp(startColor.g, endColor.g, t);
            int b = lerp(startColor.b, endColor.b, t);
            sf::Color color(r, g, b);
            colors.push_back(color);
            t += dt;
        }
    };
    gen_colors(colors, sf::Color(0x9D0000FF), sf::Color::Yellow, 128);
    gen_colors(colors, sf::Color::Yellow, sf::Color::Green, 128);
    gen_colors(colors, sf::Color::Green, sf::Color(0x00FFEDFF), 128);
    gen_colors(colors, sf::Color(0x00FFEDFF), sf::Color::Blue, 128);
}
void Spectrogram::addLine(float y)
{
    if (currLine > maxLines)
    {
        spectrogram.erase(spectrogram.begin());
        for (int i = 0; i < spectrogram.size(); i++)
        {
            spectrogram[i].setPos(i, y);
        }
        currLine = maxLines;
    }
    spectrogram.emplace_back(512);
    spectrogram.back().setPos(currLine, y);
    currLine++;
}
void Spectrogram::coloredLine(float *fft)
{
    for (int i = 0; i < 512; i++)
    {
        sf::Color color(16, 17, 18);
        if(fft[i] >= -90)
        {
            int idx = map(fft[i], 0, -90, 0, 511);
            color = colors[idx];
        }
        spectrogram.back().setColor(i, color);
    }
}
void Spectrogram::restart()
{
    spectrogram.erase(spectrogram.begin(), spectrogram.end());
    currLine = 0;
}
void Spectrogram::render(sf::RenderTarget &rt)
{
    for (auto &line : spectrogram)
    {
        line.render(rt);
    }
}