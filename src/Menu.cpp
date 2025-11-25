#include "Menu.h"
#include <iostream>

Menu::Menu() : fontLoaded(false) {

    if (!font.openFromFile("assets/fonts/Planes_ValMore.ttf")) {
        #ifdef __APPLE__
        font.openFromFile("/System/Library/Fonts/Arial.ttf");
        #endif
        fontLoaded = font.getInfo().family != "";
    } else {
        fontLoaded = true;
    }
    
    setupMainMenu();
}

bool Menu::loadFont(const std::string& fontPath) {
    if (font.openFromFile(fontPath)) {
        fontLoaded = true;
        return true;
    }
    return false;
}

void Menu::createText(sf::Text& text, const std::string& content, unsigned int size, const sf::Vector2f& position) const { // Добавляем const
    if (fontLoaded) {
        text.setFont(font);
    }
    text.setString(content);
    text.setCharacterSize(size);
    text.setFillColor(sf::Color::White);
    text.setPosition(position);
}

void Menu::update(float deltaTime) {
    // Анимация или другие обновления меню
}

void Menu::draw(sf::RenderWindow& window) const {
    sf::RectangleShape background(sf::Vector2f(1200, 800));
    background.setFillColor(sf::Color(30, 30, 60, 220));
    window.draw(background);
    
    if (fontLoaded) {
        sf::Text titleText(font, title, 72);
        titleText.setFillColor(sf::Color::White);
        
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(600 - titleBounds.size.x / 2, 100));
        window.draw(titleText);
    } else {
        sf::RectangleShape titleBg(sf::Vector2f(600, 80));
        titleBg.setPosition(sf::Vector2f(300, 90));
        titleBg.setFillColor(sf::Color(100, 100, 200, 200));
        titleBg.setOutlineColor(sf::Color::White);
        titleBg.setOutlineThickness(3);
        window.draw(titleBg);
    }
    
    for (const auto& button : buttons) {
        window.draw(button.shape);
        
        if (fontLoaded) {
            sf::Text buttonText = button.text;
            
            sf::FloatRect textBounds = buttonText.getLocalBounds();
            sf::Vector2f buttonCenter = sf::Vector2f(
                button.shape.getPosition().x + button.shape.getSize().x / 2,
                button.shape.getPosition().y + button.shape.getSize().y / 2
            );
            
            buttonText.setPosition(sf::Vector2f(
                buttonCenter.x - textBounds.size.x / 2,
                buttonCenter.y - textBounds.size.y / 2 - 5
            ));
            
            window.draw(buttonText);
        } else {
            sf::RectangleShape textBg(sf::Vector2f(
                button.shape.getSize().x - 10, 
                button.shape.getSize().y - 10
            ));
            textBg.setPosition(sf::Vector2f(
                button.shape.getPosition().x + 5,
                button.shape.getPosition().y + 5
            ));
            textBg.setFillColor(button.isHovered ? sf::Color(150, 150, 255) : sf::Color(200, 200, 255));
            window.draw(textBg);
        }
    }
    
    if (!fontLoaded) {
        sf::RectangleShape hintBg(sf::Vector2f(400, 120));
        hintBg.setPosition(sf::Vector2f(400, 600));
        hintBg.setFillColor(sf::Color(50, 50, 100, 180));
        hintBg.setOutlineColor(sf::Color::Yellow);
        hintBg.setOutlineThickness(2);
        window.draw(hintBg);
    }
}

void Menu::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (event.is<sf::Event::MouseMoved>()) {
        sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
        
        for (auto& button : buttons) {
            bool wasHovered = button.isHovered;
            button.isHovered = button.shape.getGlobalBounds().contains(mousePos);
            
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
            
            for (auto& button : buttons) {
                if (button.shape.getGlobalBounds().contains(mousePos)) {
                    if (button.action) {
                        button.action();
                    }
                }
            }
        }
    }
}

void Menu::addButton(const std::string& text, std::function<void()> action, const sf::Vector2f& position) {
    Button button(font, text);
    
    button.shape.setSize(sf::Vector2f(300, 60));
    button.shape.setPosition(position);
    button.shape.setFillColor(sf::Color(70, 70, 150));
    button.shape.setOutlineColor(sf::Color::White);
    button.shape.setOutlineThickness(2);
    
    if (fontLoaded) {
        button.text.setFillColor(sf::Color::White);
        button.text.setPosition(position);
        button.text.setCharacterSize(28);
    }
    
    button.action = action;
    button.isHovered = false;
    button.label = text;
    
    buttons.push_back(button);
}

void Menu::clearButtons() {
    buttons.clear();
}

void Menu::setTitle(const std::string& title) {
    this->title = title;
}

void Menu::setupMainMenu() {
    // Базовая настройка
}