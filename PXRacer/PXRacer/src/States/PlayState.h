#pragma once
#include "State.h"
#include "Gameplay/GameplayManager.h"
#include "Gameplay/GameModeConfig.h"
#include "UI/GameHUD.h"
#include <memory>
#include <vector>
#include <optional>

class PlayState : public State {
public:
    PlayState(Game* game, GameMode mode, const TrackDefinition* track = nullptr);

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    void onEnter() override;
    void onExit() override;

private:
    void initPauseMenu();
    void initTaskOverlay();
    void updatePauseMenuDisplay();
    void handlePauseMenuAction();
    void restartGame();
    void renderTaskOverlay(sf::RenderWindow& window);
    
    std::unique_ptr<GameplayManager> m_gameplayManager;
    std::unique_ptr<GameHUD> m_hud;
    
    // Pause state
    bool m_isPaused;
    bool m_showingTasks;
    
    // Pause menu
    sf::Font m_pauseFont;
    std::unique_ptr<sf::Text> m_pauseTitle;
    std::vector<std::unique_ptr<sf::Text>> m_pauseMenuItems;
    int m_pauseSelectedIndex;
    
    // Task overlay UI
    std::unique_ptr<sf::RectangleShape> m_taskOverlayBg;
    std::unique_ptr<sf::Text> m_taskOverlayTitle;
    std::unique_ptr<sf::Text> m_currentTaskText;
    std::unique_ptr<sf::Text> m_taskProgressText;
    std::unique_ptr<sf::RectangleShape> m_taskCheckmark;
    std::unique_ptr<sf::Text> m_taskCheckmarkText;
    std::unique_ptr<sf::Text> m_taskHintText;
    
    // Store for restart
    GameMode m_currentMode;
    std::optional<TrackDefinition> m_savedTrack;
    std::optional<CampaignTrackData> m_campaignTrackData;
    CampaignProgress m_campaignProgress;
};