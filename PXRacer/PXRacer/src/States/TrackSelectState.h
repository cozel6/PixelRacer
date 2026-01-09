#pragma once
#include "State.h"
#include "Gameplay/TrackDefinition.h"
#include "Gameplay/GameplayManager.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class TrackSelectState : public State {
public:
    TrackSelectState(Game* game, GameMode gameMode);

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    // Track card structure
    struct TrackCard {
        const TrackDefinition* track;
        std::unique_ptr<sf::RectangleShape> cardBg;
        std::unique_ptr<sf::RectangleShape> previewBg;
        std::unique_ptr<sf::Text> nameText;
        std::unique_ptr<sf::Text> countryText;
        std::unique_ptr<sf::Text> lengthText;
        std::unique_ptr<sf::Text> difficultyText;
        std::unique_ptr<sf::RectangleShape> difficultyBadge;
        sf::Color difficultyColor;
        bool isSelected;
        float hoverGlow;
    };

    GameMode m_gameMode;
    std::vector<TrackDefinition> m_availableTracks;
    std::vector<TrackCard> m_trackCards;
    int m_selectedIndex;
    int m_scrollOffset;
    
    // UI Elements
    sf::Font m_font;
    std::unique_ptr<sf::Text> m_headerText;
    std::unique_ptr<sf::Text> m_modeText;
    std::unique_ptr<sf::Text> m_hintText;
    
    // Detail panel (right side)
    std::unique_ptr<sf::RectangleShape> m_detailPanel;
    std::unique_ptr<sf::Text> m_detailName;
    std::unique_ptr<sf::Text> m_detailCountry;
    std::unique_ptr<sf::Text> m_detailLength;
    std::unique_ptr<sf::Text> m_detailLaps;
    std::unique_ptr<sf::Text> m_detailDifficulty;
    std::unique_ptr<sf::Text> m_detailDescription;
    std::unique_ptr<sf::RectangleShape> m_trackPreview;
    
    // Buttons
    std::unique_ptr<sf::RectangleShape> m_backButton;
    std::unique_ptr<sf::Text> m_backText;
    std::unique_ptr<sf::RectangleShape> m_startButton;
    std::unique_ptr<sf::Text> m_startText;
    bool m_backHovered;
    bool m_startHovered;
    
    // Animation
    float m_animTimer;
    
    // Methods
    void loadTracksForMode();
    void createUI();
    void createTrackCards();
    void updateSelection(int newIndex);
    void updateDetailPanel();
    void confirmSelection();
    sf::Color getDifficultyColor(TrackDifficulty diff);
    std::string getDifficultyString(TrackDifficulty diff);
    std::string getModeString();
};