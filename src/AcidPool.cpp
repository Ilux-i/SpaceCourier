#include "AcidPool.h"
#include <iostream>
#include <cmath>

AcidPool::AcidPool(const sf::Vector2f& size, const sf::Vector2f& position) 
    : playerInAcid(false), damageCooldown(0.f) {
    
    shape.setSize(size);
    shape.setPosition(position);
    this->position = position;
    
    createAcidEffect();
}

void AcidPool::createAcidEffect() {
    // Очень яркий кислотный цвет
    shape.setFillColor(sf::Color(0, 255, 0, 200)); // Ярко-зелёный
    shape.setOutlineColor(sf::Color(255, 255, 0, 255)); // Жёлтая обводка
    shape.setOutlineThickness(2.f);
}

void AcidPool::update(float deltaTime) {
    if (playerInAcid) {
        damageCooldown -= deltaTime;
    }
    
    // Интенсивная анимация мерцания
    static float timer = 0.f;
    timer += deltaTime;
    
    float alpha = 150 + 105 * std::sin(timer * 6.f); // Быстрое мерцание
    shape.setFillColor(sf::Color(0, 255, 0, static_cast<uint8_t>(alpha)));
    
    // Яркая пульсирующая обводка когда игрок в кислоте
    if (playerInAcid) {
        float outlineAlpha = 150 + 105 * std::sin(timer * 8.f);
        shape.setOutlineColor(sf::Color(255, 0, 0, static_cast<uint8_t>(outlineAlpha))); // Красная при контакте
    } else {
        shape.setOutlineColor(sf::Color(255, 255, 0, 255)); // Жёлтая в обычном состоянии
    }
}

void AcidPool::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

sf::FloatRect AcidPool::getBounds() const {
    return shape.getGlobalBounds();
}

bool AcidPool::isPlayerInAcid() const {
    return playerInAcid;
}

void AcidPool::setPlayerInAcid(bool inAcid) {
    playerInAcid = inAcid;
}

float AcidPool::getDamageCooldown() const {
    return damageCooldown;
}

void AcidPool::resetDamageCooldown() {
    damageCooldown = 1.0f; // 1 секунда между получением урона
}