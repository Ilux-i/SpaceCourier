#include "Level.h"
#include <iostream>
#include <algorithm>

Level::Level() {
    // В SFML 3.0.2 FloatRect создается через position и size
    levelBounds = sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(1200.f, 800.f));
    createFirstLocation();
}

void Level::update(float deltaTime) {
    player.update(deltaTime);
    handleCollisions();
}

void Level::draw(sf::RenderWindow& window) const {
    for (const auto& platform : platforms) {
        platform->draw(window);
    }
    player.draw(window);
}

void Level::handleCollisions() {
    player.setOnGround(false);
    
    // Коллизии с платформами
    for (const auto& platform : platforms) {
        if (player.getBounds().findIntersection(platform->getBounds()).has_value()) {
            // Определяем сторону столкновения
            sf::FloatRect playerBounds = player.getBounds();
            sf::FloatRect platformBounds = platform->getBounds();
            
            // В SFML 3.0.2 position и size - это поля
            sf::Vector2f playerPos = playerBounds.position;
            sf::Vector2f playerSize = playerBounds.size;
            float playerLeft = playerPos.x;
            float playerRight = playerPos.x + playerSize.x;
            float playerTop = playerPos.y;
            float playerBottom = playerPos.y + playerSize.y;
            
            sf::Vector2f platformPos = platformBounds.position;
            sf::Vector2f platformSize = platformBounds.size;
            float platformLeft = platformPos.x;
            float platformRight = platformPos.x + platformSize.x;
            float platformTop = platformPos.y;
            float platformBottom = platformPos.y + platformSize.y;
            
            // Вычисляем перекрытия
            float overlapLeft = playerRight - platformLeft;
            float overlapRight = platformRight - playerLeft;
            float overlapTop = playerBottom - platformTop;
            float overlapBottom = platformBottom - playerTop;
            
            // Находим минимальное перекрытие
            float minOverlap = std::min({overlapLeft, overlapRight, overlapTop, overlapBottom});
            
            if (minOverlap == overlapTop) {
                // Столкновение сверху (игрок на платформе)
                player.setPosition(sf::Vector2f(player.getPosition().x, platformTop - playerSize.y));
                player.setOnGround(true);
                player.setVelocity(sf::Vector2f(player.getVelocity().x, 0.f));
            }
            else if (minOverlap == overlapBottom) {
                // Столкновение снизу
                player.setPosition(sf::Vector2f(player.getPosition().x, platformBottom));
                player.setVelocity(sf::Vector2f(player.getVelocity().x, 0.f));
            }
            else if (minOverlap == overlapLeft) {
                // Столкновение слева
                player.setPosition(sf::Vector2f(platformLeft - playerSize.x, player.getPosition().y));
            }
            else if (minOverlap == overlapRight) {
                // Столкновение справа
                player.setPosition(sf::Vector2f(platformRight, player.getPosition().y));
            }
        }
    }
    
    // Границы уровня
    sf::Vector2f playerPos = player.getPosition();
    sf::FloatRect playerBounds = player.getBounds();
    sf::Vector2f playerSize = playerBounds.size;
    
    sf::Vector2f levelPos = levelBounds.position;
    sf::Vector2f levelSize = levelBounds.size;
    
    // Левая граница
    if (playerPos.x < levelPos.x) {
        player.setPosition(sf::Vector2f(levelPos.x, playerPos.y));
    }
    // Правая граница
    if (playerPos.x + playerSize.x > levelPos.x + levelSize.x) {
        player.setPosition(sf::Vector2f(levelPos.x + levelSize.x - playerSize.x, playerPos.y));
    }
    // Верхняя граница
    if (playerPos.y < levelPos.y) {
        player.setPosition(sf::Vector2f(playerPos.x, levelPos.y));
        player.setVelocity(sf::Vector2f(player.getVelocity().x, 0.f));
    }
    // Нижняя граница (смерть от падения)
    if (playerPos.y > levelPos.y + levelSize.y) {
        std::cout << "💀 Игрок упал в пропасть! Респавн..." << std::endl;
        player.setPosition(sf::Vector2f(100.f, 100.f));
        player.setVelocity(sf::Vector2f(0.f, 0.f));
    }
}

void Level::createFirstLocation() {
    std::cout << "🗺️ Создаем первую локацию..." << std::endl;
    
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
    
    // Устанавливаем игрока на стартовую позицию
    player.setPosition(sf::Vector2f(150.f, 450.f));
}

Player& Level::getPlayer() {
    return player;
}