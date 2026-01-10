#include "Core/Game.h"
#include "Core/Constants.h"
#include "States/StateManager.h"
#include "States/MenuState.h"
#include "Core/SettingsManager.h"
#include <iostream>

Game::Game()
    : m_stateManager(std::make_unique<StateManager>(this)), m_accumulator(0.0f), m_isRunning(true)
{
    // Load settings first
    SettingsManager::getInstance().loadFromFile();

    // Create window with loaded settings
    initializeWindow();

    // Push initial state
    m_stateManager->pushState(std::make_unique<MenuState>(this));

    auto &settings = SettingsManager::getInstance();
    std::cout << " PixelRacer initialized" << std::endl;
    std::cout << " Window: " << settings.getWindowWidth() << "x" << settings.getWindowHeight() << std::endl;
    std::cout << " FPS Limit: " << Config::FPS_LIMIT << std::endl;
}

Game::~Game()
{
    std::cout << " PixelRacer shutting down..." << std::endl;
}

void Game::run() {
    std::cout << "Starting game loop..." << std::endl;
    
    while (m_isRunning && m_window.isOpen()) {
        // Process events
        processEvents();
        
        // Fixed timestep update
        float deltaTime = m_clock.restart().asSeconds();
        m_accumulator += deltaTime;
        
        const float fixedDeltaTime = 1.0f / Config::FPS_LIMIT;
        
        while (m_accumulator >= fixedDeltaTime) {
            update(fixedDeltaTime);
            m_accumulator -= fixedDeltaTime;
        }
        
        // Render
        render();
    }
    
    std::cout << "Game loop ended." << std::endl;
}

void Game::processEvents() {
    while (auto event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_isRunning = false;
            m_window.close();
        }
        
        // Pass events to state manager
        m_stateManager->handleInput(*event);
    }
}

void Game::update(float deltaTime)
{
    // Update state manager
    m_stateManager->update(deltaTime);

    // Check if we should quit
    if (m_stateManager->isEmpty())
    {
        m_isRunning = false;
    }
}

void Game::render()
{
    m_window.clear(Config::BACKGROUND_COLOR);

    // Render current state
    m_stateManager->render(m_window);

    m_window.display();
}

void Game::initializeWindow() {
    auto& settings = SettingsManager::getInstance();

    // Determine window style based on fullscreen setting
    sf::State windowState = settings.isFullscreen()
        ? sf::State::Fullscreen
        : sf::State::Windowed;

    // Create window with settings dimensions
    m_window.create(
        sf::VideoMode({settings.getWindowWidth(), settings.getWindowHeight()}),
        Config::WINDOW_TITLE,
        windowState
    );

    // Apply window settings
    m_window.setFramerateLimit(Config::FPS_LIMIT);
    m_window.setVerticalSyncEnabled(Config::VSYNC_ENABLED);
}

void Game::recreateWindow() {
    auto& settings = SettingsManager::getInstance();

    // Get new resolution
    int newWidth = settings.getWindowWidth();
    int newHeight = settings.getWindowHeight();
    bool isFullscreen = settings.isFullscreen();

    // Check if we need to toggle fullscreen
    // Note: SFML doesn't have isFullscreen(), so we track it via SettingsManager
    bool wasFullscreen = (m_window.getSize().x == sf::VideoMode::getDesktopMode().size.x &&
                          m_window.getSize().y == sf::VideoMode::getDesktopMode().size.y);

    // Resize window instead of recreating (when possible)
    if (isFullscreen != wasFullscreen) {
        // Fullscreen toggle requires window recreation (SFML limitation)
        m_window.close();

        sf::State windowStyle = isFullscreen ? sf::State::Fullscreen : sf::State::Windowed;
        m_window.create(
            sf::VideoMode({static_cast<unsigned int>(newWidth), static_cast<unsigned int>(newHeight)}),
            Config::WINDOW_TITLE,
            windowStyle
        );

        // Apply window settings
        m_window.setFramerateLimit(Config::FPS_LIMIT);
        m_window.setVerticalSyncEnabled(Config::VSYNC_ENABLED);
    } else {
        // Just resize for windowed mode (preserves game state)
        m_window.setSize(sf::Vector2u(newWidth, newHeight));

        // Center the window on screen
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        int posX = (desktop.size.x - newWidth) / 2;
        int posY = (desktop.size.y - newHeight) / 2;
        m_window.setPosition(sf::Vector2i(posX, posY));
    }

    // DO NOT clear states - let the game continue!
}

void Game::applySettings() {
    // Save settings to file
    SettingsManager::getInstance().saveToFile();

    // Recreate window with new settings
    recreateWindow();
}