#include "App.h"
#include <iostream>
#include "Utils.h"
#include "implot.h"
#include <limits>
std::mutex mtx;

App::App() : window(sf::VideoMode(WIDTH, HEIGHT), "Simple music visualizer"), spectrogram(WIDTH)
{
    ImGui::SFML::Init(window);
    window.setVerticalSyncEnabled(true);
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
    auto time = clock.restart();
    float dt = time.asSeconds();
    ImGui::SFML::Update(window, time);
    imgui(dt);
}
void App::updateGraphs(float dt)
{
    ImPlot::CreateContext();
    ImGui::Begin("FFT");
    if (ImPlot::BeginPlot("FFT"))
    {
        {
            std::lock_guard<std::mutex> lock(mtx);
            ImPlot::SetupAxisLimits(ImAxis_X1, 0, float(512 * 44100.0f) / 1024.0f);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, -90);
            float frequencies[512];
            for (int i = 0; i < 512; i++)
            {
                frequencies[i] = float(i * 44100.0f) / 1024.0f;
            }
            ImPlot::PlotLine("output", frequencies, fftStream.getCurrentFFT(), 512);
        }
        if (fftStream.getStatus() == FFTStream::Playing)
        {
            float time = 2048 / (44100 * 2);
            currentTime += dt;
            if (currentTime >= time)
            {
                spectrogram.addLine(HEIGHT);
                spectrogram.coloredLine(fftStream.getCurrentFFT());
                currentTime = 0;
            }
        }
        ImPlot::EndPlot();
    }
    ImGui::End();
    ImPlot::DestroyContext();
}
void App::fileButton()
{
    if (ImGui::Button("Abrir archivo"))
    {
        fileDialog.Open();
    }
}
void App::playButton()
{
    if (ImGui::Button("Escuchar") && selectedFile.size() > 0)
    {
        switch (fftStream.getStatus())
        {
        case FFTStream::Playing:
            fftStream.stop();
            break;
        case FFTStream::Paused:
            fftStream.stop();
            break;
        case FFTStream::Stopped:
            break;
        }
        playingFile = selectedFile;
        spectrogram.restart();
        fftStream.load(selectedFile);
        fftStream.play();
    }
}
void App::toggleButton()
{
    if (ImGui::Button("Reanudar/Detener"))
    {
        fftStream.toggle();
    }
}
void App::imgui(float dt)
{
    updateGraphs(dt);
    ImGui::Begin("Configuraciones");
    fileButton();
    ImGui::SameLine();
    playButton();
    ImGui::Spacing();
    toggleButton();
    ImGui::Spacing();
    if (playingFile.size() > 0)
    {
        std::filesystem::path path(playingFile);
        ImGui::Text("%s", path.filename().c_str());
    }
    ImGui::ProgressBar(fftStream.getElapsedTime());
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
    sf::Color color(16, 17, 18);
    window.clear(color);
    spectrogram.render(window);
    ImGui::SFML::Render(window);
    window.display();
}
