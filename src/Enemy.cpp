#include "Enemy.h"
#include <iostream>
#include <cmath>

Enemy::Enemy(const sf::Vector2f& startPos, const sf::Vector2f& endPos)
    : startPosition(startPos), endPosition(endPos), patrolSpeed(100.f), active(true), stunTimer(0.f) {
    
    shape.setSize(sf::Vector2f(40.f, 40.f));
    shape.setFillColor(sf::Color(255, 100, 100, 255));
    shape.setPosition(startPosition);
    position = startPosition;
    
    // Направление патрулирования
    patrolDirection = endPosition - startPosition;
    float length = std::sqrt(patrolDirection.x * patrolDirection.x + patrolDirection.y * patrolDirection.y);
    if (length > 0) {
        patrolDirection.x /= length;
        patrolDirection.y /= length;
    }
}

void Enemy::update(float deltaTime) {
    if (!active) {
        stunTimer -= deltaTime;
        if (stunTimer <= 0.f) {
            active = true;
            shape.setFillColor(sf::Color(255, 100, 100, 255)); // Красный - активен
        }
        return;
    }
    
    updatePatrol(deltaTime);
    shape.setPosition(position);
}

void Enemy::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

sf::FloatRect Enemy::getBounds() const {
    return shape.getGlobalBounds();
}

void Enemy::updatePatrol(float deltaTime) {
    // Двигаем дрона
    position += patrolDirection * patrolSpeed * deltaTime;
    
    // ПРОСТАЯ И НАДЕЖНАЯ ПРОВЕРКА ГРАНИЦ
    float distanceToStart = std::sqrt(
        (position.x - startPosition.x) * (position.x - startPosition.x) + 
        (position.y - startPosition.y) * (position.y - startPosition.y)
    );
    
    float distanceToEnd = std::sqrt(
        (position.x - endPosition.x) * (position.x - endPosition.x) + 
        (position.y - endPosition.y) * (position.y - endPosition.y)
    );
    
    // Если достигли конечной точки ИЛИ перешли за нее - разворачиваемся
    if (distanceToEnd < 5.f || distanceToStart > std::sqrt(
        (endPosition.x - startPosition.x) * (endPosition.x - startPosition.x) + 
        (endPosition.y - startPosition.y) * (endPosition.y - startPosition.y)
    )) {
        patrolDirection = -patrolDirection;
        std::cout << "🔄 Дрон развернулся!" << std::endl;
    }
}

void Enemy::onCollisionWithPlayer() {
    if (active) {
        active = false;
        stunTimer = 2.0f; // 2 секунды стана
        shape.setFillColor(sf::Color(150, 150, 150, 255)); // Серый - оглушен
        std::cout << "🤖 Дрон оглушен на 2 секунды!" << std::endl;
    }
}

bool Enemy::isActive() const {
    return active;
}

bool Enemy::isStunned() const {
    return !active;
}