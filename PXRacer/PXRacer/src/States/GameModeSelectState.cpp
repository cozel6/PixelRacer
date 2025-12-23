
#include "GameModeSelectState.h"
#include "Core/Game.h"
#include "Core/Constants.h"
#include "States/PlayState.h"
#include "States/TrackSelectState.h"
#include "States/StateManager.h"
#include <iostream>

GameModeSelectState::GameModeSelectState(Game* game)
    : State(game) {

    //Load font
    if (!m_font.openFromFile("assets/fonts/PressStart2P-Regular.ttf")) {
        std::cerr << "Failed to load in GameModeSelectState" << std::endl;
    }

    initializeGameModes();
}

void GameModeSelectState::initializeGameModes() {

    float sectionWidth = Config::WINDOW_WIDTH;
    float sectionHeight = Config::WINDOW_HEIGHT / 3.0f;

    //Section 1: ENDLESS MODE
    GameModeSection endless;

    endless.name = "ENDLESS DRIVE";
    endless.description = "Set new distance records without damaging your car!";
    endless.sectionX = 0.0f;
    endless.sectionY = 0.0f;
    endless.sectionWidth = sectionWidth;
    endless.sectionHeight = sectionHeight;
    endless.isHovered = false;

    endless.titleText = std::make_unique<sf::Text>(m_font);
    endless.titleText->setString(endless.name);
    endless.titleText->setCharacterSize(36);
    endless.titleText->setFillColor(sf::Color::White);
    endless.titleText->setPosition(sf::Vector2f(
        sectionWidth * 0.5f - 230.0f,
        sectionHeight * 0.45f
    ));

    endless.descText = std::make_unique<sf::Text>(m_font);
    endless.descText->setString(endless.description);
    endless.descText->setCharacterSize(20);
    endless.descText->setFillColor(sf::Color(200, 200, 200));
    endless.descText->setPosition(sf::Vector2f(
        sectionWidth * 0.5f - 530.0f,
        sectionHeight * 0.65f
    ));

    m_gameModes.push_back(std::move(endless));

    //Section 2: CAMPAIGN MODE
    GameModeSection campaign;
    campaign.name = "CAMPAIGN";
    campaign.description = "Complete challenging levels\nUnlock new cars and tracks!";
    campaign.sectionX = 0.0f;
    campaign.sectionY = sectionHeight;
    campaign.sectionWidth = sectionWidth;
    campaign.sectionHeight = sectionHeight;
    campaign.isHovered = false;

    campaign.titleText = std::make_unique<sf::Text>(m_font);
    campaign.titleText->setString(campaign.name);
    campaign.titleText->setCharacterSize(36);
    campaign.titleText->setFillColor(sf::Color::White);
    campaign.titleText->setPosition(sf::Vector2f(
        sectionWidth * 0.5f - 140.0f,
        sectionHeight * 1.3f
    ));

    campaign.descText = std::make_unique<sf::Text>(m_font);
    campaign.descText->setString(campaign.description);
    campaign.descText->setCharacterSize(20);
    campaign.descText->setFillColor(sf::Color(200, 200, 200));
    campaign.descText->setPosition(sf::Vector2f(
        sectionWidth * 0.5f - 270.0f,
        sectionHeight * 1.5f
    ));

    m_gameModes.push_back(std::move(campaign));

    // Section 3: TIME TRIAL MODE
    GameModeSection timeTrial;
    timeTrial.name = "TIME TRIAL";
    timeTrial.description = "Race against the clock\nCompete for best times!";
    timeTrial.sectionX = 0.0f;
    timeTrial.sectionY = sectionHeight * 2;
    timeTrial.sectionWidth = sectionWidth;
    timeTrial.sectionHeight = sectionHeight;
    timeTrial.isHovered = false;

    timeTrial.titleText = std::make_unique<sf::Text>(m_font);
    timeTrial.titleText->setString(timeTrial.name);
    timeTrial.titleText->setCharacterSize(36);
    timeTrial.titleText->setFillColor(sf::Color::White);
    timeTrial.titleText->setPosition(sf::Vector2f(
        sectionWidth * 0.5f - 175.0f,
        sectionHeight * 2.3f
    ));

    timeTrial.descText = std::make_unique<sf::Text>(m_font);
    timeTrial.descText->setString(timeTrial.description);
    timeTrial.descText->setCharacterSize(20);
    timeTrial.descText->setFillColor(sf::Color(200, 200, 200));
    timeTrial.descText->setPosition(sf::Vector2f(
        sectionWidth * 0.5f - 240.0f,
        sectionHeight * 2.5f
    ));

    m_gameModes.push_back(std::move(timeTrial));
}

void GameModeSelectState::handleInput(const sf::Event& event) {
    if (event.is<sf::Event::MouseMoved>()) {
        const auto* mouseEvent = event.getIf<sf::Event::MouseMoved>();
        updateHover(sf::Vector2f(static_cast<float>(mouseEvent->position.x),
            static_cast<float>(mouseEvent->position.y)));
    }
    else if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos(static_cast<float>(mouseEvent->position.x),
                static_cast<float>(mouseEvent->position.y));

            for (size_t i = 0; i < m_gameModes.size(); ++i) {
                if (mousePos.x >= m_gameModes[i].sectionX &&
                    mousePos.x <= m_gameModes[i].sectionX + m_gameModes[i].sectionWidth &&
                    mousePos.y >= m_gameModes[i].sectionY &&
                    mousePos.y <= m_gameModes[i].sectionY + m_gameModes[i].sectionHeight) {
                    selectGameMode(static_cast<int>(i));
                    break;
                }
            }
        }
    }
    else if (event.is<sf::Event::KeyPressed>()) {
        const auto* key = event.getIf<sf::Event::KeyPressed>();
        if (key->code == sf::Keyboard::Key::Escape) {
            m_game->getStateManager()->popState();
        }
    }
}

void GameModeSelectState::updateHover(const sf::Vector2f& mousePos) {
    for (auto& mode : m_gameModes) {
        mode.isHovered = (mousePos.x >= mode.sectionX &&
            mousePos.x <= mode.sectionX + mode.sectionWidth &&
            mousePos.y >= mode.sectionY &&
            mousePos.y <= mode.sectionY + mode.sectionHeight);

        if (mode.isHovered) {
            mode.titleText->setFillColor(sf::Color::Yellow);
            mode.descText->setFillColor(sf::Color::Yellow);
        }
        else {
            mode.titleText->setFillColor(sf::Color::White);
            mode.descText->setFillColor(sf::Color(200, 200, 200));
        }
    }
}

void GameModeSelectState::selectGameMode(int index) {
    GameMode mode;

    switch (index) {
    case 0: // ENDLESS
        std::cout << "Starting ENDLESS mode..." << std::endl;
        mode = GameMode::Endless;
        break;
    case 1: // CAMPAIGN
        std::cout << "Starting CAMPAIGN mode..." << std::endl;
        mode = GameMode::Campaign;
        break;
    case 2: // TIME TRIAL
        std::cout << "Starting TIME TRIAL mode..." << std::endl;
        mode = GameMode::TimeTrial;
        break;
    default:
        mode = GameMode::Endless;
    }

    // ✅ Deschide TrackSelectState
    auto trackSelectState = std::make_unique<TrackSelectState>(m_game, mode);
    m_game->getStateManager()->pushState(std::move(trackSelectState));
}

void GameModeSelectState::update(float deltaTime) {
    // Animations can be added here later
}

void GameModeSelectState::render(sf::RenderWindow& window) {
    // Draw each section with different background colors
    for (size_t i = 0; i < m_gameModes.size(); ++i) {
        sf::Color bgColor;
        switch (i) {
        case 0: bgColor = sf::Color(30, 30, 60); break;  // Blue
        case 1: bgColor = sf::Color(60, 30, 30); break;  // Red
        case 2: bgColor = sf::Color(30, 60, 30); break;  // Green
        }

        // Draw section background
        sf::RectangleShape section(sf::Vector2f(m_gameModes[i].sectionWidth, m_gameModes[i].sectionHeight));
        section.setPosition(sf::Vector2f(m_gameModes[i].sectionX, m_gameModes[i].sectionY));
        section.setFillColor(bgColor);

        // Add highlight if hovered
        if (m_gameModes[i].isHovered) {
            section.setFillColor(sf::Color(
                std::min(bgColor.r + 30, 255),
                std::min(bgColor.g + 30, 255),
                std::min(bgColor.b + 30, 255)
            ));
        }

        window.draw(section);
        window.draw(*m_gameModes[i].titleText);
        window.draw(*m_gameModes[i].descText);
    }
}

void GameModeSelectState::onEnter() {
    std::cout << "Entered Game Mode Select State" << std::endl;
}