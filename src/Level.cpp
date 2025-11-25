#include "Level.h"
#include <iostream>
#include <algorithm>
#include <cmath>

Level::Level(int levelNumber) 
    : currentLevelNumber(levelNumber), levelCompleted(false), playerOnMovingPlatform(nullptr) {
    
    levelBounds = sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(1200.f, 800.f));
    
    switch(levelNumber) {
        case 1: createLevel1(); break;
        case 2: createLevel2(); break;
        case 3: createLevel3(); break;
        case 4: createLevel4(); break;
        case 5: createLevel5(); break;
        default: createLevel1(); break;
    }
    
}

void Level::update(float deltaTime) {
    if (player.getHealthSystem().isAlive()) {
        for (auto& movingPlatform : movingPlatforms) {
            movingPlatform->update(deltaTime);
        }
        
        if (playerOnMovingPlatform) {
            sf::Vector2f platformMovement = playerOnMovingPlatform->getMovement();
            
            sf::Vector2f newPosition = player.getPosition() + platformMovement * deltaTime;
            player.setPosition(newPosition);
            
            player.setVelocity(sf::Vector2f(
                player.getVelocity().x + platformMovement.x,
                player.getVelocity().y
            ));
        }
        
        player.update(deltaTime);
        
        for (auto& enemy : enemies) {
            enemy->update(deltaTime);
        }
        
        for (auto& deliveryPoint : deliveryPoints) {
            deliveryPoint->update(deltaTime);
        }
        
        for (auto& acidPool : acidPools) {
            acidPool->update(deltaTime);
        }
        
        for (auto& healthKit : healthKits) {
            healthKit->update(deltaTime);
        }
        
        handleCollisions();
        handlePlayerAcidCollisions();
        handlePlayerHealthKitCollisions();
    } else {
        respawnLevel();
    }
}

void Level::draw(sf::RenderWindow& window) const {
    for (const auto& platform : platforms) {
        platform->draw(window);
    }
    
    for (const auto& movingPlatform : movingPlatforms) {
        movingPlatform->draw(window);
    }
    
    for (const auto& acidPool : acidPools) {
        acidPool->draw(window);
    }
    
    for (const auto& deliveryPoint : deliveryPoints) {
        deliveryPoint->draw(window);
    }
    
    for (const auto& healthKit : healthKits) {
        healthKit->draw(window);
    }
    
    for (const auto& package : packages) {
        package->draw(window);
    }
    
    for (const auto& enemy : enemies) {
        enemy->draw(window);
    }
    
    player.draw(window);
}

void Level::handleCollisions() {
    player.setOnGround(false);
    bool onMovingPlatform = false;
    
    for (const auto& platform : platforms) {
        if (player.getBounds().findIntersection(platform->getBounds()).has_value()) {
            handlePlayerPlatformCollision(*platform);
        }
    }
    
    for (const auto& movingPlatform : movingPlatforms) {
        if (player.getBounds().findIntersection(movingPlatform->getBounds()).has_value()) {
            handlePlayerMovingPlatformCollision(*movingPlatform);
            onMovingPlatform = true;
        }
    }
    
    if (!onMovingPlatform && playerOnMovingPlatform) {
        if (!player.getBounds().findIntersection(playerOnMovingPlatform->getBounds()).has_value()) {
            playerOnMovingPlatform = nullptr;
        }
    }
    
    handlePlayerEnemyCollisions();
    
    sf::Vector2f playerPos = player.getPosition();
    sf::FloatRect playerBounds = player.getBounds();
    
    sf::Vector2f levelPos = levelBounds.position;
    sf::Vector2f levelSize = levelBounds.size;
    
    if (playerPos.x < levelPos.x) {
        player.setPosition(sf::Vector2f(levelPos.x, playerPos.y));
    }
    if (playerPos.x + playerBounds.size.x > levelPos.x + levelSize.x) {
        player.setPosition(sf::Vector2f(levelPos.x + levelSize.x - playerBounds.size.x, playerPos.y));
    }
    if (playerPos.y < levelPos.y) {
        player.setPosition(sf::Vector2f(playerPos.x, levelPos.y));
        player.setVelocity(sf::Vector2f(player.getVelocity().x, 0.f));
    }
    if (playerPos.y > levelPos.y + levelSize.y) {
        player.takeDamage();
        respawnLevel();
    }
}

void Level::handlePlayerPlatformCollision(const Platform& platform) {
    sf::FloatRect playerBounds = player.getBounds();
    sf::FloatRect platformBounds = platform.getBounds();
    
    auto intersection = playerBounds.findIntersection(platformBounds);
    if (!intersection.has_value()) return;
    
    sf::FloatRect overlap = intersection.value();
    
    if (overlap.size.x < overlap.size.y) {
        if (playerBounds.position.x < platformBounds.position.x) {
            player.setPosition(sf::Vector2f(
                platformBounds.position.x - playerBounds.size.x,
                player.getPosition().y
            ));
        } else {
            player.setPosition(sf::Vector2f(
                platformBounds.position.x + platformBounds.size.x,
                player.getPosition().y
            ));
        }
        player.setVelocity(sf::Vector2f(0.f, player.getVelocity().y));
    } else {
        if (playerBounds.position.y < platformBounds.position.y) {
            player.setPosition(sf::Vector2f(
                player.getPosition().x,
                platformBounds.position.y - playerBounds.size.y
            ));
            player.setVelocity(sf::Vector2f(player.getVelocity().x, 0.f));
            player.setOnGround(true);
        } else {
            player.setPosition(sf::Vector2f(
                player.getPosition().x,
                platformBounds.position.y + platformBounds.size.y
            ));
            player.setVelocity(sf::Vector2f(player.getVelocity().x, 0.f));
        }
    }
}

void Level::handlePlayerMovingPlatformCollision(const MovingPlatform& platform) {
    sf::FloatRect playerBounds = player.getBounds();
    sf::FloatRect platformBounds = platform.getBounds();
    
    auto intersection = playerBounds.findIntersection(platformBounds);
    if (!intersection.has_value()) return;
    
    sf::FloatRect overlap = intersection.value();
    if (overlap.size.x < overlap.size.y) {
        if (playerBounds.position.x < platformBounds.position.x) {
            player.setPosition(sf::Vector2f(
                platformBounds.position.x - playerBounds.size.x,
                player.getPosition().y
            ));
        } else {
            player.setPosition(sf::Vector2f(
                platformBounds.position.x + platformBounds.size.x,
                player.getPosition().y
            ));
        }
        player.setVelocity(sf::Vector2f(0.f, player.getVelocity().y));
        playerOnMovingPlatform = nullptr;
    } else {
        if (playerBounds.position.y < platformBounds.position.y) {
            player.setPosition(sf::Vector2f(
                player.getPosition().x,
                platformBounds.position.y - playerBounds.size.y
            ));
            player.setVelocity(sf::Vector2f(player.getVelocity().x, 0.f));
            player.setOnGround(true);
            playerOnMovingPlatform = &platform;
        } else {
            player.setPosition(sf::Vector2f(
                player.getPosition().x,
                platformBounds.position.y + platformBounds.size.y
            ));
            player.setVelocity(sf::Vector2f(player.getVelocity().x, 0.f));
            playerOnMovingPlatform = nullptr;
        }
    }
}

void Level::handlePlayerEnemyCollisions() {
    for (auto& enemy : enemies) {
        if (enemy->isActive() && player.getBounds().findIntersection(enemy->getBounds()).has_value()) {
            enemy->onCollisionWithPlayer();
            player.takeDamage();
            
            sf::Vector2f knockback = player.getPosition() - enemy->getPosition();
            float length = std::sqrt(knockback.x * knockback.x + knockback.y * knockback.y);
            if (length > 0) {
                knockback.x /= length;
                knockback.y /= length;
            }
            player.setVelocity(knockback * 200.f + sf::Vector2f(0.f, -100.f));
        }
    }
}

void Level::handlePlayerAcidCollisions() {
    for (auto& acidPool : acidPools) {
        bool isColliding = player.getBounds().findIntersection(acidPool->getBounds()).has_value();
        
        if (isColliding) {
            acidPool->setPlayerInAcid(true);
            
            if (acidPool->getDamageCooldown() <= 0.f) {
                player.takeDamage();
                acidPool->resetDamageCooldown();
            }
        } else {
            acidPool->setPlayerInAcid(false);
        }
    }
}

void Level::handlePlayerHealthKitCollisions() {
    for (auto& healthKit : healthKits) {
        if (!healthKit->isCollected() && 
            player.getBounds().findIntersection(healthKit->getBounds()).has_value()) {
            
            healthKit->collect();
            player.getHealthSystem().heal();
        }
    }
}

void Level::respawnLevel() {
    
    platforms.clear();
    enemies.clear();
    packages.clear();
    deliveryPoints.clear();
    acidPools.clear();
    movingPlatforms.clear();
    healthKits.clear();
    packageStartPositions.clear();
    deliveryPointStartPositions.clear();
    
    switch(currentLevelNumber) {
        case 1: createLevel1(); break;
        case 2: createLevel2(); break;
        case 3: createLevel3(); break;
        case 4: createLevel4(); break;
        case 5: createLevel5(); break;
        default: createLevel1(); break;
    }
    
    levelCompleted = false;
    playerOnMovingPlatform = nullptr;
    respawnPlayer();
    
}

void Level::respawnPlayer() {
    sf::Vector2f startPosition;
    
    switch(currentLevelNumber) {
        case 1: startPosition = sf::Vector2f(150.f, 450.f); break;
        case 2: startPosition = sf::Vector2f(150.f, 650.f); break;
        case 3: startPosition = sf::Vector2f(150.f, 650.f); break;
        case 4: startPosition = sf::Vector2f(150.f, 650.f); break;
        case 5: startPosition = sf::Vector2f(150.f, 650.f); break;
        default: startPosition = sf::Vector2f(150.f, 450.f); break;
    }
    
    player.setPosition(startPosition);
    player.setVelocity(sf::Vector2f(0.f, 0.f));
    player.setOnGround(false);
    
    if (player.isCarryingPackage()) {
        player.deliverPackage();
    }
    
    player.getHealthSystem().reset();
}

Player& Level::getPlayer() {
    return player;
}

bool Level::isLevelComplete() const {
    return levelCompleted;
}

void Level::handleEInteraction() {
    Player& player = getPlayer();
    
    if (player.isCarryingPackage()) {
        for (auto& deliveryPoint : deliveryPoints) {
            if (deliveryPoint->isActive() && 
                player.getBounds().findIntersection(deliveryPoint->getBounds()).has_value()) {
                
                player.deliverPackage();
                levelCompleted = true;
                return;
            }
        }
        std::cout << "❌ Нет точки доставки рядом!" << std::endl;
    } else {
        for (auto& package : packages) {
            if (!package->isDelivered() && !package->isCarried() && 
                player.getBounds().findIntersection(package->getBounds()).has_value()) {
                
                player.pickUpPackage(package.get());
                return;
            }
        }
        std::cout << "❌ Нет посылки для подбора рядом!" << std::endl;
    }
}

void Level::createLevel1() {

    platforms.clear();
    enemies.clear();
    packages.clear();
    deliveryPoints.clear();
    acidPools.clear();
    movingPlatforms.clear();
    healthKits.clear();

    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(1000.f, 20.f), 
        sf::Vector2f(100.f, 700.f),
        sf::Color(100, 200, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(200.f, 20.f),
        sf::Vector2f(100.f, 500.f),
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(400.f, 400.f),
        sf::Color(250, 200, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(200.f, 20.f),
        sf::Vector2f(700.f, 300.f),
        sf::Color(200, 100, 200, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(20.f, 200.f),
        sf::Vector2f(900.f, 500.f),
        sf::Color(100, 100, 150, 255)
    ));
    
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(200.f, 600.f),
        sf::Vector2f(400.f, 600.f),
        100.f,
        true
    ));
    
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(600.f, 500.f),
        sf::Vector2f(600.f, 300.f),
        80.f,
        false
    ));
    
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(200.f, 15.f),
        sf::Vector2f(150.f, 685.f)
    ));
    
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(150.f, 15.f),
        sf::Vector2f(120.f, 485.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(200.f, 450.f),
        sf::Vector2f(300.f, 450.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(500.f, 350.f),
        sf::Vector2f(600.f, 350.f)
    ));
    
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(200.f, 450.f)
    ));
    
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(750.f, 200.f)
    ));
    
    packageStartPositions.clear();
    deliveryPointStartPositions.clear();
    
    packages.push_back(std::make_unique<Package>(
        sf::Vector2f(750.f, 250.f)
    ));
    packageStartPositions.push_back(sf::Vector2f(750.f, 250.f));
    
    deliveryPoints.push_back(std::make_unique<DeliveryPoint>(
        sf::Vector2f(950.f, 650.f)
    ));
    deliveryPointStartPositions.push_back(sf::Vector2f(950.f, 650.f));
    
    respawnPlayer();
}

void Level::createLevel2() {
    platforms.clear();
    enemies.clear();
    packages.clear();
    deliveryPoints.clear();
    acidPools.clear();
    movingPlatforms.clear();
    healthKits.clear();

    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(1000.f, 20.f), 
        sf::Vector2f(100.f, 750.f),
        sf::Color(100, 200, 100, 255)
    ));

    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(150.f, 600.f),
        sf::Color(200, 150, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(400.f, 600.f),
        sf::Color(200, 150, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(700.f, 600.f),
        sf::Color(200, 150, 100, 255)
    ));

    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(200.f, 350.f),
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(500.f, 300.f),
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(800.f, 350.f),
        sf::Color(150, 150, 250, 255)
    ));

    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(300.f, 700.f),
        sf::Vector2f(300.f, 500.f),
        70.f,
        false
    ));
    
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(600.f, 500.f),
        sf::Vector2f(600.f, 350.f),
        80.f,
        false
    ));

    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(850.f, 600.f),
        sf::Vector2f(850.f, 400.f),
        75.f,
        false
    ));

    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(120.f, 15.f),
        sf::Vector2f(250.f, 735.f)
    ));
    
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(120.f, 15.f),
        sf::Vector2f(550.f, 735.f)
    ));

    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(170.f, 560.f),
        sf::Vector2f(250.f, 560.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(750.f, 560.f),
        sf::Vector2f(820.f, 560.f)
    ));

    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(220.f, 310.f),
        sf::Vector2f(280.f, 310.f)
    ));

    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(520.f, 250.f),
        sf::Vector2f(580.f, 250.f)
    ));

    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(820.f, 310.f),
        sf::Vector2f(880.f, 310.f)
    ));

    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(150.f, 580.f)
    ));
    
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(800.f, 330.f)
    ));

    packages.push_back(std::make_unique<Package>(
        sf::Vector2f(500.f, 270.f)
    ));

    deliveryPoints.push_back(std::make_unique<DeliveryPoint>(
        sf::Vector2f(900.f, 700.f)
    ));

    player.setPosition(sf::Vector2f(150.f, 700.f));
    player.setVelocity(sf::Vector2f(0.f, 0.f));
    player.setOnGround(true);
    
    if (player.isCarryingPackage()) {
        player.deliverPackage();
    }
    
    player.getHealthSystem().reset();
}

void Level::createLevel3() {
    platforms.clear();
    enemies.clear();
    packages.clear();
    deliveryPoints.clear();
    acidPools.clear();
    movingPlatforms.clear();
    healthKits.clear();

    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(1000.f, 20.f), 
        sf::Vector2f(100.f, 700.f),
        sf::Color(100, 200, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(200.f, 300.f),
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(500.f, 300.f),
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(800.f, 300.f),
        sf::Color(150, 150, 250, 255)
    ));

    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(250.f, 450.f),
        sf::Color(200, 150, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(500.f, 450.f),
        sf::Color(200, 150, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(750.f, 450.f),
        sf::Color(200, 150, 100, 255)
    ));

    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(150.f, 600.f),
        sf::Color(180, 100, 200, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(500.f, 600.f),
        sf::Color(180, 100, 200, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(850.f, 600.f),
        sf::Color(180, 100, 200, 255)
    ));
    
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(350.f, 620.f),
        sf::Vector2f(350.f, 500.f),
        80.f,
        false
    ));
    
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(650.f, 500.f),
        sf::Vector2f(650.f, 350.f),
        80.f,
        false
    ));

    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(300.f, 400.f),
        sf::Vector2f(700.f, 400.f),
        90.f,
        true
    ));

    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(150.f, 15.f),
        sf::Vector2f(200.f, 685.f)
    ));
    
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(150.f, 15.f),
        sf::Vector2f(500.f, 685.f)
    ));
    
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(150.f, 15.f),
        sf::Vector2f(800.f, 685.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(220.f, 260.f),
        sf::Vector2f(320.f, 260.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(520.f, 260.f),
        sf::Vector2f(620.f, 260.f)
    ));

    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(270.f, 410.f),
        sf::Vector2f(370.f, 410.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(770.f, 410.f),
        sf::Vector2f(870.f, 410.f)
    ));

    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(170.f, 560.f),
        sf::Vector2f(220.f, 560.f)
    ));
    
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(520.f, 420.f)
    ));
    
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(870.f, 570.f)
    ));

    packages.push_back(std::make_unique<Package>(
        sf::Vector2f(220.f, 270.f)
    ));

    deliveryPoints.push_back(std::make_unique<DeliveryPoint>(
        sf::Vector2f(900.f, 650.f)
    ));

    player.setPosition(sf::Vector2f(150.f, 650.f));
    player.setVelocity(sf::Vector2f(0.f, 0.f));
    player.setOnGround(true);
    
    if (player.isCarryingPackage()) {
        player.deliverPackage();
    }
    
    player.getHealthSystem().reset();
}

void Level::createLevel4() {
    platforms.clear();
    enemies.clear();
    packages.clear();
    deliveryPoints.clear();
    acidPools.clear();
    movingPlatforms.clear();
    healthKits.clear();

    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(1000.f, 20.f), 
        sf::Vector2f(100.f, 700.f),
        sf::Color(100, 200, 100, 255)
    ));

    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(80.f, 20.f),
        sf::Vector2f(200.f, 300.f),
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(80.f, 20.f),
        sf::Vector2f(500.f, 300.f),
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(80.f, 20.f),
        sf::Vector2f(800.f, 300.f),
        sf::Color(150, 150, 250, 255)
    ));

    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(200.f, 20.f),
        sf::Vector2f(150.f, 450.f),
        sf::Color(200, 150, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(200.f, 20.f),
        sf::Vector2f(500.f, 450.f),
        sf::Color(200, 150, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(200.f, 20.f),
        sf::Vector2f(850.f, 450.f),
        sf::Color(200, 150, 100, 255)
    ));

    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(100.f, 600.f),
        sf::Color(180, 100, 200, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(180.f, 20.f),
        sf::Vector2f(400.f, 600.f),
        sf::Color(180, 100, 200, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(800.f, 600.f),
        sf::Color(180, 100, 200, 255)
    ));

    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(60.f, 20.f),
        sf::Vector2f(350.f, 350.f),
        sf::Color(100, 200, 150, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(60.f, 20.f),
        sf::Vector2f(650.f, 350.f),
        sf::Color(100, 200, 150, 255)
    ));

    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(80.f, 20.f),
        sf::Vector2f(300.f, 620.f),
        sf::Vector2f(300.f, 320.f),
        120.f,
        false
    ));
    
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(80.f, 20.f),
        sf::Vector2f(700.f, 620.f),
        sf::Vector2f(700.f, 320.f),
        60.f,
        false
    ));

    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(200.f, 500.f),
        sf::Vector2f(800.f, 500.f),
        100.f,
        true
    ));

    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(100.f, 15.f),
        sf::Vector2f(200.f, 685.f)
    ));
    
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(200.f, 15.f),
        sf::Vector2f(450.f, 685.f)
    ));
    
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(120.f, 15.f),
        sf::Vector2f(780.f, 685.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(520.f, 260.f),
        sf::Vector2f(560.f, 260.f)
    ));

    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(170.f, 410.f),
        sf::Vector2f(320.f, 410.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(870.f, 410.f),
        sf::Vector2f(1020.f, 410.f)
    ));

    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(420.f, 560.f),
        sf::Vector2f(540.f, 560.f)
    ));
    
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(350.f, 320.f)
    ));
    
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(800.f, 270.f)
    ));

    packages.push_back(std::make_unique<Package>(
        sf::Vector2f(650.f, 320.f)
    ));

    deliveryPoints.push_back(std::make_unique<DeliveryPoint>(
        sf::Vector2f(900.f, 650.f)
    ));

    player.setPosition(sf::Vector2f(150.f, 650.f));
    player.setVelocity(sf::Vector2f(0.f, 0.f));
    player.setOnGround(true);
    
    if (player.isCarryingPackage()) {
        player.deliverPackage();
    }
    
    player.getHealthSystem().reset();
}

void Level::createLevel5() {
    platforms.clear();
    enemies.clear();
    packages.clear();
    deliveryPoints.clear();
    acidPools.clear();
    movingPlatforms.clear();
    healthKits.clear();

    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(1000.f, 20.f), 
        sf::Vector2f(100.f, 700.f),
        sf::Color(100, 200, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(60.f, 20.f),
        sf::Vector2f(200.f, 250.f),
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(60.f, 20.f),
        sf::Vector2f(400.f, 250.f),
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(60.f, 20.f),
        sf::Vector2f(600.f, 250.f),
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(60.f, 20.f),
        sf::Vector2f(800.f, 250.f),
        sf::Color(150, 150, 250, 255)
    ));

    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(150.f, 400.f),
        sf::Color(200, 150, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(550.f, 400.f),
        sf::Color(200, 150, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(950.f, 400.f),
        sf::Color(200, 150, 100, 255)
    ));

    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(100.f, 550.f),
        sf::Color(180, 100, 200, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(300.f, 550.f),
        sf::Color(180, 100, 200, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(500.f, 550.f),
        sf::Color(180, 100, 200, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(700.f, 550.f),
        sf::Color(180, 100, 200, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(900.f, 550.f),
        sf::Color(180, 100, 200, 255)
    ));
    
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(250.f, 620.f),
        sf::Vector2f(450.f, 320.f),
        80.f,
        false
    ));
    
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(80.f, 20.f),
        sf::Vector2f(650.f, 620.f),
        sf::Vector2f(650.f, 200.f),
        150.f,
        false
    ));

    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(80.f, 20.f),
        sf::Vector2f(350.f, 480.f),
        sf::Vector2f(550.f, 480.f),
        100.f,
        true
    ));
    
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(80.f, 20.f),
        sf::Vector2f(750.f, 480.f),
        sf::Vector2f(950.f, 480.f),
        100.f,
        true
    ));

    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(180.f, 15.f),
        sf::Vector2f(150.f, 685.f)
    ));
    
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(120.f, 15.f),
        sf::Vector2f(400.f, 685.f)
    ));
    
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(180.f, 15.f),
        sf::Vector2f(600.f, 685.f)
    ));
    
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(120.f, 15.f),
        sf::Vector2f(850.f, 685.f)
    ));

    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(220.f, 210.f),
        sf::Vector2f(260.f, 210.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(420.f, 210.f),
        sf::Vector2f(460.f, 210.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(620.f, 210.f),
        sf::Vector2f(660.f, 210.f)
    ));

    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(170.f, 510.f),
        sf::Vector2f(250.f, 510.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(520.f, 510.f),
        sf::Vector2f(600.f, 510.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(870.f, 510.f),
        sf::Vector2f(950.f, 510.f)
    ));

    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(380.f, 460.f),
        sf::Vector2f(420.f, 460.f)
    ));

    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(800.f, 220.f)
    ));
    
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(300.f, 510.f)
    ));
    
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(950.f, 370.f)
    ));

    packages.push_back(std::make_unique<Package>(
        sf::Vector2f(400.f, 220.f)
    ));

    deliveryPoints.push_back(std::make_unique<DeliveryPoint>(
        sf::Vector2f(900.f, 650.f)
    ));

    player.setPosition(sf::Vector2f(150.f, 650.f));
    player.setVelocity(sf::Vector2f(0.f, 0.f));
    player.setOnGround(true);
    
    if (player.isCarryingPackage()) {
        player.deliverPackage();
    }
    
    player.getHealthSystem().reset();
}