#pragma once
#include "States/State.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>

class ScreenSettingsState : public State {
public:
    explicit ScreenSettingsState(Game* game);

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    void onEnter() override;

private:
    enum MenuItem {
        RESOLUTION = 0,
        FULLSCREEN,
        APPLY,
        BACK,
        ITEM_COUNT
    };

    void updateMenuDisplay();
    void applySettings();

    // UI elements
    std::unique_ptr<sf::Sprite> m_backgroundSprite;
    std::unique_ptr<sf::Texture> m_backgroundTexture;
    sf::Font m_font;
    std::unique_ptr<sf::Text> m_titleText;
    std::vector<std::unique_ptr<sf::Text>> m_menuTexts;

    // Menu options text
    std::vector<std::string> m_menuOptions; 

    // State
    int m_selectedIndex;
    int m_resolutionIndex;
    bool m_fullscreenEnabled;
    float m_blinkTimer;
    bool m_showSelector;
};
