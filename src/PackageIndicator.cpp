#include "PackageIndicator.h"
#include <iostream>
#include <cmath>

PackageIndicator::PackageIndicator() 
    : isVisible(false), pulseTimer(0.f), textureLoaded(false),
      sprite(texture) // Инициализируем с текстурой
{
    createIcon();
    loadTexture();
}

void PackageIndicator::loadTexture() {
    if (!texture.loadFromFile("assets/sprites/ui/package_icon.png")) {
        std::cout << "❌ Не удалось загрузить текстуру иконки посылки, используем геометрическую форму" << std::endl;
        textureLoaded = false;
        return;
    }
    
    textureLoaded = true;
    
    // Настраиваем спрайт
    sprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32, 32)));
    sprite.setScale(sf::Vector2f(1.2f, 1.2f)); // Немного увеличиваем для видимости
    
    // Позиционируем по центру снизу (как было в оригинале)
    sprite.setPosition(sf::Vector2f(570.f, 700.f));
    
    std::cout << "✅ Текстура иконки посылки загружена успешно!" << std::endl;
}

void PackageIndicator::createIcon() {
    // Фон индикатора (fallback)
    background.setSize(sf::Vector2f(60.f, 60.f));
    background.setPosition(sf::Vector2f(570.f, 700.f)); // Центр снизу
    background.setFillColor(sf::Color(50, 50, 50, 200));
    background.setOutlineColor(sf::Color::White);
    background.setOutlineThickness(2.f);
    
    // Иконка посылки (fallback)
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
        
        // Пульсация для привлечения внимания (работает и для спрайта)
        if (textureLoaded) {
            float alpha = 200 + 55 * std::sin(pulseTimer * 5.f);
            sprite.setColor(sf::Color(255, 255, 255, static_cast<uint8_t>(alpha)));
        }
    }
}

void PackageIndicator::draw(sf::RenderWindow& window) const {
    if (isVisible) {
        if (textureLoaded) {
            window.draw(sprite);
        } else {
            window.draw(background);
            window.draw(icon);
        }
    }
}