#include "PlayState.h"
#include "Core/Game.h"
#include "States/StateManager.h"
#include <iostream>

PlayState::PlayState(Game* game, GameMode mode, const TrackDefinition* track)
    : State(game)
    , m_gameplayManager(std::make_unique<GameplayManager>(mode, track))
    , m_hud(std::make_unique<GameHUD>())  // ✅ NOU!
    , m_isPaused(false)
{
    std::cout << "PlayState initialized" << std::endl;
}

void PlayState::handleInput(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        const auto* key = event.getIf<sf::Event::KeyPressed>();
        if (key->code == sf::Keyboard::Key::Escape) {
            m_game->getStateManager()->popState();
        }
    }
}

void PlayState::update(float deltaTime) {
    if (!m_isPaused) {
        m_gameplayManager->update(deltaTime);
        m_hud->update(*m_gameplayManager);  // ✅ NOU!
    }
}

void PlayState::render(sf::RenderWindow& window) {
    m_gameplayManager->render(window);
    m_hud->render(window);  // ✅ NOU - desenăm HUD peste tot!
}

void PlayState::onEnter() {
    std::cout << "Entered Play State" << std::endl;
}