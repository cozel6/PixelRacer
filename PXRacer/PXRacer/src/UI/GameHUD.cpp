#include "GameHUD.h"
#include "Core/Constants.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cmath>

GameHUD::GameHUD() {
    if (!m_font.openFromFile("assets/fonts/PressStart2P-Regular.ttf")) {
        std::cerr << "[HUD] Failed to load font!" << std::endl;
    }
    
    // Speed
    m_speedText = std::make_unique<sf::Text>(m_font);
    m_speedText->setCharacterSize(28);
    m_speedText->setFillColor(sf::Color::White);
    m_speedText->setPosition(sf::Vector2f(20.0f, 20.0f));
    
    // Score
    m_scoreText = std::make_unique<sf::Text>(m_font);
    m_scoreText->setCharacterSize(18);
    m_scoreText->setFillColor(sf::Color(255, 215, 0));
    m_scoreText->setPosition(sf::Vector2f(Config::WINDOW_WIDTH - 220.0f, 20.0f));
    
    // Distance
    m_distanceText = std::make_unique<sf::Text>(m_font);
    m_distanceText->setCharacterSize(16);
    m_distanceText->setFillColor(sf::Color(100, 200, 255));
    m_distanceText->setPosition(sf::Vector2f(20.0f, Config::WINDOW_HEIGHT - 50.0f));
    
    // Level
    m_levelText = std::make_unique<sf::Text>(m_font);
    m_levelText->setCharacterSize(16);
    m_levelText->setFillColor(sf::Color(255, 100, 100));
    m_levelText->setPosition(sf::Vector2f(Config::WINDOW_WIDTH - 180.0f, Config::WINDOW_HEIGHT - 50.0f));
    
    // Lap time
    m_lapTimeText = std::make_unique<sf::Text>(m_font);
    m_lapTimeText->setCharacterSize(20);
    m_lapTimeText->setFillColor(sf::Color::White);
    m_lapTimeText->setPosition(sf::Vector2f(Config::WINDOW_WIDTH - 250.0f, 60.0f));
    
    // Best lap
    m_bestLapText = std::make_unique<sf::Text>(m_font);
    m_bestLapText->setCharacterSize(14);
    m_bestLapText->setFillColor(sf::Color(150, 255, 150));
    m_bestLapText->setPosition(sf::Vector2f(Config::WINDOW_WIDTH - 250.0f, 90.0f));
    
    // Lap count
    m_lapCountText = std::make_unique<sf::Text>(m_font);
    m_lapCountText->setCharacterSize(16);
    m_lapCountText->setFillColor(sf::Color(255, 200, 100));
    m_lapCountText->setPosition(sf::Vector2f(Config::WINDOW_WIDTH / 2.0f - 50.0f, 20.0f));
    
    // Damage text
    m_damageText = std::make_unique<sf::Text>(m_font);
    m_damageText->setCharacterSize(14);
    m_damageText->setFillColor(sf::Color(255, 100, 100));
    m_damageText->setPosition(sf::Vector2f(20.0f, 80.0f));
    
    // Speed bar
    m_speedBar = std::make_unique<sf::RectangleShape>(sf::Vector2f(180.0f, 15.0f));
    m_speedBar->setPosition(sf::Vector2f(20.0f, 55.0f));
    m_speedBar->setFillColor(sf::Color(0, 255, 0, 180));
    
    // HUD background
    m_hudBackground = std::make_unique<sf::RectangleShape>(sf::Vector2f(220.0f, 100.0f));
    m_hudBackground->setPosition(sf::Vector2f(10.0f, 10.0f));
    m_hudBackground->setFillColor(sf::Color(0, 0, 0, 150));
}

void GameHUD::update(const GameplayManager& gameplay, float deltaTime) {
    updateSpeedometer(gameplay.getPlayerSpeedKmh());
    updateScore(gameplay.getStats().currentScore);
    updateDistance(gameplay.getDistanceKm());
    updateLevel(gameplay.getCurrentLevel());
    updateLapTime(gameplay.getCurrentLapTime(), gameplay.getBestLapTime(), gameplay.getLapCount());
    
    // Update damage display
    const Player& player = gameplay.getPlayer();
    std::ostringstream dmgOss;
    dmgOss << "DMG: " << static_cast<int>(player.getTotalDamage()) << "/100";
    m_damageText->setString(dmgOss.str());
    
    // Culoare bazată pe damage
    float damagePercent = player.getTotalDamage() / 100.0f;
    if (damagePercent >= 0.75f) {
        m_damageText->setFillColor(sf::Color(255, 50, 50));  // Roșu
    } else if (damagePercent >= 0.5f) {
        m_damageText->setFillColor(sf::Color(255, 150, 50)); // Portocaliu
    } else if (damagePercent >= 0.25f) {
        m_damageText->setFillColor(sf::Color(255, 255, 50)); // Galben
    } else {
        m_damageText->setFillColor(sf::Color(100, 255, 100)); // Verde
    }
    
    // Boost feedback timer
    const auto& boost = gameplay.getBoostResult();
    
    if (boost.attempted && !m_boostFeedbackShown) {
        m_boostFeedbackTimer = 2.0f;
        m_boostFeedbackShown = true;
    }
    
    if (m_boostFeedbackTimer > 0.0f) {
        m_boostFeedbackTimer -= deltaTime;
    }
}

void GameHUD::updateSpeedometer(float speedKmh) {
    std::ostringstream oss;
    oss << static_cast<int>(speedKmh) << " KM/H";
    m_speedText->setString(oss.str());
    
    if (speedKmh > 280.0f) {
        m_speedText->setFillColor(sf::Color::Red);
    } else if (speedKmh > 200.0f) {
        m_speedText->setFillColor(sf::Color::Yellow);
    } else {
        m_speedText->setFillColor(sf::Color::White);
    }
    
    float speedPercent = std::clamp(speedKmh / 330.0f, 0.0f, 1.0f);
    m_speedBar->setSize(sf::Vector2f(180.0f * speedPercent, 15.0f));
    
    if (speedPercent > 0.85f) {
        m_speedBar->setFillColor(sf::Color(255, 50, 50, 200));
    } else if (speedPercent > 0.6f) {
        m_speedBar->setFillColor(sf::Color(255, 200, 50, 200));
    } else {
        m_speedBar->setFillColor(sf::Color(50, 255, 50, 200));
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
    oss << "LVL " << level;
    m_levelText->setString(oss.str());
}

void GameHUD::updateLapTime(float currentLap, float bestLap, int lapCount) {
    int minutes = static_cast<int>(currentLap) / 60;
    int seconds = static_cast<int>(currentLap) % 60;
    int millis = static_cast<int>((currentLap - static_cast<int>(currentLap)) * 1000);
    
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(1) << minutes << ":"
        << std::setw(2) << seconds << "."
        << std::setw(3) << millis;
    m_lapTimeText->setString(oss.str());
    
    if (bestLap > 0.0f) {
        int bMin = static_cast<int>(bestLap) / 60;
        int bSec = static_cast<int>(bestLap) % 60;
        int bMil = static_cast<int>((bestLap - static_cast<int>(bestLap)) * 1000);
        
        std::ostringstream bss;
        bss << "BEST: " << bMin << ":" << std::setfill('0') << std::setw(2) << bSec 
            << "." << std::setw(3) << bMil;
        m_bestLapText->setString(bss.str());
    } else {
        m_bestLapText->setString("BEST: --:--:---");
    }
    
    std::ostringstream lss;
    lss << "LAP " << (lapCount + 1);
    m_lapCountText->setString(lss.str());
}

void GameHUD::renderTrafficLight(sf::RenderWindow& window, float x, float y, bool isOn, bool isGreen) {
    sf::RectangleShape housing(sf::Vector2f(50.0f, 60.0f));
    housing.setOrigin(sf::Vector2f(25.0f, 30.0f));
    housing.setPosition(sf::Vector2f(x, y));
    housing.setFillColor(sf::Color(30, 30, 30));
    housing.setOutlineColor(sf::Color(60, 60, 60));
    housing.setOutlineThickness(3.0f);
    window.draw(housing);
    
    sf::CircleShape bulb(18.0f);
    bulb.setOrigin(sf::Vector2f(18.0f, 18.0f));
    bulb.setPosition(sf::Vector2f(x, y));
    
    if (isOn) {
        if (isGreen) {
            bulb.setFillColor(sf::Color(0, 255, 0));
            sf::CircleShape glow(25.0f);
            glow.setOrigin(sf::Vector2f(25.0f, 25.0f));
            glow.setPosition(sf::Vector2f(x, y));
            glow.setFillColor(sf::Color(0, 255, 0, 80));
            window.draw(glow);
        } else {
            bulb.setFillColor(sf::Color(255, 0, 0));
            sf::CircleShape glow(25.0f);
            glow.setOrigin(sf::Vector2f(25.0f, 25.0f));
            glow.setPosition(sf::Vector2f(x, y));
            glow.setFillColor(sf::Color(255, 0, 0, 80));
            window.draw(glow);
        }
    } else {
        bulb.setFillColor(sf::Color(50, 50, 50));
    }
    
    window.draw(bulb);
}

void GameHUD::renderCountdown(sf::RenderWindow& window, const GameplayManager& gameplay) {
    CountdownPhase phase = gameplay.getCountdownPhase();
    
    if (phase == CountdownPhase::Finished) {
        return;
    }
    
    float centerX = static_cast<float>(Config::WINDOW_WIDTH) / 2.0f;
    float centerY = static_cast<float>(Config::WINDOW_HEIGHT) * 0.25f;
    
    sf::RectangleShape panel(sf::Vector2f(400.0f, 120.0f));
    panel.setOrigin(sf::Vector2f(200.0f, 60.0f));
    panel.setPosition(sf::Vector2f(centerX, centerY));
    panel.setFillColor(sf::Color(20, 20, 20, 230));
    panel.setOutlineColor(sf::Color(80, 80, 80));
    panel.setOutlineThickness(3.0f);
    window.draw(panel);
    
    float lightSpacing = 70.0f;
    float startX = centerX - 2 * lightSpacing;
    
    bool isGo = (phase == CountdownPhase::Go);
    
    for (int i = 0; i < 5; ++i) {
        bool isOn = false;
        
        if (isGo) {
            isOn = true;
        } else {
            int lightIndex = static_cast<int>(phase) - static_cast<int>(CountdownPhase::Light1);
            isOn = (i <= lightIndex && lightIndex >= 0);
        }
        
        renderTrafficLight(window, startX + i * lightSpacing, centerY, isOn, isGo);
    }
    
    sf::Text statusText(m_font);
    statusText.setCharacterSize(24);
    
    if (phase == CountdownPhase::Ready) {
        statusText.setString("GET READY...");
        statusText.setFillColor(sf::Color::White);
    } else if (phase == CountdownPhase::Go) {
        statusText.setString("GO! GO! GO!");
        statusText.setFillColor(sf::Color::Green);
    } else {
        statusText.setString("HOLD...");
        statusText.setFillColor(sf::Color::Red);
    }
    
    auto textBounds = statusText.getLocalBounds();
    statusText.setOrigin(sf::Vector2f(textBounds.size.x / 2.0f, textBounds.size.y / 2.0f));
    statusText.setPosition(sf::Vector2f(centerX, centerY + 70.0f));
    window.draw(statusText);
    
    if (phase != CountdownPhase::Go && phase != CountdownPhase::Finished) {
        sf::Text hintText(m_font);
        hintText.setString("Press SHIFT at green for boost!");
        hintText.setCharacterSize(12);
        hintText.setFillColor(sf::Color(150, 150, 150));
        auto hintBounds = hintText.getLocalBounds();
        hintText.setOrigin(sf::Vector2f(hintBounds.size.x / 2.0f, 0.0f));
        hintText.setPosition(sf::Vector2f(centerX, centerY + 100.0f));
        window.draw(hintText);
    }
}

void GameHUD::renderBoostFeedback(sf::RenderWindow& window, const GameplayManager& gameplay) {
    if (m_boostFeedbackTimer <= 0.0f) return;
    
    const auto& boost = gameplay.getBoostResult();
    if (!boost.attempted) return;
    
    float centerX = static_cast<float>(Config::WINDOW_WIDTH) / 2.0f;
    float centerY = static_cast<float>(Config::WINDOW_HEIGHT) * 0.4f;
    
    sf::Text feedbackText(m_font);
    feedbackText.setCharacterSize(36);
    
    if (boost.perfect) {
        feedbackText.setString("PERFECT START!");
        feedbackText.setFillColor(sf::Color(0, 255, 100));
    } else if (boost.good) {
        feedbackText.setString("GOOD START!");
        feedbackText.setFillColor(sf::Color(255, 255, 0));
    } else if (boost.jumpStart) {
        feedbackText.setString("JUMP START!");
        feedbackText.setFillColor(sf::Color(255, 0, 0));
    } else {
        return;
    }
    
    float alpha = std::min(1.0f, m_boostFeedbackTimer / 0.5f);
    sf::Color color = feedbackText.getFillColor();
    color.a = static_cast<std::uint8_t>(alpha * 255);
    feedbackText.setFillColor(color);
    
    auto bounds = feedbackText.getLocalBounds();
    feedbackText.setOrigin(sf::Vector2f(bounds.size.x / 2.0f, bounds.size.y / 2.0f));
    feedbackText.setPosition(sf::Vector2f(centerX, centerY));
    
    float scale = 1.0f + 0.1f * std::sin(m_boostFeedbackTimer * 10.0f);
    feedbackText.setScale(sf::Vector2f(scale, scale));
    
    window.draw(feedbackText);
}

void GameHUD::render(sf::RenderWindow& window, const GameplayManager& gameplay) {
    // ✅ GAME OVER SCREEN - renderăm PRIMUL pentru a fi deasupra
    if (gameplay.isGameOver()) {
        // Overlay negru semi-transparent
        sf::RectangleShape overlay(sf::Vector2f(
            static_cast<float>(window.getSize().x),
            static_cast<float>(window.getSize().y)
        ));
        overlay.setFillColor(sf::Color(0, 0, 0, 200));
        window.draw(overlay);
        
        // Text GAME OVER
        sf::Text gameOverText(m_font);
        gameOverText.setString("GAME OVER");
        gameOverText.setCharacterSize(64);
        gameOverText.setFillColor(sf::Color::Red);
        
        auto bounds = gameOverText.getLocalBounds();
        gameOverText.setOrigin(sf::Vector2f(bounds.size.x / 2.0f, bounds.size.y / 2.0f));
        gameOverText.setPosition(sf::Vector2f(
            static_cast<float>(Config::WINDOW_WIDTH) / 2.0f,
            static_cast<float>(Config::WINDOW_HEIGHT) / 2.0f - 50.0f
        ));
        window.draw(gameOverText);
        
        // Subtitlu
        sf::Text subtitleText(m_font);
        subtitleText.setString("CAR DESTROYED!");
        subtitleText.setCharacterSize(24);
        subtitleText.setFillColor(sf::Color(255, 150, 150));
        
        auto subBounds = subtitleText.getLocalBounds();
        subtitleText.setOrigin(sf::Vector2f(subBounds.size.x / 2.0f, subBounds.size.y / 2.0f));
        subtitleText.setPosition(sf::Vector2f(
            static_cast<float>(Config::WINDOW_WIDTH) / 2.0f,
            static_cast<float>(Config::WINDOW_HEIGHT) / 2.0f + 30.0f
        ));
        window.draw(subtitleText);
        
        // Instrucțiuni
        sf::Text hintText(m_font);
        hintText.setString("Press ESC to exit");
        hintText.setCharacterSize(16);
        hintText.setFillColor(sf::Color(150, 150, 150));
        
        auto hintBounds = hintText.getLocalBounds();
        hintText.setOrigin(sf::Vector2f(hintBounds.size.x / 2.0f, hintBounds.size.y / 2.0f));
        hintText.setPosition(sf::Vector2f(
            static_cast<float>(Config::WINDOW_WIDTH) / 2.0f,
            static_cast<float>(Config::WINDOW_HEIGHT) / 2.0f + 100.0f
        ));
        window.draw(hintText);
        
        return;  // Nu mai desemăm restul HUD-ului
    }
    
    // Render countdown if active
    if (gameplay.isCountingDown() || gameplay.getCountdownPhase() == CountdownPhase::Go) {
        renderCountdown(window, gameplay);
    }
    
    // Render boost feedback
    renderBoostFeedback(window, gameplay);
    
    // Only show regular HUD after race starts
    if (!gameplay.isRaceStarted()) {
        return;
    }
    
    // HUD background
    window.draw(*m_hudBackground);
    
    // Speed
    window.draw(*m_speedText);
    window.draw(*m_speedBar);
    
    // Damage
    window.draw(*m_damageText);
    
    // Score
    window.draw(*m_scoreText);
    
    // Distance
    window.draw(*m_distanceText);
    
    // Level
    window.draw(*m_levelText);
    
    // Lap info
    window.draw(*m_lapTimeText);
    window.draw(*m_bestLapText);
    window.draw(*m_lapCountText);
}