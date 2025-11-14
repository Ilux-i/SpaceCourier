#include "Player.h"
#include <iostream>

Player::Player() : onGround(false), jumpForce(500.f), moveSpeed(300.f), gravity(800.f), 
                   carriedPackage(nullptr), textureLoaded(false), animationTimer(0.f), 
                   currentFrame(0), facingRight(true), currentState(AnimationState::IDLE)
                   // sprite уже инициализирован при объявлении
{
    // Базовый геометрический shape (fallback)
    shape.setRadius(25.f);
    shape.setFillColor(sf::Color(220, 50, 50, 255));
    shape.setPosition(position);
    baseMoveSpeed = moveSpeed;
    
    // Загружаем текстуры
    loadTextures();
}

void Player::loadTextures() {
    // Пытаемся загрузить все три текстуры
    bool idleLoaded = idleTexture.loadFromFile("assets/sprites/characters/player_idle.png");
    bool walkLoaded = walkTexture.loadFromFile("assets/sprites/characters/player_walk.png");
    bool jumpLoaded = jumpTexture.loadFromFile("assets/sprites/characters/player_jump.png");
    
    textureLoaded = idleLoaded && walkLoaded && jumpLoaded;
    
    if (!textureLoaded) {
        std::cout << "❌ Не удалось загрузить некоторые текстуры игрока:" << std::endl;
        if (!idleLoaded) std::cout << "   - player_idle.png" << std::endl;
        if (!walkLoaded) std::cout << "   - player_walk.png" << std::endl;
        if (!jumpLoaded) std::cout << "   - player_jump.png" << std::endl;
        std::cout << "   Используем геометрическую форму" << std::endl;
        return;
    }
    
    // НАСТРАИВАЕМ УЖЕ СОЗДАННЫЙ SPRITE
    sprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32, 32)));
    sprite.setScale(sf::Vector2f(1.5f, 1.5f));
    sprite.setPosition(position);
    
    std::cout << "✅ Все текстуры игрока загружены успешно!" << std::endl;
}

void Player::setAnimationState(AnimationState newState) {
    if (currentState == newState) return;
    
    currentState = newState;
    currentFrame = 0;
    animationTimer = 0.f;
    
    if (!textureLoaded) return;
    
    // Переключаем текстуру в зависимости от состояния
    switch (currentState) {
        case AnimationState::IDLE:
            sprite.setTexture(idleTexture);
            break;
        case AnimationState::WALKING:
            sprite.setTexture(walkTexture);
            break;
        case AnimationState::JUMPING:
            sprite.setTexture(jumpTexture);
            break;
    }
}

void Player::updateAnimation(float deltaTime) {
    if (!textureLoaded) return;
    
    animationTimer += deltaTime;
    
    // Определяем количество кадров для каждого состояния
    int frameCount = 1;
    float frameTime = 0.1f;
    
    switch (currentState) {
        case AnimationState::IDLE:
            frameCount = 1; // 1 кадр для стояния
            frameTime = 0.2f;
            break;
        case AnimationState::WALKING:
            frameCount = 6; // 6 кадров ходьбы
            frameTime = 0.08f;
            break;
        case AnimationState::JUMPING:
            frameCount = 6; // 6 кадров прыжка
            frameTime = 0.1f;
            break;
    }
    
    // Анимируем если есть больше 1 кадра
    if (frameCount > 1 && animationTimer > frameTime) {
        currentFrame = (currentFrame + 1) % frameCount;
        animationTimer = 0.f;
    }
    
    // Устанавливаем текущий кадр
    int frameWidth = 32; // Предполагаем ширину кадра 32 пикселя
    int frameHeight = 32; // Предполагаем высоту кадра 32 пикселя
    
    sf::Vector2i framePosition(currentFrame * frameWidth, 0);
    sf::Vector2i frameSize(frameWidth, frameHeight);
    sprite.setTextureRect(sf::IntRect(framePosition, frameSize));
    
    // Отражаем спрайт по горизонтали если смотрим влево
    if (!facingRight) {
        sprite.setScale(sf::Vector2f(-1.5f, 1.5f));
        sprite.setOrigin(sf::Vector2f(frameWidth / 2.0f, 0.f));
    } else {
        sprite.setScale(sf::Vector2f(1.5f, 1.5f));
        sprite.setOrigin(sf::Vector2f(0.f, 0.f));
    }
}

void Player::update(float deltaTime) {
    healthSystem.update(deltaTime);
    
    // ОПРЕДЕЛЯЕМ СОСТОЯНИЕ АНИМАЦИИ
    if (!onGround) {
        setAnimationState(AnimationState::JUMPING);
    } else if (std::abs(velocity.x) > 0.1f) {
        setAnimationState(AnimationState::WALKING);
        // Определяем направление
        if (velocity.x > 0) facingRight = true;
        else if (velocity.x < 0) facingRight = false;
    } else {
        setAnimationState(AnimationState::IDLE);
    }
    
    // ОБНОВЛЯЕМ АНИМАЦИЮ
    if (textureLoaded) {
        updateAnimation(deltaTime);
    }
    
    // ОБНОВЛЯЕМ ИНДИКАТОР ПОСЫЛКИ
    packageIndicator.update(deltaTime);
    
    // Обновляем позицию посылки если несём её
    if (carriedPackage && !carriedPackage->isDelivered()) {
        carriedPackage->setPosition(sf::Vector2f(
            position.x - 5.f, 
            position.y - 40.f
        ));
        carriedPackage->update(deltaTime);
    }
    
    // Применяем гравитацию
    if (!onGround) {
        velocity.y += gravity * deltaTime;
    }
    
    // Обновляем позицию
    position += velocity * deltaTime;
    
    // ОБНОВЛЯЕМ ПОЗИЦИЮ СПРАЙТА ИЛИ ФОРМЫ
    if (textureLoaded) {
        sprite.setPosition(position);
    } else {
        shape.setPosition(position);
    }
    
    // Сбрасываем горизонтальную скорость для плавного управления
    velocity.x = 0;
}

void Player::draw(sf::RenderWindow& window) const {
    if (textureLoaded) {
        window.draw(sprite);
    } else {
        window.draw(shape); // Fallback
    }
    
    // Рисуем посылку если несём её
    if (carriedPackage && !carriedPackage->isDelivered()) {
        carriedPackage->draw(window);
    }
    
    healthSystem.draw(window);
    packageIndicator.draw(window);
}

sf::FloatRect Player::getBounds() const {
    // РАЗМЕРЫ КОЛЛИЗИИ ОСТАЮТСЯ ПРЕЖНИМИ!
    if (textureLoaded) {
        // Используем те же размеры что и у геометрической формы
        return sf::FloatRect(position, sf::Vector2f(50.f, 50.f));
    }
    return shape.getGlobalBounds();
}

void Player::jump() {
    if (onGround && healthSystem.isAlive()) {
        velocity.y = -jumpForce;
        onGround = false;
        std::cout << "🔼 Игрок прыгнул!" << std::endl;
    }
}

void Player::move(const sf::Vector2f& direction) {
    if (healthSystem.isAlive()) {
        velocity.x = direction.x * moveSpeed;
    }
}

void Player::applyGravity(float deltaTime) {
    if (!onGround) {
        velocity.y += gravity * deltaTime;
    }
}

bool Player::isOnGround() const {
    return onGround;
}

void Player::setOnGround(bool grounded) {
    onGround = grounded;
}

HealthSystem& Player::getHealthSystem() {
    return healthSystem;
}

void Player::takeDamage() {
    healthSystem.takeDamage();
}

bool Player::isCarryingPackage() const {
    return carriedPackage != nullptr && !carriedPackage->isDelivered();
}

void Player::pickUpPackage(Package* package) {
    if (!isCarryingPackage() && package && !package->isDelivered()) {
        carriedPackage = package;
        carriedPackage->setCarried(true);
        
        // ВКЛЮЧАЕМ ИНДИКАТОР ПОСЫЛКИ
        packageIndicator.setVisible(true);
        
        // Замедление скорости на 20% при ношении посылки
        moveSpeed = baseMoveSpeed * 0.8f;
        
        std::cout << "📦 Игрок поднял посылку! Скорость уменьшена на 20%" << std::endl;
        std::cout << "   Новая скорость: " << moveSpeed << " (базовая: " << baseMoveSpeed << ")" << std::endl;
    } else {
        std::cout << "❌ Не могу поднять посылку: ";
        if (isCarryingPackage()) std::cout << "уже несу посылку";
        else if (!package) std::cout << "посылка nullptr";
        else if (package->isDelivered()) std::cout << "посылка уже доставлена";
        std::cout << std::endl;
    }
}

void Player::deliverPackage() {
    if (carriedPackage) {
        carriedPackage->setDelivered(true);
        carriedPackage->setCarried(false);
        carriedPackage = nullptr;
        
        // ВЫКЛЮЧАЕМ ИНДИКАТОР ПОСЫЛКИ
        packageIndicator.setVisible(false);
        
        // Восстанавливаем нормальную скорость
        moveSpeed = baseMoveSpeed;
        
        std::cout << "✅ Посылка доставлена! Скорость восстановлена: " << moveSpeed << std::endl;
    } else {
        std::cout << "❌ Не могу доставить посылку: не несу посылку" << std::endl;
    }
}

Package* Player::getCarriedPackage() {
    return carriedPackage;
}

float Player::getMoveSpeed() const {
    return moveSpeed;
}