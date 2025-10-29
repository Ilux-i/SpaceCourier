#include "Player.h"
#include <iostream>

Player::Player() : onGround(false), jumpForce(500.f), moveSpeed(300.f), gravity(800.f), carriedPackage(nullptr) {
    shape.setRadius(25.f);
    shape.setFillColor(sf::Color(220, 50, 50, 255));
    shape.setPosition(position);
    baseMoveSpeed = moveSpeed; // Сохраняем базовую скорость
}

void Player::update(float deltaTime) {
    healthSystem.update(deltaTime);
    
    // Обновляем позицию посылки если несём её
    if (carriedPackage && !carriedPackage->isDelivered()) {
        // Позиция посылки над головой игрока
        carriedPackage->setPosition(sf::Vector2f(
            position.x - 5.f, 
            position.y - 40.f
        ));
        carriedPackage->update(deltaTime);
    }
    
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
    
    // Рисуем посылку если несём её
    if (carriedPackage && !carriedPackage->isDelivered()) {
        carriedPackage->draw(window);
    }
    
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

// НОВЫЕ МЕТОДЫ ДЛЯ РАБОТЫ С ПОСЫЛКАМИ

bool Player::isCarryingPackage() const {
    return carriedPackage != nullptr && !carriedPackage->isDelivered();
}

void Player::pickUpPackage(Package* package) {
    if (!isCarryingPackage() && package && !package->isDelivered()) {
        carriedPackage = package;
        carriedPackage->setCarried(true);
        
        // Замедление скорости на 20% при ношении посылки
        moveSpeed = baseMoveSpeed * 0.8f;
        
        std::cout << "📦 Игрок поднял посылку! Скорость уменьшена на 20%" << std::endl;
        std::cout << "   Новая скорость: " << moveSpeed << " (базовая: " << baseMoveSpeed << ")" << std::endl;
    } else {
        std::cout << "❌ Не могу поднять посылку: ";
        if (isCarryingPackage()) std::cout << "уже несу посылку";
        else if (!package) std::cout << "посылка nullptr";
        else if (package->isDelivered()) std::cout << "посылка уже доставлена";
        std::cout << std::endl;
    }
}

void Player::deliverPackage() {
    if (carriedPackage) {
        carriedPackage->setDelivered(true);
        carriedPackage->setCarried(false);
        carriedPackage = nullptr;
        
        // Восстанавливаем нормальную скорость
        moveSpeed = baseMoveSpeed;
        
        std::cout << "✅ Посылка доставлена! Скорость восстановлена: " << moveSpeed << std::endl;
    } else {
        std::cout << "❌ Не могу доставить посылку: не несу посылку" << std::endl;
    }
}

Package* Player::getCarriedPackage() {
    return carriedPackage;
}

float Player::getMoveSpeed() const {
    return moveSpeed;
}