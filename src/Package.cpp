#include "Package.h"
#include <iostream>

Package::Package(const sf::Vector2f& position) 
    : carried(false), delivered(false), textureLoaded(false),
      sprite(texture)
{
    shape.setSize(sf::Vector2f(30.f, 30.f));
    shape.setFillColor(sf::Color(255, 215, 0));
    shape.setPosition(position);
    this->position = position;
    
    loadTexture();
}

void Package::loadTexture() {
    if (!texture.loadFromFile("assets/sprites/objects/package.png")) {
        std::cout << "❌ Не удалось загрузить текстуру посылки, используем геометрическую форму" << std::endl;
        textureLoaded = false;
        return;
    }
    
    textureLoaded = true;
    
    sprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32, 32)));
    sprite.setScale(sf::Vector2f(0.9f, 0.9f));
    sprite.setPosition(position);
}

void Package::update(float deltaTime) {
    if (textureLoaded) {
        sprite.setPosition(position);
    } else {
        shape.setPosition(position);
    }
}

void Package::draw(sf::RenderWindow& window) const {
    if (!delivered) {
        if (textureLoaded) {
            window.draw(sprite);
        } else {
            window.draw(shape);
        }
    }
}

sf::FloatRect Package::getBounds() const {
    if (textureLoaded) {
        sf::FloatRect spriteBounds = sprite.getGlobalBounds();
        return spriteBounds;
    }
    return shape.getGlobalBounds();
}

bool Package::isCarried() const {
    return carried;
}

void Package::setCarried(bool carried) {
    this->carried = carried;
    if (textureLoaded) {
        if (carried) {
            sprite.setColor(sf::Color(200, 165, 0));
        } else {
            sprite.setColor(sf::Color::White);
        }
    } else {
        if (carried) {
            shape.setFillColor(sf::Color(200, 165, 0));
        } else {
            shape.setFillColor(sf::Color(255, 215, 0));
        }
    }
}

bool Package::isDelivered() const {
    return delivered;
}

void Package::setDelivered(bool delivered) {
    this->delivered = delivered;
    if (textureLoaded) {
        if (delivered) {
            sprite.setColor(sf::Color(100, 100, 100, 100));
        } else {
            sprite.setColor(sf::Color::White);
        }
    } else {
        if (delivered) {
            shape.setFillColor(sf::Color(100, 100, 100, 100));
        } else {
            shape.setFillColor(sf::Color(255, 215, 0));
        }
    }
}

void Package::setPosition(const sf::Vector2f& newPosition) {
    position = newPosition;
    if (textureLoaded) {
        sprite.setPosition(position);
    } else {
        shape.setPosition(position);
    }
}