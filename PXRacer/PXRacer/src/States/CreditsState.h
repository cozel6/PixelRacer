#pragma once
#include "State.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class Game;

class CreditsState : public State {
public:
    explicit CreditsState(Game* game);
    ~CreditsState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    void onEnter() override;

private:
    sf::Font m_font;
    std::unique_ptr<sf::Text> m_titleText;
    std::vector<std::unique_ptr<sf::Text>> m_creditTexts;
    std::unique_ptr<sf::Text> m_asciiArt;

    void initializeUI();
};