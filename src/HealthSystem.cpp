#include "HealthSystem.h"
#include <iostream>

HealthSystem::HealthSystem() 
    : health(3), maxHealth(3), invincibilityTimer(0.f), isInvincible(false), textureLoaded(false) {
    
    loadTexture();
    createHearts();
}

void HealthSystem::loadTexture() {
    if (!heartTexture.loadFromFile("assets/sprites/ui/heart.png")) {
        std::cout << "❌ Не удалось загрузить текстуру сердца, используем геометрические формы" << std::endl;
        textureLoaded = false;
        return;
    }
    
    textureLoaded = true;
    std::cout << "✅ Текстура сердца загружена успешно!" << std::endl;
}

void HealthSystem::createHearts() {
    hearts.clear();
    
    // СОЗДАЕМ ЛОКАЛЬНУЮ ПУСТУЮ ТЕКСТУРУ
    sf::Texture emptyTexture;
    
    for (int i = 0; i < maxHealth; ++i) {
        // ИНИЦИАЛИЗИРУЕМ SPRITE С ТЕКСТУРОЙ
        sf::Sprite heart(textureLoaded ? heartTexture : emptyTexture);
        
        if (textureLoaded) {
            heart.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32, 32)));
            heart.setScale(sf::Vector2f(2.0f, 2.0f)); // Ваш увеличенный размер
        }
        
        // Позиционируем сердца в правом верхнем углу
        heart.setPosition(sf::Vector2f(1100.f - i * 70.f, 20.f)); // Увеличил расстояние
        
        // Устанавливаем прозрачность в зависимости от здоровья
        if (i < health) {
            heart.setColor(sf::Color::White); // Полное сердце
        } else {
            heart.setColor(sf::Color(100, 100, 100, 150)); // Серое прозрачное сердце
        }
        
        hearts.push_back(heart);
    }
    
    std::cout << "❤️ Обновлено отображение сердец: " << health << "/" << maxHealth << std::endl;
}

// Остальные методы остаются без изменений...
void HealthSystem::takeDamage() {
    if (!isInvincible && health > 0) {
        health--;
        invincibilityTimer = 1.5f; // 1.5 секунды неуязвимости
        isInvincible = true;
        
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