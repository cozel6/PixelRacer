#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

// Forward declaration
struct EndlessStats;
class GameplayManager;

class GameHUD {
public:
    GameHUD();
    
    void update(const GameplayManager& gameplay);
    void render(sf::RenderWindow& window);

private:
    void updateSpeedometer(float speedKmh);
    void updateScore(float score);
    void updateDistance(float distanceKm);
    void updateLevel(int level);
    
    // Font
    sf::Font m_font;
    
    // Text elements
    std::unique_ptr<sf::Text> m_speedText;
    std::unique_ptr<sf::Text> m_scoreText;
    std::unique_ptr<sf::Text> m_distanceText;
    std::unique_ptr<sf::Text> m_levelText;
    std::unique_ptr<sf::Text> m_checkpointText;
    
    // Visual elements
    std::unique_ptr<sf::RectangleShape> m_hudBackground;
    std::unique_ptr<sf::RectangleShape> m_speedBar;
    
    // Animation timers
    float m_checkpointTimer = 0.0f;
    bool m_showCheckpoint = false;
};