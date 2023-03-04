#ifndef APP_H
#define APP_H
#include <SFML/Graphics.hpp>
#include <vector>
#include "Bar.h"
#include "imgui-SFML.h"
#include "imgui.h"
#include "FFTStream.h"
#include "imfilebrowser.h"

class App
{
public:
    App();
    void run();
private:
    void handleEvents();
    void update();
    void render();
    void imgui(float dt);
    void updateGraphs(float dt);
    std::vector<sf::Color> colors;
    sf::RenderWindow window;
    sf::Clock clock;
    std::string playingFile, selectedFile;
    FFTStream fftStream;
    ImGui::FileBrowser fileDialog;
    ImFont *font;
    float currentTime = 0, currx = 0;
    float normalizedOutputFFT[1024];
    std::vector<Bar> spectrogram;
};
#endif
