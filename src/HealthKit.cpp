#include "HealthKit.h"
#include <iostream>

HealthKit::HealthKit(const sf::Vector2f& position) 
    : collected(false), textureLoaded(false),
      sprite(texture) // Инициализируем с текстурой
{
    // Базовый геометрический shape (fallback)
    shape.setRadius(15.f);
    shape.setFillColor(sf::Color(255, 50, 50, 255));
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(2.f);
    shape.setPosition(position);
    shape.setPointCount(4); // Ромб вместо круга
    
    this->position = position;
    
    // Загружаем текстуру
    loadTexture();
}

void HealthKit::loadTexture() {
    if (!texture.loadFromFile("assets/sprites/objects/healthkit.png")) {
        std::cout << "❌ Не удалось загрузить текстуру аптечки, используем геометрическую форму" << std::endl;
        textureLoaded = false;
        return;
    }
    
    textureLoaded = true;
    
    // Настраиваем спрайт
    sprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32, 32)));
    sprite.setScale(sf::Vector2f(0.8f, 0.8f)); // Масштаб для соответствия размерам
    sprite.setPosition(position);
    
    std::cout << "✅ Текстура аптечки загружена успешно!" << std::endl;
}

void HealthKit::update(float deltaTime) {
    // СТАТИЧЕСКИЙ СПРАЙТ - БЕЗ АНИМАЦИИ
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
        return sf::FloatRect(position, sf::Vector2f(30.f, 30.f));
    }
    return shape.getGlobalBounds();
}

bool HealthKit::isCollected() const {
    return collected;
}

void HealthKit::collect() {
    if (!collected) {
        collected = true;
        std::cout << "❤️ Аптечка собрана!" << std::endl;
    }
}

void HealthKit::respawn() {
    collected = false;
    if (textureLoaded) {
        sprite.setPosition(position);
    } else {
        shape.setPosition(position);
    }
    std::cout << "🔄 Аптечка восстановилась!" << std::endl;
}