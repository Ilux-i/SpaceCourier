#include "DeliveryPoint.h"
#include <iostream>

DeliveryPoint::DeliveryPoint(const sf::Vector2f& position) 
    : active(true), textureLoaded(false),
      sprite(texture)
{
    shape.setSize(sf::Vector2f(50.f, 50.f));
    shape.setFillColor(sf::Color(100, 255, 100, 150));
    shape.setPosition(position);
    this->position = position;
    
    loadTexture();
}

void DeliveryPoint::loadTexture() {
    if (!texture.loadFromFile("assets/sprites/objects/delivery_point.png")) {
        std::cout << "❌ Не удалось загрузить текстуру точки доставки, используем геометрическую форму" << std::endl;
        textureLoaded = false;
        return;
    }
    
    textureLoaded = true;
    
    sprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32, 32)));
    sprite.setScale(sf::Vector2f(1.6f, 1.6f));
    sprite.setPosition(position);
}

void DeliveryPoint::update(float deltaTime) {
    if (active) {
        if (textureLoaded) {
            sprite.setPosition(position);
        } else {
            shape.setPosition(position);
        }
    }
}

void DeliveryPoint::draw(sf::RenderWindow& window) const {
    if (active) {
        if (textureLoaded) {
            window.draw(sprite);
        } else {
            window.draw(shape);
        }
    }
}

sf::FloatRect DeliveryPoint::getBounds() const {
    if (textureLoaded) {
        sf::FloatRect spriteBounds = sprite.getGlobalBounds();
        return spriteBounds;
    }
    return shape.getGlobalBounds();
}

bool DeliveryPoint::isActive() const {
    return active;
}