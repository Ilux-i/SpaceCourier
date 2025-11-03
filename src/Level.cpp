#include "Level.h"
#include <iostream>
#include <algorithm>
#include <cmath>

Level::Level() {
    levelBounds = sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(1200.f, 800.f));
    createFirstLocation();
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
        
        // ОБНОВЛЯЕМ АПТЕЧКИ
        for (auto& healthKit : healthKits) {
            healthKit->update(deltaTime);
        }
        
        handleCollisions();
        handlePlayerAcidCollisions();
        handlePlayerHealthKitCollisions(); // ДОБАВЛЯЕМ ОБРАБОТКУ АПТЕЧЕК
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
    
    // Границы уровня (остаётся без изменений)
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
        // Вертикальная коллизия
        if (playerBounds.position.y < platformBounds.position.y) {
            // Игрок над платформой (приземление)
            player.setPosition(sf::Vector2f(
                player.getPosition().x,
                platformBounds.position.y - playerBounds.size.y
            ));
            player.setVelocity(sf::Vector2f(player.getVelocity().x, 0.f));
            player.setOnGround(true);
            
            // ЗАПОМИНАЕМ ПЛАТФОРМУ ТОЛЬКО ЕСЛИ ИГРОК ДЕЙСТВИТЕЛЬНО СТОИТ СВЕРХУ
            if (player.getVelocity().y >= 0) { // Игрок падает или стоит
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


// НОВЫЙ МЕТОД ДЛЯ ОБРАБОТКИ КИСЛОТЫ
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

// ЕДИНСТВЕННЫЙ МЕТОД ДЛЯ ВЗАИМОДЕЙСТВИЯ С E
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
                std::cout << "🎉 Посылка успешно доставлена!" << std::endl;
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

void Level::respawnLevel() {
    std::cout << "🔄 Респавн уровня..." << std::endl;
    
    // СБРАСЫВАЕМ ЗДОРОВЬЕ ИГРОКА
    player.getHealthSystem().reset();
    
    // ВОССТАНАВЛИВАЕМ ВСЕ АПТЕЧКИ
    for (auto& healthKit : healthKits) {
        healthKit->respawn();
    }
    
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

void Level::createFirstLocation() {
    std::cout << "🗺️ Создаем первую локацию с кислотными озёрами..." << std::endl;
    
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

    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(150.f, 20.f),
        sf::Vector2f(200.f, 600.f),  // Начальная позиция
        sf::Vector2f(400.f, 600.f),  // Конечная позиция  
        100.f,                       // Скорость
        true                         // Горизонтальное движение
    ));
    
    // Вертикальная движущаяся платформа
    movingPlatforms.push_back(std::make_unique<MovingPlatform>(
        sf::Vector2f(100.f, 20.f),
        sf::Vector2f(600.f, 500.f),  // Начальная позиция
        sf::Vector2f(600.f, 300.f),  // Конечная позиция
        80.f,                        // Скорость
        false                        // Вертикальное движение
    ));
    
    // КИСЛОТНЫЕ ОЗЁРА НА ПЛАТФОРМАХ
    // Озеро на первой платформе (y = 700 - высота озера)
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(200.f, 15.f), // Тонкое озеро на платформе
        sf::Vector2f(150.f, 685.f) // На 15px выше чем платформа (700 - 15 = 685)
    ));
    
    // Озеро на второй платформе (y = 500 - высота озера)
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(150.f, 15.f),
        sf::Vector2f(120.f, 485.f) // На 15px выше чем платформа (500 - 15 = 485)
    ));
    
    // Озеро на третьей платформе (y = 400 - высота озера)
    acidPools.push_back(std::make_unique<AcidPool>(
        sf::Vector2f(120.f, 15.f),
        sf::Vector2f(410.f, 385.f) // На 15px выше чем платформа (400 - 15 = 385)
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
    
    packageStartPositions.clear();
    deliveryPointStartPositions.clear();
    
    // Посылка
    packages.push_back(std::make_unique<Package>(
        sf::Vector2f(750.f, 250.f)
    ));
    packageStartPositions.push_back(sf::Vector2f(750.f, 250.f));
    
    // Точка доставки
    deliveryPoints.push_back(std::make_unique<DeliveryPoint>(
        sf::Vector2f(950.f, 650.f)
    ));
    deliveryPointStartPositions.push_back(sf::Vector2f(950.f, 650.f));
    
    respawnPlayer();
    
    std::cout << "✅ Добавлено: " << enemies.size() << " врагов, " 
              << packages.size() << " посылка, " 
              << deliveryPoints.size() << " точка доставки, "
              << acidPools.size() << " кислотных озёр" << std::endl;

    // АПТЕЧКИ
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(200.f, 450.f) // На первой платформе
    ));
    
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(750.f, 200.f) // На верхней платформе
    ));
    
    healthKits.push_back(std::make_unique<HealthKit>(
        sf::Vector2f(950.f, 600.f) // Рядом с точкой доставки
    ));
}

Player& Level::getPlayer() {
    return player;
}