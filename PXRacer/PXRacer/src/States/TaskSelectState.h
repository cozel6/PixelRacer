#pragma once
#include "State.h"
#include "Gameplay/GameModeConfig.h"
#include "Gameplay/TrackDefinition.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <array>
#include <optional>

class TaskSelectState : public State {
public:
    TaskSelectState(Game* game, CampaignTrackData trackData, const TrackDefinition* track);
    
    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    void createUI();
    void updateTaskDisplay();
    void toggleTaskSelection(int index);
    void confirmSelection();
    bool hasSelectedTask() const;
    int getSelectedTaskIndex() const;
    sf::Color getTaskColor(int index) const;
    
    CampaignTrackData m_trackData;
    std::optional<TrackDefinition> m_savedTrack;  // COPIE a track-ului, nu pointer!
    int m_selectedTaskIndex;
    
    // UI Elements
    sf::Font m_font;
    std::unique_ptr<sf::Text> m_headerText;
    std::unique_ptr<sf::Text> m_trackNameText;
    std::unique_ptr<sf::Text> m_difficultyText;
    std::unique_ptr<sf::Text> m_requirementText;
    std::unique_ptr<sf::Text> m_hintText;
    std::unique_ptr<sf::Text> m_selectedInfoText;
    
    // Task cards
    struct TaskCard {
        std::unique_ptr<sf::RectangleShape> background;
        std::unique_ptr<sf::Text> numberText;
        std::unique_ptr<sf::Text> descriptionText;
        std::unique_ptr<sf::Text> statusText;
        std::unique_ptr<sf::RectangleShape> checkmark;
        std::unique_ptr<sf::Text> checkmarkText;
        bool isCompleted = false;
        bool isSelected = false;
    };
    std::array<TaskCard, 3> m_taskCards;
    
    // Buttons
    std::unique_ptr<sf::RectangleShape> m_backButton;
    std::unique_ptr<sf::Text> m_backText;
    std::unique_ptr<sf::RectangleShape> m_startButton;
    std::unique_ptr<sf::Text> m_startText;
    bool m_backHovered;
    bool m_startHovered;
    
    // Animation
    float m_animTimer;
};
