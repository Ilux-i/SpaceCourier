#include "Menu.h"
#include <iostream>

Menu::Menu() : fontLoaded(false) {
    // Пытаемся загрузить шрифт
    if (!font.openFromFile("assets/fonts/arial.ttf")) { // Исправлено: openFromFile
        std::cout << "❌ Не удалось загрузить шрифт из assets/fonts/arial.ttf" << std::endl;
        // Попробуем системные пути
        #ifdef __APPLE__
        if (!font.openFromFile("/System/Library/Fonts/Arial.ttf")) { // Исправлено: openFromFile
            std::cout << "❌ Не удалось загрузить системный шрифт Arial" << std::endl;
        } else {
            fontLoaded = true;
            std::cout << "✅ Загружен системный шрифт Arial" << std::endl;
        }
        #else
        std::cout << "⚠️  Шрифт не загружен, используем геометрический интерфейс" << std::endl;
        #endif
    } else {
        fontLoaded = true;
        std::cout << "✅ Шрифт загружен успешно!" << std::endl;
    }
    
    setupMainMenu();
}

bool Menu::loadFont(const std::string& fontPath) {
    if (font.openFromFile(fontPath)) { // Исправлено: openFromFile
        fontLoaded = true;
        std::cout << "✅ Шрифт загружен: " << fontPath << std::endl;
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
    // Рисуем фон меню
    sf::RectangleShape background(sf::Vector2f(1200, 800));
    background.setFillColor(sf::Color(30, 30, 60, 220));
    window.draw(background);
    
    // Рисуем заголовок
    if (fontLoaded) {
        // Создаем текст с шрифтом
        sf::Text titleText(font, title, 72); // Исправлено: конструктор с шрифтом
        titleText.setFillColor(sf::Color::White);
        
        // Центрируем заголовок
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(sf::Vector2f(600 - titleBounds.size.x / 2, 100));
        window.draw(titleText);
    } else {
        // Запасной вариант - прямоугольник с надписью
        sf::RectangleShape titleBg(sf::Vector2f(600, 80));
        titleBg.setPosition(sf::Vector2f(300, 90));
        titleBg.setFillColor(sf::Color(100, 100, 200, 200));
        titleBg.setOutlineColor(sf::Color::White);
        titleBg.setOutlineThickness(3);
        window.draw(titleBg);
    }
    
    // Рисуем кнопки с текстом
    for (const auto& button : buttons) {
        // Фон кнопки
        window.draw(button.shape);
        
        // Текст кнопки
        if (fontLoaded) {
            // Создаем копию текста для отрисовки (чтобы избежать проблем с const)
            sf::Text buttonText = button.text;
            
            // Центрируем текст в кнопке
            sf::FloatRect textBounds = buttonText.getLocalBounds();
            sf::Vector2f buttonCenter = sf::Vector2f(
                button.shape.getPosition().x + button.shape.getSize().x / 2,
                button.shape.getPosition().y + button.shape.getSize().y / 2
            );
            
            buttonText.setPosition(sf::Vector2f(
                buttonCenter.x - textBounds.size.x / 2,
                buttonCenter.y - textBounds.size.y / 2 - 5 // Небольшая коррекция по вертикали
            ));
            
            window.draw(buttonText);
        } else {
            // Запасной вариант - цветные прямоугольники с подсказками в консоли
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
    
    // Подсказки управления если шрифт не загружен
    if (!fontLoaded) {
        sf::RectangleShape hintBg(sf::Vector2f(400, 120));
        hintBg.setPosition(sf::Vector2f(400, 600));
        hintBg.setFillColor(sf::Color(50, 50, 100, 180));
        hintBg.setOutlineColor(sf::Color::Yellow);
        hintBg.setOutlineThickness(2);
        window.draw(hintBg);
        
        std::cout << "🎮 Подсказка: Используйте мышь для выбора кнопок" << std::endl;
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
                if (!fontLoaded) {
                    std::cout << "🎯 Наведено на: " << button.label << std::endl;
                }
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
                    std::cout << "🎮 Нажата кнопка: " << button.label << std::endl;
                    if (button.action) {
                        button.action();
                    }
                }
            }
        }
    }
}

void Menu::addButton(const std::string& text, std::function<void()> action, const sf::Vector2f& position) {
    // Создаем кнопку через конструктор
    Button button(font, text); // Используем конструктор
    
    // Настройка формы кнопки
    button.shape.setSize(sf::Vector2f(300, 60));
    button.shape.setPosition(position);
    button.shape.setFillColor(sf::Color(70, 70, 150));
    button.shape.setOutlineColor(sf::Color::White);
    button.shape.setOutlineThickness(2);
    
    // Настройка текста кнопки
    if (fontLoaded) {
        button.text.setFillColor(sf::Color::White);
        button.text.setPosition(position); // Базовая позиция
        button.text.setCharacterSize(28);
    }
    
    button.action = action;
    button.isHovered = false;
    button.label = text;
    
    buttons.push_back(button);
    
    std::cout << "📝 Добавлена кнопка: " << text << std::endl;
}

void Menu::clearButtons() {
    buttons.clear();
}

void Menu::setTitle(const std::string& title) {
    this->title = title;
    std::cout << "🏷️ Установлен заголовок: " << title << std::endl;
}

void Menu::setupMainMenu() {
    // Базовая настройка
}