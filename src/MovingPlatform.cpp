#include "MovingPlatform.h"
#include <iostream>
#include <cmath>

MovingPlatform::MovingPlatform(const sf::Vector2f& size, const sf::Vector2f& startPos, 
                               const sf::Vector2f& endPos, float speed, bool horizontal)
    : startPosition(startPos)
    , endPosition(endPos)
    , movementSpeed(speed)
    , isHorizontal(horizontal)
{
    shape.setSize(size);
    shape.setFillColor(sf::Color(150, 100, 250, 255));
    shape.setOutlineColor(sf::Color(200, 150, 255, 255));
    shape.setOutlineThickness(2.f);
    
    currentTarget = endPosition;
    position = startPos;
    shape.setPosition(position);
}

void MovingPlatform::update(float deltaTime) {
    updateMovement(deltaTime);
    shape.setPosition(position); // ОБНОВЛЯЕМ ПОЗИЦИЮ ГРАФИКИ КАЖДЫЙ КАДР
}

void MovingPlatform::updateMovement(float deltaTime) {
    // Вычисляем направление к текущей цели
    sf::Vector2f direction = currentTarget - position;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    if (distance > 1.f) {
        // Нормализуем направление
        if (distance > 0) {
            direction.x /= distance;
            direction.y /= distance;
        }
        
        // Двигаем платформу
        position += direction * movementSpeed * deltaTime;
        
    } else {
        // Достигли цели - меняем направление
        if (currentTarget == endPosition) {
            currentTarget = startPosition;
        } else {
            currentTarget = endPosition;
        }
    }
}

void MovingPlatform::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

sf::FloatRect MovingPlatform::getBounds() const {
    return shape.getGlobalBounds();
}

sf::Vector2f MovingPlatform::getMovement() const {
    sf::Vector2f direction = currentTarget - position;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    if (distance > 0) {
        direction.x /= distance;
        direction.y /= distance;
    }
    
    // ВОЗВРАЩАЕМ СКОРОСТЬ ЗА СЕКУНДУ (без умножения на deltaTime)
    return direction * movementSpeed;
}