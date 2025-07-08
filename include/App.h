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
    bool m_showFileErrorPopup = false;
    Spectrogram spectrogram{{{0.f, 0.0f}, {(float)WIDTH, 512.f}}};
    ImFont *font;
    float currentTime = 0;
    float m_playbackProgress = 0.0f;
};
#endif
