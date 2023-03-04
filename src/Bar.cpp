#include "Bar.h"
#include <iostream>
Bar::Bar(int height) : bar(sf::LineStrip, height)
{
   
}

void Bar::setPos(float x, float y)
{
    float yy = y;
    for(int i = 0; i < bar.getVertexCount(); i++)
    {
        bar[i].position = {x, yy};
        yy--;
    }
}
void Bar::render(sf::RenderTarget &rt)
{
    rt.draw(bar);
}
void Bar::setColor(int i, sf::Color color)
{
    bar[i].color = color;
}