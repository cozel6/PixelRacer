#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <array>
#include "Gameplay/GameplayManager.h"

class GameHUD {
public:
    GameHUD();
    
    void update(const GameplayManager& gameplay, float deltaTime);
    void render(sf::RenderWindow& window, const GameplayManager& gameplay);

private:
    void updateSpeedometer(float speedKmh);
    void updateScore(float score);
    void updateDistance(float distanceKm);
    void updateLevel(int level);
    void updateLapTime(float currentLap, float bestLap, int lapCount);
    
    void renderCountdown(sf::RenderWindow& window, const GameplayManager& gameplay);
    void renderTrafficLight(sf::RenderWindow& window, float x, float y, bool isOn, bool isGreen);
    void renderBoostFeedback(sf::RenderWindow& window, const GameplayManager& gameplay);
    void renderObstacleWarnings(sf::RenderWindow& window, const GameplayManager& gameplay);
    
    // Font
    sf::Font m_font;
    
    // Text elements
    std::unique_ptr<sf::Text> m_speedText;
    std::unique_ptr<sf::Text> m_scoreText;
    std::unique_ptr<sf::Text> m_distanceText;
    std::unique_ptr<sf::Text> m_levelText;
    std::unique_ptr<sf::Text> m_checkpointText;
    std::unique_ptr<sf::Text> m_lapTimeText;
    std::unique_ptr<sf::Text> m_bestLapText;
    std::unique_ptr<sf::Text> m_lapCountText;
    std::unique_ptr<sf::Text> m_damageText;
    std::unique_ptr<sf::Text> m_flatTireText;
    
    // Visual elements
    std::unique_ptr<sf::RectangleShape> m_hudBackground;
    std::unique_ptr<sf::RectangleShape> m_speedBar;
    
    // Animation timers
    float m_checkpointTimer = 0.0f;
    bool m_showCheckpoint = false;
    float m_boostFeedbackTimer = 0.0f;
    bool m_boostFeedbackShown = false;
};