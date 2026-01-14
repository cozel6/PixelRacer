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
    void updateLapInfo(float currentLap, float bestLap, float lastLap, int lapCount);
    void updateDamage(float damage);
    
    void renderCountdown(sf::RenderWindow& window, const GameplayManager& gameplay);
    void renderTrafficLight(sf::RenderWindow& window, float x, float y, bool isOn, bool isGreen);
    void renderBoostFeedback(sf::RenderWindow& window, const GameplayManager& gameplay);
    void renderGameOver(sf::RenderWindow& window, const GameplayManager& gameplay);
    void renderRaceFinished(sf::RenderWindow& window, const GameplayManager& gameplay);
    
    // Helper functions
    std::string formatTime(float seconds);
    void centerText(sf::Text& text, float x, float y);
    void rightAlignText(sf::Text& text, float rightX, float y);
    
    sf::Font m_font;
    
    // Left panel - Speed and Damage
    std::unique_ptr<sf::RectangleShape> m_leftPanel;
    std::unique_ptr<sf::Text> m_speedText;
    std::unique_ptr<sf::Text> m_speedUnitText;
    std::unique_ptr<sf::RectangleShape> m_speedBarBg;
    std::unique_ptr<sf::RectangleShape> m_speedBar;
    std::unique_ptr<sf::Text> m_damageLabel;
    std::unique_ptr<sf::RectangleShape> m_damageBarBg;
    std::unique_ptr<sf::RectangleShape> m_damageBar;
    
    // Right panel - Score and Lap Times
    std::unique_ptr<sf::RectangleShape> m_rightPanel;
    std::unique_ptr<sf::Text> m_scoreLabel;
    std::unique_ptr<sf::Text> m_scoreText;
    std::unique_ptr<sf::Text> m_lapTimeLabel;
    std::unique_ptr<sf::Text> m_lapTimeText;
    std::unique_ptr<sf::Text> m_bestLapLabel;
    std::unique_ptr<sf::Text> m_bestLapText;
    std::unique_ptr<sf::Text> m_lastLapLabel;
    std::unique_ptr<sf::Text> m_lastLapText;
    
    // Center - Lap Counter
    std::unique_ptr<sf::RectangleShape> m_lapPanel;
    std::unique_ptr<sf::Text> m_lapCountText;
    
    // Animation timers and state
    float m_boostFeedbackTimer = 0.0f;
    bool m_boostFeedbackShown = false;
    float m_newLapTimer = 0.0f;
    float m_lastDisplayedLapTime = 0.0f;
};
