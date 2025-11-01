#pragma once
#include "Entity.h"
#include "HealthSystem.h"
#include "Package.h" // ДОБАВЛЯЕМ
#include <SFML/Graphics.hpp>
#include "PackageIndicator.h"

class Player : public Entity {
public:
    Player();
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) const override;
    sf::FloatRect getBounds() const override;
    
    void jump();
    void move(const sf::Vector2f& direction);
    void applyGravity(float deltaTime);
    
    bool isOnGround() const;
    void setOnGround(bool grounded);
    
    HealthSystem& getHealthSystem();
    void takeDamage();
    
    // НОВЫЕ МЕТОДЫ ДЛЯ ПОСЫЛОК
    bool isCarryingPackage() const;
    void pickUpPackage(Package* package);
    void deliverPackage();
    Package* getCarriedPackage();
    float getMoveSpeed() const;
    
private:
    sf::CircleShape shape;
    bool onGround;
    float jumpForce;
    float moveSpeed;
    float gravity;
    
    HealthSystem healthSystem;
    
    // ПОСЫЛКА
    Package* carriedPackage;
    float baseMoveSpeed; // Базовая скорость без посылки
    PackageIndicator packageIndicator;
};