#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.h"

class Package : public Entity {
public:
    Package(const sf::Vector2f& position);
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) const override;
    sf::FloatRect getBounds() const override;
    
    bool isCarried() const;
    void setCarried(bool carried);
    bool isDelivered() const;
    void setDelivered(bool delivered);
    void setPosition(const sf::Vector2f& newPosition);
    
private:
    sf::RectangleShape shape;
    bool carried;
    bool delivered;
};