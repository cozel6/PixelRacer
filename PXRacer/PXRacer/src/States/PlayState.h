#pragma once
#include "State.h"
#include "Gameplay/GameplayManager.h"
#include "UI/GameHUD.h"  // ✅ NOU!
#include <memory>

class PlayState : public State {
public:
    PlayState(Game* game, GameMode mode, const TrackDefinition* track = nullptr);

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    void onEnter() override;

private:
    std::unique_ptr<GameplayManager> m_gameplayManager;
    std::unique_ptr<GameHUD> m_hud;  // ✅ NOU!
    bool m_isPaused;
};