#include "PlayState.h"
#include "TrackSelectState.h"
#include "../Core/SettingsManager.h"
#include "MainMenuState.h"
#include "Core/Game.h"
#include "Core/Constants.h"
#include "States/StateManager.h"
#include "Core/AudioManager.h"
#include <iostream>

PlayState::PlayState(Game* game, GameMode mode, const TrackDefinition* track)
    : State(game)
    , m_isPaused(false)
    , m_showingTasks(false)
    , m_pauseSelectedIndex(0)
    , m_currentMode(mode)
{
    // Copy the track if provided (avoid dangling pointer!)
    if (track) {
        m_savedTrack = *track;  // Make a copy
        m_gameplayManager = std::make_unique<GameplayManager>(mode, &m_savedTrack.value());
    } else {
        m_savedTrack = std::nullopt;
        m_gameplayManager = std::make_unique<GameplayManager>(mode, nullptr);
    }
    
    m_hud = std::make_unique<GameHUD>();
    
    initPauseMenu();
    std::cout << "[PlayState] Initialized" << std::endl;
}

void PlayState::initPauseMenu() {
    if (!m_pauseFont.openFromFile("assets/fonts/PressStart2P-Regular.ttf")) {
        std::cerr << "[PlayState] Failed to load pause font!" << std::endl;
    }
    
    auto& settings = SettingsManager::getInstance();
    float windowWidth = static_cast<float>(settings.getWindowWidth());
    float windowHeight = static_cast<float>(settings.getWindowHeight());

    
    m_pauseTitle = std::make_unique<sf::Text>(m_pauseFont);
    m_pauseTitle->setString("PAUSED");
    m_pauseTitle->setCharacterSize(48);
    m_pauseTitle->setFillColor(sf::Color::White);
    auto titleBounds = m_pauseTitle->getLocalBounds();
    m_pauseTitle->setOrigin(sf::Vector2f(
        titleBounds.position.x + titleBounds.size.x * 0.5f,
        titleBounds.position.y + titleBounds.size.y * 0.5f
    ));
    m_pauseTitle->setPosition(sf::Vector2f(windowWidth * 0.5f, windowHeight * 0.5f - 150.f));
    
    // Menu items - add Tasks for Campaign mode
    std::vector<std::string> menuLabels;
    if (m_currentMode == GameMode::Campaign) {
        menuLabels = { "CONTINUE", "TASKS", "RESTART", "QUIT" };
    } else {
        menuLabels = { "CONTINUE", "RESTART", "QUIT" };
    }
    
    float menuStartY = windowHeight * 0.5f - 50.f;
    float menuSpacing = 50.f;
    
    m_pauseMenuItems.clear();
    for (size_t i = 0; i < menuLabels.size(); ++i) {
        auto item = std::make_unique<sf::Text>(m_pauseFont);
        item->setString(menuLabels[i]);
        item->setCharacterSize(24);
        item->setFillColor(sf::Color(180, 180, 180));
        auto itemBounds = item->getLocalBounds();
        item->setOrigin(sf::Vector2f(
            itemBounds.position.x + itemBounds.size.x * 0.5f,
            itemBounds.position.y + itemBounds.size.y * 0.5f
        ));
        item->setPosition(sf::Vector2f(windowWidth * 0.5f, menuStartY + i * menuSpacing));
        m_pauseMenuItems.push_back(std::move(item));
    }
    
    m_pauseSelectedIndex = 0;
    updatePauseMenuDisplay();
    AudioManager::getInstance().playMusic("main_menu", true);
    
    // Initialize task overlay for Campaign mode
    if (m_currentMode == GameMode::Campaign) {
        initTaskOverlay();
    }
}

void PlayState::initTaskOverlay() {
    
    auto& settings = SettingsManager::getInstance();
    float windowWidth = static_cast<float>(settings.getWindowWidth());
    float windowHeight = static_cast<float>(settings.getWindowHeight());

    
    // Background overlay
    m_taskOverlayBg = std::make_unique<sf::RectangleShape>(sf::Vector2f(500.f, 300.f));
    m_taskOverlayBg->setPosition(sf::Vector2f(windowWidth * 0.5f - 250.f, windowHeight * 0.5f - 150.f));
    m_taskOverlayBg->setFillColor(sf::Color(30, 30, 40, 240));
    m_taskOverlayBg->setOutlineThickness(3.f);
    m_taskOverlayBg->setOutlineColor(sf::Color(80, 100, 150));
    
    // Title
    m_taskOverlayTitle = std::make_unique<sf::Text>(m_pauseFont);
    m_taskOverlayTitle->setString("CURRENT TASK");
    m_taskOverlayTitle->setCharacterSize(24);
    m_taskOverlayTitle->setFillColor(sf::Color(100, 150, 255));
    auto titleBounds = m_taskOverlayTitle->getLocalBounds();
    m_taskOverlayTitle->setOrigin(sf::Vector2f(
        titleBounds.position.x + titleBounds.size.x * 0.5f,
        titleBounds.position.y + titleBounds.size.y * 0.5f
    ));
    m_taskOverlayTitle->setPosition(sf::Vector2f(windowWidth * 0.5f, windowHeight * 0.5f - 110.f));
    
    // Task description
    m_currentTaskText = std::make_unique<sf::Text>(m_pauseFont);
    m_currentTaskText->setString("");
    m_currentTaskText->setCharacterSize(14);
    m_currentTaskText->setFillColor(sf::Color::White);
    m_currentTaskText->setPosition(sf::Vector2f(windowWidth * 0.5f - 220.f, windowHeight * 0.5f - 50.f));
    
    // Progress text
    m_taskProgressText = std::make_unique<sf::Text>(m_pauseFont);
    m_taskProgressText->setString("");
    m_taskProgressText->setCharacterSize(12);
    m_taskProgressText->setFillColor(sf::Color(180, 180, 180));
    m_taskProgressText->setPosition(sf::Vector2f(windowWidth * 0.5f - 220.f, windowHeight * 0.5f));
    
    // Checkmark
    m_taskCheckmark = std::make_unique<sf::RectangleShape>(sf::Vector2f(50.f, 50.f));
    m_taskCheckmark->setPosition(sf::Vector2f(windowWidth * 0.5f + 150.f, windowHeight * 0.5f - 25.f));
    m_taskCheckmark->setOutlineThickness(2.f);
    
    m_taskCheckmarkText = std::make_unique<sf::Text>(m_pauseFont);
    m_taskCheckmarkText->setString("");
    m_taskCheckmarkText->setCharacterSize(28);
    m_taskCheckmarkText->setPosition(sf::Vector2f(windowWidth * 0.5f + 165.f, windowHeight * 0.5f - 18.f));
    
    // Hint
    m_taskHintText = std::make_unique<sf::Text>(m_pauseFont);
    m_taskHintText->setString("Press Esc to close");
    m_taskHintText->setCharacterSize(12);
    m_taskHintText->setFillColor(sf::Color(120, 120, 120));
    auto hintBounds = m_taskHintText->getLocalBounds();
    m_taskHintText->setOrigin(sf::Vector2f(
        hintBounds.position.x + hintBounds.size.x * 0.5f,
        hintBounds.position.y + hintBounds.size.y * 0.5f
    ));
    m_taskHintText->setPosition(sf::Vector2f(windowWidth * 0.5f, windowHeight * 0.5f + 100.f));
}

void PlayState::updatePauseMenuDisplay() {
    std::vector<std::string> labels;
    if (m_currentMode == GameMode::Campaign) {
        labels = { "CONTINUE", "TASKS", "RESTART", "QUIT" };
    } else {
        labels = { "CONTINUE", "RESTART", "QUIT" };
    }
    
    for (size_t i = 0; i < m_pauseMenuItems.size(); ++i) {
        if (static_cast<int>(i) == m_pauseSelectedIndex) {
            m_pauseMenuItems[i]->setFillColor(sf::Color(255, 200, 0));  // Gold
            m_pauseMenuItems[i]->setString("> " + labels[i] + " <");
        } else {
            m_pauseMenuItems[i]->setFillColor(sf::Color::White);
            m_pauseMenuItems[i]->setString(labels[i]);
        }
        
        // Re-center after string change
        auto bounds = m_pauseMenuItems[i]->getLocalBounds();
        m_pauseMenuItems[i]->setOrigin(sf::Vector2f(
            bounds.position.x + bounds.size.x * 0.5f,
            bounds.position.y + bounds.size.y * 0.5f
        ));
        //AudioManager::getInstance().playMusic("main_menu", true);
    }
}

void PlayState::handleInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        
        // Race finished - Special input handling
        if (m_currentMode == GameMode::Campaign && m_gameplayManager->isRaceFinished()) {
            switch (keyPressed->code) {
                case sf::Keyboard::Key::Enter:
                    // Go back to track selection for Campaign
                    std::cout << "[PlayState] Race finished - returning to track selection" << std::endl;
                    m_game->getStateManager()->changeState(
                        std::make_unique<TrackSelectState>(m_game, GameMode::Campaign));
                    break;
                    
                case sf::Keyboard::Key::Escape:
                    // Go back to main menu
                    std::cout << "[PlayState] Race finished - returning to main menu" << std::endl;
                    m_game->getStateManager()->changeState(
                        std::make_unique<MainMenuState>(m_game));
                    break;
                    
                default:
                    break;
            }
            return;  // Don't process other input when race is finished
        }
        
        // Game over - Special input handling (for Endless mode)
        if (m_gameplayManager->isGameOver()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                std::cout << "[PlayState] Game over - returning to main menu" << std::endl;
                m_game->getStateManager()->changeState(
                    std::make_unique<MainMenuState>(m_game));
            }
            return;  // Don't process other input when game is over
        }
        
        // Normal gameplay - Pause menu handling
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            if (m_showingTasks) {
                m_showingTasks = false; 
                AudioManager::getInstance().playMusic("main_menu", true); 
            } else {
                m_isPaused = !m_isPaused;
            }
            return;
        }
        
        if (m_isPaused && !m_showingTasks) {
            switch (keyPressed->code) {
                case sf::Keyboard::Key::Up:
                    m_pauseSelectedIndex = (m_pauseSelectedIndex - 1 + 
                        static_cast<int>(m_pauseMenuItems.size())) % m_pauseMenuItems.size();
                    updatePauseMenuDisplay();
                    break;
                    
                case sf::Keyboard::Key::Down:
                    m_pauseSelectedIndex = (m_pauseSelectedIndex + 1) % m_pauseMenuItems.size();
                    updatePauseMenuDisplay();
                    break;
                    
                case sf::Keyboard::Key::Enter:
                    handlePauseMenuAction();
                    break;
                    
                default:
                    break;
            }
        }
    }
}

void PlayState::handlePauseMenuAction() {
    std::vector<std::string> labels;
    if (m_currentMode == GameMode::Campaign) {
        labels = { "CONTINUE", "TASKS", "RESTART", "QUIT" };
    } else {
        labels = { "CONTINUE", "RESTART", "QUIT" };
    }
    
    std::string selected = labels[m_pauseSelectedIndex];
    
    if (selected == "CONTINUE") {
        m_isPaused = false;
    } else if (selected == "TASKS") {
        m_showingTasks = true;
        
        // Update task overlay content
        if (m_campaignTrackData.has_value()) {
            const CampaignTask* task = m_campaignTrackData->getSelectedTask();
            if (task) {
                m_currentTaskText->setString(task->description);
                m_taskProgressText->setString(
                    CampaignDataManager::getTaskProgress(*task, m_campaignProgress));
                
                bool completed = m_campaignProgress.checkTask(*task);
                if (completed) {
                    m_taskCheckmark->setFillColor(sf::Color(50, 150, 50, 150));
                    m_taskCheckmark->setOutlineColor(sf::Color(100, 255, 100));
                    m_taskCheckmarkText->setString("V");
                    m_taskCheckmarkText->setFillColor(sf::Color(100, 255, 100));
                } else {
                    m_taskCheckmark->setFillColor(sf::Color(80, 80, 80, 150));
                    m_taskCheckmark->setOutlineColor(sf::Color(120, 120, 120));
                    m_taskCheckmarkText->setString("");
                }
            }
        }
    } else if (selected == "RESTART") {
        restartGame();
    } else if (selected == "QUIT") {
        m_game->getStateManager()->changeState(
            std::make_unique<TrackSelectState>(m_game, m_currentMode));
    }
}

void PlayState::renderTaskOverlay(sf::RenderWindow& window) {
    window.draw(*m_taskOverlayBg);
    window.draw(*m_taskOverlayTitle);
    window.draw(*m_currentTaskText);
    window.draw(*m_taskProgressText);
    window.draw(*m_taskCheckmark);
    window.draw(*m_taskCheckmarkText);
    window.draw(*m_taskHintText);
}

void PlayState::render(sf::RenderWindow& window) {
    m_gameplayManager->render(window);
    m_hud->render(window, *m_gameplayManager);
    
    // Don't show pause menu if race is finished or game over (Hud handles those screens)
    if (m_isPaused && !m_gameplayManager->isRaceFinished() && !m_gameplayManager->isGameOver()) {
        // Dim overlay
        sf::RectangleShape overlay(sf::Vector2f(
            static_cast<float>(window.getSize().x),
            static_cast<float>(window.getSize().y)));
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(overlay);
        
        if (m_showingTasks) {
            renderTaskOverlay(window);
        } else {
            window.draw(*m_pauseTitle);
            for (const auto& item : m_pauseMenuItems) {
                window.draw(*item);
            }
        }
    }
}

void PlayState::onEnter() {
    std::cout << "Entered Play State" << std::endl;
    AudioManager::getInstance().playMusic("race_theme_2", true);

    // Load campaign track data if in Campaign mode
    if (m_currentMode == GameMode::Campaign) {
        m_campaignTrackData = m_game->getCurrentCampaignTrack();
        m_campaignProgress.reset();
        if (m_savedTrack.has_value()) {
            m_campaignProgress.totalLaps = m_savedTrack->recommendedLaps;
        }
    }
}
void PlayState::onExit(){
    std::cout << "Exit Play State" << std::endl;
    AudioManager::getInstance().playMusic("main_menu", true);
    
}

void PlayState::restartGame() {
    std::cout << "[GAME] Restarting level..." << std::endl;
    
    // Safe restart - Use our saved copy of the track!
    if (m_savedTrack.has_value()) {
        m_gameplayManager = std::make_unique<GameplayManager>(m_currentMode, &m_savedTrack.value());
    } else {
        m_gameplayManager = std::make_unique<GameplayManager>(m_currentMode, nullptr);
    }
    
    // Reset Hud
    m_hud = std::make_unique<GameHUD>();
    
    // Unpause
    m_isPaused = false;
    m_showingTasks = false;
    m_pauseSelectedIndex = 0;
    updatePauseMenuDisplay();
    std::cout << "[GAME] Level restarted!" << std::endl;
}

void PlayState::update(float deltaTime) {
    // Don't update gameplay if paused, race finished, or game over
    if (!m_isPaused && !m_gameplayManager->isRaceFinished() && !m_gameplayManager->isGameOver()) {
        m_gameplayManager->update(deltaTime);
    }
    
    // Always update Hud (for animations)
    m_hud->update(*m_gameplayManager, deltaTime);
}
