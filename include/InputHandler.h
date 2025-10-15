#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Vector2.hpp>
#include <unordered_map>
#include <functional>
#include <vector>

class InputHandler {
public:
    InputHandler();
    
    void update();
    void handleEvent(const sf::Event& event);
    
    void bindAction(sf::Keyboard::Scancode key, std::function<void()> pressedCallback);
    
    bool isKeyPressed(sf::Keyboard::Scancode key) const;
    bool isKeyJustPressed(sf::Keyboard::Scancode key) const;
    
    sf::Vector2f getMovementVector() const;

private:
    struct KeyState {
        bool isPressed = false;
        bool wasPressed = false;
    };
    
    std::unordered_map<sf::Keyboard::Scancode, KeyState> keyStates_;
    std::unordered_map<sf::Keyboard::Scancode, std::vector<std::function<void()>>> actionCallbacks_;
    
    void setupDefaultBindings();
};



// # Компилятор и флаги
// # CXX = clang++
// # CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I/opt/homebrew/include
// # LDFLAGS = -L/opt/homebrew/lib

// # # Имена библиотек SFML (возможные варианты)
// # SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

// # # Структура проекта
// # SRCDIR = src
// # INCDIR = include
// # BUILDDIR = build
// # TARGET = $(BUILDDIR)/SpaceCourier

// # # Исходные файлы
// # SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/InputHandler.cpp
// # OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)

// # .PHONY: all clean run check-libs

// # all: $(TARGET)

// # $(BUILDDIR):
// # 	@mkdir -p $(BUILDDIR)

// # $(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
// # 	@echo "🔨 Компиляция $<..."
// # 	@$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

// # $(TARGET): $(OBJECTS)
// # 	@echo "🔗 Линковка $(TARGET)..."
// # 	@$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS) $(SFML_LIBS)
// # 	@echo "✅ Сборка завершена!"

// # run: $(TARGET)
// # 	@echo "🚀 Запуск игры..."
// # 	@DYLD_LIBRARY_PATH=/opt/homebrew/lib:$(DYLD_LIBRARY_PATH) ./$(TARGET)

// # clean:
// # 	@rm -rf $(BUILDDIR)

// # # Проверка библиотек
// # check-libs:
// # 	@echo "🔍 Поиск библиотек SFML..."
// # 	@for lib in graphics window system; do \
// # 		if [ -f "/opt/homebrew/lib/libsfml-$$lib.dylib" ]; then \
// # 			echo "✅ libsfml-$$lib.dylib найдена"; \
// # 		elif [ -f "/opt/homebrew/lib/libsfml-$$lib-3.dylib" ]; then \
// # 			echo "✅ libsfml-$$lib-3.dylib найдена"; \
// # 		else \
// # 			echo "❌ libsfml-$$lib НЕ найдена"; \
// # 		fi \
// # 	done