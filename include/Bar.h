#ifndef BAR_H
#define BAR_H
#include <SFML/Graphics.hpp>
class Bar
{
public:
    Bar(int height);
    void setPos(float x, float y);
    void render(sf::RenderTarget &rt);
    void setColor(int i, sf::Color color);

private:
    sf::VertexArray bar;
};
#endif