#pragma once
#include "States/State.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class SettingsState : public State {
        public:

        // Constructor 
        explicit SettingsState(Game* game);
        ~SettingsState() override = default;

        // Core state functions
        void handleInput(const sf::Event& event) override;
        void update(float deltaTime) override;
        void render(sf::RenderWindow& window) override;

        // Lifecycle hooks
        void onEnter() override;
        void updateMenuDisplay();


    private:
        // Fonts for text
        sf::Font m_font;

        // UI Elements
        std::unique_ptr<sf::Text> m_titleText;
        std::vector<std::unique_ptr<sf::Text>> m_menuTexts;

        // Menu options
        std::vector<std::string> m_menuOptions;

        // Index of the currently selected menu option
        size_t m_selectedIndex;

        // For animation of blinking selection
        float m_blinkTimer;
        bool m_showSelector;
};