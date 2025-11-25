#include "AcidPool.h"
#include <iostream>

AcidPool::AcidPool(const sf::Vector2f& size, const sf::Vector2f& position) 
    : playerInAcid(false), damageCooldown(0.f), textureLoaded(false),
      sprite(texture)
{
    shape.setSize(size);
    shape.setPosition(position);
    this->position = position;
    
    shape.setFillColor(sf::Color(0, 255, 0, 200));
    shape.setOutlineColor(sf::Color(255, 255, 0, 255));
    shape.setOutlineThickness(2.f);
    
    loadTexture();
}

void AcidPool::loadTexture() {
    if (!texture.loadFromFile("assets/sprites/environment/acid.png")) {
        std::cout << "❌ Не удалось загрузить текстуру кислоты, используем геометрическую форму" << std::endl;
        textureLoaded = false;
        return;
    }
    
    textureLoaded = true;
    
    sf::Vector2f poolSize = shape.getSize();
    
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(
        sf::Vector2i(0, 0), 
        sf::Vector2i(static_cast<int>(poolSize.x), static_cast<int>(poolSize.y))
    ));
    
    texture.setRepeated(true);
    
    sprite.setPosition(position);
    
}

void AcidPool::update(float deltaTime) {
    if (playerInAcid) {
        damageCooldown -= deltaTime;
    }
    
    if (textureLoaded) {
        sprite.setPosition(position);
    } else {
        shape.setPosition(position);
    }
}

void AcidPool::draw(sf::RenderWindow& window) const {
    if (textureLoaded) {
        window.draw(sprite);
    } else {
        window.draw(shape);
    }
}

sf::FloatRect AcidPool::getBounds() const {
    if (textureLoaded) {
        sf::FloatRect spriteBounds = sprite.getGlobalBounds();
        return spriteBounds;
    }
    return shape.getGlobalBounds();
}

bool AcidPool::isPlayerInAcid() const {
    return playerInAcid;
}

void AcidPool::setPlayerInAcid(bool inAcid) {
    playerInAcid = inAcid;
}

float AcidPool::getDamageCooldown() const {
    return damageCooldown;
}

void AcidPool::resetDamageCooldown() {
    damageCooldown = 1.0f;
}