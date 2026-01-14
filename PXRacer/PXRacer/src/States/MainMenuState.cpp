#include "MainMenuState.h"
#include "Core/Game.h"
#include "Core/Constants.h"
#include "States/StateManager.h"
#include "States/GameModeSelectState.h"
#include "States/SettingsState.h"
#include "States/CreditsState.h"
#include "../UI/MenuStyle.h"
#include "Core/SettingsManager.h"
#include "Core/AudioManager.h"
#include <iostream>

MainMenuState::MainMenuState(Game* game)
    : State(game)
    , m_selectedIndex(0)
    , m_blinkTimer(0.0f)
    , m_showSelector(true)
{
    //Load background

    m_backgroundTexture = std::make_unique<sf::Texture>();

    if (m_backgroundTexture->loadFromFile(MenuStyle::MENU_BACKGROUND_PATH)) {

        m_backgroundSprite = std::make_unique<sf::Sprite>(*m_backgroundTexture);

        auto textureSize = m_backgroundTexture->getSize();
        auto& settings = SettingsManager::getInstance();
        float scaleX = static_cast<float>(settings.getWindowWidth()) / textureSize.x;
        float scaleY = static_cast<float>(settings.getWindowHeight()) / textureSize.y;
        m_backgroundSprite->setScale(sf::Vector2f(scaleX, scaleY));
    }
    else {
    std::cerr<< "Failed to load background texture!" <<std::endl;
    m_backgroundTexture.reset();
    }

    // Load font
    if (!m_font.openFromFile(MenuStyle::MENU_FONT_PATH)) {
        std::cerr << "Failed to load font in MainMenuState" << std::endl;
    }

    // Initialize title text with font
    m_titleText = std::make_unique<sf::Text>(m_font);
    m_titleText->setString("MAIN MENU");
    m_titleText->setCharacterSize(MenuStyle::getTitleSize());
    m_titleText->setFillColor(MenuStyle::TITLE_COLOR);
    m_titleText->setStyle(sf::Text::Bold);

    sf::FloatRect titleBounds = m_titleText->getLocalBounds();
    m_titleText->setOrigin(sf::Vector2f(titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f));

    m_titleText->setPosition(sf::Vector2f(
        MenuStyle::getTitleXPosition(), 
        MenuStyle::getTitleYPosition()));


    // Menu items
    m_menuOptions = {
        "PLAY",
        "SETTINGS",
        "CREDITS",
        "EXIT"
    };

    for (size_t i = 0; i < m_menuOptions.size(); ++i) {
    auto menuItem = std::make_unique<sf::Text>(m_font);
    menuItem->setString(m_menuOptions[i]);
    menuItem->setCharacterSize(MenuStyle::getMenuItemSize());
    menuItem->setFillColor(MenuStyle::MENU_ITEM_COLOR);
    
    sf::FloatRect bounds = menuItem->getLocalBounds();
    menuItem->setOrigin(sf::Vector2(bounds.size.x / 2.0f, bounds.size.y / 2.0f));
    
    menuItem->setPosition(sf::Vector2f(
        MenuStyle::getMenuItemXPosition(),
        MenuStyle::getMenuItemYStart() + i * MenuStyle::getMenuItemSpacing()
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
            AudioManager::getInstance().playSfx("menu_select");
            updateMenuDisplay();
            break;

        case sf::Keyboard::Key::Down:
            m_selectedIndex = (m_selectedIndex + 1) % m_menuItems.size();
            AudioManager::getInstance().playSfx("menu_select");
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
                }
                break;
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
   if (m_blinkTimer > MenuStyle::BLINK_INTERVAL) {
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
    AudioManager::getInstance().playMusic("main_menu", true);
    m_selectedIndex = 0;
    updateMenuDisplay();
}

void MainMenuState::updateMenuDisplay() {
    for (size_t i = 0; i < m_menuItems.size(); ++i) {
        std::string baseText = m_menuOptions[i];
        
        if (i == m_selectedIndex && m_showSelector) {
            m_menuItems[i]->setFillColor(MenuStyle::MENU_ITEM_SELECTED_COLOR);
            m_menuItems[i]->setString(MenuStyle::SELECTION_PREFIX + baseText);
        }
        else {
            m_menuItems[i]->setFillColor(MenuStyle::MENU_ITEM_COLOR);
            m_menuItems[i]->setString(baseText);
        }
        
        sf::FloatRect bounds = m_menuItems[i]->getLocalBounds();
        m_menuItems[i]->setOrigin(sf::Vector2(bounds.size.x / 2.0f, bounds.size.y / 2.0f));
    }
}
