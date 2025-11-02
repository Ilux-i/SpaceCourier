#pragma once
#include "Player.h"
#include "Platform.h"
#include "MovingPlatform.h"
#include "Enemy.h"
#include "Package.h"
#include "DeliveryPoint.h"
#include "AcidPool.h"
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
    
    void handleEInteraction();
    
private:
    Player player;
    std::vector<std::unique_ptr<Platform>> platforms;
    std::vector<std::unique_ptr<MovingPlatform>> movingPlatforms;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Package>> packages;
    std::vector<std::unique_ptr<DeliveryPoint>> deliveryPoints;
    std::vector<std::unique_ptr<AcidPool>> acidPools;
    
    sf::FloatRect levelBounds;

    const MovingPlatform* playerOnMovingPlatform; 
    
    std::vector<sf::Vector2f> packageStartPositions;
    std::vector<sf::Vector2f> deliveryPointStartPositions;
    
    void handlePlayerEnemyCollisions();
    void handlePlayerPlatformCollision(const Platform& platform);
    void handlePlayerMovingPlatformCollision(const MovingPlatform& platform);
    void handlePlayerAcidCollisions();
    void respawnLevel();
    void respawnPlayer();
};