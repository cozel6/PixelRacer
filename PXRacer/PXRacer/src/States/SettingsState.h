#pragma once
#include "State.h"
#include <SFML/Graphics.hpp>
#include <memory>

class Game;

class SettingsState : public State {
public:
    explicit SettingsState(Game* game);
    ~SettingsState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    void onEnter() override;

private:
    sf::Font m_font;
    std::unique_ptr<sf::Text> m_titleText;
    std::unique_ptr<sf::Text> m_instructionText;

    void initializeUI();
};