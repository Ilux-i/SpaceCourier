#include "Level.h"
#include <iostream>
#include <algorithm>
#include <cmath>

Level::Level() 
    : currentLevelNumber(1), levelCompleted(false), playerOnMovingPlatform(nullptr) {
    
    levelBounds = sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(1200.f, 800.f));
    createLevel1();
}

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
    
    std::cout << "🎮 Создан уровень " << levelNumber << std::endl;
}

void Level::update(float deltaTime) {
    if (player.getHealthSystem().isAlive()) {
        // ОБНОВЛЯЕМ ДВИЖУЩИЕСЯ ПЛАТФОРМЫ ПЕРВЫМИ
        for (auto& movingPlatform : movingPlatforms) {
            movingPlatform->update(deltaTime);
        }
        
        // ЕСЛИ ИГРОК НА ПЛАТФОРМЕ - ПЕРЕНОСИМ ЕГО
        if (playerOnMovingPlatform) {
            sf::Vector2f platformMovement = playerOnMovingPlatform->getMovement();
            player.setPosition(player.getPosition() + platformMovement * deltaTime);
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
        std::cout << "💀 Игрок умер! Запускаем респавн уровня..." << std::endl;
        respawnLevel();
    }
}

void Level::draw(sf::RenderWindow& window) const {
    // СНАЧАЛА рисуем платформы (самый нижний слой)
    for (const auto& platform : platforms) {
        platform->draw(window);
    }
    
    // ЗАТЕМ рисуем движущиеся платформы
    for (const auto& movingPlatform : movingPlatforms) {
        movingPlatform->draw(window);
    }
    
    // ЗАТЕМ рисуем кислотные озёра
    for (const auto& acidPool : acidPools) {
        acidPool->draw(window);
    }
    
    // ЗАТЕМ рисуем точки доставки
    for (const auto& deliveryPoint : deliveryPoints) {
        deliveryPoint->draw(window);
    }
    
    // ЗАТЕМ рисуем аптечки
    for (const auto& healthKit : healthKits) {
        healthKit->draw(window);
    }
    
    // ЗАТЕМ рисуем посылки
    for (const auto& package : packages) {
        package->draw(window);
    }
    
    // ЗАТЕМ рисуем врагов
    for (const auto& enemy : enemies) {
        enemy->draw(window);
    }
    
    // ПОСЛЕДНИМ рисуем игрока (поверх всего)
    player.draw(window);
}

void Level::handleCollisions() {
    player.setOnGround(false);
    bool onMovingPlatform = false;
    
    // Коллизии с обычными платформами
    for (const auto& platform : platforms) {
        if (player.getBounds().findIntersection(platform->getBounds()).has_value()) {
            handlePlayerPlatformCollision(*platform);
        }
    }
    
    // Коллизии с движущимися платформами
    for (const auto& movingPlatform : movingPlatforms) {
        if (player.getBounds().findIntersection(movingPlatform->getBounds()).has_value()) {
            handlePlayerMovingPlatformCollision(*movingPlatform);
            onMovingPlatform = true;
        }
    }
    
    // ЕСЛИ ИГРОК УШЁЛ С ПЛАТФОРМЫ - СБРАСЫВАЕМ
    if (!onMovingPlatform && playerOnMovingPlatform) {
        // ДОПОЛНИТЕЛЬНАЯ ПРОВЕРКА: игрок действительно ушёл с платформы
        if (!player.getBounds().findIntersection(playerOnMovingPlatform->getBounds()).has_value()) {
            playerOnMovingPlatform = nullptr;
            std::cout << "🚶 Игрок сошёл с движущейся платформы" << std::endl;
        }
    }
    
    handlePlayerEnemyCollisions();
    
    // Границы уровня
    sf::Vector2f playerPos = player.getPosition();
    sf::FloatRect playerBounds = player.getBounds();
    
    sf::Vector2f levelPos = levelBounds.position;
    sf::Vector2f levelSize = levelBounds.size;
    
    // Левая граница
    if (playerPos.x < levelPos.x) {
        player.setPosition(sf::Vector2f(levelPos.x, playerPos.y));
    }
    // Правая граница
    if (playerPos.x + playerBounds.size.x > levelPos.x + levelSize.x) {
        player.setPosition(sf::Vector2f(levelPos.x + levelSize.x - playerBounds.size.x, playerPos.y));
    }
    // Верхняя граница
    if (playerPos.y < levelPos.y) {
        player.setPosition(sf::Vector2f(playerPos.x, levelPos.y));
        player.setVelocity(sf::Vector2f(player.getVelocity().x, 0.f));
    }
    // Нижняя граница (смерть от падения)
    if (playerPos.y > levelPos.y + levelSize.y) {
        std::cout << "💀 Игрок упал в пропасть!" << std::endl;
        player.takeDamage();
        respawnLevel();
    }
}

// МЕТОД ДЛЯ КОЛЛИЗИЙ С ПЛАТФОРМАМИ
void Level::handlePlayerPlatformCollision(const Platform& platform) {
    sf::FloatRect playerBounds = player.getBounds();
    sf::FloatRect platformBounds = platform.getBounds();
    
    auto intersection = playerBounds.findIntersection(platformBounds);
    if (!intersection.has_value()) return;
    
    sf::FloatRect overlap = intersection.value();
    
    // Определяем направление коллизии
    if (overlap.size.x < overlap.size.y) {
        // Горизонтальная коллизия
        if (playerBounds.position.x < platformBounds.position.x) {
            // Игрок слева от платформы
            player.setPosition(sf::Vector2f(
                platformBounds.position.x - playerBounds.size.x,
                player.getPosition().y
            ));
        } else {
            // Игрок справа от платформы
            player.setPosition(sf::Vector2f(
                platformBounds.position.x + platformBounds.size.x,
                player.getPosition().y
            ));
        }
        player.setVelocity(sf::Vector2f(0.f, player.getVelocity().y));
    } else {
        // Вертикальная коллизия
        if (playerBounds.position.y < platformBounds.position.y) {
            // Игрок над платформой (приземление)
            player.setPosition(sf::Vector2f(
                player.getPosition().x,
                platformBounds.position.y - playerBounds.size.y
            ));
            player.setVelocity(sf::Vector2f(player.getVelocity().x, 0.f));
            player.setOnGround(true);
        } else {
            // Игрок под платформой (удар головой)
            player.setPosition(sf::Vector2f(
                player.getPosition().x,
                platformBounds.position.y + platformBounds.size.y
            ));
            player.setVelocity(sf::Vector2f(player.getVelocity().x, 0.f));
        }
    }
}

// МЕТОД ДЛЯ КОЛЛИЗИЙ С ДВИЖУЩИМИСЯ ПЛАТФОРМАМИ
void Level::handlePlayerMovingPlatformCollision(const MovingPlatform& platform) {
    sf::FloatRect playerBounds = player.getBounds();
    sf::FloatRect platformBounds = platform.getBounds();
    
    auto intersection = playerBounds.findIntersection(platformBounds);
    if (!intersection.has_value()) return;
    
    sf::FloatRect overlap = intersection.value();
    
    // Определяем направление коллизии
    if (overlap.size.x < overlap.size.y) {
        // Горизонтальная коллизия
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
        playerOnMovingPlatform = nullptr; // СБРАСЫВАЕМ ЕСЛИ СБОКУ
    } else {
        // Вертикальная коллизии
        if (playerBounds.position.y < platformBounds.position.y) {
            // Игрок над платформой (приземление)
            player.setPosition(sf::Vector2f(
                player.getPosition().x,
                platformBounds.position.y - playerBounds.size.y
            ));
            player.setVelocity(sf::Vector2f(player.getVelocity().x, 0.f));
            player.setOnGround(true);
            
            // ЗАПОМИНАЕМ ПЛАТФОРМУ ТОЛЬКО ЕСЛИ ИГРОК ДЕЙСТВИТЕЛЬНО СТОИТ СВЕРХУ
            if (player.getVelocity().y >= 0) {
                playerOnMovingPlatform = &platform;
                std::cout << "🎯 Игрок встал на движущуюся платформу!" << std::endl;
            }
            
        } else {
            // Игрок под платформой (удар головой)
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
            
            // Отбрасывание
            sf::Vector2f knockback = player.getPosition() - enemy->getPosition();
            float length = std::sqrt(knockback.x * knockback.x + knockback.y * knockback.y);
            if (length > 0) {
                knockback.x /= length;
                knockback.y /= length;
            }
            player.setVelocity(knockback * 600.f + sf::Vector2f(0.f, -300.f));
            
            std::cout << "💥 Столкновение с дроном! Здоровье: " 
                      << player.getHealthSystem().getHealth() << "/3" << std::endl;
            
            if (!player.getHealthSystem().isAlive()) {
                std::cout << "💀 Игрок умер от дрона!" << std::endl;
            }
        }
    }
}

void Level::handlePlayerAcidCollisions() {
    for (auto& acidPool : acidPools) {
        bool isColliding = player.getBounds().findIntersection(acidPool->getBounds()).has_value();
        
        if (isColliding) {
            acidPool->setPlayerInAcid(true);
            
            // Проверяем кулдаун урона (раз в секунду)
            if (acidPool->getDamageCooldown() <= 0.f) {
                player.takeDamage();
                acidPool->resetDamageCooldown();
                std::cout << "☠️ Игрок в кислоте! Здоровье: " 
                          << player.getHealthSystem().getHealth() << "/3" << std::endl;
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
            
            // Автоматический подбор и лечение
            healthKit->collect();
            player.getHealthSystem().heal();
            
            std::cout << "❤️ Игрок подобрал аптечку! Здоровье: " 
                      << player.getHealthSystem().getHealth() << "/3" << std::endl;
        }
    }
}

void Level::respawnLevel() {
    std::cout << "🔄 Респавн уровня..." << std::endl;
    
    // СБРАСЫВАЕМ ЗДОРОВЬЕ ИГРОКА
    player.getHealthSystem().reset();
    
    // ВОССТАНАВЛИВАЕМ ВСЕ АПТЕЧКИ
    for (auto& healthKit : healthKits) {
        healthKit->respawn();
    }
    
    // СБРАСЫВАЕМ СОСТОЯНИЕ ПЛАТФОРМЫ
    playerOnMovingPlatform = nullptr;
    
    // РЕСПАВНИМ ИГРОКА
    respawnPlayer();
    
    // ВОССТАНАВЛИВАЕМ ПОСЫЛКИ
    for (size_t i = 0; i < packages.size() && i < packageStartPositions.size(); ++i) {
        packages[i]->setCarried(false);
        packages[i]->setDelivered(false);
        packages[i]->setPosition(packageStartPositions[i]);
        packages[i]->update(0.f);
    }
    
    std::cout << "✅ Уровень перезапущен!" << std::endl;
}

void Level::respawnPlayer() {
    player.setPosition(sf::Vector2f(150.f, 450.f));
    player.setVelocity(sf::Vector2f(0.f, 0.f));
    player.setOnGround(false);
    
    // Сбрасываем посылку если игрок её нёс
    if (player.isCarryingPackage()) {
        std::cout << "🔄 Сбрасываем carried package..." << std::endl;
        player.deliverPackage();
    }
    
    std::cout << "👤 Игрок респавнут на стартовой позиции" << std::endl;
}

Player& Level::getPlayer() {
    return player;
}

bool Level::isLevelComplete() const {
    return levelCompleted;
}

void Level::handleEInteraction() {
    Player& player = getPlayer();
    
    std::cout << "🔄 Обработка E-взаимодействия..." << std::endl;
    
    // СНАЧАЛА проверяем доставку (если несем посылку)
    if (player.isCarryingPackage()) {
        std::cout << "   Проверка доставки..." << std::endl;
        for (auto& deliveryPoint : deliveryPoints) {
            if (deliveryPoint->isActive() && 
                player.getBounds().findIntersection(deliveryPoint->getBounds()).has_value()) {
                
                player.deliverPackage();
                levelCompleted = true; // УРОВЕНЬ ЗАВЕРШЁН!
                std::cout << "🎉 Посылка доставлена! Уровень " << currentLevelNumber << " пройден!" << std::endl;
                return;
            }
        }
        std::cout << "❌ Нет точки доставки рядом!" << std::endl;
    } 
    // ЗАТЕМ проверяем подбор (если не несем посылку)
    else {
        std::cout << "   Проверка подбора посылки..." << std::endl;
        for (auto& package : packages) {
            if (!package->isDelivered() && !package->isCarried() && 
                player.getBounds().findIntersection(package->getBounds()).has_value()) {
                
                player.pickUpPackage(package.get());
                std::cout << "📦 Посылка поднята!" << std::endl;
                return;
            }
        }
        std::cout << "❌ Нет посылки для подбора рядом!" << std::endl;
    }
}

void Level::createLevel1() {
    // СУЩЕСТВУЮЩАЯ ЛОГИКА из createFirstLocation()
    platforms.clear();
    enemies.clear();
    packages.clear();
    deliveryPoints.clear();
    acidPools.clear();
    movingPlatforms.clear();
    healthKits.clear();

    // Платформы
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
    
    // Движущиеся платформы
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
    
    // Кислотные озёра
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(200.f, 15.f),
        sf::Vector2f(150.f, 685.f)
    ));
    
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(150.f, 15.f),
        sf::Vector2f(120.f, 485.f)
    ));
    
    // Враги
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(200.f, 450.f),
        sf::Vector2f(300.f, 450.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(500.f, 350.f),
        sf::Vector2f(600.f, 350.f)
    ));
    
    // Аптечки
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(200.f, 450.f)
    ));
    
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(750.f, 200.f)
    ));
    
    // Посылка и точка доставки
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

    // УРОВЕНЬ 2: Больше движущихся платформ
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(800.f, 20.f), 
        sf::Vector2f(200.f, 700.f),
        sf::Color(100, 200, 100, 255)
    ));
    
    // Больше движущихся платформ
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(300.f, 600.f),
        sf::Vector2f(500.f, 600.f),
        120.f,
        true
    ));
    
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(600.f, 500.f),
        sf::Vector2f(600.f, 350.f),
        90.f,
        false
    ));
    
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(400.f, 400.f),
        sf::Vector2f(700.f, 400.f),
        80.f,
        true
    ));
    
    // Кислотные озёра
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(180.f, 15.f),
        sf::Vector2f(250.f, 685.f)
    ));
    
    // Враги
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(300.f, 450.f),
        sf::Vector2f(450.f, 450.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(550.f, 350.f),
        sf::Vector2f(650.f, 350.f)
    ));
    
    // Аптечки
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(350.f, 450.f)
    ));
    
    // Посылка и точка доставки
    packages.push_back(std::make_unique<Package>(
        sf::Vector2f(800.f, 350.f)
    ));
    
    deliveryPoints.push_back(std::make_unique<DeliveryPoint>(
        sf::Vector2f(300.f, 650.f)
    ));
    
    respawnPlayer();
}

void Level::createLevel3() {
    platforms.clear();
    enemies.clear();
    packages.clear();
    deliveryPoints.clear();
    acidPools.clear();
    movingPlatforms.clear();
    healthKits.clear();

    // УРОВЕНЬ 3: Сложная навигация
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(600.f, 20.f), 
        sf::Vector2f(300.f, 700.f),
        sf::Color(100, 200, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(200.f, 550.f),
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(800.f, 500.f),
        sf::Color(250, 200, 100, 255)
    ));
    
    // Сложные движущиеся платформы
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(80.f, 20.f),
        sf::Vector2f(400.f, 600.f),
        sf::Vector2f(600.f, 600.f),
        150.f,
        true
    ));
    
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(80.f, 20.f),
        sf::Vector2f(500.f, 500.f),
        sf::Vector2f(500.f, 300.f),
        110.f,
        false
    ));
    
    // Много кислоты
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(100.f, 15.f),
        sf::Vector2f(350.f, 685.f)
    ));
    
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(100.f, 15.f),
        sf::Vector2f(500.f, 685.f)
    ));
    
    // Больше врагов
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(350.f, 450.f),
        sf::Vector2f(500.f, 450.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(600.f, 350.f),
        sf::Vector2f(750.f, 350.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(400.f, 250.f),
        sf::Vector2f(550.f, 250.f)
    ));
    
    // Аптечки
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(250.f, 520.f)
    ));
    
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(850.f, 450.f)
    ));
    
    // Посылка и точка доставки
    packages.push_back(std::make_unique<Package>(
        sf::Vector2f(850.f, 250.f)
    ));
    
    deliveryPoints.push_back(std::make_unique<DeliveryPoint>(
        sf::Vector2f(350.f, 650.f)
    ));
    
    respawnPlayer();
}

void Level::createLevel4() {
    // УРОВЕНЬ 4: Вертикальный уровень
    platforms.clear();
    enemies.clear();
    packages.clear();
    deliveryPoints.clear();
    acidPools.clear();
    movingPlatforms.clear();
    healthKits.clear();

    // Вертикальные платформы
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(200.f, 20.f), 
        sf::Vector2f(100.f, 700.f),
        sf::Color(100, 200, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(200.f, 20.f),
        sf::Vector2f(400.f, 600.f),
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(200.f, 20.f),
        sf::Vector2f(100.f, 500.f),
        sf::Color(250, 200, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(200.f, 20.f),
        sf::Vector2f(400.f, 400.f),
        sf::Color(200, 100, 200, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(200.f, 20.f),
        sf::Vector2f(100.f, 300.f),
        sf::Color(100, 100, 150, 255)
    ));
    
    // Вертикальные движущиеся платформы
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(600.f, 700.f),
        sf::Vector2f(600.f, 400.f),
        100.f,
        false
    ));
    
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(800.f, 500.f),
        sf::Vector2f(800.f, 200.f),
        120.f,
        false
    ));
    
    // Кислотные озёра
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(150.f, 15.f),
        sf::Vector2f(300.f, 685.f)
    ));
    
    // Враги
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(150.f, 450.f),
        sf::Vector2f(300.f, 450.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(450.f, 350.f),
        sf::Vector2f(600.f, 350.f)
    ));
    
    // Аптечки
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(450.f, 580.f)
    ));
    
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(150.f, 250.f)
    ));
    
    // Посылка и точка доставки
    packages.push_back(std::make_unique<Package>(
        sf::Vector2f(150.f, 250.f)
    ));
    
    deliveryPoints.push_back(std::make_unique<DeliveryPoint>(
        sf::Vector2f(450.f, 650.f)
    ));
    
    respawnPlayer();
}

void Level::createLevel5() {
    // УРОВЕНЬ 5: Финальный босс
    platforms.clear();
    enemies.clear();
    packages.clear();
    deliveryPoints.clear();
    acidPools.clear();
    movingPlatforms.clear();
    healthKits.clear();

    // Большая сложная арена
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(1000.f, 20.f), 
        sf::Vector2f(100.f, 700.f),
        sf::Color(100, 200, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(200.f, 550.f),
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(500.f, 550.f),
        sf::Color(250, 200, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(800.f, 550.f),
        sf::Color(200, 100, 200, 255)
    ));
    
    // Сложные движущиеся платформы - ИСПРАВЛЯЕМ ОШИБКУ:
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(350.f, 450.f),
        sf::Vector2f(600.f, 450.f),
        130.f,
        true
    ));
    
    movingPlatforms.push_back(std::make_unique<MovingPlatform>( // ИСПРАВЛЕНО: make_unique вместо makeUnique
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(500.f, 350.f),
        sf::Vector2f(500.f, 200.f),
        140.f,
        false
    ));
    
    // Много кислоты
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(120.f, 15.f),
        sf::Vector2f(250.f, 685.f)
    ));
    
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(120.f, 15.f),
        sf::Vector2f(500.f, 685.f)
    ));
    
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(120.f, 15.f),
        sf::Vector2f(750.f, 685.f)
    ));
    
    // Много врагов
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(200.f, 450.f),
        sf::Vector2f(350.f, 450.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(450.f, 450.f),
        sf::Vector2f(600.f, 450.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(700.f, 450.f),
        sf::Vector2f(850.f, 450.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(300.f, 250.f),
        sf::Vector2f(500.f, 250.f)
    ));
    
    // Аптечки
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(280.f, 520.f)
    ));
    
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(530.f, 520.f)
    ));
    
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(780.f, 520.f)
    ));
    
    // Посылка и точка доставки
    packages.push_back(std::make_unique<Package>(
        sf::Vector2f(550.f, 200.f)
    ));
    
    deliveryPoints.push_back(std::make_unique<DeliveryPoint>(
        sf::Vector2f(550.f, 650.f)
    ));
    
    respawnPlayer();
}