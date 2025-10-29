#pragma once
#include "Player.h"
#include "Platform.h"
#include "Enemy.h"
#include "Package.h"
#include "DeliveryPoint.h"
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
    
    // ЕДИНСТВЕННЫЙ МЕТОД ДЛЯ ВЗАИМОДЕЙСТВИЯ С E
    void handleEInteraction();
    
private:
    Player player;
    std::vector<std::unique_ptr<Platform>> platforms;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Package>> packages;
    std::vector<std::unique_ptr<DeliveryPoint>> deliveryPoints;
    
    sf::FloatRect levelBounds;
    
    // НАЧАЛЬНЫЕ ПОЗИЦИИ ДЛЯ РЕСПАВНА
    std::vector<sf::Vector2f> packageStartPositions;
    std::vector<sf::Vector2f> deliveryPointStartPositions;
    
    void handlePlayerEnemyCollisions();
    void handlePlayerPlatformCollision(const Platform& platform);
    void respawnLevel();
    void respawnPlayer();
};