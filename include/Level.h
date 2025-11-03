#pragma once
#include "Player.h"
#include "Platform.h"
#include "MovingPlatform.h"
#include "Enemy.h"
#include "Package.h"
#include "DeliveryPoint.h"
#include "AcidPool.h"
#include "HealthKit.h"
#include <vector>
#include <memory>

class Level {
public:
    Level();
    Level(int levelNumber);
    
    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    
    void handleCollisions();
    
    Player& getPlayer();
    
    void handleEInteraction();
    bool isLevelComplete() const;
    
private:
    Player player;
    std::vector<std::unique_ptr<Platform>> platforms;
    std::vector<std::unique_ptr<MovingPlatform>> movingPlatforms;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Package>> packages;
    std::vector<std::unique_ptr<DeliveryPoint>> deliveryPoints;
    std::vector<std::unique_ptr<AcidPool>> acidPools;
    std::vector<std::unique_ptr<HealthKit>> healthKits;
    
    sf::FloatRect levelBounds;
    int currentLevelNumber;
    bool levelCompleted;
    
    std::vector<sf::Vector2f> packageStartPositions;
    std::vector<sf::Vector2f> deliveryPointStartPositions;
    
    // УБИРАЕМ playerOnMovingPlatform - используем локальную переменную в методах
    const MovingPlatform* playerOnMovingPlatform; // ДОБАВЛЯЕМ ОБРАТНО
    
    void handlePlayerEnemyCollisions();
    void handlePlayerPlatformCollision(const Platform& platform);
    void handlePlayerMovingPlatformCollision(const MovingPlatform& platform);
    void handlePlayerAcidCollisions();
    void handlePlayerHealthKitCollisions();
    void respawnLevel();
    void respawnPlayer();
    
    void createLevel1();
    void createLevel2();
    void createLevel3();
    void createLevel4();
    void createLevel5();
};