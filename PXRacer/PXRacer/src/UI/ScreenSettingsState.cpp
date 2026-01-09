#include "ScreenSettingsState.h"
#include "Core/Game.h"
#include "Core/Constants.h"
#include "Core/SettingsManager.h"
#include "States/StateManager.h"
#include "UI/MenuStyle.h"
#include <iostream>

// Constructor - Initializes the screen settings menu
ScreenSettingsState::ScreenSettingsState(Game* game)
    : State(game)
    , m_selectedIndex(0)
    , m_blinkTimer(0.0f)
    , m_showSelector(true)
{
    auto& settings = SettingsManager::getInstance();
    
    // Load current settings
    m_resolutionIndex = settings.getCurrentResolutionIndex();
    m_fullscreenEnabled = settings.isFullscreen();

    // Load background
    m_backgroundTexture = std::make_unique<sf::Texture>();
    if (m_backgroundTexture->loadFromFile(MenuStyle::MENU_BACKGROUND_PATH)) {
        m_backgroundSprite = std::make_unique<sf::Sprite>(*m_backgroundTexture);
        
        auto textureSize = m_backgroundTexture->getSize();
        float scaleX = static_cast<float>(settings.getWindowWidth()) / textureSize.x;
        float scaleY = static_cast<float>(settings.getWindowHeight()) / textureSize.y;
        m_backgroundSprite->setScale(sf::Vector2f(scaleX, scaleY));
    } else {
        std::cerr << "Failed to load background texture!" << std::endl;
    }

    // Load font
    if (!m_font.openFromFile(MenuStyle::MENU_FONT_PATH)) {
        std::cerr << "Failed to load font!" << std::endl;
    }

    // Create title
    m_titleText = std::make_unique<sf::Text>(m_font, sf::String());
    m_titleText->setString("SCREEN SETTINGS");
    m_titleText->setCharacterSize(MenuStyle::getTitleSize());
    m_titleText->setFillColor(MenuStyle::TITLE_COLOR);

    auto titleBounds = m_titleText->getLocalBounds();
    m_titleText->setOrigin(sf::Vector2f(titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f));
    m_titleText->setPosition(sf::Vector2f(MenuStyle::getTitleXPosition(), MenuStyle::getTitleYPosition()));

    // Create menu options
    m_menuOptions = {
        "Resolution: ",
        "Fullscreen: ",
        "Apply",
        "Back"
    };

    // Create menu texts
    for (size_t i = 0; i < m_menuOptions.size(); ++i) {
        auto text = std::make_unique<sf::Text>(m_font, sf::String());
        text->setCharacterSize(MenuStyle::getMenuItemSize());
        text->setFillColor(MenuStyle::MENU_ITEM_COLOR);
        
        float yPos = MenuStyle::getMenuItemYStart() + (i * MenuStyle::getMenuItemSpacing());
        text->setPosition(sf::Vector2f(MenuStyle::getMenuItemXPosition(), yPos));
        
        m_menuTexts.push_back(std::move(text));
    }

    updateMenuDisplay();
}

// Handles user input
void ScreenSettingsState::handleInput(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        const auto* key = event.getIf<sf::Event::KeyPressed>();

        switch (key->code) {
            case sf::Keyboard::Key::Up:
                m_selectedIndex = (m_selectedIndex - 1 + static_cast<int>(m_menuOptions.size())) % m_menuOptions.size();
                m_showSelector = true;
                m_blinkTimer = 0.0f;
                updateMenuDisplay();
                break;

            case sf::Keyboard::Key::Down:
                m_selectedIndex = (m_selectedIndex + 1) % m_menuOptions.size();
                m_showSelector = true;
                m_blinkTimer = 0.0f;
                updateMenuDisplay();
                break;

            case sf::Keyboard::Key::Left:
                if (m_selectedIndex == 0) {  // Resolution
                    const auto& resolutions = SettingsManager::AVAILABLE_RESOLUTIONS;
                    m_resolutionIndex = (m_resolutionIndex - 1 + static_cast<int>(resolutions.size())) % resolutions.size();
                    updateMenuDisplay();
                }
                else if (m_selectedIndex == 1) {  // Fullscreen
                    m_fullscreenEnabled = !m_fullscreenEnabled;
                    updateMenuDisplay();
                }
                break;

            case sf::Keyboard::Key::Right:
                if (m_selectedIndex == 0) {  // Resolution
                    const auto& resolutions = SettingsManager::AVAILABLE_RESOLUTIONS;
                    m_resolutionIndex = (m_resolutionIndex + 1) % resolutions.size();
                    updateMenuDisplay();
                }
                else if (m_selectedIndex == 1) {  // Fullscreen
                    m_fullscreenEnabled = !m_fullscreenEnabled;
                    updateMenuDisplay();
                }
                break;

            case sf::Keyboard::Key::Enter:
                if (m_selectedIndex == 2) {  // Apply
                    applySettings();
                }
                else if (m_selectedIndex == 3) {  // Back
                    m_game->getStateManager()->popState();
                }
                break;

            case sf::Keyboard::Key::Escape:
                m_game->getStateManager()->popState();
                break;

            default:
                break;
        }
    }
}

// Updates the screen settings state
void ScreenSettingsState::update(float deltaTime) {
    m_blinkTimer += deltaTime;

    if (m_blinkTimer >= MenuStyle::BLINK_INTERVAL) {
        m_showSelector = !m_showSelector;
        m_blinkTimer = 0.0f;
        updateMenuDisplay();
    }
}

// Renders the screen settings state
void ScreenSettingsState::render(sf::RenderWindow& window) {
    if (m_backgroundSprite) {
        window.draw(*m_backgroundSprite);
    }

    window.draw(*m_titleText);

    for (const auto& text : m_menuTexts) {
        window.draw(*text);
    }
}

// Called when entering the state
void ScreenSettingsState::onEnter() {
    std::cout << "Entered Screen Settings State" << std::endl;
}

// Updates the menu display with current values
void ScreenSettingsState::updateMenuDisplay() {
    const auto& resolutions = SettingsManager::AVAILABLE_RESOLUTIONS;

    for (size_t i = 0; i < m_menuTexts.size(); ++i) {
        std::string displayText = m_menuOptions[i];

        // Add current values for settings
        if (i == 0) {  // Resolution
            displayText += resolutions[m_resolutionIndex].name;
        }
        else if (i == 1) {  // Fullscreen
            displayText += m_fullscreenEnabled ? "ON" : "OFF";
        }

        // Add selector prefix if selected
        if (i == m_selectedIndex && m_showSelector) {
            m_menuTexts[i]->setFillColor(MenuStyle::MENU_ITEM_SELECTED_COLOR);
            m_menuTexts[i]->setString(MenuStyle::SELECTION_PREFIX + displayText);
        } else {
            m_menuTexts[i]->setFillColor(MenuStyle::MENU_ITEM_COLOR);
            m_menuTexts[i]->setString(displayText);
        }

        // Recalculate origin after changing text
        sf::FloatRect bounds = m_menuTexts[i]->getLocalBounds();
        m_menuTexts[i]->setOrigin(sf::Vector2f(bounds.size.x / 2.0f, bounds.size.y / 2.0f));
    }
}

// Applies the settings and recreates the window
void ScreenSettingsState::applySettings() {
    auto& settings = SettingsManager::getInstance();

    std::cout << "Applying settings..." << std::endl;
    std::cout << "Resolution: " << SettingsManager::AVAILABLE_RESOLUTIONS[m_resolutionIndex].name << std::endl;
    std::cout << "Fullscreen: " << (m_fullscreenEnabled ? "ON" : "OFF") << std::endl;

    // Update settings
    settings.setResolution(m_resolutionIndex);
    settings.setFullscreen(m_fullscreenEnabled);

    // Apply settings (saves to file and recreates window)
    m_game->applySettings();

    // Note: After applySettings(), the window is recreated and all states are cleared
    // We will automatically return to MenuState
}
