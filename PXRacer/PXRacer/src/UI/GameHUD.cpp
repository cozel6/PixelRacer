#include "GameHUD.h"
#include "Core/Constants.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cmath>

namespace HUDConfig {
    // Layout
    constexpr float PANEL_PADDING = 15.0f;
    constexpr float PANEL_MARGIN = 10.0f;
    constexpr float LEFT_PANEL_WIDTH = 200.0f;
    constexpr float LEFT_PANEL_HEIGHT = 120.0f;
    constexpr float RIGHT_PANEL_WIDTH = 220.0f;
    constexpr float RIGHT_PANEL_HEIGHT = 140.0f;
    constexpr float LAP_PANEL_WIDTH = 120.0f;
    constexpr float LAP_PANEL_HEIGHT = 45.0f;
    constexpr float BAR_HEIGHT = 12.0f;
    constexpr float BAR_WIDTH = 170.0f;
    
    // Colors
    const sf::Color PANEL_BG = sf::Color(0, 0, 0, 180);
    const sf::Color PANEL_BORDER = sf::Color(80, 80, 80);
    const sf::Color TEXT_WHITE = sf::Color::White;
    const sf::Color TEXT_GOLD = sf::Color(255, 215, 0);
    const sf::Color TEXT_GREEN = sf::Color(100, 255, 100);
    const sf::Color TEXT_GRAY = sf::Color(180, 180, 180);
    const sf::Color BAR_BG = sf::Color(40, 40, 40);
}

GameHUD::GameHUD() {
    if (!m_font.openFromFile("assets/fonts/PressStart2P-Regular.ttf")) {
        std::cerr << "[HUD] Failed to load font!" << std::endl;
    }
    
    float winW = static_cast<float>(Config::WINDOW_WIDTH);
    float winH = static_cast<float>(Config::WINDOW_HEIGHT);
    
    // ═══════════════════════════════════════════════════════════════
    // LEFT PANEL - Speed & Damage
    // ═══════════════════════════════════════════════════════════════
    m_leftPanel = std::make_unique<sf::RectangleShape>(
        sf::Vector2f(HUDConfig::LEFT_PANEL_WIDTH, HUDConfig::LEFT_PANEL_HEIGHT));
    m_leftPanel->setPosition(sf::Vector2f(HUDConfig::PANEL_MARGIN, HUDConfig::PANEL_MARGIN));
    m_leftPanel->setFillColor(HUDConfig::PANEL_BG);
    m_leftPanel->setOutlineColor(HUDConfig::PANEL_BORDER);
    m_leftPanel->setOutlineThickness(2.0f);
    
    float leftX = HUDConfig::PANEL_MARGIN + HUDConfig::PANEL_PADDING;
    float leftY = HUDConfig::PANEL_MARGIN + HUDConfig::PANEL_PADDING;
    
    // Speed number (mare)
    m_speedText = std::make_unique<sf::Text>(m_font);
    m_speedText->setCharacterSize(32);
    m_speedText->setFillColor(HUDConfig::TEXT_WHITE);
    m_speedText->setPosition(sf::Vector2f(leftX, leftY));
    m_speedText->setString("0");
    
    // Speed unit
    m_speedUnitText = std::make_unique<sf::Text>(m_font);
    m_speedUnitText->setCharacterSize(12);
    m_speedUnitText->setFillColor(HUDConfig::TEXT_GRAY);
    m_speedUnitText->setString("KM/H");
    
    // Speed bar background
    m_speedBarBg = std::make_unique<sf::RectangleShape>(
        sf::Vector2f(HUDConfig::BAR_WIDTH, HUDConfig::BAR_HEIGHT));
    m_speedBarBg->setPosition(sf::Vector2f(leftX, leftY + 42.0f));
    m_speedBarBg->setFillColor(HUDConfig::BAR_BG);
    
    // Speed bar
    m_speedBar = std::make_unique<sf::RectangleShape>(
        sf::Vector2f(0.0f, HUDConfig::BAR_HEIGHT));
    m_speedBar->setPosition(sf::Vector2f(leftX, leftY + 42.0f));
    m_speedBar->setFillColor(sf::Color(50, 255, 50));
    
    // Damage label
    m_damageLabel = std::make_unique<sf::Text>(m_font);
    m_damageLabel->setCharacterSize(10);
    m_damageLabel->setFillColor(HUDConfig::TEXT_GRAY);
    m_damageLabel->setString("DAMAGE");
    m_damageLabel->setPosition(sf::Vector2f(leftX, leftY + 62.0f));
    
    // Damage bar background
    m_damageBarBg = std::make_unique<sf::RectangleShape>(
        sf::Vector2f(HUDConfig::BAR_WIDTH, HUDConfig::BAR_HEIGHT));
    m_damageBarBg->setPosition(sf::Vector2f(leftX, leftY + 80.0f));
    m_damageBarBg->setFillColor(HUDConfig::BAR_BG);
    
    // Damage bar
    m_damageBar = std::make_unique<sf::RectangleShape>(
        sf::Vector2f(0.0f, HUDConfig::BAR_HEIGHT));
    m_damageBar->setPosition(sf::Vector2f(leftX, leftY + 80.0f));
    m_damageBar->setFillColor(sf::Color(100, 255, 100));
    
    // ═══════════════════════════════════════════════════════════════
    // RIGHT PANEL - Score & Lap Times
    // ═══════════════════════════════════════════════════════════════
    float rightPanelX = winW - HUDConfig::RIGHT_PANEL_WIDTH - HUDConfig::PANEL_MARGIN;
    
    m_rightPanel = std::make_unique<sf::RectangleShape>(
        sf::Vector2f(HUDConfig::RIGHT_PANEL_WIDTH, HUDConfig::RIGHT_PANEL_HEIGHT));
    m_rightPanel->setPosition(sf::Vector2f(rightPanelX, HUDConfig::PANEL_MARGIN));
    m_rightPanel->setFillColor(HUDConfig::PANEL_BG);
    m_rightPanel->setOutlineColor(HUDConfig::PANEL_BORDER);
    m_rightPanel->setOutlineThickness(2.0f);
    
    float rightX = rightPanelX + HUDConfig::PANEL_PADDING;
    float rightY = HUDConfig::PANEL_MARGIN + HUDConfig::PANEL_PADDING;
    
    // Score label
    m_scoreLabel = std::make_unique<sf::Text>(m_font);
    m_scoreLabel->setCharacterSize(10);
    m_scoreLabel->setFillColor(HUDConfig::TEXT_GRAY);
    m_scoreLabel->setString("SCORE");
    m_scoreLabel->setPosition(sf::Vector2f(rightX, rightY));
    
    // Score value
    m_scoreText = std::make_unique<sf::Text>(m_font);
    m_scoreText->setCharacterSize(20);
    m_scoreText->setFillColor(HUDConfig::TEXT_GOLD);
    m_scoreText->setString("0");
    m_scoreText->setPosition(sf::Vector2f(rightX, rightY + 14.0f));
    
    // Current lap time label
    m_lapTimeLabel = std::make_unique<sf::Text>(m_font);
    m_lapTimeLabel->setCharacterSize(10);
    m_lapTimeLabel->setFillColor(HUDConfig::TEXT_GRAY);
    m_lapTimeLabel->setString("TIME");
    m_lapTimeLabel->setPosition(sf::Vector2f(rightX, rightY + 45.0f));
    
    // Current lap time
    m_lapTimeText = std::make_unique<sf::Text>(m_font);
    m_lapTimeText->setCharacterSize(18);
    m_lapTimeText->setFillColor(HUDConfig::TEXT_WHITE);
    m_lapTimeText->setString("0:00.000");
    m_lapTimeText->setPosition(sf::Vector2f(rightX, rightY + 58.0f));
    
    // Best lap label
    m_bestLapLabel = std::make_unique<sf::Text>(m_font);
    m_bestLapLabel->setCharacterSize(10);
    m_bestLapLabel->setFillColor(HUDConfig::TEXT_GRAY);
    m_bestLapLabel->setString("BEST");
    m_bestLapLabel->setPosition(sf::Vector2f(rightX, rightY + 85.0f));
    
    // Best lap time
    m_bestLapText = std::make_unique<sf::Text>(m_font);
    m_bestLapText->setCharacterSize(14);
    m_bestLapText->setFillColor(HUDConfig::TEXT_GREEN);
    m_bestLapText->setString("--:--.---");
    m_bestLapText->setPosition(sf::Vector2f(rightX, rightY + 98.0f));
    
    // Last lap label
    m_lastLapLabel = std::make_unique<sf::Text>(m_font);
    m_lastLapLabel->setCharacterSize(10);
    m_lastLapLabel->setFillColor(HUDConfig::TEXT_GRAY);
    m_lastLapLabel->setString("LAST");
    m_lastLapLabel->setPosition(sf::Vector2f(rightX + 100.0f, rightY + 85.0f));
    
    // Last lap time
    m_lastLapText = std::make_unique<sf::Text>(m_font);
    m_lastLapText->setCharacterSize(14);
    m_lastLapText->setFillColor(HUDConfig::TEXT_WHITE);
    m_lastLapText->setString("--:--.---");
    m_lastLapText->setPosition(sf::Vector2f(rightX + 100.0f, rightY + 98.0f));
    
    // ═══════════════════════════════════════════════════════════════
    // CENTER - Lap Counter
    // ═══════════════════════════════════════════════════════════════
    float lapPanelX = (winW - HUDConfig::LAP_PANEL_WIDTH) / 2.0f;
    
    m_lapPanel = std::make_unique<sf::RectangleShape>(
        sf::Vector2f(HUDConfig::LAP_PANEL_WIDTH, HUDConfig::LAP_PANEL_HEIGHT));
    m_lapPanel->setPosition(sf::Vector2f(lapPanelX, HUDConfig::PANEL_MARGIN));
    m_lapPanel->setFillColor(HUDConfig::PANEL_BG);
    m_lapPanel->setOutlineColor(HUDConfig::PANEL_BORDER);
    m_lapPanel->setOutlineThickness(2.0f);
    
    m_lapCountText = std::make_unique<sf::Text>(m_font);
    m_lapCountText->setCharacterSize(18);
    m_lapCountText->setFillColor(HUDConfig::TEXT_WHITE);
    m_lapCountText->setString("LAP 1");
}

std::string GameHUD::formatTime(float seconds) {
    if (seconds <= 0.0f) return "--:--.---";
    
    int mins = static_cast<int>(seconds) / 60;
    int secs = static_cast<int>(seconds) % 60;
    int millis = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);
    
    std::ostringstream oss;
    oss << mins << ":" << std::setfill('0') << std::setw(2) << secs 
        << "." << std::setw(3) << millis;
    return oss.str();
}

void GameHUD::centerText(sf::Text& text, float x, float y) {
    auto bounds = text.getLocalBounds();
    text.setOrigin(sf::Vector2f(bounds.size.x / 2.0f, bounds.size.y / 2.0f));
    text.setPosition(sf::Vector2f(x, y));
}

void GameHUD::update(const GameplayManager& gameplay, float deltaTime) {
    updateSpeedometer(gameplay.getPlayerSpeedKmh());
    updateScore(gameplay.getStats().currentScore);
    updateLapInfo(gameplay.getCurrentLapTime(), gameplay.getBestLapTime(), 
                  gameplay.getLastLapTime(), gameplay.getLapCount());
    updateDamage(gameplay.getPlayer().getTotalDamage());
    
    // Boost feedback
    const auto& boost = gameplay.getBoostResult();
    if (boost.attempted && !m_boostFeedbackShown) {
        m_boostFeedbackTimer = 2.0f;
        m_boostFeedbackShown = true;
    }
    
    if (m_boostFeedbackTimer > 0.0f) {
        m_boostFeedbackTimer -= deltaTime;
    }
    
    // New lap animation
    if (m_newLapTimer > 0.0f) {
        m_newLapTimer -= deltaTime;
    }
}

void GameHUD::updateSpeedometer(float speedKmh) {
    std::ostringstream oss;
    oss << static_cast<int>(speedKmh);
    m_speedText->setString(oss.str());
    
    // Poziționăm KM/H după număr
    auto speedBounds = m_speedText->getGlobalBounds();
    m_speedUnitText->setPosition(sf::Vector2f(
        speedBounds.position.x + speedBounds.size.x + 8.0f,
        speedBounds.position.y + speedBounds.size.y - 14.0f
    ));
    
    // Culoare bazată pe viteză
    if (speedKmh > 280.0f) {
        m_speedText->setFillColor(sf::Color(255, 80, 80));
    } else if (speedKmh > 200.0f) {
        m_speedText->setFillColor(sf::Color(255, 220, 80));
    } else {
        m_speedText->setFillColor(sf::Color::White);
    }
    
    // Speed bar
    float speedPercent = std::clamp(speedKmh / 330.0f, 0.0f, 1.0f);
    m_speedBar->setSize(sf::Vector2f(HUDConfig::BAR_WIDTH * speedPercent, HUDConfig::BAR_HEIGHT));
    
    if (speedPercent > 0.85f) {
        m_speedBar->setFillColor(sf::Color(255, 60, 60));
    } else if (speedPercent > 0.6f) {
        m_speedBar->setFillColor(sf::Color(255, 200, 60));
    } else {
        m_speedBar->setFillColor(sf::Color(60, 255, 60));
    }
}

void GameHUD::updateScore(float score) {
    std::ostringstream oss;
    oss << static_cast<int>(score);
    m_scoreText->setString(oss.str());
}

void GameHUD::updateLapInfo(float currentLap, float bestLap, float lastLap, int lapCount) {
    m_lapTimeText->setString(formatTime(currentLap));
    m_bestLapText->setString(formatTime(bestLap));
    m_lastLapText->setString(formatTime(lastLap));
    
    std::ostringstream oss;
    oss << "LAP " << (lapCount + 1);
    m_lapCountText->setString(oss.str());
    
    // Centram textul în panou
    float lapPanelX = m_lapPanel->getPosition().x;
    float lapPanelW = m_lapPanel->getSize().x;
    float lapPanelY = m_lapPanel->getPosition().y;
    float lapPanelH = m_lapPanel->getSize().y;
    centerText(*m_lapCountText, lapPanelX + lapPanelW / 2.0f, lapPanelY + lapPanelH / 2.0f);
}

void GameHUD::updateDamage(float damage) {
    float damagePercent = std::clamp(damage / 100.0f, 0.0f, 1.0f);
    m_damageBar->setSize(sf::Vector2f(HUDConfig::BAR_WIDTH * damagePercent, HUDConfig::BAR_HEIGHT));
    
    // Culoare: verde → galben → roșu
    if (damagePercent >= 0.75f) {
        m_damageBar->setFillColor(sf::Color(255, 50, 50));
        m_damageLabel->setFillColor(sf::Color(255, 100, 100));
    } else if (damagePercent >= 0.5f) {
        m_damageBar->setFillColor(sf::Color(255, 150, 50));
        m_damageLabel->setFillColor(sf::Color(255, 180, 100));
    } else if (damagePercent >= 0.25f) {
        m_damageBar->setFillColor(sf::Color(255, 255, 50));
        m_damageLabel->setFillColor(HUDConfig::TEXT_GRAY);
    } else {
        m_damageBar->setFillColor(sf::Color(100, 255, 100));
        m_damageLabel->setFillColor(HUDConfig::TEXT_GRAY);
    }
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
    
    if (phase == CountdownPhase::Finished) return;
    
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
    
    centerText(statusText, centerX, centerY + 70.0f);
    window.draw(statusText);
    
    if (phase != CountdownPhase::Go && phase != CountdownPhase::Finished) {
        sf::Text hintText(m_font);
        hintText.setString("Press SHIFT at green for boost!");
        hintText.setCharacterSize(12);
        hintText.setFillColor(sf::Color(150, 150, 150));
        centerText(hintText, centerX, centerY + 100.0f);
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
    
    centerText(feedbackText, centerX, centerY);
    
    float scale = 1.0f + 0.1f * std::sin(m_boostFeedbackTimer * 10.0f);
    feedbackText.setScale(sf::Vector2f(scale, scale));
    
    window.draw(feedbackText);
}

void GameHUD::renderGameOver(sf::RenderWindow& window, const GameplayManager& gameplay) {
    float winW = static_cast<float>(window.getSize().x);
    float winH = static_cast<float>(window.getSize().y);
    
    // Overlay
    sf::RectangleShape overlay(sf::Vector2f(winW, winH));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);
    
    // Panel central
    sf::RectangleShape panel(sf::Vector2f(400.0f, 250.0f));
    panel.setOrigin(sf::Vector2f(200.0f, 125.0f));
    panel.setPosition(sf::Vector2f(winW / 2.0f, winH / 2.0f));
    panel.setFillColor(sf::Color(20, 20, 20, 240));
    panel.setOutlineColor(sf::Color(255, 50, 50));
    panel.setOutlineThickness(3.0f);
    window.draw(panel);
    
    // GAME OVER
    sf::Text gameOverText(m_font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(48);
    gameOverText.setFillColor(sf::Color::Red);
    centerText(gameOverText, winW / 2.0f, winH / 2.0f - 70.0f);
    window.draw(gameOverText);
    
    // CAR DESTROYED
    sf::Text subtitleText(m_font);
    subtitleText.setString("CAR DESTROYED!");
    subtitleText.setCharacterSize(18);
    subtitleText.setFillColor(sf::Color(255, 150, 150));
    centerText(subtitleText, winW / 2.0f, winH / 2.0f - 20.0f);
    window.draw(subtitleText);
    
    // Final score
    sf::Text scoreText(m_font);
    std::ostringstream oss;
    oss << "FINAL SCORE: " << static_cast<int>(gameplay.getStats().currentScore);
    scoreText.setString(oss.str());
    scoreText.setCharacterSize(16);
    scoreText.setFillColor(HUDConfig::TEXT_GOLD);
    centerText(scoreText, winW / 2.0f, winH / 2.0f + 25.0f);
    window.draw(scoreText);
    
    // Laps completed
    sf::Text lapsText(m_font);
    std::ostringstream lss;
    lss << "LAPS: " << gameplay.getLapCount();
    lapsText.setString(lss.str());
    lapsText.setCharacterSize(14);
    lapsText.setFillColor(HUDConfig::TEXT_WHITE);
    centerText(lapsText, winW / 2.0f, winH / 2.0f + 55.0f);
    window.draw(lapsText);
    
    // Instructions
    sf::Text hintText(m_font);
    hintText.setString("Press ESC to exit");
    hintText.setCharacterSize(12);
    hintText.setFillColor(sf::Color(120, 120, 120));
    centerText(hintText, winW / 2.0f, winH / 2.0f + 95.0f);
    window.draw(hintText);
}

void GameHUD::renderRaceFinished(sf::RenderWindow& window, const GameplayManager& gameplay) {
    float winW = static_cast<float>(window.getSize().x);
    float winH = static_cast<float>(window.getSize().y);
    
    // Overlay
    sf::RectangleShape overlay(sf::Vector2f(winW, winH));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);
    
    // Panel central - mai mare pentru a încăpea toate informațiile
    sf::RectangleShape panel(sf::Vector2f(500.0f, 400.0f));
    panel.setOrigin(sf::Vector2f(250.0f, 200.0f));
    panel.setPosition(sf::Vector2f(winW / 2.0f, winH / 2.0f));
    panel.setFillColor(sf::Color(20, 30, 20, 245));
    panel.setOutlineColor(sf::Color(100, 255, 100));
    panel.setOutlineThickness(4.0f);
    window.draw(panel);
    
    const auto& progress = gameplay.getCampaignProgress();
    const auto& track = gameplay.getCampaignTrack();
    bool taskCompleted = gameplay.isObjectiveCompleted();
    
    // RACE FINISHED!
    sf::Text titleText(m_font);
    titleText.setString("RACE FINISHED!");
    titleText.setCharacterSize(36);
    titleText.setFillColor(sf::Color(100, 255, 100));
    centerText(titleText, winW / 2.0f, winH / 2.0f - 160.0f);
    window.draw(titleText);
    
    // Track name
    sf::Text trackText(m_font);
    trackText.setString(track.name);
    trackText.setCharacterSize(16);
    trackText.setFillColor(sf::Color(200, 200, 200));
    centerText(trackText, winW / 2.0f, winH / 2.0f - 115.0f);
    window.draw(trackText);
    
    // Separator line
    sf::RectangleShape separator(sf::Vector2f(400.0f, 2.0f));
    separator.setOrigin(sf::Vector2f(200.0f, 1.0f));
    separator.setPosition(sf::Vector2f(winW / 2.0f, winH / 2.0f - 90.0f));
    separator.setFillColor(sf::Color(80, 80, 80));
    window.draw(separator);
    
    // Task result
    sf::Text taskLabel(m_font);
    taskLabel.setString("TASK:");
    taskLabel.setCharacterSize(12);
    taskLabel.setFillColor(sf::Color(150, 150, 150));
    taskLabel.setPosition(sf::Vector2f(winW / 2.0f - 200.0f, winH / 2.0f - 70.0f));
    window.draw(taskLabel);
    
    const CampaignTask* selectedTask = track.getSelectedTask();
    if (selectedTask) {
        sf::Text taskDesc(m_font);
        taskDesc.setString(selectedTask->description);
        taskDesc.setCharacterSize(11);
        taskDesc.setFillColor(sf::Color::White);
        taskDesc.setPosition(sf::Vector2f(winW / 2.0f - 200.0f, winH / 2.0f - 50.0f));
        window.draw(taskDesc);
    }
    
    // Task status (COMPLETED / FAILED)
    sf::Text taskStatus(m_font);
    if (taskCompleted) {
        taskStatus.setString("COMPLETED!");
        taskStatus.setFillColor(sf::Color(100, 255, 100));
    } else {
        taskStatus.setString("NOT COMPLETED");
        taskStatus.setFillColor(sf::Color(255, 150, 100));
    }
    taskStatus.setCharacterSize(14);
    taskStatus.setPosition(sf::Vector2f(winW / 2.0f + 100.0f, winH / 2.0f - 60.0f));
    window.draw(taskStatus);
    
    // Statistics section
    float statsY = winH / 2.0f - 15.0f;
    float leftCol = winW / 2.0f - 180.0f;
    float rightCol = winW / 2.0f + 20.0f;
    
    // Total time
    sf::Text timeLabel(m_font);
    timeLabel.setString("TOTAL TIME:");
    timeLabel.setCharacterSize(11);
    timeLabel.setFillColor(sf::Color(150, 150, 150));
    timeLabel.setPosition(sf::Vector2f(leftCol, statsY));
    window.draw(timeLabel);
    
    sf::Text timeValue(m_font);
    timeValue.setString(formatTime(progress.raceTime));
    timeValue.setCharacterSize(14);
    timeValue.setFillColor(sf::Color::White);
    timeValue.setPosition(sf::Vector2f(leftCol, statsY + 16.0f));
    window.draw(timeValue);
    
    // Best lap
    sf::Text bestLabel(m_font);
    bestLabel.setString("BEST LAP:");
    bestLabel.setCharacterSize(11);
    bestLabel.setFillColor(sf::Color(150, 150, 150));
    bestLabel.setPosition(sf::Vector2f(rightCol, statsY));
    window.draw(bestLabel);
    
    sf::Text bestValue(m_font);
    bestValue.setString(formatTime(progress.bestLapTime));
    bestValue.setCharacterSize(14);
    bestValue.setFillColor(HUDConfig::TEXT_GREEN);
    bestValue.setPosition(sf::Vector2f(rightCol, statsY + 16.0f));
    window.draw(bestValue);
    
    // Laps and top speed
    statsY += 50.0f;
    
    sf::Text lapsLabel(m_font);
    lapsLabel.setString("LAPS:");
    lapsLabel.setCharacterSize(11);
    lapsLabel.setFillColor(sf::Color(150, 150, 150));
    lapsLabel.setPosition(sf::Vector2f(leftCol, statsY));
    window.draw(lapsLabel);
    
    sf::Text lapsValue(m_font);
    std::ostringstream lapsSS;
    lapsSS << progress.currentLap << "/" << progress.totalLaps;
    lapsValue.setString(lapsSS.str());
    lapsValue.setCharacterSize(14);
    lapsValue.setFillColor(sf::Color::White);
    lapsValue.setPosition(sf::Vector2f(leftCol, statsY + 16.0f));
    window.draw(lapsValue);
    
    sf::Text speedLabel(m_font);
    speedLabel.setString("TOP SPEED:");
    speedLabel.setCharacterSize(11);
    speedLabel.setFillColor(sf::Color(150, 150, 150));
    speedLabel.setPosition(sf::Vector2f(rightCol, statsY));
    window.draw(speedLabel);
    
    sf::Text speedValue(m_font);
    std::ostringstream speedSS;
    speedSS << static_cast<int>(progress.topSpeed) << " KM/H";
    speedValue.setString(speedSS.str());
    speedValue.setCharacterSize(14);
    speedValue.setFillColor(sf::Color(255, 200, 100));
    speedValue.setPosition(sf::Vector2f(rightCol, statsY + 16.0f));
    window.draw(speedValue);
    
    // Perfect laps info
    statsY += 50.0f;
    
    if (progress.perfectLapsCount > 0) {
        sf::Text perfectLabel(m_font);
        std::ostringstream perfectSS;
        perfectSS << "PERFECT LAPS: " << progress.perfectLapsCount;
        perfectLabel.setString(perfectSS.str());
        perfectLabel.setCharacterSize(12);
        perfectLabel.setFillColor(sf::Color(255, 215, 0));
        centerText(perfectLabel, winW / 2.0f, statsY);
        window.draw(perfectLabel);
        statsY += 25.0f;
    }
    
    // Unlocked message (if task completed)
    if (taskCompleted) {
        sf::RectangleShape unlockPanel(sf::Vector2f(380.0f, 40.0f));
        unlockPanel.setOrigin(sf::Vector2f(190.0f, 20.0f));
        unlockPanel.setPosition(sf::Vector2f(winW / 2.0f, statsY + 15.0f));
        unlockPanel.setFillColor(sf::Color(40, 80, 40, 200));
        unlockPanel.setOutlineColor(sf::Color(100, 200, 100));
        unlockPanel.setOutlineThickness(2.0f);
        window.draw(unlockPanel);
        
        sf::Text unlockText(m_font);
        unlockText.setString("PROGRESS SAVED!");
        unlockText.setCharacterSize(14);
        unlockText.setFillColor(sf::Color(150, 255, 150));
        centerText(unlockText, winW / 2.0f, statsY + 15.0f);
        window.draw(unlockText);
    }
    
    // Instructions
    sf::Text hintText(m_font);
    hintText.setString("Press ENTER to continue - ESC to exit");
    hintText.setCharacterSize(10);
    hintText.setFillColor(sf::Color(120, 120, 120));
    centerText(hintText, winW / 2.0f, winH / 2.0f + 175.0f);
    window.draw(hintText);
}

void GameHUD::render(sf::RenderWindow& window, const GameplayManager& gameplay) {
    // Draw panels
    window.draw(*m_leftPanel);
    window.draw(*m_speedText);
    window.draw(*m_speedUnitText);
    window.draw(*m_speedBarBg);
    window.draw(*m_speedBar);
    window.draw(*m_damageLabel);
    window.draw(*m_damageBarBg);
    window.draw(*m_damageBar);
    
    window.draw(*m_rightPanel);
    window.draw(*m_scoreLabel);
    window.draw(*m_scoreText);
    window.draw(*m_lapTimeLabel);
    window.draw(*m_lapTimeText);
    window.draw(*m_bestLapLabel);
    window.draw(*m_bestLapText);
    window.draw(*m_lastLapLabel);
    window.draw(*m_lastLapText);
    
    window.draw(*m_lapPanel);
    window.draw(*m_lapCountText);
    
    // Countdown
    renderCountdown(window, gameplay);
    
    // Boost feedback
    renderBoostFeedback(window, gameplay);
    
    // Game over (pentru Endless când mașina e distrusă)
    if (gameplay.isGameOver()) {
        renderGameOver(window, gameplay);
    }
    
    // Race finished (pentru Campaign când se termină toate turele)
    if (gameplay.getGameMode() == GameMode::Campaign && gameplay.isRaceFinished()) {
        renderRaceFinished(window, gameplay);
    }
}