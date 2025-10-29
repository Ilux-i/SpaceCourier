#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.h"

class DeliveryPoint : public Entity {
public:
    DeliveryPoint(const sf::Vector2f& position);
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) const override;
    sf::FloatRect getBounds() const override;
    
    bool isActive() const;
    
private:
    sf::RectangleShape shape;
    bool active;
};