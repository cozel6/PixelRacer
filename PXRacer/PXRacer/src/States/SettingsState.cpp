#include "SettingsState.h"
#include "Core/Game.h"
#include "Core/Constants.h"
#include "States/StateManager.h"
#include <iostream>

SettingsState::SettingsState(Game* game)
    : State(game)
{
    if (!m_font.openFromFile("assets/fonts/PressStart2P-Regular.ttf")) {
        std::cerr << "Failed to load font in SettingsState" << std::endl;
    }
    initializeUI();
}

void SettingsState::initializeUI() {
    m_titleText = std::make_unique<sf::Text>(m_font);
    m_titleText->setString("SETTINGS");
    m_titleText->setCharacterSize(36);
    m_titleText->setFillColor(sf::Color::White);
    m_titleText->setPosition(sf::Vector2f(Config::WINDOW_WIDTH * 0.5f - 150.0f, 100.0f));

    m_instructionText = std::make_unique<sf::Text>(m_font);
    m_instructionText->setString("Settings under construction!\n\nComing soon:\n- Graphics\n- Sound\n- Controls\n\nPress ESC to return");
    m_instructionText->setCharacterSize(16);
    m_instructionText->setFillColor(sf::Color(200, 200, 200));
    m_instructionText->setPosition(sf::Vector2f(Config::WINDOW_WIDTH * 0.5f - 200.0f, 200.0f));
}

void SettingsState::handleInput(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        const auto* key = event.getIf<sf::Event::KeyPressed>();
        if (key->code == sf::Keyboard::Key::Escape) {
            m_game->getStateManager()->popState();
        }
    }
}

void SettingsState::update(float deltaTime) {}

void SettingsState::render(sf::RenderWindow& window) {
    window.clear(sf::Color(40, 40, 60));
    window.draw(*m_titleText);
    window.draw(*m_instructionText);
}

void SettingsState::onEnter() {
    std::cout << "Entered Settings State" << std::endl;
}