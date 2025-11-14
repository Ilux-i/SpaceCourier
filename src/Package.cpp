#include "Package.h"
#include <iostream>

Package::Package(const sf::Vector2f& position) 
    : carried(false), delivered(false), textureLoaded(false),
      sprite(texture) // Инициализируем с текстурой
{
    // Базовый геометрический shape (fallback)
    shape.setSize(sf::Vector2f(30.f, 30.f));
    shape.setFillColor(sf::Color(255, 215, 0)); // Золотой цвет посылки
    shape.setPosition(position);
    this->position = position;
    
    // Загружаем текстуру
    loadTexture();
}

void Package::loadTexture() {
    if (!texture.loadFromFile("assets/sprites/objects/package.png")) {
        std::cout << "❌ Не удалось загрузить текстуру посылки, используем геометрическую форму" << std::endl;
        textureLoaded = false;
        return;
    }
    
    textureLoaded = true;
    
    // Настраиваем спрайт
    sprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32, 32)));
    sprite.setScale(sf::Vector2f(0.9f, 0.9f)); // Немного меньше для соответствия размерам
    sprite.setPosition(position);
    
    std::cout << "✅ Текстура посылки загружена успешно!" << std::endl;
}

void Package::update(float deltaTime) {
    // ОБНОВЛЯЕМ ПОЗИЦИЮ ФОРМЫ И СПРАЙТА
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
        return sf::FloatRect(position, sf::Vector2f(30.f, 30.f));
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
            sprite.setColor(sf::Color(200, 165, 0)); // Темнее когда несут
        } else {
            sprite.setColor(sf::Color::White); // Обычный цвет
        }
    } else {
        if (carried) {
            shape.setFillColor(sf::Color(200, 165, 0)); // Темнее когда несут
        } else {
            shape.setFillColor(sf::Color(255, 215, 0)); // Обычный золотой
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
            sprite.setColor(sf::Color(100, 100, 100, 100)); // Полупрозрачная серая при доставке
        } else {
            sprite.setColor(sf::Color::White); // Возвращаем нормальный цвет
        }
    } else {
        if (delivered) {
            shape.setFillColor(sf::Color(100, 100, 100, 100)); // Полупрозрачная серая при доставке
        } else {
            shape.setFillColor(sf::Color(255, 215, 0)); // Возвращаем золотой цвет
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