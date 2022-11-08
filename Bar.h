#ifndef BAR_H
#define BAR_H
#include <SFML/Graphics.hpp>
class Bar
{
public:
    Bar();
    void setHeight(float height);
    void setPos(float x, float y);
    void render(sf::RenderTarget &rt);

private:
    sf::RectangleShape bar;
};
#endif