#include "resources.h"
#include "view.h"
#include <iostream>

ScreenView::ScreenView()
{ // Default constructor

}

ScreenView::ScreenView(sf::Vector2f size):
    sf::View(size*0.5f, size)
{
	int m_navPadding = 36;
    updatePos();
}

void ScreenView::updatePos()
{
    m_screenPos.topLeft.setValues(this->getCenter().x - (this->getSize().x/2),
                                      this->getCenter().y - (this->getSize().y/2));
    m_screenPos.bottomRight.setValues(this->getCenter().x + (this->getSize().x/2),
                                  this->getCenter().y + (this->getSize().y/2));
    m_screenPos.topRight.setValues(this->getCenter().x + (this->getSize().x/2),
                                      this->getCenter().y - (this->getSize().y/2));
    m_screenPos.bottomLeft.setValues(this->getCenter().x - (this->getSize().x/2),
                                  this->getCenter().y + (this->getSize().y/2));
}


ScreenView::~ScreenView()
{

}



void ScreenView::update(float time)
{
	viewMoving(time);
	viewScaling();
	updatePos();
}

void ScreenView::viewMoving(float time) {

    if (Keyboard::isKeyPressed(Keyboard::A)) {
        //if(this->getCenter().x - (m_rect.width/2) >= m_mapSize.bottomLeft.x)
            this->move(-0.5*time, 0);
    }

    if (Keyboard::isKeyPressed(Keyboard::W)) {
        //if(this->getCenter().y - (m_rect.height/2) >= m_mapSize.topLeft.y)
            this->move(0, -0.5*time);
    }

    if (Keyboard::isKeyPressed(Keyboard::D)) {
        //if(this->getCenter().x + (m_rect.width/2) <= m_mapSize.topRight.x)
            this->move(0.5*time, 0);
    }
    if (Keyboard::isKeyPressed(Keyboard::S)) {
        //if(this->getCenter().y + (m_rect.height/2) <= m_mapSize.bottomRight.y)
            this->move(0, 0.5*time);
    }

}

void ScreenView::viewScaling(){

    if (Keyboard::isKeyPressed(Keyboard::U)) {
        zoom(1.1000f); //масштабируем, уменьшение
        //this->setSize(this->getSize().x*1.1000f,this->getSize().x*1.1000f);
    }

    if (Keyboard::isKeyPressed(Keyboard::Y)) {
        zoom(0.9000f); //масштабируем, увеличение
        //this->setSize(this->getSize().x*0.9000f,this->getSize().x*0.9000f);
    }
}
