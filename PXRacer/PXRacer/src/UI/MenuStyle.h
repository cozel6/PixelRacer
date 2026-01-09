#pragma once
#include <SFML/Graphics.hpp>
#include "../Core/Constants.h"

// Global menu styling configuration - DYNAMIC based on resolution
namespace MenuStyle {
    // Font paths
    static const char* MENU_FONT_PATH = "assets/fonts/PressStart2P-Regular.ttf";
    
    // Background
    static const char* MENU_BACKGROUND_PATH = "assets/textures/mainmenubg.png";
    
    // Title size scales with resolution
    static unsigned int getTitleSize() {
        return static_cast<unsigned int>(Config::WINDOW_HEIGHT * 0.08f);  // 8% of screen height
    }
    
    static const sf::Color TITLE_COLOR = sf::Color(30, 30, 30);  // Dark gray/black
    
    // Title position - centered horizontally, 1/6 down vertically
    static float getTitleYPosition() {
        return Config::WINDOW_HEIGHT / 6.0f;
    }
    
    static float getTitleXPosition() {
        return Config::WINDOW_WIDTH * 0.5f;  // Centered
    }
    
    // Menu item size scales with resolution
    static unsigned int getMenuItemSize() {
        return static_cast<unsigned int>(Config::WINDOW_HEIGHT * 0.04f);  // 4% of screen height
    }
    
    static const sf::Color MENU_ITEM_COLOR = Config::TEXT_COLOR;
    static const sf::Color MENU_ITEM_SELECTED_COLOR = Config::ACCENT_COLOR;
    
    // Menu items start position
    static float getMenuItemYStart() {
        return Config::WINDOW_HEIGHT * 0.55f;  // 55% down the screen
    }
    
    static float getMenuItemXPosition() {
        return Config::WINDOW_WIDTH * 0.5f;  // Centered
    }
    
    // Spacing between menu items (scales with resolution)
    static float getMenuItemSpacing() {
        return Config::WINDOW_HEIGHT * 0.07f;  // 7% of screen height
    }
    
    static const char* SELECTION_PREFIX = "> ";
    static const float BLINK_INTERVAL = 0.3f;  // Seconds
    
    // Use this if you want text left-aligned instead of centered
    static float getLeftAlignedXOffset() {
        return -Config::WINDOW_WIDTH * 0.08f;  // 8% to the left of center
    }
}
