#include "VictoryMenu.h"
#include <iostream>

VictoryMenu::VictoryMenu() 
    : fontLoaded(false), animationTimer(0.f), currentLevel(1), totalLevels(5), isFinalLevel(false) 
{
    if (!font.openFromFile("assets/fonts/Planes_ValMore.ttf")) {
        #ifdef __APPLE__
        font.openFromFile("/System/Library/Fonts/Arial.ttf");
        #endif
        fontLoaded = font.getInfo().family != "";
    } else {
        fontLoaded = true;
    }
    
    setupUI();
}

void VictoryMenu::setLevelInfo(int currentLevel, int totalLevels) {
    this->currentLevel = currentLevel;
    this->totalLevels = totalLevels;
    this->isFinalLevel = (currentLevel == totalLevels);
    
    setupUI();
}

void VictoryMenu::setupUI() {
    titles.clear();
    buttons.clear();
    buttonTexts.clear();
    
    if (!fontLoaded) return;
    
    if (isFinalLevel) {
        sf::Text title{font, "MISSION COMPLETE!", 64};
        title.setFillColor(sf::Color::Yellow);
        title.setPosition(sf::Vector2f(350.f, 120.f));
        titles.push_back(title);
        
        sf::Text message{font, "You've delivered all packages across the galaxy!", 36};
        message.setFillColor(sf::Color::White);
        message.setPosition(sf::Vector2f(250.f, 200.f));
        titles.push_back(message);
        
        sf::Text stats{font, "All " + std::to_string(totalLevels) + " levels completed!", 32};
        stats.setFillColor(sf::Color::Green);
        stats.setPosition(sf::Vector2f(400.f, 260.f));
        titles.push_back(stats);
    } else {
        sf::Text title{font, "LEVEL " + std::to_string(currentLevel) + " COMPLETE!", 64};
        title.setFillColor(sf::Color::Green);
        title.setPosition(sf::Vector2f(300.f, 120.f));
        titles.push_back(title);
        
        sf::Text message{font, "Package delivered successfully!", 36};
        message.setFillColor(sf::Color::White);
        message.setPosition(sf::Vector2f(400.f, 200.f));
        titles.push_back(message);
        
        sf::Text stats{font, "Progress: " + std::to_string(currentLevel) + "/" + std::to_string(totalLevels) + " levels", 32};
        stats.setFillColor(sf::Color::Cyan);
        stats.setPosition(sf::Vector2f(420.f, 260.f));
        titles.push_back(stats);
    }
    
    if (!isFinalLevel) {
        sf::RectangleShape nextLevelButton;
        nextLevelButton.setSize(sf::Vector2f(300.f, 60.f));
        nextLevelButton.setPosition(sf::Vector2f(450.f, 350.f));
        nextLevelButton.setFillColor(sf::Color(70, 150, 70));
        nextLevelButton.setOutlineColor(sf::Color::White);
        nextLevelButton.setOutlineThickness(2.f);
        buttons.push_back(nextLevelButton);
        
        sf::Text nextLevelText{font, "NEXT LEVEL", 32};
        nextLevelText.setFillColor(sf::Color::White);
        
        sf::FloatRect textBounds = nextLevelText.getLocalBounds();
        nextLevelText.setPosition(sf::Vector2f(
            450.f + 150.f - textBounds.size.x / 2,
            350.f + 30.f - textBounds.size.y / 2 - 5
        ));
        buttonTexts.push_back(nextLevelText);
    }
    
    sf::RectangleShape levelSelectButton;
    levelSelectButton.setSize(sf::Vector2f(300.f, 60.f));
    levelSelectButton.setPosition(sf::Vector2f(450.f, isFinalLevel ? 350.f : 430.f));
    levelSelectButton.setFillColor(sf::Color(70, 70, 150));
    levelSelectButton.setOutlineColor(sf::Color::White);
    levelSelectButton.setOutlineThickness(2.f);
    buttons.push_back(levelSelectButton);
    
    sf::Text levelSelectText{font, "LEVEL SELECT", 32};
    levelSelectText.setFillColor(sf::Color::White);
    
    sf::FloatRect textBounds = levelSelectText.getLocalBounds();
    levelSelectText.setPosition(sf::Vector2f(
        450.f + 150.f - textBounds.size.x / 2,
        (isFinalLevel ? 350.f : 430.f) + 30.f - textBounds.size.y / 2 - 5
    ));
    buttonTexts.push_back(levelSelectText);
    
    sf::RectangleShape mainMenuButton;
    mainMenuButton.setSize(sf::Vector2f(300.f, 60.f));
    mainMenuButton.setPosition(sf::Vector2f(450.f, isFinalLevel ? 430.f : 510.f));
    mainMenuButton.setFillColor(sf::Color(150, 70, 70));
    mainMenuButton.setOutlineColor(sf::Color::White);
    mainMenuButton.setOutlineThickness(2.f);
    buttons.push_back(mainMenuButton);
    
    sf::Text mainMenuText{font, "MAIN MENU", 32};
    mainMenuText.setFillColor(sf::Color::White);
    
    textBounds = mainMenuText.getLocalBounds();
    mainMenuText.setPosition(sf::Vector2f(
        450.f + 150.f - textBounds.size.x / 2,
        (isFinalLevel ? 430.f : 510.f) + 30.f - textBounds.size.y / 2 - 5
    ));
    buttonTexts.push_back(mainMenuText);
}

void VictoryMenu::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
            
            for (size_t i = 0; i < buttons.size(); ++i) {
                if (buttons[i].getGlobalBounds().contains(mousePos)) {
                    if (!isFinalLevel && i == 0 && nextLevelCallback) {
                        nextLevelCallback();
                    } else if ((isFinalLevel && i == 0) || (!isFinalLevel && i == 1)) {
                        if (mainMenuCallback) mainMenuCallback();
                    } else if ((isFinalLevel && i == 1) || (!isFinalLevel && i == 2)) {
                        if (mainMenuCallback) mainMenuCallback();
                    }
                }
            }
        }
    }
    
    if (event.is<sf::Event::MouseMoved>()) {
        sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
        
        for (size_t i = 0; i < buttons.size(); ++i) {
            bool isHovered = buttons[i].getGlobalBounds().contains(mousePos);
            
            if (!isFinalLevel && i == 0) {
                buttons[i].setFillColor(isHovered ? sf::Color(100, 200, 100) : sf::Color(70, 150, 70));
            } else if ((isFinalLevel && i == 0) || (!isFinalLevel && i == 1)) {
                buttons[i].setFillColor(isHovered ? sf::Color(100, 100, 200) : sf::Color(70, 70, 150));
            } else {
                buttons[i].setFillColor(isHovered ? sf::Color(200, 100, 100) : sf::Color(150, 70, 70));
            }
        }
    }
}

void VictoryMenu::update(float deltaTime) {
    animationTimer += deltaTime;
    if (!titles.empty()) {
        float alpha = 200 + 55 * std::sin(animationTimer * 2.f);
        titles[0].setFillColor(sf::Color(
            isFinalLevel ? 255 : 0, 
            isFinalLevel ? 255 : 255,
            0,
            static_cast<uint8_t>(alpha)
        ));
    }
}

void VictoryMenu::draw(sf::RenderWindow& window) const {
    sf::RectangleShape background(sf::Vector2f(1200, 800));
    background.setFillColor(sf::Color(10, 10, 40, 240));
    window.draw(background);
    
    for (const auto& title : titles) {
        window.draw(title);
    }
    
    for (size_t i = 0; i < buttons.size(); ++i) {
        window.draw(buttons[i]);
        if (i < buttonTexts.size()) {
            window.draw(buttonTexts[i]);
        }
    }
}

void VictoryMenu::setMainMenuCallback(std::function<void()> callback) {
    mainMenuCallback = callback;
}

void VictoryMenu::setExitCallback(std::function<void()> callback) {
    exitCallback = callback;
}

void VictoryMenu::setNextLevelCallback(std::function<void()> callback) {
    nextLevelCallback = callback;
}