#include "Entity.h"

Entity::Entity() : position(0.f, 0.f), velocity(0.f, 0.f) {}

bool Entity::checkCollision(const Entity& other) const {
    return getBounds().findIntersection(other.getBounds()).has_value();
}

void Entity::setPosition(const sf::Vector2f& newPosition) {
    position = newPosition;
}

sf::Vector2f Entity::getPosition() const {
    return position;
}

void Entity::setVelocity(const sf::Vector2f& newVelocity) {
    velocity = newVelocity;
}

sf::Vector2f Entity::getVelocity() const {
    return velocity;
}