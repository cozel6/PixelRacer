#include "SettingsManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


// Define available resolutions (16:9 aspect ratio)
const std::vector<SettingsManager::Resolution> SettingsManager::AVAILABLE_RESOLUTIONS = {
    { 1280, 720, "1280x720 (HD)" },
    { 1920, 1080, "1920x1080 (Full HD)" },
    { 2560, 1440, "2560x1440 (2K)" }
};

// Singleton instance
SettingsManager& SettingsManager::getInstance() {
    static SettingsManager instance;
    return instance;
}

// Constructor - initialize with default values
SettingsManager::SettingsManager()
    : m_windowWidth(1280)
    , m_windowHeight(720)
    , m_isFullscreen(false)
    , m_currentResolutionIndex(0)
{
    std::cout << "[SettingsManager] Initialized with defaults" << std::endl;
}

// Set resolution by index
void SettingsManager::setResolution(int index) {
    if (index >= 0 && index < static_cast<int>(AVAILABLE_RESOLUTIONS.size())) {
        m_currentResolutionIndex = index;
        m_windowWidth = AVAILABLE_RESOLUTIONS[index].width;
        m_windowHeight = AVAILABLE_RESOLUTIONS[index].height;
        
        std::cout << "[SettingsManager] Resolution set to " 
                  << AVAILABLE_RESOLUTIONS[index].name << std::endl;
    }
}

// Set fullscreen mode
void SettingsManager::setFullscreen(bool fullscreen) {
    m_isFullscreen = fullscreen;
    std::cout << "[SettingsManager] Fullscreen set to " 
              << (fullscreen ? "ON" : "OFF") << std::endl;
}

// Save settings to text file
void SettingsManager::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "[SettingsManager] Failed to save settings to " << filename << std::endl;
        return;
    }

    // Write in simple key=value format
    file << "width=" << m_windowWidth << "\n";
    file << "height=" << m_windowHeight << "\n";
    file << "fullscreen=" << (m_isFullscreen ? "1" : "0") << "\n";
    file << "resolutionIndex=" << m_currentResolutionIndex << "\n";

    file.close();
    std::cout << "[SettingsManager] Settings saved to " << filename << std::endl;
}

// Load settings from text file
void SettingsManager::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cout << "[SettingsManager] Settings file not found, using defaults" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Parse key=value format
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        // Parse each setting
        if (key == "width") {
            m_windowWidth = std::stoi(value);
        }
        else if (key == "height") {
            m_windowHeight = std::stoi(value);
        }
        else if (key == "fullscreen") {
            m_isFullscreen = (value == "1");
        }
        else if (key == "resolutionIndex") {
            m_currentResolutionIndex = std::stoi(value);
        }
    }

    file.close();
    std::cout << "[SettingsManager] Settings loaded: " 
              << m_windowWidth << "x" << m_windowHeight 
              << " Fullscreen=" << (m_isFullscreen ? "ON" : "OFF") << std::endl;
}
