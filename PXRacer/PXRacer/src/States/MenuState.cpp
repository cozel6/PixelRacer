#include "MenuState.h"
#include "Core/Game.h"
#include "Core/Constants.h"
#include "States/StateManager.h"
#include "States/MainMenuState.h"
#include <iostream>

MenuState::MenuState(Game* game)
    : State(game)
    , m_blinkTimer(0.0f)
    , m_showStartText(true)
{
    // Load font
    if (!m_font.openFromFile("assets/fonts/LiberationMono-Regular.ttf")) {
        std::cerr << "Failed to load font, using default" << std::endl;
    }

    // Initialize texts with font - CORECT pentru SFML 3.0
    m_titleText = std::make_unique<sf::Text>(m_font);
    m_startText = std::make_unique<sf::Text>(m_font);
    m_versionText = std::make_unique<sf::Text>(m_font);

    // Title text
    m_titleText->setString("PIXELRACER");
    m_titleText->setCharacterSize(80);
    m_titleText->setFillColor(Config::ACCENT_COLOR);
    m_titleText->setStyle(sf::Text::Bold);
    m_titleText->setPosition(sf::Vector2f(Config::WINDOW_WIDTH * 0.5f - 200.0f, Config::WINDOW_HEIGHT / 3.0f));

    // Start text
    m_startText->setString("Press ENTER to Start");
    m_startText->setCharacterSize(24);
    m_startText->setFillColor(Config::TEXT_COLOR);
    m_startText->setPosition(sf::Vector2f(Config::WINDOW_WIDTH * 0.5f - 120.0f, Config::WINDOW_HEIGHT * 0.66f));

    // Version text
    m_versionText->setString("Alpha0.1 - Development Build");
    m_versionText->setCharacterSize(16);
    m_versionText->setFillColor(sf::Color(150, 150, 150));
    m_versionText->setPosition(sf::Vector2f(10.0f, Config::WINDOW_HEIGHT - 30.0f));
}

void MenuState::handleInput(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        const auto* key = event.getIf<sf::Event::KeyPressed>();
        if (key->code == sf::Keyboard::Key::Enter) {
            std::cout << "Moving to Main Menu..." << std::endl;
            m_game->getStateManager()->changeState(std::make_unique<MainMenuState>(m_game));
        }
        else if (key->code == sf::Keyboard::Key::Escape) {
            std::cout << "Exiting game..." << std::endl;
            m_game->quit();
        }
    }
}

void MenuState::update(float deltaTime) {
    m_blinkTimer += deltaTime;
    if (m_blinkTimer > 0.5f) {
        m_showStartText = !m_showStartText;
        m_blinkTimer = 0.0f;
    }
}

void MenuState::render(sf::RenderWindow& window) {
    // Nu mai este nevoie să setăm fontul aici, deoarece este deja setat în constructor

    // Draw title
    window.draw(*m_titleText);

    // Draw blinking start text
    if (m_showStartText) {
        window.draw(*m_startText);
    }

    // Draw version
    window.draw(*m_versionText);
}

void MenuState::onEnter() {
    std::cout << "Entered Menu State" << std::endl;
}