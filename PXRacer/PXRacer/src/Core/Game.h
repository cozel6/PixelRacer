#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include "Gameplay/GameModeConfig.h" 

// Forward declarations
class StateManager;

class Game {
public:
    Game();
    ~Game();

    // Main game loop
    void run();

    // Getters
    sf::RenderWindow& getWindow() { return m_window; }
    const sf::RenderWindow& getWindow() const { return m_window; }

    // Game control
    void quit() { m_isRunning = false; }

    // Settings management
    void applySettings();

    //StateManager
    StateManager* getStateManager() { return m_stateManager.get(); }

    void setCurrentCampaignTrack(const CampaignTrackData& track) {
        m_currentCampaignTrack = track;
    }
    
    std::optional<CampaignTrackData> getCurrentCampaignTrack() const {
        return m_currentCampaignTrack;
    }

private:
    void processEvents();
    void update(float deltaTime);
    void render();
    void initializeWindow();
    void recreateWindow();

    // Core components
    sf::RenderWindow m_window;
    std::unique_ptr<StateManager> m_stateManager;

    // Timing
    sf::Clock m_clock;
    float m_accumulator;

    // State
    bool m_isRunning;

    std::optional<CampaignTrackData> m_currentCampaignTrack;
};
