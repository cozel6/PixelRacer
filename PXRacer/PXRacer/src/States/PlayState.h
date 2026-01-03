#pragma once
#include "State.h"
#include "Gameplay/GameplayManager.h"
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

private:
    void initPauseMenu();
    void updatePauseMenuDisplay();
    void handlePauseMenuAction();
    void restartGame();
    
    std::unique_ptr<GameplayManager> m_gameplayManager;
    std::unique_ptr<GameHUD> m_hud;
    
    // Pause state
    bool m_isPaused;
    
    // Pause menu
    sf::Font m_pauseFont;
    std::unique_ptr<sf::Text> m_pauseTitle;
    std::vector<std::unique_ptr<sf::Text>> m_pauseMenuItems;
    int m_pauseSelectedIndex;
    
    // Store for restart - ✅ COPY track, don't store pointer!
    GameMode m_currentMode;
    std::optional<TrackDefinition> m_savedTrack;  // ✅ COPY of track!
};