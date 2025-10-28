#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>

class Enemy : public Entity {
public:
    Enemy(const sf::Vector2f& startPos, const sf::Vector2f& endPos);
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) const override;
    sf::FloatRect getBounds() const override;
    
    // Взаимодействие с игроком
    void onCollisionWithPlayer();
    bool isActive() const;
    bool isStunned() const;
    
private:
    sf::RectangleShape shape;
    sf::Vector2f startPosition;
    sf::Vector2f endPosition;
    sf::Vector2f patrolDirection;
    float patrolSpeed;
    bool active;
    float stunTimer;
    
    void updatePatrol(float deltaTime);
};