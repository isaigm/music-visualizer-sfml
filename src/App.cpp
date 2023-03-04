#include "App.h"
#include <iostream>
#include "Utils.h"
#include "implot.h"
#include <limits>
std::mutex mtx;

App::App() : window(sf::VideoMode(WIDTH, HEIGHT), "Simple music visualizer")
{

    ImGui::SFML::Init(window);
    fftStream.setCtx(normalizedOutputFFT);
    window.setVerticalSyncEnabled(true);
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
    gen_colors(colors, sf::Color(0x00FFEDFF), sf::Color::Blue, 64);
    gen_colors(colors, sf::Color::Blue, sf::Color::Black, 64);
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
        std::lock_guard<std::mutex> lock(mtx);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, float(512 * 44100.0f) / 1024.0f);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, -90);
        float time = 2048 / (44100 * 2);
        if (fftStream.getStatus() != FFTStream::Playing)
            goto end;
        currentTime += dt;
        if (currentTime < time)
            goto end;
        float x[512];
        if (currx > WIDTH)
        {
            currx = 0;
            spectrogram.erase(spectrogram.begin());
            for (int i = 0; i < spectrogram.size(); i++)
            {
                spectrogram[i].setPos(i, HEIGHT);
            }
            currx = WIDTH;
        }
        spectrogram.emplace_back(512);
        spectrogram.back().setPos(currx, HEIGHT);
        for (int i = 0; i < 512; i++)
        {
            x[i] = float(i * 44100.0f) / 1024.0f;
            int idx = map(normalizedOutputFFT[i], 0, -90, 0, 511);
            spectrogram.back().setColor(i, colors[idx]);
        }
        currx++;
        ImPlot::PlotLine("output", x, normalizedOutputFFT, 512);
        currentTime = 0;
        end:
        ImPlot::EndPlot();
    }
    ImGui::End();
    ImPlot::DestroyContext();
}
void App::imgui(float dt)
{
    updateGraphs(dt);
    ImGui::Begin("Configuraciones");

    if (ImGui::Button("Abrir archivo"))
    {
        fileDialog.Open();
    }
    ImGui::SameLine();
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
        sf::SoundBuffer sb;
        playingFile = selectedFile;
        sb.loadFromFile(selectedFile);
        spectrogram.erase(spectrogram.begin(), spectrogram.end());
        currx = 0;
        fftStream.load(sb);
        fftStream.play();
    }
    ImGui::Spacing();
    if (ImGui::Button("Reanudar/Detener"))
    {
        if (fftStream.getStatus() == fftStream.Playing)
        {
            fftStream.pause();
        }
        else if (fftStream.getStatus() == fftStream.Paused)
        {
            fftStream.play();
        }
    }
    ImGui::Spacing();
    if (playingFile.size() > 0)
    {
        std::filesystem::path path(playingFile);

        ImGui::Text("%s", path.filename().c_str());
    }
    float frac = 0;
    if (fftStream.getStatus() == fftStream.Playing || fftStream.getStatus() == fftStream.Paused)
    {
        frac = fftStream.getPlayingOffset().asSeconds() / fftStream.getDuration();
    }
    ImGui::ProgressBar(frac);
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
    for (auto &b : spectrogram)
    {
        b.render(window);
    }
    ImGui::SFML::Render(window);
    window.display();
}
