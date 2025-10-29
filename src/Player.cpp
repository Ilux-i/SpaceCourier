#include "Player.h"
#include <iostream>

Player::Player() : onGround(false), jumpForce(500.f), moveSpeed(300.f), gravity(800.f) {
    shape.setRadius(25.f);
    shape.setFillColor(sf::Color(220, 50, 50, 255));
    shape.setPosition(position);
}

void Player::update(float deltaTime) {
    healthSystem.update(deltaTime);
    
    // Применяем гравитацию
    if (!onGround) {
        velocity.y += gravity * deltaTime;
    }
    
    // Обновляем позицию
    position += velocity * deltaTime;
    shape.setPosition(position);
    
    // Сбрасываем горизонтальную скорость для плавного управления
    velocity.x = 0;
}

void Player::draw(sf::RenderWindow& window) const {
    window.draw(shape);
    healthSystem.draw(window);  // Рисуем здоровье
}

sf::FloatRect Player::getBounds() const {
    return shape.getGlobalBounds();
}

void Player::jump() {
    if (onGround && healthSystem.isAlive()) {
        velocity.y = -jumpForce;
        onGround = false;
        std::cout << "🔼 Игрок прыгнул!" << std::endl;
    }
}

void Player::move(const sf::Vector2f& direction) {
    if (healthSystem.isAlive()) {
        velocity.x = direction.x * moveSpeed;
    }
}

void Player::applyGravity(float deltaTime) {
    if (!onGround) {
        velocity.y += gravity * deltaTime;
    }
}

bool Player::isOnGround() const {
    return onGround;
}

void Player::setOnGround(bool grounded) {
    onGround = grounded;
}

HealthSystem& Player::getHealthSystem() {
    return healthSystem;
}

void Player::takeDamage() {
    healthSystem.takeDamage();
}