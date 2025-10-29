#include "Package.h"

Package::Package(const sf::Vector2f& position) : carried(false), delivered(false) {
    shape.setSize(sf::Vector2f(30.f, 30.f));
    shape.setFillColor(sf::Color(255, 215, 0)); // Золотой цвет посылки
    shape.setPosition(position);
    this->position = position;
}

void Package::update(float deltaTime) {
    // ОБНОВЛЯЕМ ПОЗИЦИЮ ФОРМЫ
    shape.setPosition(position);
}

void Package::draw(sf::RenderWindow& window) const {
    if (!delivered) {
        window.draw(shape);
    }
}

sf::FloatRect Package::getBounds() const {
    return shape.getGlobalBounds();
}

bool Package::isCarried() const {
    return carried;
}

void Package::setCarried(bool carried) {
    this->carried = carried;
    if (carried) {
        shape.setFillColor(sf::Color(200, 165, 0)); // Темнее когда несут
    } else {
        shape.setFillColor(sf::Color(255, 215, 0)); // Обычный золотой
    }
}

bool Package::isDelivered() const {
    return delivered;
}

void Package::setDelivered(bool delivered) {
    this->delivered = delivered;
    if (delivered) {
        shape.setFillColor(sf::Color(100, 100, 100, 100)); // Полупрозрачная серая при доставке
    } else {
        shape.setFillColor(sf::Color(255, 215, 0)); // Возвращаем золотой цвет
    }
}

// ДОБАВЛЯЕМ ЯВНЫЙ СЕТТЕР ДЛЯ ПОЗИЦИИ
void Package::setPosition(const sf::Vector2f& newPosition) {
    position = newPosition;
    shape.setPosition(position); // СРАЗУ ОБНОВЛЯЕМ ФОРМУ
}