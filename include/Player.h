#pragma once
#include "Entity.h"
#include "HealthSystem.h"
#include <SFML/Graphics.hpp>

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
    
private:
    sf::CircleShape shape;
    bool onGround;
    float jumpForce;
    float moveSpeed;
    float gravity;
    
    HealthSystem healthSystem;  // Добавляем систему здоровья
};