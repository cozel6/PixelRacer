#include "GameHUD.h"
#include "Gameplay/GameplayManager.h"
#include "Core/Constants.h"
#include <sstream>
#include <iomanip>
#include <iostream>  // ✅ ADĂUGAT pentru std::cerr

GameHUD::GameHUD() {
    // Load font
    if (!m_font.openFromFile("assets/fonts/PressStart2P-Regular.ttf")) {
        std::cerr << "Failed to load HUD font!" << std::endl;
    }
    
    // ✅ SPEEDOMETER (stânga sus)
    m_speedText = std::make_unique<sf::Text>(m_font);
    m_speedText->setCharacterSize(32);
    m_speedText->setFillColor(sf::Color::White);
    m_speedText->setPosition(sf::Vector2f(20.0f, 20.0f));
    m_speedText->setString("0 KM/H");
    
    // ✅ SCORE (dreapta sus)
    m_scoreText = std::make_unique<sf::Text>(m_font);
    m_scoreText->setCharacterSize(24);
    m_scoreText->setFillColor(sf::Color(255, 215, 0)); // Gold
    m_scoreText->setPosition(sf::Vector2f(Config::WINDOW_WIDTH - 250.0f, 20.0f));
    m_scoreText->setString("SCORE: 0");
    
    // ✅ DISTANCE (stânga jos)
    m_distanceText = std::make_unique<sf::Text>(m_font);
    m_distanceText->setCharacterSize(20);
    m_distanceText->setFillColor(sf::Color(100, 200, 255)); // Light blue
    m_distanceText->setPosition(sf::Vector2f(20.0f, Config::WINDOW_HEIGHT - 60.0f));
    m_distanceText->setString("0.00 KM");
    
    // ✅ LEVEL (dreapta jos)
    m_levelText = std::make_unique<sf::Text>(m_font);
    m_levelText->setCharacterSize(20);
    m_levelText->setFillColor(sf::Color(255, 100, 100)); // Light red
    m_levelText->setPosition(sf::Vector2f(Config::WINDOW_WIDTH - 200.0f, Config::WINDOW_HEIGHT - 60.0f));
    m_levelText->setString("LEVEL 1");
    
    // ✅ CHECKPOINT MESSAGE (centru ecran)
    m_checkpointText = std::make_unique<sf::Text>(m_font);
    m_checkpointText->setCharacterSize(40);
    m_checkpointText->setFillColor(sf::Color::Green);
    m_checkpointText->setPosition(sf::Vector2f(Config::WINDOW_WIDTH * 0.5f - 200.0f, Config::WINDOW_HEIGHT * 0.3f));
    m_checkpointText->setString("CHECKPOINT!");
    
    // ✅ SPEED BAR (visual speedometer)
    m_speedBar = std::make_unique<sf::RectangleShape>(sf::Vector2f(200.0f, 20.0f));
    m_speedBar->setPosition(sf::Vector2f(20.0f, 70.0f));
    m_speedBar->setFillColor(sf::Color(0, 255, 0, 180));
    
    // ✅ HUD BACKGROUND (semi-transparent)
    m_hudBackground = std::make_unique<sf::RectangleShape>(sf::Vector2f(300.0f, 100.0f));
    m_hudBackground->setPosition(sf::Vector2f(10.0f, 10.0f));
    m_hudBackground->setFillColor(sf::Color(0, 0, 0, 128));
}

void GameHUD::update(const GameplayManager& gameplay) {
    updateSpeedometer(gameplay.getPlayerSpeedKmh());
    updateScore(gameplay.getStats().currentScore);
    updateDistance(gameplay.getDistanceKm());
    updateLevel(gameplay.getCurrentLevel());
    
    // ✅ Checkpoint message timer
    if (m_showCheckpoint) {
        m_checkpointTimer -= 0.016f; // ~60 FPS
        if (m_checkpointTimer <= 0.0f) {
            m_showCheckpoint = false;
        }
    }
}

void GameHUD::updateSpeedometer(float speedKmh) {
    std::ostringstream oss;
    oss << static_cast<int>(speedKmh) << " KM/H";
    m_speedText->setString(oss.str());
    
    // ✅ Color coding pentru viteza
    if (speedKmh > 250.0f) {
        m_speedText->setFillColor(sf::Color::Red);      // Foarte rapid!
    } else if (speedKmh > 180.0f) {
        m_speedText->setFillColor(sf::Color::Yellow);   // Rapid
    } else {
        m_speedText->setFillColor(sf::Color::White);    // Normal
    }
    
    // ✅ Speed bar (0-330 km/h scale)
    float speedPercent = std::clamp(speedKmh / 330.0f, 0.0f, 1.0f);
    m_speedBar->setSize(sf::Vector2f(200.0f * speedPercent, 20.0f));
    
    // Bar color gradient
    if (speedPercent > 0.8f) {
        m_speedBar->setFillColor(sf::Color(255, 0, 0, 180)); // Red
    } else if (speedPercent > 0.5f) {
        m_speedBar->setFillColor(sf::Color(255, 255, 0, 180)); // Yellow
    } else {
        m_speedBar->setFillColor(sf::Color(0, 255, 0, 180)); // Green
    }
}

void GameHUD::updateScore(float score) {
    std::ostringstream oss;
    oss << "SCORE: " << static_cast<int>(score);
    m_scoreText->setString(oss.str());
}

void GameHUD::updateDistance(float distanceKm) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << distanceKm << " KM";
    m_distanceText->setString(oss.str());
}

void GameHUD::updateLevel(int level) {
    std::ostringstream oss;
    oss << "LEVEL " << level;
    m_levelText->setString(oss.str());
}

void GameHUD::render(sf::RenderWindow& window) {
    // Background
    window.draw(*m_hudBackground);
    
    // Speed
    window.draw(*m_speedText);
    window.draw(*m_speedBar);
    
    // Score
    window.draw(*m_scoreText);
    
    // Distance
    window.draw(*m_distanceText);
    
    // Level
    window.draw(*m_levelText);
    
    // Checkpoint message (if active)
    if (m_showCheckpoint) {
        window.draw(*m_checkpointText);
    }
}