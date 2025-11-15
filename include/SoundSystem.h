#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <memory>

enum class MusicType {
    MAIN_MENU,
    LEVEL_1,
    LEVEL_2, 
    LEVEL_3,
    LEVEL_4,
    LEVEL_5,
    VICTORY,
    NONE
};

class SoundSystem {
public:
    SoundSystem();
    ~SoundSystem();
    
    bool initialize();
    void update(float deltaTime);
    
    // Управление музыкой
    void playMusic(MusicType musicType, bool loop = true);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    
    // Настройки громкости
    void setMusicVolume(float volume);
    float getMusicVolume() const { return musicVolume; }
    
    bool isMusicPlaying() const { return currentMusic != MusicType::NONE; }
    MusicType getCurrentMusic() const { return currentMusic; }

private:
    bool loadMusic();
    std::string getMusicPath(MusicType type) const;
    
    std::unique_ptr<sf::Music> currentMusicPlayer;
    MusicType currentMusic;
    float musicVolume;
    bool initialized;
};