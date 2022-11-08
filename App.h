#ifndef APP_H
#define APP_H
#include <SFML/Graphics.hpp>
#include <vector>
#include "Bar.h"
#include "imgui-SFML.h"
#include "imgui.h"
#include "FFTStream.h"
#include "imfilebrowser.h"
#include "Utils.h"
class App
{
public:
    App();
    void run();
private:
    void handleEvents();
    void update();
    void render();
    void imgui();
    sf::RenderWindow window;
    sf::Clock clock;
    std::string selectedFile;
    FFTStream fftStream;
    ImGui::FileBrowser fileDialog;

    Bar bars[512];
};
#endif