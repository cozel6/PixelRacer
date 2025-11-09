#include "SettingsState.h"
#include "../Core/Game.h"
#include "../Core/Constants.h"
#include "../States/StateManager.h"
#include <iostream>

SettingsState::SettingsState(Game *game)
    : State(game), m_selectedIndex(0), m_blinkTimer(0.0f), m_showSelector(true)
{
    // Load font
    if (!m_font.openFromFile("assets/fonts/LiberationMono-Regular.ttf"))
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
    m_titleText->setCharacterSize(60);
    m_titleText->setFillColor(Config::ACCENT_COLOR);
    m_titleText->setStyle(sf::Text::Style::Bold);

    // Create option texts (positioned like MainMenuState)
    for (size_t i = 0; i < m_menuOptions.size(); ++i)
    {
        auto text = std::make_unique<sf::Text>(m_font);
        text->setString(m_menuOptions[i]);
        text->setCharacterSize(32);
        text->setFillColor(Config::TEXT_COLOR);
        text->setPosition(sf::Vector2f(
            Config::WINDOW_WIDTH * 0.5f - 80.0f,
            Config::WINDOW_HEIGHT * 0.5f + i * 50.0f
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
            case 0:
                std::cout << "Screen option selected (placeholder)\n";
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
    if (m_blinkTimer >= 0.3f)
    {
        m_showSelector = !m_showSelector;
        m_blinkTimer = 0.0f;
        updateMenuDisplay();
    }
}

void SettingsState::render(sf::RenderWindow &window)
{
    // Draw background
    window.clear(Config::BACKGROUND_COLOR);

    // Position and draw title
    sf::FloatRect titleBounds = m_titleText->getLocalBounds();
    m_titleText->setOrigin(sf::Vector2f(titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f));
    m_titleText->setPosition(sf::Vector2f(
        Config::WINDOW_WIDTH / 2.0f,
        Config::WINDOW_HEIGHT * 0.2f));
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
        if (i == m_selectedIndex && m_showSelector)
        {
            m_menuTexts[i]->setFillColor(Config::ACCENT_COLOR);
            // Add selection ">"
            std::string currentText = m_menuTexts[i]->getString();
            if (currentText.substr(0, 2) != "> ")
            {
                m_menuTexts[i]->setString("> " + currentText);
            }
        }
        else
        {
            m_menuTexts[i]->setFillColor(Config::TEXT_COLOR);
            // Remove selection ">"
            std::string currentText = m_menuTexts[i]->getString();
            if (currentText.substr(0, 2) == "> ")
            {
                m_menuTexts[i]->setString(currentText.substr(2));
            }
        }
    }
}