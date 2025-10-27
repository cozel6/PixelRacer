#include "Core/Game.h"
#include "Core/Constants.h"
#include "States/StateManager.h"
#include "States/MenuState.h"
#include <iostream>

Game::Game()
    : m_window(sf::VideoMode(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT), Config::WINDOW_TITLE)
    , m_stateManager(std::make_unique<StateManager>(this))
    , m_accumulator(0.0f)
    , m_isRunning(true)
{
    // Window settings
    m_window.setFramerateLimit(Config::FPS_LIMIT);
    m_window.setVerticalSyncEnabled(Config::VSYNC_ENABLED);

    // Push initial state
    m_stateManager->pushState(std::make_unique<MenuState>(this));

    std::cout << "  PixelRacer initialized" << std::endl;
    std::cout << "   Window: " << Config::WINDOW_WIDTH << "x" << Config::WINDOW_HEIGHT << std::endl;
    std::cout << "   FPS Limit: " << Config::FPS_LIMIT << std::endl;
}

Game::~Game() {
    std::cout << " PixelRacer shutting down..." << std::endl;
}

void Game::run() {
    m_clock.restart();

    while (m_isRunning && m_window.isOpen()) {
        // Calculate delta time
        float deltaTime = m_clock.restart().asSeconds();

        // Prevent spiral of death
        if (deltaTime > Config::MAX_DELTA_TIME) {
            deltaTime = Config::MAX_DELTA_TIME;
        }

        // Process events
        processEvents();

        // Fixed timestep update
        m_accumulator += deltaTime;
        while (m_accumulator >= Config::FIXED_TIMESTEP) {
            update(Config::FIXED_TIMESTEP);
            m_accumulator -= Config::FIXED_TIMESTEP;
        }

        // Render
        render();
    }

    m_window.close();
}

void Game::processEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_isRunning = false;
        }

        // Pass event to current state
        m_stateManager->handleInput(event);
    }
}

void Game::update(float deltaTime) {
    // Update state manager
    m_stateManager->update(deltaTime);

    // Check if we should quit
    if (m_stateManager->isEmpty()) {
        m_isRunning = false;
    }
}

void Game::render() {
    m_window.clear(Config::BACKGROUND_COLOR);

    // Render current state
    m_stateManager->render(m_window);

    m_window.display();
}