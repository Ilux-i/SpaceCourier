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
    
    std::cout << "   ✅ Уровень " << levelNumber << " создан успешно" << std::endl;
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
            
            // Переносим игрока с учётом deltaTime и скорости платформы
            sf::Vector2f newPosition = player.getPosition() + platformMovement * deltaTime;
            player.setPosition(newPosition);
            
            // Обновляем скорость игрока для плавности
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
    
    if (overlap.size.x < overlap.size.y) {
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

// МЕТОД ДЛЯ КОЛЛИЗИЙ С ДВИЖУЩИМИСЯ ПЛАТФОРМАМИ
void Level::handlePlayerMovingPlatformCollision(const MovingPlatform& platform) {
    sf::FloatRect playerBounds = player.getBounds();
    sf::FloatRect platformBounds = platform.getBounds();
    
    auto intersection = playerBounds.findIntersection(platformBounds);
    if (!intersection.has_value()) return;
    
    sf::FloatRect overlap = intersection.value();
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
        playerOnMovingPlatform = nullptr;
    } else {
        // Вертикальная коллизия
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
    
    platforms.clear();
    enemies.clear();
    packages.clear();
    deliveryPoints.clear();
    acidPools.clear();
    movingPlatforms.clear();
    healthKits.clear();
    packageStartPositions.clear();
    deliveryPointStartPositions.clear();
    
    // ВОССТАНАВЛИВАЕМ ВСЕ АПТЕЧКИ
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
        case 2: startPosition = sf::Vector2f(150.f, 650.f); break; // 🔥 БЕЗОПАСНАЯ СТАРТОВАЯ ПОЗИЦИЯ
        case 3: startPosition = sf::Vector2f(150.f, 650.f); break; // 🔥 БЕЗОПАСНАЯ СТАРТОВАЯ ПОЗИЦИЯ
        case 4: startPosition = sf::Vector2f(150.f, 650.f); break; // 🔥 БЕЗОПАСНАЯ СТАРТОВАЯ ПОЗИЦИЯ
        case 5: startPosition = sf::Vector2f(150.f, 650.f); break; // 🔥 БЕЗОПАСНАЯ СТАРТОВАЯ ПОЗИЦИЯ
        default: startPosition = sf::Vector2f(150.f, 450.f); break;
    }
    
    player.setPosition(startPosition);
    player.setVelocity(sf::Vector2f(0.f, 0.f));
    player.setOnGround(false);
    
    if (player.isCarryingPackage()) {
        player.deliverPackage();
    }
    
    player.getHealthSystem().reset();
    
    std::cout << "👤 Игрок респавнут на позиции: " << startPosition.x << ", " << startPosition.y << std::endl;
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

    // 🔥 БОЛЬШАЯ БАЗОВАЯ ПЛАТФОРМА ДЛЯ СТАРТА И ПЕРЕДВИЖЕНИЯ
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(1000.f, 20.f), 
        sf::Vector2f(100.f, 700.f),
        sf::Color(100, 200, 100, 255)
    ));

    // 🔥 ВЕРХНИЕ ПЛАТФОРМЫ (3 штуки как в уровне 1)
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(200.f, 500.f),
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(500.f, 500.f),
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(800.f, 500.f),
        sf::Color(150, 150, 250, 255)
    ));

    // 🔥 СРЕДНИЕ ПЛАТФОРМЫ (3 штуки как в уровне 1)
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(250.f, 600.f),
        sf::Color(200, 150, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(500.f, 600.f),
        sf::Color(200, 150, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(750.f, 600.f),
        sf::Color(200, 150, 100, 255)
    ));

    // 🔥 ДВИЖУЩИЕСЯ ПЛАТФОРМЫ (вертикальные для доступа к верху)
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(400.f, 620.f),  // Низ: над базовой платформой
        sf::Vector2f(400.f, 450.f),  // Верх: под верхними платформами
        80.f,
        false
    ));
    
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(600.f, 620.f),  // Низ: над базовой платформой
        sf::Vector2f(600.f, 450.f),  // Верх: под верхними платформами
        80.f,
        false
    ));

    // 🔥 КИСЛОТНЫЕ ЛУЖИ НА БАЗОВОЙ ПЛАТФОРМЕ (3 штуки)
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

    // 🔥 ВРАГИ НАД ПЛАТФОРМАМИ (не в них!)
    
    // Враги на верхних платформах
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(220.f, 460.f),  // Y: 500-40=460 (над платформой)
        sf::Vector2f(320.f, 460.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(520.f, 460.f),  // Y: 500-40=460 (над платформой)
        sf::Vector2f(620.f, 460.f)
    ));

    // Враги на средних платформах
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(270.f, 560.f),  // Y: 600-40=560 (над платформой)
        sf::Vector2f(370.f, 560.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(770.f, 560.f),  // Y: 600-40=560 (над платформой)
        sf::Vector2f(870.f, 560.f)
    ));

    // 🔥 АПТЕЧКИ НАД ПЛАТФОРМАМИ
    
    // Аптечка на средней платформе
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(520.f, 570.f)  // Y: 600-30=570 (над платформой)
    ));
    
    // Аптечка на верхней платформе
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(820.f, 470.f)  // Y: 500-30=470 (над платформой)
    ));

    // 🔥 ПОСЫЛКА НАД ПЛАТФОРМОЙ (верхняя правая)
    packages.push_back(std::make_unique<Package>(
        sf::Vector2f(820.f, 470.f)  // Y: 500-30=470 (над платформой)
    ));

    // 🔥 ТОЧКА ДОСТАВКИ НА БЕЗОПАСНОЙ БАЗОВОЙ ПЛАТФОРМЕ (правый край)
    deliveryPoints.push_back(std::make_unique<DeliveryPoint>(
        sf::Vector2f(900.f, 650.f)  // Y: 700-50=650 (над платформой)
    ));

    // 🔥 СТАРТОВАЯ ПОЗИЦИЯ ИГРОКА НА БЕЗОПАСНОЙ БАЗОВОЙ ПЛАТФОРМЕ (левый край)
    player.setPosition(sf::Vector2f(150.f, 650.f));  // Y: 700-50=650 (над платформой)
    player.setVelocity(sf::Vector2f(0.f, 0.f));
    player.setOnGround(true);
    
    if (player.isCarryingPackage()) {
        player.deliverPackage();
    }
    
    player.getHealthSystem().reset();
    
    std::cout << "🎮 Уровень 2 загружен: Двойные платформы" << std::endl;
    std::cout << "👤 Игрок стартует на позиции: 150, 650" << std::endl;
    std::cout << "📦 Посылка на позиции: 820, 470" << std::endl;
    std::cout << "🚀 Точка доставки на позиции: 900, 650" << std::endl;
    std::cout << "✅ Все объекты над платформами, доставка в безопасном месте" << std::endl;
}

void Level::createLevel3() {
    platforms.clear();
    enemies.clear();
    packages.clear();
    deliveryPoints.clear();
    acidPools.clear();
    movingPlatforms.clear();
    healthKits.clear();

    // 🔥 БОЛЬШАЯ БАЗОВАЯ ПЛАТФОРМА ДЛЯ СТАРТА И ПЕРЕДВИЖЕНИЯ
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(1000.f, 20.f), 
        sf::Vector2f(100.f, 700.f),
        sf::Color(100, 200, 100, 255)
    ));

    // 🔥 РАВНОМЕРНОЕ РАСПРЕДЕЛЕНИЕ ПЛАТФОРМ ПО ВЫСОТЕ (300-600)
    
    // ВЫСОКИЙ УРОВЕНЬ (3 платформы)
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(200.f, 300.f),  // 🔥 ВЫСОКАЯ: 300
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(500.f, 300.f),  // 🔥 ВЫСОКАЯ: 300
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(800.f, 300.f),  // 🔥 ВЫСОКАЯ: 300
        sf::Color(150, 150, 250, 255)
    ));

    // СРЕДНИЙ УРОВЕНЬ (3 платформы)
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(250.f, 450.f),  // 🔥 СРЕДНЯЯ: 450
        sf::Color(200, 150, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(500.f, 450.f),  // 🔥 СРЕДНЯЯ: 450
        sf::Color(200, 150, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(750.f, 450.f),  // 🔥 СРЕДНЯЯ: 450
        sf::Color(200, 150, 100, 255)
    ));

    // НИЗКИЙ УРОВЕНЬ (3 платформы)
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(150.f, 600.f),  // 🔥 НИЗКАЯ: 600
        sf::Color(180, 100, 200, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(500.f, 600.f),  // 🔥 НИЗКАЯ: 600
        sf::Color(180, 100, 200, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(850.f, 600.f),  // 🔥 НИЗКАЯ: 600
        sf::Color(180, 100, 200, 255)
    ));

    // 🔥 ДВИЖУЩИЕСЯ ПЛАТФОРМЫ ДЛЯ СВЯЗИ УРОВНЕЙ
    
    // Вертикальная движущаяся платформа между низким и средним уровнями
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(350.f, 620.f),  // Низ: над базовой платформой
        sf::Vector2f(350.f, 500.f),  // Верх: под средними платформами
        80.f,
        false
    ));
    
    // Вертикальная движущаяся платформа между средним и высоким уровнями
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(650.f, 500.f),  // Низ: под средними платформами
        sf::Vector2f(650.f, 350.f),  // Верх: под высокими платформами
        80.f,
        false
    ));

    // Горизонтальная движущаяся платформа на среднем уровне
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(300.f, 400.f),  // Лево
        sf::Vector2f(700.f, 400.f),  // Право (между уровнями)
        90.f,
        true
    ));

    // 🔥 КИСЛОТНЫЕ ЛУЖИ НА БАЗОВОЙ ПЛАТФОРМЕ (3 штуки)
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

    // 🔥 ВРАГИ НАД ПЛАТФОРМАМИ (равномерное распределение)
    
    // Враги на высоких платформах
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(220.f, 260.f),  // Y: 300-40=260 (над платформой)
        sf::Vector2f(320.f, 260.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(520.f, 260.f),  // Y: 300-40=260 (над платформой)
        sf::Vector2f(620.f, 260.f)
    ));

    // Враги на средних платформах
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(270.f, 410.f),  // Y: 450-40=410 (над платформой)
        sf::Vector2f(370.f, 410.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(770.f, 410.f),  // Y: 450-40=410 (над платформой)
        sf::Vector2f(870.f, 410.f)
    ));

    // Враги на низких платформах
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(170.f, 560.f),  // Y: 600-40=560 (над платформой)
        sf::Vector2f(220.f, 560.f)
    ));

    // 🔥 АПТЕЧКИ НАД ПЛАТФОРМАМИ
    
    // Аптечка на средней платформе
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(520.f, 420.f)  // Y: 450-30=420 (над платформой)
    ));
    
    // Аптечка на низкой платформе
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(870.f, 570.f)  // Y: 600-30=570 (над платформой)
    ));

    // 🔥 ПОСЫЛКА НА САМОЙ ВЫСОКОЙ ПЛАТФОРМЕ (левый верх)
    packages.push_back(std::make_unique<Package>(
        sf::Vector2f(220.f, 270.f)  // Y: 300-30=270 (над платформой)
    ));

    // 🔥 ТОЧКА ДОСТАВКИ НА БЕЗОПАСНОЙ БАЗОВОЙ ПЛАТФОРМЕ
    deliveryPoints.push_back(std::make_unique<DeliveryPoint>(
        sf::Vector2f(900.f, 650.f)  // Y: 700-50=650 (над платформой)
    ));

    // 🔥 СТАРТОВАЯ ПОЗИЦИЯ ИГРОКА НА БЕЗОПАСНОЙ БАЗОВОЙ ПЛАТФОРМЕ
    player.setPosition(sf::Vector2f(150.f, 650.f));  // Y: 700-50=650 (над платформой)
    player.setVelocity(sf::Vector2f(0.f, 0.f));
    player.setOnGround(true);
    
    if (player.isCarryingPackage()) {
        player.deliverPackage();
    }
    
    player.getHealthSystem().reset();
    
    std::cout << "🎮 Уровень 3 загружен: Равномерные высоты" << std::endl;
    std::cout << "👤 Игрок стартует на позиции: 150, 650" << std::endl;
    std::cout << "📦 Посылка на позиции: 220, 270 (высокий уровень)" << std::endl;
    std::cout << "🚀 Точка доставки на позиции: 900, 650" << std::endl;
    std::cout << "📊 Распределение платформ: Высокие(300) - Средние(450) - Низкие(600)" << std::endl;
}

void Level::createLevel4() {
    platforms.clear();
    enemies.clear();
    packages.clear();
    deliveryPoints.clear();
    acidPools.clear();
    movingPlatforms.clear();
    healthKits.clear();

    // 🔥 БОЛЬШАЯ БАЗОВАЯ ПЛАТФОРМА ДЛЯ СТАРТА И ПЕРЕДВИЖЕНИЯ
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(1000.f, 20.f), 
        sf::Vector2f(100.f, 700.f),
        sf::Color(100, 200, 100, 255)
    ));

    // 🔥 РАЗНООБРАЗНЫЕ ПЛАТФОРМЫ РАЗНЫХ ФОРМ И РАЗМЕРОВ
    
    // ВЫСОКИЕ УЗКИЕ ПЛАТФОРМЫ (3 штуки)
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(80.f, 20.f),    // 🔥 УЗКАЯ
        sf::Vector2f(200.f, 300.f),  // ВЫСОКАЯ: 300
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(80.f, 20.f),    // 🔥 УЗКАЯ
        sf::Vector2f(500.f, 300.f),
        sf::Color(150, 150, 250, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(80.f, 20.f),    // 🔥 УЗКАЯ
        sf::Vector2f(800.f, 300.f),
        sf::Color(150, 150, 250, 255)
    ));

    // СРЕДНИЕ ШИРОКИЕ ПЛАТФОРМЫ (3 штуки)
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(200.f, 20.f),   // 🔥 ШИРОКАЯ
        sf::Vector2f(150.f, 450.f),  // СРЕДНЯЯ: 450
        sf::Color(200, 150, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(200.f, 20.f),   // 🔥 ШИРОКАЯ
        sf::Vector2f(500.f, 450.f),
        sf::Color(200, 150, 100, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(200.f, 20.f),   // 🔥 ШИРОКАЯ
        sf::Vector2f(850.f, 450.f),
        sf::Color(200, 150, 100, 255)
    ));

    // НИЗКИЕ ПЛАТФОРМЫ РАЗНОЙ ДЛИНЫ (3 штуки)
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(120.f, 20.f),   // 🔥 СРЕДНЯЯ
        sf::Vector2f(100.f, 600.f),  // НИЗКАЯ: 600
        sf::Color(180, 100, 200, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(180.f, 20.f),   // 🔥 ДЛИННАЯ
        sf::Vector2f(400.f, 600.f),
        sf::Color(180, 100, 200, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(100.f, 20.f),   // 🔥 КОРОТКАЯ
        sf::Vector2f(800.f, 600.f),
        sf::Color(180, 100, 200, 255)
    ));

    // 🔥 ДОПОЛНИТЕЛЬНЫЕ ПЛАТФОРМЫ ДЛЯ ИНТЕРЕСНЫХ МАРШРУТОВ
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(60.f, 20.f),    // 🔥 ОЧЕНЬ УЗКАЯ
        sf::Vector2f(350.f, 350.f),  // ПРОМЕЖУТОЧНАЯ: 350
        sf::Color(100, 200, 150, 255)
    ));
    
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(60.f, 20.f),    // 🔥 ОЧЕНЬ УЗКАЯ
        sf::Vector2f(650.f, 350.f),
        sf::Color(100, 200, 150, 255)
    ));

    // 🔥 РАЗНООБРАЗНЫЕ ДВИЖУЩИЕСЯ ПЛАТФОРМЫ
    
    // Быстрая вертикальная платформа (левый маршрут)
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(80.f, 20.f),
        sf::Vector2f(300.f, 620.f),  // Низ
        sf::Vector2f(300.f, 320.f),  // Верх (быстро)
        120.f,                       // 🔥 ВЫСОКАЯ СКОРОСТЬ
        false
    ));
    
    // Медленная вертикальная платформа (правый маршрут)
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(80.f, 20.f),
        sf::Vector2f(700.f, 620.f),  // Низ
        sf::Vector2f(700.f, 320.f),  // Верх (медленно)
        60.f,                        // 🔥 НИЗКАЯ СКОРОСТЬ
        false
    ));

    // Длинная горизонтальная платформа (связывает стороны)
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(200.f, 500.f),  // Лево
        sf::Vector2f(800.f, 500.f),  // Право (длинный путь)
        100.f,
        true
    ));

    // 🔥 КИСЛОТНЫЕ ЛУЖИ РАЗНЫХ РАЗМЕРОВ
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(100.f, 15.f),   // 🔥 МАЛЕНЬКАЯ
        sf::Vector2f(200.f, 685.f)
    ));
    
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(200.f, 15.f),   // 🔥 БОЛЬШАЯ
        sf::Vector2f(450.f, 685.f)
    ));
    
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(120.f, 15.f),   // 🔥 СРЕДНЯЯ
        sf::Vector2f(780.f, 685.f)
    ));

    // 🔥 ВРАГИ НА РАЗНЫХ ТИПАХ ПЛАТФОРМ
    
    // Враги на узких высоких платформах (сложно избежать)
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(520.f, 260.f),  // Y: 300-40=260
        sf::Vector2f(560.f, 260.f)   // 🔥 КОРОТКИЙ ПУТЬ
    ));

    // Враги на широких средних платформах (длинные маршруты)
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(170.f, 410.f),  // Y: 450-40=410
        sf::Vector2f(320.f, 410.f)   // 🔥 ДЛИННЫЙ ПУТЬ
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(870.f, 410.f),  // Y: 450-40=410
        sf::Vector2f(1020.f, 410.f)  // 🔥 ДЛИННЫЙ ПУТЬ
    ));

    // Враг на низкой платформе (рядом со стартом)
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(420.f, 560.f),  // Y: 600-40=560
        sf::Vector2f(540.f, 560.f)   // 🔥 НА ДЛИННОЙ ПЛАТФОРМЕ
    ));

    // 🔥 АПТЕЧКИ В ТРУДНОДОСТУПНЫХ МЕСТАХ
    
    // Аптечка на очень узкой платформе
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(350.f, 320.f)   // Y: 350-30=320
    ));
    
    // Аптечка на высокой узкой платформе
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(800.f, 270.f)   // Y: 300-30=270
    ));

    // 🔥 ПОСЫЛКА НА САМОЙ СЛОЖНОЙ ПЛАТФОРМЕ (очень узкая + высокая)
    packages.push_back(std::make_unique<Package>(
        sf::Vector2f(650.f, 320.f)   // Y: 350-30=320 (очень узкая платформа)
    ));

    // 🔥 ТОЧКА ДОСТАВКИ НА БЕЗОПАСНОЙ БАЗОВОЙ ПЛАТФОРМЕ
    deliveryPoints.push_back(std::make_unique<DeliveryPoint>(
        sf::Vector2f(900.f, 650.f)   // Y: 700-50=650
    ));

    // 🔥 СТАРТОВАЯ ПОЗИЦИЯ ИГРОКА НА БЕЗОПАСНОЙ БАЗОВОЙ ПЛАТФОРМЕ
    player.setPosition(sf::Vector2f(150.f, 650.f));  // Y: 700-50=650
    player.setVelocity(sf::Vector2f(0.f, 0.f));
    player.setOnGround(true);
    
    if (player.isCarryingPackage()) {
        player.deliverPackage();
    }
    
    player.getHealthSystem().reset();
    
    std::cout << "🎮 Уровень 4 загружен: Разнообразие платформ" << std::endl;
    std::cout << "👤 Игрок стартует на позиции: 150, 650" << std::endl;
    std::cout << "📦 Посылка на позиции: 650, 320 (очень узкая платформа)" << std::endl;
    std::cout << "🚀 Точка доставки на позиции: 900, 650" << std::endl;
    std::cout << "🎯 Особенности: узкие/широкие платформы, разные скорости, сложные маршруты" << std::endl;
}

void Level::createLevel5() {
    platforms.clear();
    enemies.clear();
    packages.clear();
    deliveryPoints.clear();
    acidPools.clear();
    movingPlatforms.clear();
    healthKits.clear();

    // 🔥 БОЛЬШАЯ БАЗОВАЯ ПЛАТФОРМА С ПЕРЕСЕКАЮЩИМИСЯ КИСЛОТНЫМИ ПОЛЯМИ
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(1000.f, 20.f), 
        sf::Vector2f(100.f, 700.f),
        sf::Color(100, 200, 100, 255)
    ));

    // 🔥 СЛОЖНАЯ МНОГОУРОВНЕВАЯ СТРУКТУРА
    
    // УРОВЕНЬ 1: ОЧЕНЬ ВЫСОКИЕ ТОЧЕЧНЫЕ ПЛАТФОРМЫ (требуют точных прыжков)
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(60.f, 20.f),    // 🔥 ОЧЕНЬ УЗКАЯ
        sf::Vector2f(200.f, 250.f),  // ОЧЕНЬ ВЫСОКО: 250
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

    // УРОВЕНЬ 2: ПЕРЕМЕЩАЮЩИЕСЯ ПЛАТФОРМЫ-МОСТЫ
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(120.f, 20.f),
        sf::Vector2f(150.f, 400.f),  // ВЫСОКО: 400
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

    // УРОВЕНЬ 3: ЗИГЗАГООБРАЗНЫЕ ПЛАТФОРМЫ
    platforms.push_back(std::make_unique<Platform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(100.f, 550.f),  // СРЕДНЯЯ: 550
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

    // 🔥 СЛОЖНЫЕ ДВИЖУЩИЕСЯ ПЛАТФОРМЫ
    
    // ДИАГОНАЛЬНАЯ движущаяся платформа (уникальная механика)
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(250.f, 620.f),  // Низ-Лево
        sf::Vector2f(450.f, 320.f),  // Верх-Право (ДИАГОНАЛЬНО)
        80.f,
        false
    ));
    
    // БЫСТРАЯ вертикальная платформа с широким диапазоном
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(80.f, 20.f),
        sf::Vector2f(650.f, 620.f),  // Низ
        sf::Vector2f(650.f, 200.f),  // ОЧЕНЬ ВЫСОКО
        150.f,                       // 🔥 ОЧЕНЬ БЫСТРО
        false
    ));

    // ДВОЙНАЯ горизонтальная платформа (синхронное движение)
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(80.f, 20.f),
        sf::Vector2f(350.f, 480.f),  // Лево
        sf::Vector2f(550.f, 480.f),  // Право
        100.f,
        true
    ));
    
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(80.f, 20.f),
        sf::Vector2f(750.f, 480.f),  // Лево
        sf::Vector2f(950.f, 480.f),  // Право
        100.f,
        true
    ));

    // 🔥 СЛОЖНАЯ КИСЛОТНАЯ СИСТЕМА (лабиринт)
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

    // 🔥 СЛОЖНЫЕ ВРАЖЕСКИЕ ПАТРУЛИ
    
    // Враги на очень высоких платформах (опасные позиции)
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(220.f, 210.f),  // Y: 250-40=210
        sf::Vector2f(260.f, 210.f)   // 🔥 КОРОТКИЙ ПУТЬ НА УЗКОЙ ПЛАТФОРМЕ
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(420.f, 210.f),
        sf::Vector2f(460.f, 210.f)
    ));
    
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(620.f, 210.f),
        sf::Vector2f(660.f, 210.f)
    ));

    // Враги на средних платформах (длинные маршруты)
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(170.f, 510.f),  // Y: 550-40=510
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

    // Враг на движущейся платформе (динамическая угроза)
    enemies.push_back(std::make_unique<Enemy>(
        sf::Vector2f(380.f, 460.f),  // Y: 480-20=460 (на движущейся)
        sf::Vector2f(420.f, 460.f)
    ));

    // 🔥 СТРАТЕГИЧЕСКИ РАСПОЛОЖЕННЫЕ АПТЕЧКИ
    
    // Аптечка на самой высокой точке (награда за сложный прыжок)
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(800.f, 220.f)   // Y: 250-30=220
    ));
    
    // Аптечка в опасной зоне (рядом с врагами)
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(300.f, 510.f)   // Y: 550-40=510
    ));
    
    // Аптечка на безопасном маршруте (стратегический выбор)
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(950.f, 370.f)   // Y: 400-30=370
    ));

    // 🔥 ПОСЫЛКА В САМОМ СЛОЖНОМ МЕСТЕ (центр высокого уровня)
    packages.push_back(std::make_unique<Package>(
        sf::Vector2f(400.f, 220.f)   // Y: 250-30=220 (самая сложная позиция)
    ));

    // 🔥 ТОЧКА ДОСТАВКИ НА БЕЗОПАСНОЙ БАЗОВОЙ ПЛАТФОРМЕ
    deliveryPoints.push_back(std::make_unique<DeliveryPoint>(
        sf::Vector2f(900.f, 650.f)   // Y: 700-50=650
    ));

    // 🔥 СТАРТОВАЯ ПОЗИЦИЯ ИГРОКА НА БЕЗОПАСНОЙ БАЗОВОЙ ПЛАТФОРМЕ
    player.setPosition(sf::Vector2f(150.f, 650.f));  // Y: 700-50=650
    player.setVelocity(sf::Vector2f(0.f, 0.f));
    player.setOnGround(true);
    
    if (player.isCarryingPackage()) {
        player.deliverPackage();
    }
    
    player.getHealthSystem().reset();
    
    std::cout << "🎮 Уровень 5 загружен: Финальный вызов" << std::endl;
    std::cout << "👤 Игрок стартует на позиции: 150, 650" << std::endl;
    std::cout << "📦 Посылка на позиции: 400, 220 (самая сложная точка)" << std::endl;
    std::cout << "🚀 Точка доставки на позиции: 900, 650" << std::endl;
    std::cout << "💀 Сложность: диагональные платформы, враги на движущихся, точные прыжки" << std::endl;
    std::cout << "🏆 Финальный уровень требует мастерского владения всеми механиками!" << std::endl;
}