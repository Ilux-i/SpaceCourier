#include "OptionsMenu.h"
#include <iostream>

OptionsMenu::OptionsMenu() : fontLoaded(false) {
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

void OptionsMenu::setupUI() {
    sf::Text title{font, "SETTINGS", 72};
    title.setFillColor(sf::Color::White);
    title.setPosition(sf::Vector2f(450.f, 80.f));
    titles.push_back(title);
    
    Slider musicSlider(font);
    createSlider(musicSlider, "Music Volume", 200.f);
    musicSlider.currentValue = 50.f;
    updateSliderVisuals(musicSlider);
    sliders.push_back(musicSlider);
    
    sf::RectangleShape backButton;
    backButton.setSize(sf::Vector2f(200.f, 60.f));
    backButton.setPosition(sf::Vector2f(500.f, 500.f));
    backButton.setFillColor(sf::Color(70, 70, 150));
    backButton.setOutlineColor(sf::Color::White);
    backButton.setOutlineThickness(2.f);
    buttons.push_back(backButton);
    
    sf::Text backText{font, "BACK", 32};
    backText.setFillColor(sf::Color::White);
    
    sf::FloatRect textBounds = backText.getLocalBounds();
    backText.setPosition(sf::Vector2f(
        500.f + 100.f - textBounds.size.x / 2,
        500.f + 30.f - textBounds.size.y / 2 - 5
    ));
    buttonTexts.push_back(backText);
}

void OptionsMenu::createSlider(Slider& slider, const std::string& label, float yPos) {

    slider.track.setSize(sf::Vector2f(SLIDER_WIDTH, SLIDER_HEIGHT));
    slider.track.setPosition(sf::Vector2f(400.f, yPos));
    slider.track.setFillColor(sf::Color(100, 100, 100));
    slider.track.setOutlineColor(sf::Color::White);
    slider.track.setOutlineThickness(2.f);
    
    slider.thumb.setSize(sf::Vector2f(THUMB_WIDTH, THUMB_HEIGHT));
    slider.thumb.setFillColor(sf::Color(200, 200, 200));
    slider.thumb.setOutlineColor(sf::Color::White);
    slider.thumb.setOutlineThickness(2.f);
    
    slider.label.setString(label);
    slider.label.setFillColor(sf::Color::White);
    slider.label.setPosition(sf::Vector2f(400.f, yPos - 40.f));
    
    slider.valueText.setFillColor(sf::Color::Yellow);
    slider.valueText.setPosition(sf::Vector2f(400.f + SLIDER_WIDTH + 20.f, yPos - 5.f));
}

void OptionsMenu::updateSliderValue(Slider& slider, float mouseX) {
    float trackStart = slider.track.getPosition().x;
    float trackEnd = trackStart + SLIDER_WIDTH;
    
    float clampedX = std::max(trackStart, std::min(mouseX, trackEnd));
    
    float normalizedValue = (clampedX - trackStart) / SLIDER_WIDTH;
    
    slider.currentValue = slider.minValue + normalizedValue * (slider.maxValue - slider.minValue);
    
    updateSliderVisuals(slider);
    
    if (!sliders.empty() && &slider == &sliders[0] && volumeCallback) {
        volumeCallback(slider.currentValue);
    }
}

void OptionsMenu::updateSliderVisuals(Slider& slider) {

    float normalizedValue = (slider.currentValue - slider.minValue) / (slider.maxValue - slider.minValue);
    float thumbX = slider.track.getPosition().x + normalizedValue * SLIDER_WIDTH - THUMB_WIDTH / 2;
    slider.thumb.setPosition(sf::Vector2f(thumbX, slider.track.getPosition().y - (THUMB_HEIGHT - SLIDER_HEIGHT) / 2));
    
    std::string valueStr = std::to_string(static_cast<int>(slider.currentValue)) + "%";
    slider.valueText.setString(valueStr);
}

void OptionsMenu::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
            
            for (auto& slider : sliders) {
                if (slider.thumb.getGlobalBounds().contains(mousePos) || 
                    slider.track.getGlobalBounds().contains(mousePos)) {
                    slider.isDragging = true;
                    updateSliderValue(slider, mousePos.x);
                }
            }
            
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
        
        for (auto& slider : sliders) {
            if (slider.isDragging) {
                updateSliderValue(slider, mousePos.x);
            }
        }
        
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