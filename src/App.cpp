#include "App.h"
#include <iostream>
#include "Utils.h"
#include "implot.h"
#include <limits>
#include <filesystem> // Asegúrate de que este include está presente
#include <optional>   // Asegúrate de que este include está presente

std::mutex mtx;

App::App() : window(sf::VideoMode({WIDTH, HEIGHT}), "Spectrogram visualizer")
{
    ImGui::SFML::Init(window);
    auto &io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF("assets/Roboto-Medium.ttf", 28.0f);
    ImGui::SFML::UpdateFontTexture(); // ← **¡Muy importante!**
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
    while (std::optional<sf::Event> ev = window.pollEvent())
    {
        if (!ev.has_value())
            break;
        auto event = ev.value();
        ImGui::SFML::ProcessEvent(window, event);
        if (event.is<sf::Event::Closed>())
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
    // Esta función se mantiene exactamente como la tenías.
    ImPlot::CreateContext();
    ImGui::Begin("Spectrum");
    if (ImPlot::BeginPlot("Frequency Spectrum"))
    {
        {
            int halfSize = SIZE_FFT / 2;
            std::lock_guard<std::mutex> lock(mtx);
            ImPlot::SetupAxis(ImAxis_X1, "Hz");
            ImPlot::SetupAxis(ImAxis_Y1, "DB");

            ImPlot::SetupAxisLimits(ImAxis_X1, 0, float(halfSize * fftStream.getSampleRate()) / float(SIZE_FFT));
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, -90);
            float frequencies[SIZE_FFT];
            for (int i = 0; i < SIZE_FFT; i++)
            {
                frequencies[i] = float(i * fftStream.getSampleRate()) / SIZE_FFT;
            }
            ImPlot::PlotLine("output", frequencies, fftStream.getCurrentFFT(), int(halfSize));
        }
        if (fftStream.getStatus() == FFTStream::Status::Playing)
        {
            auto channelCount = fftStream.getChannelCount();
            auto sampleRate = fftStream.getSampleRate();
            if (sampleRate > 0 && channelCount > 0)
            {
                float time_interval = float(FFTStream::samplesToStream) / float(sampleRate * channelCount);
                currentTime += dt;
                if (currentTime >= time_interval)
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    spectrogram.addLine();
                    spectrogram.coloredLine(fftStream.getCurrentFFT());
                    currentTime = 0;
                }
            }
        }
        ImPlot::EndPlot();
    }
    ImGui::End();
    ImPlot::DestroyContext();
}

void App::fileButton()
{
    if (ImGui::Button("Open file"))
    {
        fileDialog.Open();
    }
}

void App::playButton()
{
    if (ImGui::Button("Start") && selectedFile.size() > 0)
    {
        // ... (la lógica del switch se mantiene) ...
        switch (fftStream.getStatus())
        {
        case FFTStream::Status::Playing:
            fftStream.stop();
            break;
        case FFTStream::Status::Paused:
            fftStream.stop();
            break;
        case FFTStream::Status::Stopped:
            break;
        }
        playingFile = selectedFile;
        spectrogram.restart();

        // --- CAMBIO: Comprobar si el archivo se pudo cargar ---
        if (fftStream.load(selectedFile))
        {
            // Si tuvo éxito, reproducir.
            fftStream.play();
        }
        else
        {
            // Si falló, preparamos la ventana emergente de error.
            m_showFileErrorPopup = true;
            playingFile = ""; // Limpiamos el nombre del archivo que falló.
        }
    }
}

void App::toggleButton()
{
    if (ImGui::Button("Play/Stop"))
    {
        // Tu lógica original de toggle.
        if (fftStream.getStatus() == FFTStream::Status::Playing)
            fftStream.pause();
        else if (fftStream.getStatus() == FFTStream::Status::Paused)
            fftStream.play();
    }
}

void App::imgui(float dt)
{
    updateGraphs(dt);
    ImGui::Begin("Menu");
    fileButton();
    ImGui::SameLine();
    playButton();
    ImGui::Spacing();
    toggleButton();
    ImGui::Spacing();
    if (playingFile.size() > 0)
    {
        std::filesystem::path path(playingFile);
        ImGui::Text("%s", path.filename().string().c_str());
    }
    if (ImGui::SliderFloat("##Progress", &m_playbackProgress, 0.0f, 1.0f))
    {
        fftStream.seek(m_playbackProgress);
    }
    else
    {
        m_playbackProgress = fftStream.getElapsedTime();
    }

    // --- CAMBIO: Lógica para mostrar la ventana de error ---
    if (m_showFileErrorPopup)
    {
        ImGui::OpenPopup("Error");
        m_showFileErrorPopup = false; // Reseteamos el flag
    }

    // Definimos cómo se ve la ventana emergente
    if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Can't load the selected file.\nIt may be corrupted, have an invalid format, or not exist.");
        ImGui::Separator();
        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
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
    sf::Color color(16, 17, 18);
    window.clear(color);
    spectrogram.render(window);
    ImGui::SFML::Render(window);
    window.display();
}