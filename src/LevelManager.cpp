#include "LevelManager.h"
#include <iostream>

LevelManager::LevelManager() 
    : currentLevel(1), unlockedLevels(1), levelCompleted(false) {
    
    for (int i = 1; i <= 5; ++i) {
        recreateLevel(i);
    }
    
}

void LevelManager::recreateLevel(int levelNumber) {
    
    int index = levelNumber - 1;
    
    if (index < levels.size()) {
        levels[index] = std::make_unique<Level>(levelNumber);
    } else {
        levels.push_back(std::make_unique<Level>(levelNumber));
    }

}

void LevelManager::loadLevel(int levelNumber) {
    if (levelNumber >= 1 && levelNumber <= 5 && levelNumber <= unlockedLevels) {
        currentLevel = levelNumber;
        levelCompleted = false;
        
        recreateLevel(levelNumber);
        
    } else {
        std::cout << "❌ Уровень " << levelNumber << " недоступен! Открыто: " << unlockedLevels << std::endl;
    }
}

Level& LevelManager::getCurrentLevel() {
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
        unlockedLevels = std::min(unlockedLevels + 1, 5);
    }
    
}

bool LevelManager::isLevelComplete() const {
    return levelCompleted;
}

void LevelManager::unlockNextLevel() {
    if (unlockedLevels < 5) {
        unlockedLevels++;
    }
}