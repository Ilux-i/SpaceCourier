#include "Player.h"
#include <iostream>

Player::Player() : onGround(false), jumpForce(500.f), moveSpeed(300.f), gravity(800.f) {
    shape.setRadius(25.f);
    shape.setFillColor(sf::Color(220, 50, 50, 255));
    shape.setPosition(position);
}

void Player::update(float deltaTime) {
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
}

sf::FloatRect Player::getBounds() const {
    return shape.getGlobalBounds();
}

void Player::jump() {
    if (onGround) {
        velocity.y = -jumpForce;
        onGround = false;
    }
}

void Player::move(const sf::Vector2f& direction) {
    velocity.x = direction.x * moveSpeed;
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