#include "Level.h"
#include <iostream>
#include <algorithm>
#include <cmath>

Level::Level() {
    levelBounds = sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(1200.f, 800.f));
    createFirstLocation();
}

void Level::update(float deltaTime) {
    // Проверяем жив ли игрок перед обновлением
    if (player.getHealthSystem().isAlive()) {
        player.update(deltaTime);
        
        // Обновляем врагов
        for (auto& enemy : enemies) {
            enemy->update(deltaTime);
        }
        
        // Обновляем точки доставки
        for (auto& deliveryPoint : deliveryPoints) {
            deliveryPoint->update(deltaTime);
        }
        
        handleCollisions();
        // УБРАЛИ ВЫЗОВЫ handlePackageInteractions и handleDeliveryInteractions
        // Теперь взаимодействие ТОЛЬКО через handleEInteraction() по нажатию E
    } else {
        // Игрок мертв - респавним уровень
        std::cout << "💀 Игрок умер! Запускаем респавн уровня..." << std::endl;
        respawnLevel();
    }
}

void Level::draw(sf::RenderWindow& window) const {
    // Рисуем платформы
    for (const auto& platform : platforms) {
        platform->draw(window);
    }
    
    // Рисуем точки доставки
    for (const auto& deliveryPoint : deliveryPoints) {
        deliveryPoint->draw(window);
    }
    
    // Рисуем посылки
    for (const auto& package : packages) {
        package->draw(window);
    }
    
    // Рисуем врагов
    for (const auto& enemy : enemies) {
        enemy->draw(window);
    }
    
    // Рисуем игрока
    player.draw(window);
}

void Level::handleCollisions() {
    player.setOnGround(false);
    
    // Коллизии с платформами
    for (const auto& platform : platforms) {
        if (player.getBounds().findIntersection(platform->getBounds()).has_value()) {
            handlePlayerPlatformCollision(*platform);
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
        
        // ВСЕГДА ВЫЗЫВАЕМ ПОЛНЫЙ РЕСПАВН УРОВНЯ ПРИ ПАДЕНИИ В ПРОПАСТЬ
        std::cout << "🔄 Запуск полного респавна уровня..." << std::endl;
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
    
    // РЕСПАВНИМ ИГРОКА
    respawnPlayer();
    
    // ВОССТАНАВЛИВАЕМ ПОСЫЛКИ НА НАЧАЛЬНЫЕ ПОЗИЦИИ
    for (size_t i = 0; i < packages.size() && i < packageStartPositions.size(); ++i) {
        packages[i]->setCarried(false);
        packages[i]->setDelivered(false);
        
        // ЯВНО УСТАНАВЛИВАЕМ ПОЗИЦИЮ И ОБНОВЛЯЕМ
        sf::Vector2f startPos = packageStartPositions[i];
        packages[i]->setPosition(startPos);
        
        // ВЫЗЫВАЕМ update ДЛЯ ПРИМЕНЕНИЯ ИЗМЕНЕНИЙ
        packages[i]->update(0.f);
        
        std::cout << "📦 Посылка " << i << " возвращена в позицию: (" 
                  << startPos.x << ", " << startPos.y << ")" << std::endl;
    }
    
    std::cout << "✅ Уровень перезапущен! Здоровье: " 
              << player.getHealthSystem().getHealth() << "/3" << std::endl;
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

void Level::createFirstLocation() {
    std::cout << "🗺️ Создаем первую локацию с посылками..." << std::endl;
    
    platforms.clear();
    enemies.clear();
    packages.clear();
    deliveryPoints.clear();
    
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
    
    // ПОСЫЛКА НА САМОЙ ВЕРХНЕЙ ПЛАТФОРМЕ
    packages.push_back(std::make_unique<Package>(
        sf::Vector2f(750.f, 250.f)
    ));
    packageStartPositions.push_back(sf::Vector2f(750.f, 250.f));
    
    // ТОЧКА ДОСТАВКИ ЗА СТЕНКОЙ
    deliveryPoints.push_back(std::make_unique<DeliveryPoint>(
        sf::Vector2f(950.f, 650.f)
    ));
    deliveryPointStartPositions.push_back(sf::Vector2f(950.f, 650.f));
    
    respawnPlayer();
    
    std::cout << "✅ Добавлено: " << enemies.size() << " врагов, " 
              << packages.size() << " посылка, " 
              << deliveryPoints.size() << " точка доставки" << std::endl;
}

Player& Level::getPlayer() {
    return player;
}