#include "HealthSystem.h"
#include <iostream>

HealthSystem::HealthSystem() : health(3), maxHealth(3), invincibilityTimer(0.f), isInvincible(false) {
    createHearts();
}

void HealthSystem::takeDamage() {
    if (!isInvincible && health > 0) {
        health--;
        invincibilityTimer = 1.5f; // 1.5 секунды неуязвимости
        isInvincible = true;
        
        // ОБНОВЛЯЕМ ОТОБРАЖЕНИЕ СЕРДЕЦ ПРИ ПОЛУЧЕНИИ УРОНА
        createHearts();
        
        std::cout << "💔 Получен урон! Здоровье: " << health << "/" << maxHealth << std::endl;
        
        if (health <= 0) {
            std::cout << "💀 Игрок умер!" << std::endl;
        }
    }
}

void HealthSystem::heal() {
    if (health < maxHealth) {
        health++;
        
        // ОБНОВЛЯЕМ ОТОБРАЖЕНИЕ СЕРДЕЦ ПРИ ЛЕЧЕНИИ
        createHearts();
        
        std::cout << "❤️ Восстановлено здоровье: " << health << "/" << maxHealth << std::endl;
    }
}

bool HealthSystem::isAlive() const {
    return health > 0;
}

int HealthSystem::getHealth() const {
    return health;
}

void HealthSystem::reset() {
    health = maxHealth;
    invincibilityTimer = 0.f;
    isInvincible = false;
    
    // ОБНОВЛЯЕМ ОТОБРАЖЕНИЕ СЕРДЕЦ ПРИ СБРОСЕ
    createHearts();
    
    std::cout << "🔄 Здоровье сброшено: " << health << "/" << maxHealth << std::endl;
}

void HealthSystem::update(float deltaTime) {
    if (isInvincible) {
        invincibilityTimer -= deltaTime;
        if (invincibilityTimer <= 0.f) {
            isInvincible = false;
            std::cout << "🛡️ Неуязвимость закончилась" << std::endl;
        }
    }
}

void HealthSystem::draw(sf::RenderWindow& window) const {
    for (const auto& heart : hearts) {
        window.draw(heart);
    }
}

void HealthSystem::createHearts() {
    hearts.clear();
    
    for (int i = 0; i < maxHealth; ++i) {
        sf::CircleShape heart(15.f);
        heart.setPosition(sf::Vector2f(1100.f - i * 40.f, 20.f)); 
        
        // Красное если есть здоровье, серое если нет
        if (i < health) {
            heart.setFillColor(sf::Color(255, 100, 100, 255)); // Красное сердце
        } else {
            heart.setFillColor(sf::Color(100, 100, 100, 150)); // Серое прозрачное сердце
        }
        
        hearts.push_back(heart);
    }
    
    // Отладочная информация
    std::cout << "❤️ Обновлено отображение сердец: " << health << "/" << maxHealth << std::endl;
}