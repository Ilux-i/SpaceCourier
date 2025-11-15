#include "OptionsMenu.h"
#include <iostream>

OptionsMenu::OptionsMenu() : fontLoaded(false) {
    // Пытаемся загрузить шрифт
    if (!font.openFromFile("assets/fonts/Planes_ValMore.ttf")) {
        std::cout << "❌ Не удалось загрузить шрифт для меню настроек" << std::endl;
        #ifdef __APPLE__
        if (!font.openFromFile("/System/Library/Fonts/Arial.ttf")) {
            std::cout << "❌ Не удалось загрузить системный шрифт" << std::endl;
        } else {
            fontLoaded = true;
        }
        #else
        std::cout << "⚠️  Шрифт не загружен, используем геометрический интерфейс" << std::endl;
        #endif
    } else {
        fontLoaded = true;
        std::cout << "✅ Шрифт для настроек загружен успешно!" << std::endl;
    }
    
    setupUI();
}

void OptionsMenu::setupUI() {
    // Заголовок
    sf::Text title{font, "SETTINGS", 72};
    title.setFillColor(sf::Color::White);
    title.setPosition(sf::Vector2f(450.f, 80.f));
    titles.push_back(title);
    
    // Слайдер громкости музыки - ИСПРАВЛЕНО: используем конструктор
    Slider musicSlider(font);  // 👈 Передаём шрифт в конструктор
    createSlider(musicSlider, "Music Volume", 200.f);
    musicSlider.currentValue = 50.f;
    updateSliderVisuals(musicSlider);
    sliders.push_back(musicSlider);
    
    // Кнопка назад
    sf::RectangleShape backButton;
    backButton.setSize(sf::Vector2f(200.f, 60.f));
    backButton.setPosition(sf::Vector2f(500.f, 500.f));
    backButton.setFillColor(sf::Color(70, 70, 150));
    backButton.setOutlineColor(sf::Color::White);
    backButton.setOutlineThickness(2.f);
    buttons.push_back(backButton);
    
    // Текст кнопки
    sf::Text backText{font, "BACK", 32};
    backText.setFillColor(sf::Color::White);
    
    // Центрируем текст в кнопке
    sf::FloatRect textBounds = backText.getLocalBounds();
    backText.setPosition(sf::Vector2f(
        500.f + 100.f - textBounds.size.x / 2,
        500.f + 30.f - textBounds.size.y / 2 - 5
    ));
    buttonTexts.push_back(backText);
}

void OptionsMenu::createSlider(Slider& slider, const std::string& label, float yPos) {
    // Трек слайдера
    slider.track.setSize(sf::Vector2f(SLIDER_WIDTH, SLIDER_HEIGHT));
    slider.track.setPosition(sf::Vector2f(400.f, yPos));
    slider.track.setFillColor(sf::Color(100, 100, 100));
    slider.track.setOutlineColor(sf::Color::White);
    slider.track.setOutlineThickness(2.f);
    
    // Ползунок
    slider.thumb.setSize(sf::Vector2f(THUMB_WIDTH, THUMB_HEIGHT));
    slider.thumb.setFillColor(sf::Color(200, 200, 200));
    slider.thumb.setOutlineColor(sf::Color::White);
    slider.thumb.setOutlineThickness(2.f);
    
    // Метка
    slider.label.setString(label);
    slider.label.setFillColor(sf::Color::White);
    slider.label.setPosition(sf::Vector2f(400.f, yPos - 40.f));
    
    // Значение
    slider.valueText.setFillColor(sf::Color::Yellow);
    slider.valueText.setPosition(sf::Vector2f(400.f + SLIDER_WIDTH + 20.f, yPos - 5.f));
}

void OptionsMenu::updateSliderValue(Slider& slider, float mouseX) {
    float trackStart = slider.track.getPosition().x;
    float trackEnd = trackStart + SLIDER_WIDTH;
    
    // Ограничиваем позицию мыши в пределах трека
    float clampedX = std::max(trackStart, std::min(mouseX, trackEnd));
    
    // Вычисляем значение от 0 до 1
    float normalizedValue = (clampedX - trackStart) / SLIDER_WIDTH;
    
    // Конвертируем в диапазон слайдера
    slider.currentValue = slider.minValue + normalizedValue * (slider.maxValue - slider.minValue);
    
    updateSliderVisuals(slider);
    
    // Вызываем callback если это слайдер громкости
    if (!sliders.empty() && &slider == &sliders[0] && volumeCallback) {
        volumeCallback(slider.currentValue);
    }
}

void OptionsMenu::updateSliderVisuals(Slider& slider) {
    // Позиционируем ползунок
    float normalizedValue = (slider.currentValue - slider.minValue) / (slider.maxValue - slider.minValue);
    float thumbX = slider.track.getPosition().x + normalizedValue * SLIDER_WIDTH - THUMB_WIDTH / 2;
    slider.thumb.setPosition(sf::Vector2f(thumbX, slider.track.getPosition().y - (THUMB_HEIGHT - SLIDER_HEIGHT) / 2));
    
    // Обновляем текст значения
    std::string valueStr = std::to_string(static_cast<int>(slider.currentValue)) + "%";
    slider.valueText.setString(valueStr);
}

void OptionsMenu::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
            
            // Проверяем клик по слайдерам
            for (auto& slider : sliders) {
                if (slider.thumb.getGlobalBounds().contains(mousePos) || 
                    slider.track.getGlobalBounds().contains(mousePos)) {
                    slider.isDragging = true;
                    updateSliderValue(slider, mousePos.x);
                }
            }
            
            // Проверяем клик по кнопке назад
            if (!buttons.empty() && buttons[0].getGlobalBounds().contains(mousePos) && backCallback) {
                backCallback();
            }
        }
    }
    
    if (event.is<sf::Event::MouseButtonReleased>()) {
        const auto* mouseEvent = event.getIf<sf::Event::MouseButtonReleased>();
        if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
            for (auto& slider : sliders) {
                slider.isDragging = false;
            }
        }
    }
    
    if (event.is<sf::Event::MouseMoved>()) {
        sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
        
        // Обновляем перетаскиваемые слайдеры
        for (auto& slider : sliders) {
            if (slider.isDragging) {
                updateSliderValue(slider, mousePos.x);
            }
        }
        
        // Подсветка кнопок при наведении
        for (size_t i = 0; i < buttons.size(); ++i) {
            bool isHovered = buttons[i].getGlobalBounds().contains(mousePos);
            buttons[i].setFillColor(isHovered ? sf::Color(100, 100, 200) : sf::Color(70, 70, 150));
        }
    }
}

void OptionsMenu::update(float deltaTime) {
    // Анимации или другие обновления
}

void OptionsMenu::draw(sf::RenderWindow& window) const {
    // Фон
    sf::RectangleShape background(sf::Vector2f(1200, 800));
    background.setFillColor(sf::Color(30, 30, 60, 220));
    window.draw(background);
    
    // Заголовки
    for (const auto& title : titles) {
        window.draw(title);
    }
    
    // Слайдеры
    for (const auto& slider : sliders) {
        window.draw(slider.track);
        window.draw(slider.thumb);
        window.draw(slider.label);
        window.draw(slider.valueText);
    }
    
    // Кнопки
    for (size_t i = 0; i < buttons.size(); ++i) {
        window.draw(buttons[i]);
        if (i < buttonTexts.size()) {
            window.draw(buttonTexts[i]);
        }
    }
}

void OptionsMenu::setBackCallback(std::function<void()> callback) {
    backCallback = callback;
}

void OptionsMenu::setVolumeCallback(std::function<void(float)> callback) {
    volumeCallback = callback;
}

void OptionsMenu::setMusicVolume(float volume) {
    if (!sliders.empty()) {
        sliders[0].currentValue = volume;
        updateSliderVisuals(sliders[0]);
    }
}