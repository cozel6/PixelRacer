#include "SettingsState.h"
#include "../Core/Game.h"
#include "../Core/Constants.h"
#include "../States/StateManager.h"
#include <iostream>
#include "../UI/MenuStyle.h"
#include "../UI/ScreenSettingsState.h"


SettingsState::SettingsState(Game *game)
    : State(game), m_selectedIndex(0), m_blinkTimer(0.0f), m_showSelector(true)
{
    // Load background
    m_backgroundTexture = std::make_unique<sf::Texture>();
    
    // Load background texture
    if (m_backgroundTexture->loadFromFile(MenuStyle::MENU_BACKGROUND_PATH)){
        // Create background sprite
        m_backgroundSprite = std::make_unique<sf::Sprite>(*m_backgroundTexture);

        // Optionally, scale background to fit window
        auto textureSize = m_backgroundTexture->getSize();
        float scaleX = static_cast<float>(Config::WINDOW_WIDTH) / textureSize.x;
        float scaleY = static_cast<float>(Config::WINDOW_HEIGHT) / textureSize.y;
        m_backgroundSprite->setScale(sf::Vector2f(scaleX,scaleY));
    }
    else{
         std::cerr << "Failed to load background texture!" << std::endl;
         m_backgroundTexture.reset();
    }

    // Load font
    if (!m_font.openFromFile(MenuStyle::MENU_FONT_PATH))
    {
        std::cerr << "Failed to load font!" << std::endl;
    }

    // Define menu options
    m_menuOptions = {
        "SCREEN",
        "SOUNDS",
        "MUSIC",
        "CONTROLS",
        "RETURN"};

    // Create title text
    m_titleText = std::make_unique<sf::Text>(m_font);
    m_titleText->setString("SETTINGS");
    m_titleText->setCharacterSize(MenuStyle::getTitleSize());
    m_titleText->setFillColor(MenuStyle::TITLE_COLOR);
    m_titleText->setStyle(sf::Text::Style::Bold);


    // Create option texts (positioned like MainMenuState)
    for (size_t i = 0; i < m_menuOptions.size(); ++i)
    {
        auto text = std::make_unique<sf::Text>(m_font);
        text->setCharacterSize(MenuStyle::getMenuItemSize());
        text->setFillColor(MenuStyle::MENU_ITEM_COLOR);

        sf::FloatRect bounds = text->getLocalBounds();
        text->setOrigin(sf::Vector2f(bounds.size.x / 2.0f, bounds.size.y / 2.0f));
        text->setPosition(sf::Vector2f(
            MenuStyle::getMenuItemXPosition(),
            MenuStyle::getMenuItemYStart() + i * MenuStyle::getMenuItemSpacing()
        ));

        m_menuTexts.push_back(std::move(text));
    }
    updateMenuDisplay();
}
// Lifecycle hook
void SettingsState::onEnter()
{
    std::cout << "Entered SettingsState" << std::endl;
}
// Core state functions
void SettingsState::handleInput(const sf::Event &event)
{
    // Check for key pressed event
    if (event.is<sf::Event::KeyPressed>())
    {
        const auto *keyEvent = event.getIf<sf::Event::KeyPressed>();
        if (!keyEvent)
            return;

        switch (keyEvent->code)
        {
        case sf::Keyboard::Key::Up:
            // Move up in the menu
            if (m_selectedIndex > 0)
                m_selectedIndex--;
            else
                m_selectedIndex = m_menuOptions.size() - 1;
            m_blinkTimer = 0.0f;
            m_showSelector = true;
            updateMenuDisplay();
            break;

        case sf::Keyboard::Key::Down:
            // Move down in the menu
            if (m_selectedIndex < m_menuOptions.size() - 1)
                m_selectedIndex++;
            else
                m_selectedIndex = 0;
            m_blinkTimer = 0.0f;
            m_showSelector = true;
            updateMenuDisplay();
            break;

        case sf::Keyboard::Key::Enter:
            // Execute action for selected option
            switch (m_selectedIndex)
            {
            case 0:  // SCREEN
                std::cout << "Opening Screen Settings..." << std::endl;
                m_game->getStateManager()->pushState(std::make_unique<ScreenSettingsState>(m_game));
                break;
            case 1:
                std::cout << "Sounds option selected (placeholder)\n";
                break;
            case 2:
                std::cout << "Music option selected (placeholder)\n";
                break;
            case 3:
                std::cout << "Controls option selected (placeholder)\n";
                break;
            case 4:
                m_game->getStateManager()->popState(); // Back to MainMenu
                break;
            }
            break;

        case sf::Keyboard::Key::Escape:
            // Escape = back to MainMenu
            m_game->getStateManager()->popState();
            break;

        default:
            break;
        }
    }
}

void SettingsState::update(float deltaTime)
{
    // Update timer for selector animation
    m_blinkTimer += deltaTime;
    if (m_blinkTimer >= MenuStyle::BLINK_INTERVAL)
    {
        m_showSelector = !m_showSelector;
        m_blinkTimer = 0.0f;
        updateMenuDisplay();
    }
}

void SettingsState::render(sf::RenderWindow &window)
{
    // Draw background image
    if(m_backgroundSprite) {
        window.draw(*m_backgroundSprite);
    }

    sf::FloatRect titleBounds = m_titleText->getLocalBounds();
    m_titleText->setOrigin(sf::Vector2f(titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f));
    m_titleText->setPosition(sf::Vector2f(
    MenuStyle::getTitleXPosition(),
    MenuStyle::getTitleYPosition()));
    window.draw(*m_titleText);

    // Draw menu options (positioned in constructor, just draw here)
    for (auto& text : m_menuTexts)
    {
        window.draw(*text);
    }
}
void SettingsState::updateMenuDisplay()
{
    for (size_t i = 0; i < m_menuTexts.size(); ++i)
    {
        // Get the base text (without selector)
        std::string baseText = m_menuOptions[i];
        
        if (i == m_selectedIndex && m_showSelector)
        {
            // Selected item - add selector and use accent color
            m_menuTexts[i]->setFillColor(MenuStyle::MENU_ITEM_SELECTED_COLOR);
            m_menuTexts[i]->setString(MenuStyle::SELECTION_PREFIX + baseText);
        }
        else
        {
            // Not selected - no selector, normal color
            m_menuTexts[i]->setFillColor(MenuStyle::MENU_ITEM_COLOR);
            m_menuTexts[i]->setString(baseText);
        }
        
        sf::FloatRect bounds = m_menuTexts[i]->getLocalBounds();
        m_menuTexts[i]->setOrigin(sf::Vector2(bounds.size.x / 2.0f, bounds.size.y / 2.0f));
    }
}
