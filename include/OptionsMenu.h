#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include <string>

class OptionsMenu {
public:
    OptionsMenu();
    
    void setBackCallback(std::function<void()> callback);
    void setVolumeCallback(std::function<void(float)> callback);
    
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    
    void setMusicVolume(float volume);

private:
    struct Slider {
        // ИСПРАВЛЕНО: Добавляем конструктор для Slider
        Slider(const sf::Font& font) 
            : label(font, "", 28)
            , valueText(font, "", 24)
            , minValue(0.f)
            , maxValue(100.f)
            , currentValue(50.f)
            , isDragging(false)
        {}
        
        sf::RectangleShape track;
        sf::RectangleShape thumb;
        sf::Text label;
        sf::Text valueText;
        float minValue;
        float maxValue;
        float currentValue;
        bool isDragging;
    };
    
    void setupUI();
    void createSlider(Slider& slider, const std::string& label, float yPos);
    void updateSliderValue(Slider& slider, float mouseX);
    void updateSliderVisuals(Slider& slider);
    
    std::vector<Slider> sliders;
    std::vector<sf::Text> titles;
    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text> buttonTexts;
    
    std::function<void()> backCallback;
    std::function<void(float)> volumeCallback;
    
    sf::Font font;
    bool fontLoaded;
    
    // Константы для UI
    static constexpr float SLIDER_WIDTH = 400.f;
    static constexpr float SLIDER_HEIGHT = 20.f;
    static constexpr float THUMB_WIDTH = 30.f;
    static constexpr float THUMB_HEIGHT = 40.f;
};