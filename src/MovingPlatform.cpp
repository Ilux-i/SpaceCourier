#include "MovingPlatform.h"
#include <iostream>
#include <cmath>

MovingPlatform::MovingPlatform(const sf::Vector2f& size, const sf::Vector2f& startPos, 
                               const sf::Vector2f& endPos, float speed, bool horizontal)
    : startPosition(startPos), endPosition(endPos), movementSpeed(speed), 
      isHorizontal(horizontal), textureLoaded(false),
      sprite(texture)
{

    shape.setSize(size);
    shape.setFillColor(sf::Color(150, 100, 250, 255));
    shape.setOutlineColor(sf::Color(200, 150, 255, 255));
    shape.setOutlineThickness(2.f);
    
    currentTarget = endPosition;
    position = startPos;
    shape.setPosition(position);
    
    loadTexture();
}

void MovingPlatform::loadTexture() {
    if (!texture.loadFromFile("assets/sprites/environment/moving_platform.png")) {
        std::cout << "❌ Не удалось загрузить текстуру движущейся платформы, используем геометрическую форму" << std::endl;
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

void MovingPlatform::update(float deltaTime) {
    updateMovement(deltaTime);
    
    if (textureLoaded) {
        sprite.setPosition(position);
    } else {
        shape.setPosition(position);
    }
}

void MovingPlatform::updateMovement(float deltaTime) {
    sf::Vector2f direction = currentTarget - position;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    if (distance > 1.f) {
        if (distance > 0) {
            direction.x /= distance;
            direction.y /= distance;
        }
        
        position += direction * movementSpeed * deltaTime;
        
    } else {
        if (currentTarget == endPosition) {
            currentTarget = startPosition;
        } else {
            currentTarget = endPosition;
        }
    }
}

void MovingPlatform::draw(sf::RenderWindow& window) const {
    if (textureLoaded) {
        window.draw(sprite);
    } else {
        window.draw(shape);
    }
}

sf::FloatRect MovingPlatform::getBounds() const {
    if (textureLoaded) {
        return sprite.getGlobalBounds();
    }
    return shape.getGlobalBounds();
}

sf::Vector2f MovingPlatform::getMovement() const {
    sf::Vector2f direction = currentTarget - position;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    if (distance > 0) {
        direction.x /= distance;
        direction.y /= distance;
    }
    
    return direction * movementSpeed;
}