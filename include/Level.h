#pragma once
#include "Player.h"
#include "Platform.h"
#include <vector>
#include <memory>

class Level {
public:
    Level();
    
    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    
    void handleCollisions();
    void createFirstLocation();
    
    Player& getPlayer();
    
private:
    Player player;
    std::vector<std::unique_ptr<Platform>> platforms;
    
    // Границы уровня
    sf::FloatRect levelBounds;
};