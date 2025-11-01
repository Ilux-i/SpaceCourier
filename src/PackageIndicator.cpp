#include "PackageIndicator.h"
#include <cmath>

PackageIndicator::PackageIndicator() : isVisible(false), pulseTimer(0.f) {
    createIcon();
}

void PackageIndicator::createIcon() {
    // Фон индикатора
    background.setSize(sf::Vector2f(60.f, 60.f));
    background.setPosition(sf::Vector2f(570.f, 700.f)); // Центр снизу
    background.setFillColor(sf::Color(50, 50, 50, 200));
    background.setOutlineColor(sf::Color::White);
    background.setOutlineThickness(2.f);
    
    // Иконка посылки
    icon.setSize(sf::Vector2f(30.f, 30.f));
    icon.setPosition(sf::Vector2f(585.f, 715.f)); // Центрируем на фоне
    icon.setFillColor(sf::Color(255, 215, 0)); // Золотой цвет как у посылки
    icon.setOutlineColor(sf::Color(200, 165, 0));
    icon.setOutlineThickness(2.f);
}

void PackageIndicator::setVisible(bool visible) {
    isVisible = visible;
    pulseTimer = 0.f; // Сбрасываем таймер анимации
}

void PackageIndicator::update(float deltaTime) {
    if (isVisible) {
        pulseTimer += deltaTime;
        
        // Пульсация для привлечения внимания
        float alpha = 200 + 55 * std::sin(pulseTimer * 5.f);
        icon.setFillColor(sf::Color(255, 215, 0, static_cast<uint8_t>(alpha)));
    }
}

void PackageIndicator::draw(sf::RenderWindow& window) const {
    if (isVisible) {
        window.draw(background);
        window.draw(icon);
    }
}