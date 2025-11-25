#include "HealthKit.h"
#include <iostream>

HealthKit::HealthKit(const sf::Vector2f& position) 
    : collected(false), textureLoaded(false),
      sprite(texture)
{
    shape.setRadius(15.f);
    shape.setFillColor(sf::Color(255, 50, 50, 255));
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(2.f);
    shape.setPosition(position);
    shape.setPointCount(4);
    
    this->position = position;
    
    loadTexture();
}

void HealthKit::loadTexture() {
    if (!texture.loadFromFile("assets/sprites/objects/healthkit.png")) {
        std::cout << "❌ Не удалось загрузить текстуру аптечки, используем геометрическую форму" << std::endl;
        textureLoaded = false;
        return;
    }
    
    textureLoaded = true;
    
    sprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32, 32)));
    sprite.setScale(sf::Vector2f(0.8f, 0.8f));
    sprite.setPosition(position);
    
}

void HealthKit::update(float deltaTime) {
    if (!collected) {
        if (textureLoaded) {
            sprite.setPosition(position);
        } else {
            shape.setPosition(position);
        }
    }
}

void HealthKit::draw(sf::RenderWindow& window) const {
    if (!collected) {
        if (textureLoaded) {
            window.draw(sprite);
        } else {
            window.draw(shape);
        }
    }
}

sf::FloatRect HealthKit::getBounds() const {
    if (textureLoaded) {
        sf::FloatRect spriteBounds = sprite.getGlobalBounds();
        return spriteBounds;
    }
    return shape.getGlobalBounds();
}

bool HealthKit::isCollected() const {
    return collected;
}

void HealthKit::collect() {
    if (!collected) {
        collected = true;
    }
}

void HealthKit::respawn() {
    collected = false;
    if (textureLoaded) {
        sprite.setPosition(position);
    } else {
        shape.setPosition(position);
    }
}