#pragma once
#include "Entity.h"
#include "HealthSystem.h"
#include "Package.h"
#include <SFML/Graphics.hpp>
#include "PackageIndicator.h"
#include <unordered_map>
#include <memory>

class Player : public Entity {
public:
    // Перечисление состояний анимации
    enum class AnimationState {
        IDLE,
        WALKING,
        JUMPING
    };
    
    Player();
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) const override;
    sf::FloatRect getBounds() const override;
    
    void jump();
    void move(const sf::Vector2f& direction);
    void applyGravity(float deltaTime);
    
    bool isOnGround() const;
    void setOnGround(bool grounded);
    
    HealthSystem& getHealthSystem();
    void takeDamage();
    
    bool isCarryingPackage() const;
    void pickUpPackage(Package* package);
    void deliverPackage();
    Package* getCarriedPackage();
    float getMoveSpeed() const;
    
private:
    sf::CircleShape shape;
    bool onGround;
    float jumpForce;
    float moveSpeed;
    float gravity;
    
    HealthSystem healthSystem;
    
    Package* carriedPackage;
    float baseMoveSpeed;
    PackageIndicator packageIndicator;
    
    sf::Texture idleTexture;
    sf::Texture walkTexture;
    sf::Texture jumpTexture;
    std::unique_ptr<sf::Sprite> sprite;
    bool textureLoaded;

    AnimationState currentState;
    float animationTimer;
    int currentFrame;
    bool facingRight;
    
    struct AnimationConfig {
        int frameCount;
        float frameTime;
        bool loop;
    };
    
    std::unordered_map<AnimationState, AnimationConfig> animationConfigs;
    float frameTimer;
    
    void loadTextures();
    void updateAnimation(float deltaTime);
    void setAnimationState(AnimationState newState);
    void setupAnimations();
    void updateVisualPosition();
};