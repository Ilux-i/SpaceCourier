#include "HealthKit.h"
#include <iostream>
#include <cmath>

HealthKit::HealthKit(const sf::Vector2f& position) 
    : collected(false), animationTimer(0.f) { // УБИРАЕМ respawnTimer
    this->position = position;
    createHealthKit();
}

void HealthKit::createHealthKit() {
    shape.setRadius(15.f);
    shape.setFillColor(sf::Color(255, 50, 50, 255));
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(2.f);
    shape.setPosition(position);
    shape.setPointCount(4); // Ромб вместо круга
}

void HealthKit::update(float deltaTime) {
    if (!collected) {
        updateAnimation(deltaTime);
    }
    // УБИРАЕМ ЛОГИКУ АВТОРЕСПАВНА ЧЕРЕЗ 10 СЕКУНД
}

void HealthKit::updateAnimation(float deltaTime) {
    animationTimer += deltaTime;
    
    // Пульсация и подпрыгивание
    float scale = 1.0f + 0.1f * std::sin(animationTimer * 5.f);
    shape.setScale(sf::Vector2f(scale, scale));
    
    // Лёгкое подпрыгивание
    float bounce = 2.f * std::sin(animationTimer * 3.f);
    shape.setPosition(sf::Vector2f(position.x, position.y + bounce));
}

void HealthKit::draw(sf::RenderWindow& window) const {
    if (!collected) {
        window.draw(shape);
    }
}

sf::FloatRect HealthKit::getBounds() const {
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
    animationTimer = 0.f;
    shape.setScale(sf::Vector2f(1.f, 1.f));
    shape.setPosition(position);
    std::cout << "🔄 Аптечка восстановилась!" << std::endl;
}