#include "LevelManager.h"
#include <iostream>

LevelManager::LevelManager() 
    : currentLevel(1), unlockedLevels(1), levelCompleted(false) {
    
    std::cout << "🔧 Инициализация LevelManager..." << std::endl;
    
    // 👇 ИЗНАЧАЛЬНО СОЗДАЁМ ВСЕ 5 УРОВНЕЙ
    for (int i = 1; i <= 5; ++i) {
        recreateLevel(i);
    }
    
    std::cout << "✅ LevelManager инициализирован, создано уровней: " << levels.size() << std::endl;
}

void LevelManager::recreateLevel(int levelNumber) {
    std::cout << "🔧 recreateLevel(" << levelNumber << ")..." << std::endl;
    
    // 👇 БЕЗОПАСНОЕ СОЗДАНИЕ/ЗАМЕНА УРОВНЯ
    int index = levelNumber - 1;
    
    if (index < levels.size()) {
        // Заменяем существующий уровень
        levels[index] = std::make_unique<Level>(levelNumber);
        std::cout << "   🔄 Заменён уровень " << levelNumber << " по индексу " << index << std::endl;
    } else {
        // Добавляем новый уровень
        levels.push_back(std::make_unique<Level>(levelNumber));
        std::cout << "   ➕ Добавлен уровень " << levelNumber << " по индексу " << index << std::endl;
    }
    
    std::cout << "   ✅ Уровень " << levelNumber << " пересоздан. Всего уровней: " << levels.size() << std::endl;
}

void LevelManager::loadLevel(int levelNumber) {
    std::cout << "🎮 loadLevel(" << levelNumber << ")..." << std::endl;
    
    if (levelNumber >= 1 && levelNumber <= 5 && levelNumber <= unlockedLevels) {
        currentLevel = levelNumber;
        levelCompleted = false;
        
        // 👇 ПЕРЕСОЗДАЁМ УРОВЕНЬ ПРИ КАЖДОЙ ЗАГРУЗКЕ
        recreateLevel(levelNumber);
        
        std::cout << "✅ Загружен и пересоздан уровень " << currentLevel 
                  << " (открыто: " << unlockedLevels << ")" << std::endl;
    } else {
        std::cout << "❌ Уровень " << levelNumber << " недоступен! Открыто: " << unlockedLevels << std::endl;
    }
}

Level& LevelManager::getCurrentLevel() {
    // 👇 МАКСИМАЛЬНАЯ ПРОВЕРКА ДЛЯ БЕЗОПАСНОСТИ
    int index = currentLevel - 1;
    
    if (levels.empty()) {
        std::cout << "🆘 КРИТИЧЕСКАЯ ОШИБКА: levels пуст!" << std::endl;
        recreateLevel(1);
        return *levels[0];
    }
    
    if (index < 0 || index >= levels.size()) {
        std::cout << "🆘 КРИТИЧЕСКАЯ ОШИБКА: Неверный индекс " << index << " при размере " << levels.size() << std::endl;
        recreateLevel(1);
        currentLevel = 1;
        return *levels[0];
    }
    
    if (!levels[index]) {
        std::cout << "🆘 КРИТИЧЕСКАЯ ОШИБКА: levels[" << index << "] является nullptr!" << std::endl;
        recreateLevel(currentLevel);
    }
    
    return *levels[index];
}

int LevelManager::getCurrentLevelNumber() const {
    return currentLevel;
}

int LevelManager::getUnlockedLevels() const {
    return unlockedLevels;
}

int LevelManager::getLastUnlockedLevel() const {
    return unlockedLevels;
}

void LevelManager::markLevelComplete() {
    levelCompleted = true;
    
    if (currentLevel == unlockedLevels) {
        unlockedLevels = std::min(unlockedLevels + 1, 5); // 👈 Не больше 5 уровней
        std::cout << "🔓 Открыт уровень " << unlockedLevels << "!" << std::endl;
    }
    
    std::cout << "🎉 Уровень " << currentLevel << " пройден! Открыто уровней: " << unlockedLevels << std::endl;
}

bool LevelManager::isLevelComplete() const {
    return levelCompleted;
}

void LevelManager::unlockNextLevel() {
    if (unlockedLevels < 5) {
        unlockedLevels++;
        std::cout << "🔓 Открыт уровень " << unlockedLevels << "!" << std::endl;
    }
}