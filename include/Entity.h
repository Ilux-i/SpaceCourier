#pragma once
#include <SFML/Graphics.hpp>

class Entity {
public:
    Entity();
    virtual ~Entity() = default;
    
    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;
    
    // Виртуальный метод для коллизий
    virtual sf::FloatRect getBounds() const = 0;
    bool checkCollision(const Entity& other) const;
    
    // Геттеры/сеттеры
    void setPosition(const sf::Vector2f& position);
    sf::Vector2f getPosition() const;
    void setVelocity(const sf::Vector2f& velocity);
    sf::Vector2f getVelocity() const;
    
protected:
    sf::Vector2f position;
    sf::Vector2f velocity;
};