#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>

class Platform : public Entity {
public:
    Platform(const sf::Vector2f& size, const sf::Vector2f& position, const sf::Color& color = sf::Color::Green);
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) const override;
    sf::FloatRect getBounds() const override;  // Убрал override
    
private:
    sf::RectangleShape shape;
};