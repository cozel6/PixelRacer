#pragma once
#include <SFML/Graphics.hpp>
#include "../Core/Constants.h"
#include "../Core/SettingsManager.h"

// Global menu styling configuration - DYNAMIC based on resolution
namespace MenuStyle
{
    // Font paths
    static const char *MENU_FONT_PATH = "assets/fonts/PressStart2P-Regular.ttf";

    // Background
    static const char *MENU_BACKGROUND_PATH = "assets/textures/mainmenubg.png";

    // Title size scales with resolution
    static unsigned int getTitleSize()
    {
        return static_cast<unsigned int>(SettingsManager::getInstance().getWindowHeight() * 0.08f);
    }

    static const sf::Color TITLE_COLOR = sf::Color(30, 30, 30);  // Dark gray/black

    // Title position
    static float getTitleYPosition()
    {
        return SettingsManager::getInstance().getWindowHeight() / 6.0f;
    }

    static float getTitleXPosition()
    {
        return SettingsManager::getInstance().getWindowWidth() * 0.5f;
    }

    // Menu item size
    static unsigned int getMenuItemSize()
    {
        return static_cast<unsigned int>(SettingsManager::getInstance().getWindowHeight() * 0.04f);
    }

    static const sf::Color MENU_ITEM_COLOR = Config::TEXT_COLOR;
    static const sf::Color MENU_ITEM_SELECTED_COLOR = Config::ACCENT_COLOR;

    // Menu items start position
    static float getMenuItemYStart()
    {
        return SettingsManager::getInstance().getWindowHeight() * 0.55f;
    }

    static float getMenuItemXPosition()
    {
        return SettingsManager::getInstance().getWindowWidth() * 0.5f;
    }

    // Spacing between menu items
    static float getMenuItemSpacing()
    {
        return SettingsManager::getInstance().getWindowHeight() * 0.07f;
    }

    // Left-aligned offset
    static float getLeftAlignedXOffset()
    {
        return -SettingsManager::getInstance().getWindowWidth() * 0.08f;
    }

    static const char* SELECTION_PREFIX = "> ";
    static const float BLINK_INTERVAL = 0.3f;  
}
