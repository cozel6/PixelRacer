#include "CreditsState.h"
#include "../Core/SettingsManager.h"
#include "Core/Game.h"
#include "Core/Constants.h"
#include "States/StateManager.h"
#include "Core/AudioManager.h"
#include <iostream>

CreditsState::CreditsState(Game* game)
    : State(game)
{
    if (!m_font.openFromFile("assets/fonts/PressStart2P-Regular.ttf")) {
        std::cerr << "Failed to load font in CreditsState" << std::endl;
    }
    initializeUI();
}

void CreditsState::initializeUI() {
    auto& settings = SettingsManager::getInstance();
    float centerX = settings.getWindowWidth() / 2.0f;

    m_titleText = std::make_unique<sf::Text>(m_font);
    m_titleText->setString("CREDITS");
    m_titleText->setCharacterSize(36);
    m_titleText->setFillColor(sf::Color::White);
    m_titleText->setPosition(sf::Vector2f(centerX - 100.0f, 80.0f));

    std::vector<std::string> credits = {
        "CREATED BY",
        "",
        "COZEL",
        "SERKIY",
        "",
        "Special thanks to:",
        "SFML Team",
        "Beta Testers"
    };

    for (size_t i = 0; i < credits.size(); ++i) {
        auto creditText = std::make_unique<sf::Text>(m_font);
        creditText->setString(credits[i]);
        creditText->setCharacterSize(16);

        if (credits[i] == "COZEL" || credits[i] == "SERKIY") {
            creditText->setFillColor(sf::Color::Yellow);
        }
        else {
            creditText->setFillColor(sf::Color(200, 200, 200));
        }

        creditText->setPosition(sf::Vector2f(centerX - 120.0f, 180.0f + i * 40.0f));
        m_creditTexts.push_back(std::move(creditText));
    }

    m_asciiArt = std::make_unique<sf::Text>(m_font);
    m_asciiArt->setString("  _____\n /     \\\n|  O O  |\n|   ^   |\n|  \\_/  |\n \\_____/");
    m_asciiArt->setCharacterSize(14);
    m_asciiArt->setFillColor(sf::Color::Green);
    float windowHeight = static_cast<float>(settings.getWindowHeight());
    m_asciiArt->setPosition(sf::Vector2f(centerX - 50.0f, windowHeight - 150.0f));
}

void CreditsState::handleInput(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        const auto* key = event.getIf<sf::Event::KeyPressed>();
        if (key->code == sf::Keyboard::Key::Escape) {
            m_game->getStateManager()->popState();
        }
    }
}

void CreditsState::update(float deltaTime) {}

void CreditsState::render(sf::RenderWindow& window) {
    window.clear(sf::Color(25, 25, 35));
    window.draw(*m_titleText);
    for (auto& text : m_creditTexts) {
        window.draw(*text);
    }
    window.draw(*m_asciiArt);
}

void CreditsState::onEnter() {
    std::cout << "Entered Credits State" << std::endl;
    AudioManager::getInstance().playMusic("credits", true);
}