#include "MainMenuState.h"
#include "Core/Game.h"
#include "Core/Constants.h"
#include "States/StateManager.h"
#include "States/GameModeSelectState.h" 
#include "States/SettingsState.h"
#include "States/CreditsState.h"
#include <iostream>

MainMenuState::MainMenuState(Game* game)
    : State(game)
    , m_selectedIndex(0)
    , m_blinkTimer(0.0f)
    , m_showSelector(true)
{
    //Load background

    m_backgroundTexture = std::make_unique<sf::Texture>();

    if (m_backgroundTexture->loadFromFile("assets/textures/mainmenubg.png")) {
        m_backgroundSprite = std::make_unique<sf::Sprite>(*m_backgroundTexture);

        auto textureSize = m_backgroundTexture->getSize();
        float scaleX = static_cast<float>(Config::WINDOW_WIDTH) / textureSize.x;
        float scaleY = static_cast<float>(Config::WINDOW_HEIGHT) / textureSize.y;
        m_backgroundSprite->setScale(sf::Vector2f(scaleX, scaleY));
    }
    else {
    std::cerr<< "Failed to load background texture!" <<std::endl;
    m_backgroundTexture.reset();
    }
    // Load font
    if (!m_font.openFromFile("assets/fonts/PressStart2P-Regular.ttf")) {
        std::cerr << "Failed to load font in MainMenuState" << std::endl;
    }

    // Initialize title text with font
    m_titleText = std::make_unique<sf::Text>(m_font);
    m_titleText->setString("MAIN MENU");
    m_titleText->setCharacterSize(60);
    m_titleText->setFillColor(sf::Color(30,30,30));
    m_titleText->setStyle(sf::Text::Bold);
    m_titleText->setPosition(sf::Vector2f(Config::WINDOW_WIDTH * 0.5f - 250.0f, Config::WINDOW_HEIGHT / 6.0f));

    // Menu items
    std::vector<std::string> menuOptions = {
        "PLAY",
        "SETTINGS",
        "CREDITS",
        "EXIT"
    };

    for (size_t i = 0; i < menuOptions.size(); ++i) {
        // Initialize each menu item with font
        auto menuItem = std::make_unique<sf::Text>(m_font);
        menuItem->setString(menuOptions[i]);
        menuItem->setCharacterSize(32);
        menuItem->setFillColor(Config::TEXT_COLOR);
        menuItem->setPosition(sf::Vector2f(
            Config::WINDOW_WIDTH * 0.5f - 100.0f,
            Config::WINDOW_HEIGHT * 0.70f + i * 50.0f
        ));
        m_menuItems.push_back(std::move(menuItem));
    }

    updateMenuDisplay();
}

void MainMenuState::handleInput(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        const auto* key = event.getIf<sf::Event::KeyPressed>();

        switch (key->code) {
        case sf::Keyboard::Key::Up:
            m_selectedIndex = (m_selectedIndex - 1 + m_menuItems.size()) % m_menuItems.size();
            updateMenuDisplay();
            break;

        case sf::Keyboard::Key::Down:
            m_selectedIndex = (m_selectedIndex + 1) % m_menuItems.size();
            updateMenuDisplay();
            break;

        case sf::Keyboard::Key::Enter:
            switch (m_selectedIndex) {
            case 0: // PLAY
                if(key->code == sf::Keyboard::Key::Enter){
                std::cout << "Opening Game Mode Selection..." << std::endl;
                m_game->getStateManager()->pushState(std::make_unique<GameModeSelectState>(m_game));
                }
                break;
            case 1: // SETTINGS
                if (key->code == sf::Keyboard::Key::Enter) {
                    std::cout << "Opening Settings..." << std::endl;
                    m_game->getStateManager()->pushState(std::make_unique<SettingsState>(m_game));
                break;
                }
            case 2: // CREDITS
                if (key->code == sf::Keyboard::Key::Enter) {
                    std::cout << "Opening Credits..." << std::endl;
                    m_game->getStateManager()->pushState(std::make_unique<CreditsState>(m_game));
                break;
                }
            case 3: // EXIT
                m_game->quit();
                break;
            }
            break;

        case sf::Keyboard::Key::Escape:
            // Go back to initial menu
            m_game->getStateManager()->popState();
            break;
        }
    }
}

void MainMenuState::update(float deltaTime) {
    // Blink effect for selector
    m_blinkTimer += deltaTime;
    if (m_blinkTimer > 0.3f) {
        m_showSelector = !m_showSelector;
        m_blinkTimer = 0.0f;
        updateMenuDisplay();
    }
}

void MainMenuState::render(sf::RenderWindow& window) {
    
    if (m_backgroundSprite) {
        window.draw(*m_backgroundSprite);
    }
    // Draw title
    window.draw(*m_titleText);

    // Draw menu items
    for (auto& item : m_menuItems) {
        window.draw(*item);
    }
}

void MainMenuState::onEnter() {
    std::cout << "Entered Main Menu State" << std::endl;
    m_selectedIndex = 0;
    updateMenuDisplay();
}

void MainMenuState::updateMenuDisplay() {
    for (size_t i = 0; i < m_menuItems.size(); ++i) {
        if (i == m_selectedIndex && m_showSelector) {
            m_menuItems[i]->setFillColor(Config::ACCENT_COLOR);
            // Add selector ">"
            std::string currentText = m_menuItems[i]->getString();
            if (currentText.substr(0, 2) != "> ") {
                m_menuItems[i]->setString("> " + currentText);
            }
        }
        else {
            m_menuItems[i]->setFillColor(Config::TEXT_COLOR);
            // Remove selector ">" dacă există
            std::string currentText = m_menuItems[i]->getString();
            if (currentText.substr(0, 2) == "> ") {
                m_menuItems[i]->setString(currentText.substr(2));
            }
        }
    }
}