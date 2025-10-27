#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

// Forward declarations
class StateManager;

class Game {
public:
    Game();
    ~Game();

    // Main game loop
    void run();

    // Getters
    sf::RenderWindow& getWindow() { return m_window; }
    const sf::RenderWindow& getWindow() const { return m_window; }

    // Game control
    void quit() { m_isRunning = false; }

private:
    void processEvents();
    void update(float deltaTime);
    void render();

    // Core components
    sf::RenderWindow m_window;
    std::unique_ptr<StateManager> m_stateManager;

    // Timing
    sf::Clock m_clock;
    float m_accumulator;

    // State
    bool m_isRunning;
};
