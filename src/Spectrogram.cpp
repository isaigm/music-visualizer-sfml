#include "Spectrogram.h"
#include "Utils.h"
#include <cmath>
#include <algorithm>

Spectrogram::Spectrogram(const sf::FloatRect& bounds) : m_bounds(bounds)
{
    generateColors();
}

void Spectrogram::generateColors()
{
    // --- PALETA DE COLORES RECREADA DE LA IMAGEN DE AUDACITY ---
    // Esta paleta va de Púrpura oscuro -> Rosa/Magenta -> Naranja -> Amarillo
    
    // Definimos los puntos clave del degradado
    const std::vector<sf::Color> audacity_keys = {
        sf::Color(11, 8, 40),      // Casi negro, con un toque de púrpura
        sf::Color(68, 1, 84),      // Púrpura oscuro
        sf::Color(180, 40, 120),   // Rosa/Magenta
        sf::Color(250, 100, 40),   // Naranja
        sf::Color(240, 240, 80)    // Amarillo brillante
    };

    colors.clear();
    int total_colors = 512; // Número de colores en nuestro degradado final

    // Interpolamos entre los colores clave para crear el degradado suave
    for (int i = 0; i < total_colors; ++i) {
        float t_global = (float)i / (total_colors - 1);
        float t_scaled = t_global * (audacity_keys.size() - 1);
        int index1 = static_cast<int>(t_scaled);
        int index2 = std::min(index1 + 1, (int)audacity_keys.size() - 1);
        float t_local = t_scaled - index1;

        sf::Color c1 = audacity_keys[index1];
        sf::Color c2 = audacity_keys[index2];

        uint8_t r = static_cast<uint8_t>(lerp(c1.r, c2.r, t_local));
        uint8_t g = static_cast<uint8_t>(lerp(c1.g, c2.g, t_local));
        uint8_t b = static_cast<uint8_t>(lerp(c1.b, c2.b, t_local));
        colors.push_back(sf::Color(r, g, b));
    }
}

void Spectrogram::addLine()
{
    // Tu código para añadir una línea es perfecto. No se cambia.
    if (currLine >= m_bounds.size.x) {
        spectrogram.erase(spectrogram.begin());
        for (size_t i = 0; i < spectrogram.size(); i++) {
            spectrogram[i].setPos(m_bounds.position.x + i, m_bounds.position.y + m_bounds.size.y);
        }
        currLine = m_bounds.size.x - 1;
    }
    spectrogram.emplace_back(static_cast<int>(m_bounds.size.y));
    spectrogram.back().setPos(m_bounds.position.x + currLine, m_bounds.position.y + m_bounds.size.y);
    currLine++;
}

void Spectrogram::coloredLine(float* fft_data)
{
    if (spectrogram.empty()) return;

    const int fft_bands = SIZE_FFT / 2;
    const int bar_height = static_cast<int>(m_bounds.size.y);

    for (int y = 0; y < bar_height; y++)
    {
        // --- MAPEO LOGARÍTMICO DE FRECUENCIA (COMO EN AUDACITY) ---
        // Normalizamos la posición del píxel
        float normalized_y = (float)y / (bar_height - 1);
        // Usamos pow() para que las frecuencias bajas ocupen más espacio, como en Audacity.
        // Esto es crucial para que la cara no se vea distorsionada.
        int fft_index = static_cast<int>(std::pow(normalized_y, 1.8) * (fft_bands - 1));

        float db_value = fft_data[fft_index];
        sf::Color color(11, 8, 40); // Color de fondo (el más oscuro de la paleta)

        if (db_value >= -90.0f)
        {
            // Mapeamos los decibelios a nuestra nueva paleta de colores.
            int color_idx = map(db_value, 0, -90, colors.size() - 1, 0);
            color_idx = std::max(0, std::min((int)colors.size() - 1, color_idx));
            color = colors[color_idx];
        }

       
        spectrogram.back().setColor(y, color);
    }
}

void Spectrogram::restart()
{
    spectrogram.clear();
    currLine = 0;
}

void Spectrogram::render(sf::RenderTarget &rt)
{
    for (auto &line : spectrogram)
    {
        line.render(rt);
    }
}