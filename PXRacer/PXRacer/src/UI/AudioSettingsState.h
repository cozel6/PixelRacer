#pragma once
#include "States/State.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>

class AudioSettingsState : public State {
public:
    explicit AudioSettingsState(Game* game);

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    void onEnter() override;

private:
    enum MenuItem {
        MUSIC_VOLUME = 0,
        SFX_VOLUME,
        APPLY,
        BACK,
        ITEM_COUNT
    };

    void updateMenuDisplay();
    void applySettings();

    // UI elements (same pattern as ScreenSettingsState)
    std::unique_ptr<sf::Sprite> m_backgroundSprite;
    std::unique_ptr<sf::Texture> m_backgroundTexture;
    sf::Font m_font;
    std::unique_ptr<sf::Text> m_titleText;
    std::vector<std::unique_ptr<sf::Text>> m_menuTexts;

    // Menu options text
    std::vector<std::string> m_menuOptions;

    // State
    int m_selectedIndex;
    float m_musicVolume;  // Local copy (0-100)
    float m_sfxVolume;    // Local copy (0-100)
    float m_blinkTimer;
    bool m_showSelector;

    // Volume adjustment increment
    static constexpr float VOLUME_INCREMENT = 5.0f;
};
