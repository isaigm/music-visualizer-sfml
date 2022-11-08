#include "App.h"
#include <iostream>
App::App() : window(sf::VideoMode(WIDTH, HEIGHT), "simple music visualizer")
{
    window.setVerticalSyncEnabled(true);
    ImGui::SFML::Init(window);
    fftStream.setCtx(bars);
}
void App::run()
{
    while (window.isOpen())
    {
        handleEvents();
        update();
        render();
    }
    ImGui::SFML::Shutdown();
}
void App::handleEvents()
{
    sf::Event ev;
    while (window.pollEvent(ev))
    {
        ImGui::SFML::ProcessEvent(ev);
        if (ev.type == sf::Event::Closed)
        {
            window.close();
            break;
        }
    }
}
void App::update()
{
    ImGui::SFML::Update(window, clock.restart());
}
void App::imgui()
{
    ImGui::Begin("Configuraciones");
    if (ImGui::Button("Abrir archivo"))
    {
        fileDialog.Open();
    }
    ImGui::SameLine();
    if (ImGui::Button("Escuchar") && selectedFile.size() > 0)
    {
        sf::SoundBuffer sb;
        sb.loadFromFile(selectedFile);
        fftStream.load(sb);
        fftStream.play();
    }
    if (ImGui::Button("Detener") && fftStream.getStatus() == fftStream.Playing)
    {
        fftStream.pause();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reanudar") && fftStream.getStatus() == fftStream.Paused)
    {
        fftStream.play();
    }
    ImGui::End();
    fileDialog.Display();
    if (fileDialog.HasSelected())
    {
        selectedFile = fileDialog.GetSelected().string();
        fileDialog.ClearSelected();
    }
}
void App::render()
{
    imgui();

    window.clear();
    std::lock_guard<std::mutex> lock(mtx);
    for (int i = 0; i < 512; i++)
    {
        bars[i].render(window);
    }

    ImGui::SFML::Render(window);
    window.display();
}