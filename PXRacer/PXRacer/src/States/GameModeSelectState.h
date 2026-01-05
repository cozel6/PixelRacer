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
    struct GameModeCard {
        std::string name;
        std::string description;
        std::string iconSymbol;      // Simple text icon
        sf::Color primaryColor;
        sf::Color accentColor;
        
        std::unique_ptr<sf::Text> titleText;
        std::unique_ptr<sf::Text> descText;
        std::unique_ptr<sf::Text> iconText;
        std::unique_ptr<sf::RectangleShape> cardShape;
        std::unique_ptr<sf::RectangleShape> iconBg;
        
        float targetScale;
        float currentScale;
        float hoverGlow;
        bool isSelected;
    };

    std::vector<GameModeCard> m_cards;
    sf::Font m_font;
    std::unique_ptr<sf::Text> m_headerText;
    std::unique_ptr<sf::Text> m_hintText;
    
    int m_selectedIndex;
    float m_transitionTimer;
    
    // Background animation
    float m_bgAnimTimer;
    
    void initializeCards();
    void updateSelection(int newIndex);
    void selectCurrentMode();
    void updateCardAnimations(float deltaTime);
    void drawBackground(sf::RenderWindow& window);
    void drawCard(sf::RenderWindow& window, GameModeCard& card, size_t index);
};