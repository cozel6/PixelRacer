#pragma once
#include "State.h"
#include "Gameplay/TrackDefinition.h"
#include "Gameplay/GameplayManager.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>  // ✅ Pentru std::unique_ptr

class TrackSelectState : public State {
public:
    TrackSelectState(Game* game, GameMode gameMode);

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    GameMode m_gameMode;
    std::vector<TrackDefinition> m_availableTracks;
    int m_selectedIndex;

    // UI Elements
    sf::Font m_font;
    std::unique_ptr<sf::Text> m_titleText;      // ✅ Folosim unique_ptr
    std::unique_ptr<sf::Text> m_trackInfoText;  // ✅ Folosim unique_ptr
    sf::RectangleShape m_infoPanel;

    struct SimpleButton {
        sf::RectangleShape shape;
        std::string label;
        bool isHovered;

        SimpleButton() : isHovered(false) {}
    };

    std::vector<SimpleButton> m_trackButtons;
    SimpleButton m_backButton;
    SimpleButton m_confirmButton;

    void loadTracksForMode();
    void createUI();
    void selectTrack(int index);
    void confirmSelection();
    void displayTrackInfo(const TrackDefinition& track);
    void updateButtonStates(const sf::Vector2f& mousePos);
};