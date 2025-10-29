#include "DeliveryPoint.h"
#include <cmath>
#include <cstdint> // ДОБАВЛЯЕМ ДЛЯ uint8_t

DeliveryPoint::DeliveryPoint(const sf::Vector2f& position) : active(true) {
    shape.setSize(sf::Vector2f(50.f, 50.f));
    shape.setFillColor(sf::Color(100, 255, 100, 150)); // Зелёный полупрозрачный
    shape.setPosition(position);
    this->position = position;
}

void DeliveryPoint::update(float deltaTime) {
    // Анимация мигания для привлечения внимания
    static float timer = 0.f;
    timer += deltaTime;
    
    if (timer > 1.f) timer = 0.f;
    
    if (active) {
        float alpha = 150 + 105 * std::sin(timer * 3.14f * 2.f);
        shape.setFillColor(sf::Color(100, 255, 100, static_cast<uint8_t>(alpha)));
    }
}

void DeliveryPoint::draw(sf::RenderWindow& window) const {
    if (active) {
        window.draw(shape);
    }
}

sf::FloatRect DeliveryPoint::getBounds() const {
    return shape.getGlobalBounds();
}

bool DeliveryPoint::isActive() const {
    return active;
}