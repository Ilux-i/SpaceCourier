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
}

void HealthSystem::createHearts() {
    hearts.clear();
    
    sf::Texture emptyTexture;
    
    for (int i = 0; i < maxHealth; ++i) {

        sf::Sprite heart(textureLoaded ? heartTexture : emptyTexture);
        
        if (textureLoaded) {
            heart.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32, 32)));
            heart.setScale(sf::Vector2f(2.0f, 2.0f));
        }
        
        heart.setPosition(sf::Vector2f(1100.f - i * 70.f, 20.f));
        
        if (i < health) {
            heart.setColor(sf::Color::White);
        } else {
            heart.setColor(sf::Color(100, 100, 100, 150));
        }
        
        hearts.push_back(heart);
    }
    
}

void HealthSystem::takeDamage() {
    if (!isInvincible && health > 0) {
        health--;
        invincibilityTimer = 1.5f;
        isInvincible = true;
        
        createHearts();
    }
}

void HealthSystem::heal() {
    if (health < maxHealth) {
        health++;
        
        createHearts();
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
}

void HealthSystem::update(float deltaTime) {
    if (isInvincible) {
        invincibilityTimer -= deltaTime;
        if (invincibilityTimer <= 0.f) {
            isInvincible = false;
        }
    }
}

void HealthSystem::draw(sf::RenderWindow& window) const {
    for (const auto& heart : hearts) {
        window.draw(heart);
    }
}