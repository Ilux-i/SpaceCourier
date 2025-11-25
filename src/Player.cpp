#include "Player.h"
#include <iostream>

Player::Player() 
    : Entity(),
      onGround(false), 
      jumpForce(500.f), 
      moveSpeed(300.f), 
      gravity(800.f), 
      carriedPackage(nullptr), 
      textureLoaded(false), 
      animationTimer(0.f), 
      currentFrame(0), 
      facingRight(true), 
      currentState(AnimationState::IDLE),
      frameTimer(0.f)
{
    shape.setRadius(25.f);
    shape.setFillColor(sf::Color(220, 50, 50, 255));
    shape.setPosition(position);
    baseMoveSpeed = moveSpeed;
    
    setupAnimations();
    
    loadTextures();
}

void Player::setupAnimations() {
    animationConfigs[AnimationState::IDLE] = {4, 0.2f, true};
    animationConfigs[AnimationState::WALKING] = {6, 0.1f, true};
    animationConfigs[AnimationState::JUMPING] = {3, 0.15f, false};
}

void Player::updateVisualPosition() {
    if (textureLoaded && sprite) {
        sprite->setPosition(position);
    } else {
        shape.setPosition(position);
    }
}

void Player::loadTextures() {
    bool idleLoaded = idleTexture.loadFromFile("assets/sprites/characters/player_idle.png");
    bool walkLoaded = walkTexture.loadFromFile("assets/sprites/characters/player_walk.png");
    bool jumpLoaded = jumpTexture.loadFromFile("assets/sprites/characters/player_jump.png");
    
    textureLoaded = idleLoaded && walkLoaded && jumpLoaded;
    
    if (!textureLoaded) {
        std::cout << "❌ Не удалось загрузить некоторые текстуры игрока" << std::endl;
        return;
    } else {
        sprite = std::make_unique<sf::Sprite>(idleTexture);
        sprite->setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32, 32)));
        sprite->setScale(sf::Vector2f(1.5f, 1.5f));
        
        sf::FloatRect bounds = sprite->getLocalBounds();
        sprite->setOrigin(sf::Vector2f(bounds.size.x / 2, bounds.size.y / 2));
        sprite->setPosition(position);
    }
}

void Player::setAnimationState(AnimationState newState) {
    if (currentState == newState) return;
    
    currentState = newState;
    currentFrame = 0;
    frameTimer = 0.f;
    
    if (!textureLoaded || !sprite) return;
    
    switch (currentState) {
        case AnimationState::IDLE:
            sprite->setTexture(idleTexture);
            break;
        case AnimationState::WALKING:
            sprite->setTexture(walkTexture);
            break;
        case AnimationState::JUMPING:
            sprite->setTexture(jumpTexture);
            break;
    }
    
    int frameWidth = 32;
    int frameHeight = 32;
    sprite->setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(frameWidth, frameHeight)));
}

void Player::updateAnimation(float deltaTime) {
    if (!textureLoaded || !sprite) return;
    
    if (deltaTime > 0.1f) deltaTime = 0.1f;
    
    auto& anim = animationConfigs[currentState];
    frameTimer += deltaTime;
    
    if (anim.frameCount > 1 && frameTimer >= anim.frameTime) {
        currentFrame = (currentFrame + 1) % anim.frameCount;
        frameTimer = 0.f;
        
        int frameWidth = 32;
        int frameHeight = 32;
        sf::Vector2i framePosition(currentFrame * frameWidth, 0);
        sf::Vector2i frameSize(frameWidth, frameHeight);
        sprite->setTextureRect(sf::IntRect(framePosition, frameSize));
    }
    
    if (!facingRight) {
        sprite->setScale(sf::Vector2f(-1.5f, 1.5f));
        sprite->setOrigin(sf::Vector2f(32.f, 0.f));
    } else {
        sprite->setScale(sf::Vector2f(1.5f, 1.5f));
        sprite->setOrigin(sf::Vector2f(0.f, 0.f));
    }
}

void Player::update(float deltaTime) {
    healthSystem.update(deltaTime);
    
    if (!onGround) {
        setAnimationState(AnimationState::JUMPING);
    } else if (std::abs(velocity.x) > 10.0f) {
        setAnimationState(AnimationState::WALKING);
        if (velocity.x > 0) facingRight = true;
        else if (velocity.x < 0) facingRight = false;
    } else {
        if (currentState != AnimationState::IDLE) {
            setAnimationState(AnimationState::IDLE);
        }
    }
    
    if (textureLoaded) {
        updateAnimation(deltaTime);
    }
    
    packageIndicator.update(deltaTime);
    
    if (carriedPackage && !carriedPackage->isDelivered()) {
        carriedPackage->setPosition(sf::Vector2f(
            position.x - 5.f, 
            position.y - 20.f
        ));
        carriedPackage->update(deltaTime);
    }
    
    if (!onGround) {
        velocity.y += gravity * deltaTime;
    }
    
    position += velocity * deltaTime;
    updateVisualPosition();
    velocity.x = 0;
}

void Player::draw(sf::RenderWindow& window) const {
    if (textureLoaded && sprite) {
        window.draw(*sprite);
    } else {
        window.draw(shape);
    }
    
    if (carriedPackage && !carriedPackage->isDelivered()) {
        carriedPackage->draw(window);
    }
    
    healthSystem.draw(window);
    packageIndicator.draw(window);
}

sf::FloatRect Player::getBounds() const {
    if (textureLoaded && sprite) {
        sf::FloatRect spriteBounds = sprite->getGlobalBounds();
        
        float width = spriteBounds.size.x * 0.8f;
        float height = spriteBounds.size.y;
        float offsetX = (spriteBounds.size.x - width) / 2;
        float offsetY = (spriteBounds.size.y - height) / 2;
        
        return sf::FloatRect(
            sf::Vector2f(spriteBounds.position.x + offsetX, spriteBounds.position.y + offsetY),
            sf::Vector2f(width, height)
        );
    }
    return shape.getGlobalBounds();
}

void Player::jump() {
    if (onGround && healthSystem.isAlive()) {
        velocity.y = -jumpForce;
        onGround = false;
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
        
        packageIndicator.setVisible(true);
        
        moveSpeed = baseMoveSpeed * 0.8f;
        
    }
}

void Player::deliverPackage() {
    if (carriedPackage) {
        carriedPackage->setDelivered(true);
        carriedPackage->setCarried(false);
        carriedPackage = nullptr;
        
        packageIndicator.setVisible(false);
        
        moveSpeed = baseMoveSpeed;
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