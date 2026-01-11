#include "AudioManager.h"
#include "SettingsManager.h"
#include <iostream>
#include <algorithm>

// Singleton instance
AudioManager& AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}

// Constructor - initialize with default values
AudioManager::AudioManager()
    : m_musicVolume(75.0f)
    , m_sfxVolume(50.0f)
{
    std::cout << "[AudioManager] Initialized" << std::endl;

    // Register music tracks for different states

    // Menu items
    m_musicTracks["main_menu"] = "assets/music/menu_theme.ogg";

    // Credits
    m_musicTracks["credits"] = "assets/music/credits_theme.ogg";

    std::cout << "[AudioManager] Registered " << m_musicTracks.size() << " music tracks" << std::endl;

    // Menu

    m_sfxRegistry["menu_select"] = "assets/music/select_item_menu_theme.ogg";
    m_sfxRegistry["gamemode_select"] = "assets/music/select_confirmation_effect.ogg";

    // Race
    m_musicTracks["race_theme_1"] = "assets/music/race_song_theme_1.ogg";
    m_musicTracks["race_theme_2"] = "assets/music/race_song_theme_2.ogg";
}

// Play music track
void AudioManager::playMusic(const std::string& trackId, bool loop) {
    // If already playing this track, do nothing
    if (m_currentTrackId == trackId && m_currentMusic && m_currentMusic->getStatus() == sf::SoundSource::Status::Playing) {
        return;
    }

    // Stop current music if playing
    stopMusic();

    // Check if track exists in registry
    auto it = m_musicTracks.find(trackId);
    if (it == m_musicTracks.end()) {
        std::cerr << "[AudioManager] Track not found: " << trackId << std::endl;
        return;
    }

    // Load new music file
    if (!loadMusicFile(it->second)) {
        std::cerr << "[AudioManager] Failed to load music file: " << it->second << std::endl;
        return;
    }

    // Store current track ID
    m_currentTrackId = trackId;

    // Set volume, looping, and play
    m_currentMusic->setVolume(m_musicVolume);
    m_currentMusic->setLooping(loop);
    m_currentMusic->play();

    std::cout << "[AudioManager] Playing music: " << trackId << " (volume: " << m_musicVolume << "%)" << std::endl;
}

// Stop music
void AudioManager::stopMusic() {
    if (m_currentMusic && m_currentMusic->getStatus() != sf::SoundSource::Status::Stopped) {
        m_currentMusic->stop();
        std::cout << "[AudioManager] Music stopped" << std::endl;
    }
    m_currentTrackId.clear();
}

// Pause music
void AudioManager::pauseMusic() {
    if (m_currentMusic && m_currentMusic->getStatus() == sf::SoundSource::Status::Playing) {
        m_currentMusic->pause();
        std::cout << "[AudioManager] Music paused" << std::endl;
    }
}

// Resume music
void AudioManager::resumeMusic() {
    if (m_currentMusic && m_currentMusic->getStatus() == sf::SoundSource::Status::Paused) {
        m_currentMusic->play();
        std::cout << "[AudioManager] Music resumed" << std::endl;
    }
}

// Set music volume (0-100)
void AudioManager::setMusicVolume(float volume) {
    m_musicVolume = std::clamp(volume, 0.0f, 100.0f);

    // Apply to currently playing music
    if (m_currentMusic) {
        m_currentMusic->setVolume(m_musicVolume);
    }

    std::cout << "[AudioManager] Music volume set to " << m_musicVolume << "%" << std::endl;
}

// Set SFX volume (0-100)
void AudioManager::setSfxVolume(float volume) {
    m_sfxVolume = std::clamp(volume, 0.0f, 100.0f);
    std::cout << "[AudioManager] SFX volume set to " << m_sfxVolume << "%" << std::endl;
}

// Update volumes from SettingsManager
void AudioManager::updateVolumesFromSettings() {
    auto& settings = SettingsManager::getInstance();

    setMusicVolume(settings.getMusicVolume());
    setSfxVolume(settings.getSfxVolume());

    std::cout << "[AudioManager] Volumes updated from SettingsManager" << std::endl;
}

void AudioManager::playSfx(const std::string& sfxId) {
    // Check if SFX exists in registry
    auto registryIt = m_sfxRegistry.find(sfxId);
    if (registryIt == m_sfxRegistry.end()) {
        std::cerr << "[AudioManager] SFX not found in registry: " << sfxId << std::endl;
        return;
    }

    std::string filepath = registryIt->second;

    // Clean up finished sound effects FIRST
    m_activeSfx.erase(
        std::remove_if(m_activeSfx.begin(), m_activeSfx.end(),
            [](const std::unique_ptr<sf::Music>& sfx) {
                return sfx->getStatus() == sf::SoundSource::Status::Stopped;
            }),
        m_activeSfx.end()
    );

    // Create new sound effect using sf::Music
    auto sfx = std::make_unique<sf::Music>();
    if (!sfx->openFromFile(filepath)) {
        std::cerr << "[AudioManager] Failed to load SFX file: " << filepath << std::endl;
        return;
    }

    sfx->setVolume(m_sfxVolume);
    sfx->setLooping(false);  // Important: don't loop SFX!
    sfx->play();

    // Add to active pool
    m_activeSfx.push_back(std::move(sfx));

    std::cout << "[AudioManager] Playing SFX: " << sfxId << " (volume: " << m_sfxVolume << "%)" << std::endl;
}

// Helper to load music file
bool AudioManager::loadMusicFile(const std::string& filepath) {
    m_currentMusic = std::make_unique<sf::Music>();

    if (!m_currentMusic->openFromFile(filepath)) {
        std::cerr << "[AudioManager] Failed to load music file: " << filepath << std::endl;
        m_currentMusic.reset();
        return false;
    }

    std::cout << "[AudioManager] Loaded music file: " << filepath << std::endl;
    return true;
}
