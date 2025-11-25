#include "Platform.h"
#include <iostream>

Platform::Platform(const sf::Vector2f& size, const sf::Vector2f& position, const sf::Color& color) 
    : textureLoaded(false),
      sprite(texture)
{
    shape.setSize(size);
    shape.setFillColor(color);
    shape.setPosition(position);
    this->position = position;
    
    loadTexture();
}

void Platform::loadTexture() {
    if (!texture.loadFromFile("assets/sprites/environment/platform.png")) {
        std::cout << "❌ Не удалось загрузить текстуру платформы, используем геометрическую форму" << std::endl;
        textureLoaded = false;
        return;
    }
    
    textureLoaded = true;
    
    sf::Vector2f platformSize = shape.getSize();
    
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(
        sf::Vector2i(0, 0), 
        sf::Vector2i(static_cast<int>(platformSize.x), static_cast<int>(platformSize.y))
    ));
    
    texture.setRepeated(true);
    
    sprite.setPosition(position);
}

void Platform::update(float deltaTime) {
    if (textureLoaded) {
        sprite.setPosition(position);
    } else {
        shape.setPosition(position);
    }
}

void Platform::draw(sf::RenderWindow& window) const {
    if (textureLoaded) {
        window.draw(sprite);
    } else {
        window.draw(shape);
    }
}

sf::FloatRect Platform::getBounds() const {
    if (textureLoaded) {
        return sprite.getGlobalBounds();
    }
    return shape.getGlobalBounds();
}