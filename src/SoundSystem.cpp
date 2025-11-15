#include "SoundSystem.h"
#include <iostream>
#include <filesystem>

SoundSystem::SoundSystem() 
    : currentMusicPlayer(nullptr)
    , currentMusic(MusicType::NONE)
    , musicVolume(50.f)
    , initialized(false)
{
}

SoundSystem::~SoundSystem() {
    if (currentMusicPlayer) {
        currentMusicPlayer->stop();
    }
}

bool SoundSystem::initialize() {
    initialized = true;
    return true;
}

void SoundSystem::update(float deltaTime) {}

void SoundSystem::playMusic(MusicType musicType, bool loop) {
    // Если уже играет эта музыка - ничего не делаем
    if (currentMusic == musicType && currentMusicPlayer && 
        currentMusicPlayer->getStatus() == sf::Music::Status::Playing) {
        return;
    }
    
    // Останавливаем текущую музыку
    if (currentMusicPlayer) {
        currentMusicPlayer->stop();
    }
    
    // Создаём новый плеер
    currentMusicPlayer = std::make_unique<sf::Music>();
    std::string path = getMusicPath(musicType);
    
    if (!currentMusicPlayer->openFromFile(path)) {
        currentMusicPlayer.reset();
        currentMusic = MusicType::NONE;
        return;
    }
    
    // Настраиваем музыку
    currentMusicPlayer->setVolume(musicVolume);
    currentMusicPlayer->play();
    currentMusic = musicType;
}

void SoundSystem::stopMusic() {
    if (currentMusicPlayer) {
        currentMusicPlayer->stop();
        currentMusic = MusicType::NONE;
    }
}

void SoundSystem::pauseMusic() {
    if (currentMusicPlayer) {
        currentMusicPlayer->pause();
    }
}

void SoundSystem::resumeMusic() {
    if (currentMusicPlayer) {
        currentMusicPlayer->play();
    }
}

void SoundSystem::setMusicVolume(float volume) {
    musicVolume = volume;
    if (currentMusicPlayer) {
        currentMusicPlayer->setVolume(musicVolume);
    }
}

std::string SoundSystem::getMusicPath(MusicType type) const {
    switch (type) {
        case MusicType::MAIN_MENU: return "assets/music/main_menu.ogg";
        case MusicType::LEVEL_1: return "assets/music/level_1.ogg";
        case MusicType::LEVEL_2: return "assets/music/level_2.ogg";
        case MusicType::LEVEL_3: return "assets/music/level_3.ogg";
        case MusicType::LEVEL_4: return "assets/music/level_4.ogg";
        case MusicType::LEVEL_5: return "assets/music/level_5.ogg";
        case MusicType::VICTORY: return "assets/music/victory.ogg";
        default: return "";
    }
}