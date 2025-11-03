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
    void unlockNextLevel();
    
    bool isLevelComplete() const;
    void markLevelComplete();
    
private:
    std::vector<std::unique_ptr<Level>> levels;
    int currentLevel;
    int unlockedLevels;
    bool levelCompleted;
    
    void createLevel1();
    void createLevel2();
    void createLevel3();
    void createLevel4();
    void createLevel5();
};