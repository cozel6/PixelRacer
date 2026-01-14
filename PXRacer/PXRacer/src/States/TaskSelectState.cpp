#include "TaskSelectState.h"
#include "../Core/SettingsManager.h"
#include "PlayState.h"
#include "TrackSelectState.h"
#include "Core/Game.h"
#include "Core/AudioManager.h"
#include "States/StateManager.h"
#include "Core/Constants.h"
#include <iostream>
#include <cmath>
#include <cstdint>

// Constructor - stores a COPY of the track
TaskSelectState::TaskSelectState(Game* game, CampaignTrackData trackData, const TrackDefinition* track)
    : State(game)
    , m_trackData(std::move(trackData))
    , m_selectedTaskIndex(0)
    , m_backHovered(false)
    , m_startHovered(false)
    , m_animTimer(0.0f)
{
    // Save a COPY of the track to avoid dangling pointer
    if (track) {
        m_savedTrack = *track;
    }
    
    if (!m_font.openFromFile("assets/fonts/PressStart2P-Regular.ttf")) {
        std::cerr << "[TaskSelect] Failed to load font!" << std::endl;
    }
    createUI();
    updateTaskDisplay();
}

void TaskSelectState::createUI() {
    auto& settings = SettingsManager::getInstance();
    const float windowWidth = static_cast<float>(settings.getWindowWidth());
    const float windowHeight = static_cast<float>(settings.getWindowHeight());
    
    // Header
    m_headerText = std::make_unique<sf::Text>(m_font);
    m_headerText->setString("SELECT TASK");
    m_headerText->setCharacterSize(32);
    m_headerText->setFillColor(sf::Color::White);
    auto headerBounds = m_headerText->getLocalBounds();
    m_headerText->setOrigin(sf::Vector2f(
        headerBounds.position.x + headerBounds.size.x * 0.5f,
        headerBounds.position.y + headerBounds.size.y * 0.5f
    ));
    m_headerText->setPosition(sf::Vector2f(windowWidth * 0.5f, 50.f));
    
    // Track name
    m_trackNameText = std::make_unique<sf::Text>(m_font);
    m_trackNameText->setString(m_trackData.name);
    m_trackNameText->setCharacterSize(24);
    m_trackNameText->setFillColor(sf::Color(200, 200, 200));
    auto trackBounds = m_trackNameText->getLocalBounds();
    m_trackNameText->setOrigin(sf::Vector2f(
        trackBounds.position.x + trackBounds.size.x * 0.5f,
        trackBounds.position.y + trackBounds.size.y * 0.5f
    ));
    m_trackNameText->setPosition(sf::Vector2f(windowWidth * 0.5f, 100.f));
    
    // Difficulty
    std::string diffStr = CampaignDataManager::getDifficultyString(m_trackData.difficulty);
    m_difficultyText = std::make_unique<sf::Text>(m_font);
    m_difficultyText->setString(diffStr);
    m_difficultyText->setCharacterSize(18);
    sf::Color diffColor;
    switch (m_trackData.difficulty) {
        case CampaignTrackDifficulty::Easy:   diffColor = sf::Color(100, 200, 100); break;
        case CampaignTrackDifficulty::Medium: diffColor = sf::Color(255, 200, 50); break;
        case CampaignTrackDifficulty::Hard:   diffColor = sf::Color(255, 80, 80); break;
    }
    m_difficultyText->setFillColor(diffColor);
    auto diffBounds = m_difficultyText->getLocalBounds();
    m_difficultyText->setOrigin(sf::Vector2f(
        diffBounds.position.x + diffBounds.size.x * 0.5f,
        diffBounds.position.y + diffBounds.size.y * 0.5f
    ));
    m_difficultyText->setPosition(sf::Vector2f(windowWidth * 0.5f, 135.f));
    
    // Requirement text
    int required = m_trackData.getRequiredTaskCount();
    int completed = m_trackData.getCompletedTaskCount();
    std::string reqStr = "Complete " + std::to_string(required) + " task(s) to unlock next (" 
                        + std::to_string(completed) + "/" + std::to_string(required) + " done)";
    m_requirementText = std::make_unique<sf::Text>(m_font);
    m_requirementText->setString(reqStr);
    m_requirementText->setCharacterSize(12);
    m_requirementText->setFillColor(sf::Color(180, 180, 180));
    auto reqBounds = m_requirementText->getLocalBounds();
    m_requirementText->setOrigin(sf::Vector2f(
        reqBounds.position.x + reqBounds.size.x * 0.5f,
        reqBounds.position.y + reqBounds.size.y * 0.5f
    ));
    m_requirementText->setPosition(sf::Vector2f(windowWidth * 0.5f, 170.f));
    
    // Task cards
    float cardWidth = 280.f;
    float cardHeight = 100.f;
    float cardSpacing = 20.f;
    float totalWidth = 3 * cardWidth + 2 * cardSpacing;
    float startX = (windowWidth - totalWidth) / 2.f;
    float cardY = 220.f;
    
    for (int i = 0; i < 3; ++i) {
        auto& card = m_taskCards[i];
        float cardX = startX + i * (cardWidth + cardSpacing);
        
        // Background
        card.background = std::make_unique<sf::RectangleShape>(sf::Vector2f(cardWidth, cardHeight));
        card.background->setPosition(sf::Vector2f(cardX, cardY));
        card.background->setFillColor(sf::Color(40, 40, 50));
        card.background->setOutlineThickness(3.f);
        card.background->setOutlineColor(sf::Color(80, 80, 100));
        
        // Task number
        card.numberText = std::make_unique<sf::Text>(m_font);
        card.numberText->setString("TASK " + std::to_string(i + 1));
        card.numberText->setCharacterSize(14);
        card.numberText->setFillColor(sf::Color(150, 150, 150));
        card.numberText->setPosition(sf::Vector2f(cardX + 15.f, cardY + 10.f));
        
        // Description
        card.descriptionText = std::make_unique<sf::Text>(m_font);
        card.descriptionText->setString(m_trackData.tasks[i].description);
        card.descriptionText->setCharacterSize(10);
        card.descriptionText->setFillColor(sf::Color::White);
        card.descriptionText->setPosition(sf::Vector2f(cardX + 15.f, cardY + 40.f));
        
        // Checkmark area
        card.checkmark = std::make_unique<sf::RectangleShape>(sf::Vector2f(30.f, 30.f));
        card.checkmark->setPosition(sf::Vector2f(cardX + cardWidth - 45.f, cardY + cardHeight - 45.f));
        card.checkmark->setOutlineThickness(2.f);
        
        card.checkmarkText = std::make_unique<sf::Text>(m_font);
        card.checkmarkText->setCharacterSize(18);
        card.checkmarkText->setPosition(sf::Vector2f(cardX + cardWidth - 40.f, cardY + cardHeight - 42.f));
        
        card.isCompleted = m_trackData.tasks[i].completed;
        
        // Update checkmark appearance
        if (card.isCompleted) {
            card.checkmark->setFillColor(sf::Color(50, 150, 50, 100));
            card.checkmark->setOutlineColor(sf::Color(100, 200, 100));
            card.checkmarkText->setString("V");
            card.checkmarkText->setFillColor(sf::Color(100, 255, 100));
        } else {
            card.checkmark->setFillColor(sf::Color(60, 60, 60, 100));
            card.checkmark->setOutlineColor(sf::Color(100, 100, 100));
            card.checkmarkText->setString("");
        }
    }
    
    // Hint text
    m_hintText = std::make_unique<sf::Text>(m_font);
    m_hintText->setString("LEFT/RIGHT to select - ENTER to start - ESC to go back");
    m_hintText->setCharacterSize(10);
    m_hintText->setFillColor(sf::Color(120, 120, 120));
    auto hintBounds = m_hintText->getLocalBounds();
    m_hintText->setOrigin(sf::Vector2f(
        hintBounds.position.x + hintBounds.size.x * 0.5f,
        hintBounds.position.y + hintBounds.size.y * 0.5f
    ));
    m_hintText->setPosition(sf::Vector2f(windowWidth * 0.5f, windowHeight - 80.f));
    
    // Back button
    m_backButton = std::make_unique<sf::RectangleShape>(sf::Vector2f(120.f, 40.f));
    m_backButton->setPosition(sf::Vector2f(50.f, windowHeight - 60.f));
    m_backButton->setFillColor(sf::Color(60, 60, 70));
    m_backButton->setOutlineThickness(2.f);
    m_backButton->setOutlineColor(sf::Color(100, 100, 120));
    
    m_backText = std::make_unique<sf::Text>(m_font);
    m_backText->setString("BACK");
    m_backText->setCharacterSize(14);
    m_backText->setFillColor(sf::Color::White);
    m_backText->setPosition(sf::Vector2f(80.f, windowHeight - 52.f));
    
    // Start button
    m_startButton = std::make_unique<sf::RectangleShape>(sf::Vector2f(160.f, 40.f));
    m_startButton->setPosition(sf::Vector2f(windowWidth - 210.f, windowHeight - 60.f));
    m_startButton->setFillColor(sf::Color(50, 120, 50));
    m_startButton->setOutlineThickness(2.f);
    m_startButton->setOutlineColor(sf::Color(80, 180, 80));
    
    m_startText = std::make_unique<sf::Text>(m_font);
    m_startText->setString("START RACE");
    m_startText->setCharacterSize(14);
    m_startText->setFillColor(sf::Color::White);
    m_startText->setPosition(sf::Vector2f(windowWidth - 195.f, windowHeight - 52.f));
}

void TaskSelectState::updateTaskDisplay() {
    for (int i = 0; i < 3; ++i) {
        auto& card = m_taskCards[i];
        
        if (i == m_selectedTaskIndex) {
            card.background->setFillColor(sf::Color(60, 60, 80));
            card.background->setOutlineColor(sf::Color(100, 150, 255));
            card.background->setOutlineThickness(4.f);
        } else {
            card.background->setFillColor(sf::Color(40, 40, 50));
            card.background->setOutlineColor(sf::Color(80, 80, 100));
            card.background->setOutlineThickness(3.f);
        }
    }
}

void TaskSelectState::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        switch (keyPressed->code) {
            case sf::Keyboard::Key::Left:
                m_selectedTaskIndex = (m_selectedTaskIndex - 1 + 3) % 3;
                AudioManager::getInstance().playSfx("menu_select");
                updateTaskDisplay();
                break;

            case sf::Keyboard::Key::Right:
                m_selectedTaskIndex = (m_selectedTaskIndex + 1) % 3;
                AudioManager::getInstance().playSfx("menu_select");
                updateTaskDisplay();
                break;

            case sf::Keyboard::Key::Enter:
                AudioManager::getInstance().playSfx("menu_select");
                confirmSelection();
                break;

            case sf::Keyboard::Key::Escape:
                m_game->getStateManager()->changeState(
                    std::make_unique<TrackSelectState>(m_game, GameMode::Campaign));
                break;

            default:
                break;
        }
    }

    // Mouse handling
    if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
        sf::Vector2f mousePos(static_cast<float>(mouseMoved->position.x),
                              static_cast<float>(mouseMoved->position.y));

        m_backHovered = m_backButton->getGlobalBounds().contains(mousePos);
        m_startHovered = m_startButton->getGlobalBounds().contains(mousePos);

        // Check task cards
        for (int i = 0; i < 3; ++i) {
            if (m_taskCards[i].background->getGlobalBounds().contains(mousePos)) {
                if (m_selectedTaskIndex != i) {
                    AudioManager::getInstance().playSfx("menu_select");
                    m_selectedTaskIndex = i;
                    updateTaskDisplay();
                }
            }
        }
    }

    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos(static_cast<float>(mousePressed->position.x),
                                  static_cast<float>(mousePressed->position.y));

            if (m_backButton->getGlobalBounds().contains(mousePos)) {
                m_game->getStateManager()->changeState(
                    std::make_unique<TrackSelectState>(m_game, GameMode::Campaign));
            } else if (m_startButton->getGlobalBounds().contains(mousePos)) {
                AudioManager::getInstance().playSfx("menu_select");
                confirmSelection();
            } else {
                for (int i = 0; i < 3; ++i) {
                    if (m_taskCards[i].background->getGlobalBounds().contains(mousePos)) {
                        m_selectedTaskIndex = i;
                        AudioManager::getInstance().playSfx("menu_select");
                        confirmSelection();
                        break;
                    }
                }
            }
        }
    }
}

void TaskSelectState::confirmSelection() {
    m_trackData.selectedTaskIndex = m_selectedTaskIndex;
    
    std::cout << "[TaskSelect] Starting race with Task " << (m_selectedTaskIndex + 1) 
              << ": " << m_trackData.tasks[m_selectedTaskIndex].description << std::endl;
    
    // Store the campaign data in game and start PlayState
    m_game->setCurrentCampaignTrack(m_trackData);
    
    // Use the saved copy of the track
    if (m_savedTrack.has_value()) {
        m_game->getStateManager()->changeState(
            std::make_unique<PlayState>(m_game, GameMode::Campaign, &m_savedTrack.value()));
    } else {
        std::cerr << "[TaskSelect] Error: No saved track!" << std::endl;
        m_game->getStateManager()->changeState(
            std::make_unique<TrackSelectState>(m_game, GameMode::Campaign));
    }
}

void TaskSelectState::update(float deltaTime) {
    m_animTimer += deltaTime;
    
    // Animate selected card
    float pulse = 0.5f + 0.5f * std::sin(m_animTimer * 3.f);
    auto& selectedCard = m_taskCards[m_selectedTaskIndex];
    sf::Color outlineColor(
        static_cast<std::uint8_t>(100 + 55 * pulse),
        static_cast<std::uint8_t>(150 + 55 * pulse),
        255
    );
    selectedCard.background->setOutlineColor(outlineColor);
    
    // Button hover effects
    if (m_backHovered) {
        m_backButton->setFillColor(sf::Color(80, 80, 90));
    } else {
        m_backButton->setFillColor(sf::Color(60, 60, 70));
    }
    
    if (m_startHovered) {
        m_startButton->setFillColor(sf::Color(70, 150, 70));
    } else {
        m_startButton->setFillColor(sf::Color(50, 120, 50));
    }
}

void TaskSelectState::render(sf::RenderWindow& window) {
    window.clear(sf::Color(25, 25, 35));
    
    window.draw(*m_headerText);
    window.draw(*m_trackNameText);
    window.draw(*m_difficultyText);
    window.draw(*m_requirementText);
    
    // Draw task cards
    for (int i = 0; i < 3; ++i) {
        auto& card = m_taskCards[i];
        window.draw(*card.background);
        window.draw(*card.numberText);
        window.draw(*card.descriptionText);
        window.draw(*card.checkmark);
        window.draw(*card.checkmarkText);
    }
    
    window.draw(*m_hintText);
    window.draw(*m_backButton);
    window.draw(*m_backText);
    window.draw(*m_startButton);
    window.draw(*m_startText);
}

sf::Color TaskSelectState::getTaskColor(int index) const {
    if (m_taskCards[index].isCompleted) {
        return sf::Color(100, 200, 100);
    }
    return sf::Color::White;
}