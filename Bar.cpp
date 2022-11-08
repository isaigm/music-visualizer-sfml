#include "Bar.h"
Bar::Bar()
{
    bar.setFillColor(sf::Color::Green);
}
void Bar::setHeight(float height)
{
    bar.setSize({2, height});
}
void Bar::setPos(float x, float y)
{
    bar.setPosition(x, y);
}
void Bar::render(sf::RenderTarget &rt)
{
    rt.draw(bar);
}