#pragma once
#include <SFML/Graphics.hpp>

namespace Config {
    // Window Settings
    constexpr unsigned int WINDOW_WIDTH = 1280;
    constexpr unsigned int WINDOW_HEIGHT = 720;
    constexpr const char* WINDOW_TITLE = "PixelRacer - Alpha 0.1";
    constexpr unsigned int FPS_LIMIT = 60;
    constexpr bool VSYNC_ENABLED = true;

    // Game Loop
    constexpr float FIXED_TIMESTEP = 1.0f / 60.0f; // 60 FPS physics
    constexpr float MAX_DELTA_TIME = 0.25f; // Prevent spiral of death

    // Isometric Settings
    constexpr float ISO_TILE_WIDTH = 64.0f;
    constexpr float ISO_TILE_HEIGHT = 32.0f;
    constexpr float ISO_RATIO = ISO_TILE_HEIGHT / ISO_TILE_WIDTH;
    constexpr float ISO_SCALE = 1.0f;

    // Physics
    constexpr float GRAVITY = 9.81f;
    constexpr float FRICTION = 0.95f;

    // Player Settings
    constexpr float PLAYER_ACCELERATION = 300.0f;
    constexpr float PLAYER_MAX_SPEED = 500.0f;
    constexpr float PLAYER_TURN_SPEED = 200.0f;
    constexpr float PLAYER_BRAKE_FORCE = 400.0f;

    // Colors
    const sf::Color BACKGROUND_COLOR = sf::Color(40, 44, 52);
    const sf::Color TEXT_COLOR = sf::Color::White;
    const sf::Color ACCENT_COLOR = sf::Color(255, 100, 0);
}

// Utility functions for isometric conversion
namespace IsoUtils {
    // Convert 2D coordinates to isometric screen position
    inline sf::Vector2f toIsometric(float x, float y) {
        float isoX = (x - y) * (Config::ISO_TILE_WIDTH / 2.0f);
        float isoY = (x + y) * (Config::ISO_TILE_HEIGHT / 2.0f);
        return sf::Vector2f(isoX, isoY);
    }

    // Convert isometric screen position back to 2D coordinates
    inline sf::Vector2f fromIsometric(float isoX, float isoY) {
        float x = (isoX / (Config::ISO_TILE_WIDTH / 2.0f) + isoY / (Config::ISO_TILE_HEIGHT / 2.0f)) / 2.0f;
        float y = (isoY / (Config::ISO_TILE_HEIGHT / 2.0f) - isoX / (Config::ISO_TILE_WIDTH / 2.0f)) / 2.0f;
        return sf::Vector2f(x, y);
    }
}
