#pragma once
#include "State.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class Game;

class GameModeSelectState : public State {
public:
    explicit GameModeSelectState(Game* game);
    ~GameModeSelectState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

    void onEnter() override;

private:
    struct GameModeSection {
        std::string name;
        std::string description;
        std::unique_ptr<sf::Text> titleText;
        std::unique_ptr<sf::Text> descText;
        float sectionX;
        float sectionY;
        float sectionWidth;
        float sectionHeight;
        bool isHovered;
    };

    std::vector<GameModeSection> m_gameModes;
    sf::Font m_font;
    std::unique_ptr<sf::Text> m_titleText;

    void initializeGameModes();
    void updateHover(const sf::Vector2f& mousePos);
    void selectGameMode(int index);
};