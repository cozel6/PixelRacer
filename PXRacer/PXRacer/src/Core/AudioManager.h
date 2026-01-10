#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <unordered_map>
#include <memory>

class AudioManager {
public:
    // Singleton pattern
    static AudioManager& getInstance();

    // Delete copy constructor and assignment
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    // Music management
    void playMusic(const std::string& trackId, bool loop = true);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();

    // Volume control (0-100 range)
    void setMusicVolume(float volume);  // 0-100
    void setSfxVolume(float volume);    // 0-100
    float getMusicVolume() const { return m_musicVolume; }
    float getSfxVolume() const { return m_sfxVolume; }

    // Update volumes from SettingsManager
    void updateVolumesFromSettings();

    // Sound effect support (future)
    void playSfx(const std::string& sfxId);

private:
    // Private constructor
    AudioManager();

    // Music tracks registry
    std::unordered_map<std::string, std::string> m_musicTracks;

    // Current music instance
    std::unique_ptr<sf::Music> m_currentMusic;
    std::string m_currentTrackId;

    // Volume settings (0-100)
    float m_musicVolume;
    float m_sfxVolume;

    // Helper to load music file
    bool loadMusicFile(const std::string& filepath);
};
