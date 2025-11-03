#include "LevelManager.h"
#include <iostream>

LevelManager::LevelManager() 
    : currentLevel(1), unlockedLevels(1), levelCompleted(false) {
    
    // Создаём все уровни
    createLevel1();
    createLevel2();
    createLevel3();
    createLevel4();
    createLevel5();
}

void LevelManager::loadLevel(int levelNumber) {
    if (levelNumber >= 1 && levelNumber <= levels.size() && levelNumber <= unlockedLevels) {
        currentLevel = levelNumber;
        levelCompleted = false;
        std::cout << "🎮 Загружен уровень " << currentLevel << std::endl;
    } else {
        std::cout << "❌ Уровень " << levelNumber << " недоступен!" << std::endl;
    }
}

Level& LevelManager::getCurrentLevel() {
    return *levels[currentLevel - 1];
}

int LevelManager::getCurrentLevelNumber() const {
    return currentLevel;
}

int LevelManager::getUnlockedLevels() const {
    return unlockedLevels;
}

void LevelManager::unlockNextLevel() {
    if (currentLevel == unlockedLevels) {
        unlockedLevels++;
        std::cout << "🔓 Открыт уровень " << unlockedLevels << "!" << std::endl;
    }
}

bool LevelManager::isLevelComplete() const {
    return levelCompleted;
}

void LevelManager::markLevelComplete() {
    levelCompleted = true;
    unlockNextLevel();
    std::cout << "🎉 Уровень " << currentLevel << " пройден!" << std::endl;
}

void LevelManager::createLevel1() {
    levels.push_back(std::make_unique<Level>(1));
}

void LevelManager::createLevel2() {
    levels.push_back(std::make_unique<Level>(2));
}

void LevelManager::createLevel3() {
    levels.push_back(std::make_unique<Level>(3));
}

void LevelManager::createLevel4() {
    levels.push_back(std::make_unique<Level>(4));
}

void LevelManager::createLevel5() {
    levels.push_back(std::make_unique<Level>(5));
}