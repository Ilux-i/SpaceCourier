#include "Menu.h"
#include <iostream>

Menu::Menu() {
    // Простая версия без шрифта - будем использовать геометрические фигуры
    setupMainMenu();
}

void Menu::update(float deltaTime) {
    // Анимация или другие обновления меню
}

void Menu::draw(sf::RenderWindow& window) const {
    // Рисуем заголовок как прямоугольник с текстом (пока без шрифта)
    sf::RectangleShape titleBackground(sf::Vector2f(400, 60));
    titleBackground.setPosition(sf::Vector2f(300, 80)); // Исправлено: один аргумент Vector2f
    titleBackground.setFillColor(sf::Color(100, 100, 200));
    titleBackground.setOutlineColor(sf::Color::White);
    titleBackground.setOutlineThickness(2);
    window.draw(titleBackground);
    
    // Рисуем кнопки
    for (const auto& button : buttons) {
        window.draw(button.shape);
        
        // Временно рисуем простой текст (можно заменить на спрайты)
        sf::RectangleShape textBg(sf::Vector2f(button.shape.getSize().x - 20, button.shape.getSize().y - 20));
        textBg.setPosition(sf::Vector2f( // Исправлено: один аргумент Vector2f
            button.shape.getPosition().x + 10, 
            button.shape.getPosition().y + 10
        ));
        textBg.setFillColor(sf::Color(200, 200, 200));
        window.draw(textBg);
    }
}

void Menu::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (event.is<sf::Event::MouseMoved>()) {
        sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
        
        // Проверяем наведение на кнопки
        for (auto& button : buttons) {
            bool wasHovered = button.isHovered;
            button.isHovered = button.shape.getGlobalBounds().contains(mousePos);
            
            // Меняем цвет при наведении
            if (button.isHovered && !wasHovered) {
                button.shape.setFillColor(sf::Color(100, 100, 200));
            } else if (!button.isHovered && wasHovered) {
                button.shape.setFillColor(sf::Color(70, 70, 150));
            }
        }
    }
    
    if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
            
            // Проверяем клик по кнопкам
            for (auto& button : buttons) {
                if (button.shape.getGlobalBounds().contains(mousePos)) {
                    std::cout << "🎮 Нажата кнопка!" << std::endl;
                    if (button.action) {
                        button.action();
                    }
                }
            }
        }
    }
}

void Menu::addButton(const std::string& text, std::function<void()> action, const sf::Vector2f& position) {
    Button button;
    
    // Настройка формы кнопки
    button.shape.setSize(sf::Vector2f(200, 50));
    button.shape.setPosition(position); // Уже правильный формат - один Vector2f
    button.shape.setFillColor(sf::Color(70, 70, 150));
    button.shape.setOutlineColor(sf::Color::White);
    button.shape.setOutlineThickness(2);
    
    button.action = action;
    button.isHovered = false;
    
    buttons.push_back(button);
    
    std::cout << "📝 Добавлена кнопка: " << text << " в позиции (" 
              << position.x << ", " << position.y << ")" << std::endl;
}

void Menu::clearButtons() {
    buttons.clear();
}

void Menu::setTitle(const std::string& title) {
    // Сохраняем заголовок для будущего использования
    std::cout << "🏷️ Установлен заголовок: " << title << std::endl;
}

void Menu::setupMainMenu() {
    // Базовая настройка (можно расширить)
}