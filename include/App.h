#ifndef APP_H
#define APP_H
#include <SFML/Graphics.hpp>
#include <vector>
#include "imgui-SFML.h"
#include "imgui.h"
#include "FFTStream.h"
#include "imfilebrowser.h"
#include "Spectrogram.h"
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
    void fileButton();
    void playButton();
    void toggleButton();
    sf::RenderWindow window;
    sf::Clock clock;
    std::string playingFile, selectedFile;
    FFTStream fftStream;
    ImGui::FileBrowser fileDialog;
    Spectrogram spectrogram;
    ImFont *font;
    float currentTime = 0;
};
#endif
