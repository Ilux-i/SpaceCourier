#include "Enemy.h"
#include <iostream>
#include <cmath>

Enemy::Enemy(const sf::Vector2f& startPos, const sf::Vector2f& endPos)
    : startPosition(startPos), endPosition(endPos), patrolSpeed(100.f), 
      active(true), stunTimer(0.f), textureLoaded(false),
      sprite(texture)
{
    shape.setSize(sf::Vector2f(40.f, 40.f));
    shape.setFillColor(sf::Color(255, 100, 100, 255));
    shape.setPosition(startPosition);
    position = startPosition;
    
    patrolDirection = endPosition - startPosition;
    float length = std::sqrt(patrolDirection.x * patrolDirection.x + patrolDirection.y * patrolDirection.y);
    if (length > 0) {
        patrolDirection.x /= length;
        patrolDirection.y /= length;
    }
    
    loadTexture();
}

void Enemy::loadTexture() {
    if (!texture.loadFromFile("assets/sprites/characters/enemy.png")) {
        std::cout << "❌ Не удалось загрузить текстуру врага, используем геометрическую форму" << std::endl;
        textureLoaded = false;
        return;
    }
    
    textureLoaded = true;
    
    sprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32, 32)));
    sprite.setScale(sf::Vector2f(1.25f, 1.25f));
    sprite.setPosition(position);
    
    std::cout << "✅ Текстура врага загружена успешно!" << std::endl;
}

void Enemy::update(float deltaTime) {
    if (!active) {
        stunTimer -= deltaTime;
        if (stunTimer <= 0.f) {
            active = true;
            if (textureLoaded) {
                sprite.setColor(sf::Color::White);
            } else {
                shape.setFillColor(sf::Color(255, 100, 100, 255));
            }
        }
        
        if (textureLoaded) {
            sprite.setPosition(position);
        } else {
            shape.setPosition(position);
        }
        return;
    }
    
    // СОХРАНЯЕМ ПРЕДЫДУЩЕЕ НАПРАВЛЕНИЕ ДЛЯ ПРОВЕРКИ ИЗМЕНЕНИЯ
    sf::Vector2f oldDirection = patrolDirection;
    
    updatePatrol(deltaTime);
    
    // ЕСЛИ НАПРАВЛЕНИЕ ИЗМЕНИЛОСЬ - ОБНОВЛЯЕМ ОТРАЖЕНИЕ СПРАЙТА
    if (textureLoaded && (patrolDirection.x != oldDirection.x)) {
        if (patrolDirection.x < 0) {
            // Движение влево - отражаем спрайт
            sprite.setScale(sf::Vector2f(-1.25f, 1.25f));
            sprite.setOrigin(sf::Vector2f(32.f, 0.f)); // Устанавливаем origin для правильного отражения
        } else if (patrolDirection.x > 0) {
            // Движение вправо - нормальный спрайт
            sprite.setScale(sf::Vector2f(1.25f, 1.25f));
            sprite.setOrigin(sf::Vector2f(0.f, 0.f));
        }
    }
    
    if (textureLoaded) {
        sprite.setPosition(position);
    } else {
        shape.setPosition(position);
    }
}

void Enemy::draw(sf::RenderWindow& window) const {
    if (textureLoaded) {
        window.draw(sprite);
        // УБИРАЕМ ДЕБАГ-КОНТУР
    } else {
        window.draw(shape);
    }
}

sf::FloatRect Enemy::getBounds() const {
    if (textureLoaded) {
        return sf::FloatRect(position, sf::Vector2f(40.f, 40.f));
    }
    return shape.getGlobalBounds();
}

void Enemy::onCollisionWithPlayer() {
    if (active) {
        active = false;
        stunTimer = 2.0f;
        
        if (textureLoaded) {
            sprite.setColor(sf::Color(150, 150, 150, 255));
        } else {
            shape.setFillColor(sf::Color(150, 150, 150, 255));
        }
        
        std::cout << "🤖 Дрон оглушен на 2 секунды!" << std::endl;
    }
}

bool Enemy::isActive() const {
    return active;
}

bool Enemy::isStunned() const {
    return !active;
}

void Enemy::updatePatrol(float deltaTime) {
    position += patrolDirection * patrolSpeed * deltaTime;
    
    float distanceToStart = std::sqrt(
        (position.x - startPosition.x) * (position.x - startPosition.x) + 
        (position.y - startPosition.y) * (position.y - startPosition.y)
    );
    
    float distanceToEnd = std::sqrt(
        (position.x - endPosition.x) * (position.x - endPosition.x) + 
        (position.y - endPosition.y) * (position.y - endPosition.y)
    );
    
    if (distanceToEnd < 5.f || distanceToStart > std::sqrt(
        (endPosition.x - startPosition.x) * (endPosition.x - startPosition.x) + 
        (endPosition.y - startPosition.y) * (endPosition.y - startPosition.y)
    )) {
        patrolDirection = -patrolDirection;
        std::cout << "🔄 Дрон развернулся!" << std::endl;
    }
}