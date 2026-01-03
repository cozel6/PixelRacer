#include "PlayState.h"
#include "Core/Game.h"
#include "Core/Constants.h"
#include "States/StateManager.h"
#include <iostream>

PlayState::PlayState(Game* game, GameMode mode, const TrackDefinition* track)
    : State(game)
    , m_isPaused(false)
    , m_pauseSelectedIndex(0)
    , m_currentMode(mode)
{
    // ✅ COPY the track if provided (avoid dangling pointer!)
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
    // Load font
    if (!m_pauseFont.openFromFile("assets/fonts/PressStart2P-Regular.ttf")) {
        std::cerr << "[PlayState] Failed to load pause menu font!" << std::endl;
        return;  // ✅ Don't crash if font fails!
    }
    
    // Title
    m_pauseTitle = std::make_unique<sf::Text>(m_pauseFont);
    m_pauseTitle->setString("PAUSED");
    m_pauseTitle->setCharacterSize(48);
    m_pauseTitle->setFillColor(sf::Color::White);
    m_pauseTitle->setStyle(sf::Text::Bold);
    
    // Center title
    auto titleBounds = m_pauseTitle->getLocalBounds();
    m_pauseTitle->setOrigin(sf::Vector2f(
        titleBounds.position.x + titleBounds.size.x * 0.5f,
        titleBounds.position.y + titleBounds.size.y * 0.5f
    ));
    m_pauseTitle->setPosition(sf::Vector2f(
        Config::WINDOW_WIDTH * 0.5f,
        Config::WINDOW_HEIGHT * 0.3f
    ));
    
    // Menu options
    std::vector<std::string> options = {
        "CONTINUE",
        "RESTART",
        "EXIT TO MENU"
    };
    
    for (size_t i = 0; i < options.size(); ++i) {
        auto menuItem = std::make_unique<sf::Text>(m_pauseFont);
        menuItem->setString(options[i]);
        menuItem->setCharacterSize(28);
        menuItem->setFillColor(sf::Color::White);
        
        // Center each item
        auto bounds = menuItem->getLocalBounds();
        menuItem->setOrigin(sf::Vector2f(
            bounds.position.x + bounds.size.x * 0.5f,
            bounds.position.y + bounds.size.y * 0.5f
        ));
        menuItem->setPosition(sf::Vector2f(
            Config::WINDOW_WIDTH * 0.5f,
            Config::WINDOW_HEIGHT * 0.5f + i * 60.0f
        ));
        
        m_pauseMenuItems.push_back(std::move(menuItem));
    }
    
    updatePauseMenuDisplay();
}

void PlayState::updatePauseMenuDisplay() {
    const std::vector<std::string> labels = {"CONTINUE", "RESTART", "EXIT TO MENU"};
    
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
    }
}

void PlayState::handlePauseMenuAction() {
    switch (m_pauseSelectedIndex) {
        case 0:  // CONTINUE
            m_isPaused = false;
            std::cout << "[GAME] Resumed" << std::endl;
            break;
            
        case 1:  // RESTART
            restartGame();
            break;
            
        case 2:  // EXIT TO MENU
            std::cout << "[GAME] Exiting to menu..." << std::endl;
            m_game->getStateManager()->popState();
            break;
    }
}

void PlayState::restartGame() {
    std::cout << "[GAME] Restarting level..." << std::endl;
    
    // ✅ SAFE RESTART - Use our saved copy of the track!
    if (m_savedTrack.has_value()) {
        m_gameplayManager = std::make_unique<GameplayManager>(m_currentMode, &m_savedTrack.value());
    } else {
        m_gameplayManager = std::make_unique<GameplayManager>(m_currentMode, nullptr);
    }
    
    // Reset HUD
    m_hud = std::make_unique<GameHUD>();
    
    // Unpause
    m_isPaused = false;
    m_pauseSelectedIndex = 0;
    updatePauseMenuDisplay();
    
    std::cout << "[GAME] Level restarted!" << std::endl;
}

void PlayState::handleInput(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        const auto* key = event.getIf<sf::Event::KeyPressed>();
        
        // SHIFT = Clutch pentru start boost
        if (key->code == sf::Keyboard::Key::LShift || 
            key->code == sf::Keyboard::Key::RShift) {
            m_gameplayManager->handleStartInput(true);
        }
        
        if (key->code == sf::Keyboard::Key::Escape) {
            if (m_isPaused) {
                m_isPaused = false;
                std::cout << "[GAME] Resumed" << std::endl;
            } else {
                m_isPaused = true;
                m_pauseSelectedIndex = 0;
                updatePauseMenuDisplay();
                std::cout << "[GAME] Paused" << std::endl;
            }
            return;
        }
        
        // Handle pause menu navigation
        if (m_isPaused) {
            switch (key->code) {
                case sf::Keyboard::Key::Up:
                case sf::Keyboard::Key::W:
                    m_pauseSelectedIndex = (m_pauseSelectedIndex - 1 + 3) % 3;
                    updatePauseMenuDisplay();
                    break;
                    
                case sf::Keyboard::Key::Down:
                case sf::Keyboard::Key::S:
                    m_pauseSelectedIndex = (m_pauseSelectedIndex + 1) % 3;
                    updatePauseMenuDisplay();
                    break;
                    
                case sf::Keyboard::Key::Enter:
                case sf::Keyboard::Key::Space:
                    handlePauseMenuAction();
                    break;
                    
                default:
                    break;
            }
        }
    }
}

void PlayState::update(float deltaTime) {
    if (!m_isPaused) {
        m_gameplayManager->update(deltaTime);
        m_hud->update(*m_gameplayManager, deltaTime);  // ✅ Adaugă deltaTime!
    }
}

void PlayState::render(sf::RenderWindow& window) {
    m_gameplayManager->render(window);
    m_hud->render(window, *m_gameplayManager);  // ✅ Adaugă gameplay!
    
    // Draw pause overlay
    if (m_isPaused) {
        sf::RectangleShape overlay(sf::Vector2f(
            static_cast<float>(window.getSize().x),
            static_cast<float>(window.getSize().y)
        ));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(overlay);
        
        sf::RectangleShape menuBox(sf::Vector2f(400.0f, 300.0f));
        menuBox.setOrigin(sf::Vector2f(200.0f, 150.0f));
        menuBox.setPosition(sf::Vector2f(
            Config::WINDOW_WIDTH * 0.5f,
            Config::WINDOW_HEIGHT * 0.5f
        ));
        menuBox.setFillColor(sf::Color(30, 30, 30, 230));
        menuBox.setOutlineColor(sf::Color::White);
        menuBox.setOutlineThickness(3.0f);
        window.draw(menuBox);
        
        if (m_pauseTitle) {
            window.draw(*m_pauseTitle);
        }
        
        for (const auto& item : m_pauseMenuItems) {
            if (item) {
                window.draw(*item);
            }
        }
    }
}

void PlayState::onEnter() {
    std::cout << "[PlayState] Entered" << std::endl;
}