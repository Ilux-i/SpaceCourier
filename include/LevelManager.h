#pragma once
#include "Level.h"
#include <vector>
#include <memory>

class LevelManager {
public:
    LevelManager();
    
    void loadLevel(int levelNumber);
    Level& getCurrentLevel();
    int getCurrentLevelNumber() const;
    int getUnlockedLevels() const;
    int getLastUnlockedLevel() const;
    
    void unlockNextLevel();
    bool isLevelComplete() const;
    void markLevelComplete();

private:
    void recreateLevel(int levelNumber);  // 👈 ОСНОВНОЙ МЕТОД
    
    std::vector<std::unique_ptr<Level>> levels;
    int currentLevel;
    int unlockedLevels;
    bool levelCompleted;
};