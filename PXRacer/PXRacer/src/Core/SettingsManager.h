#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

class SettingsManager {
public:
    // Using Singleton pattern
    static SettingsManager& getInstance();

    // Resolution structure
    struct Resolution {
        unsigned int width;
        unsigned int height;
        std::string name;
    };

    // Available resolutions
    static const std::vector<Resolution> AVAILABLE_RESOLUTIONS;

    // Getters
    unsigned int getWindowWidth() const { return m_windowWidth; }
    unsigned int getWindowHeight() const { return m_windowHeight; }
    bool isFullscreen() const { return m_isFullscreen; }
    int getCurrentResolutionIndex() const { return m_currentResolutionIndex; }
    float getMasterVolume() const { return m_masterVolume;}
    float getMusicVolume() const { return m_musicVolume; }
    float getSfxVolume() const { return m_sfxVolume; }
    bool isMusicMuted() const { return m_musicMuted; }
    bool isSfxMuted() const { return m_sfxMuted; }

    // Setters
    void setResolution(int index);
    void setFullscreen(bool fullscreen);
    void setMusicVolume(float volume);
    void setSfxVolume(float volume);
    void setMasterVolume(float volume);
    void setMusicMuted(bool muted);
    void setSfxMuted(bool muted);

    // Persistence (simple text file format)
    void loadFromFile(const std::string& filename = "settings.txt");
    void saveToFile(const std::string& filename = "settings.txt");

private:
    // Private constructor for singleton
    SettingsManager();
    
    // Delete copy constructor and assignment operator
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    // Settings data
    unsigned int m_windowWidth;
    unsigned int m_windowHeight;
    bool m_isFullscreen;
    int m_currentResolutionIndex;

    // Audio settings
    float m_musicVolume;
    float m_sfxVolume;
    float m_masterVolume;
    bool m_musicMuted;
    bool m_sfxMuted;
};
