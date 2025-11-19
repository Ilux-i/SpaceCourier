#include "VictoryMenu.h"
#include <iostream>

VictoryMenu::VictoryMenu() 
    : fontLoaded(false), animationTimer(0.f), currentLevel(1), totalLevels(5), isFinalLevel(false) 
{
    if (!font.openFromFile("assets/fonts/Planes_ValMore.ttf")) {
        #ifdef __APPLE__
        if (!font.openFromFile("/System/Library/Fonts/Arial.ttf")) {
            std::cout << "❌ Не удалось загрузить шрифт для экрана победы" << std::endl;
        } else {
            fontLoaded = true;
        }
        #else
        std::cout << "⚠️ Шрифт не загружен, используем геометрический интерфейс" << std::endl;
        #endif
    } else {
        fontLoaded = true;
        std::cout << "✅ Шрифт для экрана победы загружен успешно!" << std::endl;
    }
    
    setupUI();
}

void VictoryMenu::setLevelInfo(int currentLevel, int totalLevels) {
    this->currentLevel = currentLevel;
    this->totalLevels = totalLevels;
    this->isFinalLevel = (currentLevel == totalLevels);
    
    setupUI(); // Перестраиваем UI с новой информацией
}

void VictoryMenu::setupUI() {
    titles.clear();
    buttons.clear();
    buttonTexts.clear();
    
    if (!fontLoaded) return;
    
    // 🔥 ДИНАМИЧЕСКИЙ ЗАГОЛОВОК В ЗАВИСИМОСТИ ОТ УРОВНЯ
    if (isFinalLevel) {
        // ФИНАЛЬНЫЙ УРОВЕНЬ - ОСОБОЕ ПОЗДРАВЛЕНИЕ
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
        // ОБЫЧНЫЙ УРОВЕНЬ
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
    
    // 🔥 ДИНАМИЧЕСКИЕ КНОПКИ В ЗАВИСИМОСТИ ОТ УРОВНЯ
    if (!isFinalLevel) {
        // КНОПКА СЛЕДУЮЩЕГО УРОВНЯ (только не для финального)
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
    
    // Кнопка выбора уровня
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
    
    // Кнопка главного меню
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
            
            // Проверяем клик по кнопкам
            for (size_t i = 0; i < buttons.size(); ++i) {
                if (buttons[i].getGlobalBounds().contains(mousePos)) {
                    if (!isFinalLevel && i == 0 && nextLevelCallback) {
                        // Кнопка "NEXT LEVEL"
                        std::cout << "🎮 Переход к следующему уровню" << std::endl;
                        nextLevelCallback();
                    } else if ((isFinalLevel && i == 0) || (!isFinalLevel && i == 1)) {
                        // Кнопка "LEVEL SELECT"
                        std::cout << "🗂️ Переход к выбору уровня" << std::endl;
                        if (mainMenuCallback) mainMenuCallback(); // Временно используем mainMenuCallback
                    } else if ((isFinalLevel && i == 1) || (!isFinalLevel && i == 2)) {
                        // Кнопка "MAIN MENU"
                        std::cout << "🏠 Возврат в главное меню" << std::endl;
                        if (mainMenuCallback) mainMenuCallback();
                    }
                }
            }
        }
    }
    
    // Подсветка кнопок при наведении
    if (event.is<sf::Event::MouseMoved>()) {
        sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
        
        for (size_t i = 0; i < buttons.size(); ++i) {
            bool isHovered = buttons[i].getGlobalBounds().contains(mousePos);
            
            // Разные цвета для разных кнопок
            if (!isFinalLevel && i == 0) {
                // NEXT LEVEL - зелёная
                buttons[i].setFillColor(isHovered ? sf::Color(100, 200, 100) : sf::Color(70, 150, 70));
            } else if ((isFinalLevel && i == 0) || (!isFinalLevel && i == 1)) {
                // LEVEL SELECT - синяя
                buttons[i].setFillColor(isHovered ? sf::Color(100, 100, 200) : sf::Color(70, 70, 150));
            } else {
                // MAIN MENU - красная
                buttons[i].setFillColor(isHovered ? sf::Color(200, 100, 100) : sf::Color(150, 70, 70));
            }
        }
    }
}

void VictoryMenu::update(float deltaTime) {
    // Анимация мерцания заголовка
    animationTimer += deltaTime;
    if (!titles.empty()) {
        float alpha = 200 + 55 * std::sin(animationTimer * 2.f);
        titles[0].setFillColor(sf::Color(
            isFinalLevel ? 255 : 0,    // R: жёлтый для финала, зелёный для обычного
            isFinalLevel ? 255 : 255,  // G
            0,                         // B
            static_cast<uint8_t>(alpha)
        ));
    }
}

void VictoryMenu::draw(sf::RenderWindow& window) const {
    // Фон с звёздами
    sf::RectangleShape background(sf::Vector2f(1200, 800));
    background.setFillColor(sf::Color(10, 10, 40, 240));
    window.draw(background);
    
    // Заголовки и текст
    for (const auto& title : titles) {
        window.draw(title);
    }
    
    // Кнопки
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