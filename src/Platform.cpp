#include "Platform.h"

Platform::Platform(const sf::Vector2f& size, const sf::Vector2f& position, const sf::Color& color) {
    shape.setSize(size);
    shape.setFillColor(color);
    shape.setPosition(position);
    this->position = position;
}

void Platform::update(float deltaTime) {
}

void Platform::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

sf::FloatRect Platform::getBounds() const {
    return shape.getGlobalBounds();
}