#include "GameModeSelectState.h"
#include "../Core/SettingsManager.h"
#include "Core/Game.h"
#include "Core/Constants.h"
#include "States/PlayState.h"
#include "States/TrackSelectState.h"
#include "States/StateManager.h"
#include "Core/AudioManager.h"
#include <iostream>
#include <cmath>

GameModeSelectState::GameModeSelectState(Game* game)
    : State(game)
    , m_selectedIndex(0)
    , m_transitionTimer(0.0f)
    , m_bgAnimTimer(0.0f)
{
    if (!m_font.openFromFile("assets/fonts/PressStart2P-Regular.ttf")) {
        std::cerr << "[GameModeSelect] Failed to load font!" << std::endl;
    }

    initializeCards();
}

void GameModeSelectState::initializeCards() {
    // Header
    m_headerText = std::make_unique<sf::Text>(m_font);
    m_headerText->setString("SELECT GAME MODE");
    m_headerText->setCharacterSize(32);
    m_headerText->setFillColor(sf::Color::White);
    m_headerText->setStyle(sf::Text::Bold);
    auto headerBounds = m_headerText->getLocalBounds();
    m_headerText->setOrigin(sf::Vector2f(
        headerBounds.position.x + headerBounds.size.x * 0.5f,
        headerBounds.position.y + headerBounds.size.y * 0.5f
    ));
    
    auto& settings = SettingsManager::getInstance();
    float windowWidth = static_cast<float>(settings.getWindowWidth());
    float windowHeight = static_cast<float>(settings.getWindowHeight());

    m_headerText->setPosition(sf::Vector2f(windowWidth * 0.5f, 60.0f));

    // Hint text
    m_hintText = std::make_unique<sf::Text>(m_font);
    m_hintText->setString("Use ARROWS or MOUSE to select - ENTER to confirm - ESC to go back");
    m_hintText->setCharacterSize(12);
    m_hintText->setFillColor(sf::Color(150, 150, 150));
    auto hintBounds = m_hintText->getLocalBounds();
    m_hintText->setOrigin(sf::Vector2f(
        hintBounds.position.x + hintBounds.size.x * 0.5f,
        hintBounds.position.y + hintBounds.size.y * 0.5f
    ));
    m_hintText->setPosition(sf::Vector2f(windowWidth * 0.5f, windowHeight - 40.0f));

    // Card dimensions
    const float cardWidth = 350.0f;
    const float cardHeight = 400.0f;
    const float cardSpacing = 40.0f;
    const float totalWidth = 3 * cardWidth + 2 * cardSpacing;
    const float startX = (windowWidth - totalWidth) * 0.5f;
    const float cardY = (windowHeight - cardHeight) * 0.5f + 20.0f;

    // Card 1: Endless Mode
    GameModeCard endless;
    endless.name = "ENDLESS";
    endless.description = "Drive as far as you can!\nAvoid traffic and\nset new records.";
    endless.iconSymbol = "8";
    endless.primaryColor = sf::Color(41, 98, 255);
    endless.accentColor = sf::Color(0, 176, 255);
    endless.targetScale = 1.0f;
    endless.currentScale = 1.0f;
    endless.hoverGlow = 0.0f;
    endless.isSelected = true;

    float cardX = startX;
    
    // Card background
    endless.cardShape = std::make_unique<sf::RectangleShape>(sf::Vector2f(cardWidth, cardHeight));
    endless.cardShape->setPosition(sf::Vector2f(cardX, cardY));
    endless.cardShape->setFillColor(sf::Color(25, 25, 35, 240));
    endless.cardShape->setOutlineThickness(3.0f);
    endless.cardShape->setOutlineColor(endless.primaryColor);
    
    // Icon background circle
    endless.iconBg = std::make_unique<sf::RectangleShape>(sf::Vector2f(80.0f, 80.0f));
    endless.iconBg->setOrigin(sf::Vector2f(40.0f, 40.0f));
    endless.iconBg->setPosition(sf::Vector2f(cardX + cardWidth * 0.5f, cardY + 80.0f));
    endless.iconBg->setFillColor(endless.primaryColor);
    
    // Icon text
    endless.iconText = std::make_unique<sf::Text>(m_font);
    endless.iconText->setString(endless.iconSymbol);
    endless.iconText->setCharacterSize(48);
    endless.iconText->setFillColor(sf::Color::White);
    endless.iconText->setStyle(sf::Text::Bold);
    auto iconBounds = endless.iconText->getLocalBounds();
    endless.iconText->setOrigin(sf::Vector2f(
        iconBounds.position.x + iconBounds.size.x * 0.5f,
        iconBounds.position.y + iconBounds.size.y * 0.5f
    ));
    endless.iconText->setPosition(sf::Vector2f(cardX + cardWidth * 0.5f, cardY + 80.0f));
    
    // Title
    endless.titleText = std::make_unique<sf::Text>(m_font);
    endless.titleText->setString(endless.name);
    endless.titleText->setCharacterSize(24);
    endless.titleText->setFillColor(sf::Color::White);
    endless.titleText->setStyle(sf::Text::Bold);
    auto titleBounds = endless.titleText->getLocalBounds();
    endless.titleText->setOrigin(sf::Vector2f(
        titleBounds.position.x + titleBounds.size.x * 0.5f,
        titleBounds.position.y + titleBounds.size.y * 0.5f
    ));
    endless.titleText->setPosition(sf::Vector2f(cardX + cardWidth * 0.5f, cardY + 170.0f));
    
    // Description
    endless.descText = std::make_unique<sf::Text>(m_font);
    endless.descText->setString(endless.description);
    endless.descText->setCharacterSize(14);
    endless.descText->setFillColor(sf::Color(180, 180, 180));
    endless.descText->setLineSpacing(1.5f);
    auto descBounds = endless.descText->getLocalBounds();
    endless.descText->setOrigin(sf::Vector2f(
        descBounds.position.x + descBounds.size.x * 0.5f,
        descBounds.position.y
    ));
    endless.descText->setPosition(sf::Vector2f(cardX + cardWidth * 0.5f, cardY + 220.0f));

    m_cards.push_back(std::move(endless));

    // Card 2: Campaign Mode
    GameModeCard campaign;
    campaign.name = "CAMPAIGN";
    campaign.description = "Race through levels!\nComplete challenges\nand unlock rewards.";
    campaign.iconSymbol = "!";
    campaign.primaryColor = sf::Color(255, 152, 0);
    campaign.accentColor = sf::Color(255, 193, 7);
    campaign.targetScale = 1.0f;
    campaign.currentScale = 1.0f;
    campaign.hoverGlow = 0.0f;
    campaign.isSelected = false;

    cardX = startX + cardWidth + cardSpacing;
    
    campaign.cardShape = std::make_unique<sf::RectangleShape>(sf::Vector2f(cardWidth, cardHeight));
    campaign.cardShape->setPosition(sf::Vector2f(cardX, cardY));
    campaign.cardShape->setFillColor(sf::Color(25, 25, 35, 240));
    campaign.cardShape->setOutlineThickness(3.0f);
    campaign.cardShape->setOutlineColor(sf::Color(60, 60, 70));
    
    campaign.iconBg = std::make_unique<sf::RectangleShape>(sf::Vector2f(80.0f, 80.0f));
    campaign.iconBg->setOrigin(sf::Vector2f(40.0f, 40.0f));
    campaign.iconBg->setPosition(sf::Vector2f(cardX + cardWidth * 0.5f, cardY + 80.0f));
    campaign.iconBg->setFillColor(campaign.primaryColor);
    
    campaign.iconText = std::make_unique<sf::Text>(m_font);
    campaign.iconText->setString(campaign.iconSymbol);
    campaign.iconText->setCharacterSize(48);
    campaign.iconText->setFillColor(sf::Color::White);
    campaign.iconText->setStyle(sf::Text::Bold);
    iconBounds = campaign.iconText->getLocalBounds();
    campaign.iconText->setOrigin(sf::Vector2f(
        iconBounds.position.x + iconBounds.size.x * 0.5f,
        iconBounds.position.y + iconBounds.size.y * 0.5f
    ));
    campaign.iconText->setPosition(sf::Vector2f(cardX + cardWidth * 0.5f, cardY + 80.0f));
    
    campaign.titleText = std::make_unique<sf::Text>(m_font);
    campaign.titleText->setString(campaign.name);
    campaign.titleText->setCharacterSize(24);
    campaign.titleText->setFillColor(sf::Color::White);
    campaign.titleText->setStyle(sf::Text::Bold);
    titleBounds = campaign.titleText->getLocalBounds();
    campaign.titleText->setOrigin(sf::Vector2f(
        titleBounds.position.x + titleBounds.size.x * 0.5f,
        titleBounds.position.y + titleBounds.size.y * 0.5f
    ));
    campaign.titleText->setPosition(sf::Vector2f(cardX + cardWidth * 0.5f, cardY + 170.0f));
    
    campaign.descText = std::make_unique<sf::Text>(m_font);
    campaign.descText->setString(campaign.description);
    campaign.descText->setCharacterSize(14);
    campaign.descText->setFillColor(sf::Color(180, 180, 180));
    campaign.descText->setLineSpacing(1.5f);
    descBounds = campaign.descText->getLocalBounds();
    campaign.descText->setOrigin(sf::Vector2f(
        descBounds.position.x + descBounds.size.x * 0.5f,
        descBounds.position.y
    ));
    campaign.descText->setPosition(sf::Vector2f(cardX + cardWidth * 0.5f, cardY + 220.0f));

    m_cards.push_back(std::move(campaign));

    // Card 3: Time Trial Mode
    GameModeCard timeTrial;
    timeTrial.name = "TIME TRIAL";
    timeTrial.description = "Race against time!\nSet the fastest lap\nand beat your records.";
    timeTrial.iconSymbol = "T";
    timeTrial.primaryColor = sf::Color(76, 175, 80);
    timeTrial.accentColor = sf::Color(139, 195, 74);
    timeTrial.targetScale = 1.0f;
    timeTrial.currentScale = 1.0f;
    timeTrial.hoverGlow = 0.0f;
    timeTrial.isSelected = false;

    cardX = startX + 2 * (cardWidth + cardSpacing);
    
    timeTrial.cardShape = std::make_unique<sf::RectangleShape>(sf::Vector2f(cardWidth, cardHeight));
    timeTrial.cardShape->setPosition(sf::Vector2f(cardX, cardY));
    timeTrial.cardShape->setFillColor(sf::Color(25, 25, 35, 240));
    timeTrial.cardShape->setOutlineThickness(3.0f);
    timeTrial.cardShape->setOutlineColor(sf::Color(60, 60, 70));
    
    timeTrial.iconBg = std::make_unique<sf::RectangleShape>(sf::Vector2f(80.0f, 80.0f));
    timeTrial.iconBg->setOrigin(sf::Vector2f(40.0f, 40.0f));
    timeTrial.iconBg->setPosition(sf::Vector2f(cardX + cardWidth * 0.5f, cardY + 80.0f));
    timeTrial.iconBg->setFillColor(timeTrial.primaryColor);
    
    timeTrial.iconText = std::make_unique<sf::Text>(m_font);
    timeTrial.iconText->setString(timeTrial.iconSymbol);
    timeTrial.iconText->setCharacterSize(48);
    timeTrial.iconText->setFillColor(sf::Color::White);
    timeTrial.iconText->setStyle(sf::Text::Bold);
    iconBounds = timeTrial.iconText->getLocalBounds();
    timeTrial.iconText->setOrigin(sf::Vector2f(
        iconBounds.position.x + iconBounds.size.x * 0.5f,
        iconBounds.position.y + iconBounds.size.y * 0.5f
    ));
    timeTrial.iconText->setPosition(sf::Vector2f(cardX + cardWidth * 0.5f, cardY + 80.0f));
    
    timeTrial.titleText = std::make_unique<sf::Text>(m_font);
    timeTrial.titleText->setString(timeTrial.name);
    timeTrial.titleText->setCharacterSize(24);
    timeTrial.titleText->setFillColor(sf::Color::White);
    timeTrial.titleText->setStyle(sf::Text::Bold);
    titleBounds = timeTrial.titleText->getLocalBounds();
    timeTrial.titleText->setOrigin(sf::Vector2f(
        titleBounds.position.x + titleBounds.size.x * 0.5f,
        titleBounds.position.y + titleBounds.size.y * 0.5f
    ));
    timeTrial.titleText->setPosition(sf::Vector2f(cardX + cardWidth * 0.5f, cardY + 170.0f));
    
    timeTrial.descText = std::make_unique<sf::Text>(m_font);
    timeTrial.descText->setString(timeTrial.description);
    timeTrial.descText->setCharacterSize(14);
    timeTrial.descText->setFillColor(sf::Color(180, 180, 180));
    timeTrial.descText->setLineSpacing(1.5f);
    descBounds = timeTrial.descText->getLocalBounds();
    timeTrial.descText->setOrigin(sf::Vector2f(
        descBounds.position.x + descBounds.size.x * 0.5f,
        descBounds.position.y
    ));
    timeTrial.descText->setPosition(sf::Vector2f(cardX + cardWidth * 0.5f, cardY + 220.0f));

    m_cards.push_back(std::move(timeTrial));
    
    updateSelection(0);
}

void GameModeSelectState::handleInput(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        const auto* key = event.getIf<sf::Event::KeyPressed>();

        switch (key->code) {
            case sf::Keyboard::Key::Left:
            case sf::Keyboard::Key::A: {
                int newIndex = (m_selectedIndex - 1 + 3) % 3;
                AudioManager::getInstance().playSfx("menu_select");
                updateSelection(newIndex);
                break;
            }

            case sf::Keyboard::Key::Right:
            case sf::Keyboard::Key::D: {
                int newIndex = (m_selectedIndex + 1) % 3;
                AudioManager::getInstance().playSfx("menu_select");
                updateSelection(newIndex);
                break;
            }

            case sf::Keyboard::Key::Enter:
            case sf::Keyboard::Key::Space:
                AudioManager::getInstance().playSfx("menu_select");
                selectCurrentMode();
                break;

            case sf::Keyboard::Key::Escape:
                m_game->getStateManager()->popState();
                break;

            default:
                break;
        }
    }
    else if (event.is<sf::Event::MouseMoved>()) {
        const auto* mouseEvent = event.getIf<sf::Event::MouseMoved>();
        sf::Vector2f mousePos(static_cast<float>(mouseEvent->position.x),
                              static_cast<float>(mouseEvent->position.y));

        for (size_t i = 0; i < m_cards.size(); ++i) {
            auto bounds = m_cards[i].cardShape->getGlobalBounds();
            if (bounds.contains(mousePos)) {
                if (m_selectedIndex != static_cast<int>(i)) {
                    AudioManager::getInstance().playSfx("menu_select");
                    updateSelection(static_cast<int>(i));
                }
                break;
            }
        }
    }
    else if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos(static_cast<float>(mouseEvent->position.x),
                                  static_cast<float>(mouseEvent->position.y));

            for (size_t i = 0; i < m_cards.size(); ++i) {
                auto bounds = m_cards[i].cardShape->getGlobalBounds();
                if (bounds.contains(mousePos)) {
                    AudioManager::getInstance().playSfx("menu_select");
                    selectCurrentMode();
                    break;
                }
            }
        }
    }
}


void GameModeSelectState::updateSelection(int newIndex) {
    // Deselect old card
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_cards.size())) {
        m_cards[m_selectedIndex].isSelected = false;
        m_cards[m_selectedIndex].targetScale = 1.0f;
        m_cards[m_selectedIndex].cardShape->setOutlineColor(sf::Color(60, 60, 70));
    }
    
    // Select new card
    m_selectedIndex = newIndex;
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_cards.size())) {
        m_cards[m_selectedIndex].isSelected = true;
        m_cards[m_selectedIndex].targetScale = 1.05f;
        m_cards[m_selectedIndex].cardShape->setOutlineColor(m_cards[m_selectedIndex].primaryColor);
    }
}

void GameModeSelectState::selectCurrentMode() {
    GameMode mode;
    
    switch (m_selectedIndex) {
        case 0:
            std::cout << "[GameMode] Selected: ENDLESS" << std::endl;
            mode = GameMode::Endless;
            break;
        case 1:
            std::cout << "[GameMode] Selected: CAMPAIGN" << std::endl;
            mode = GameMode::Campaign;
            break;
        case 2:
            std::cout << "[GameMode] Selected: TIME TRIAL" << std::endl;
            mode = GameMode::TimeTrial;
            break;
        default:
            mode = GameMode::Endless;
    }
    
    auto trackSelectState = std::make_unique<TrackSelectState>(m_game, mode);
    m_game->getStateManager()->pushState(std::move(trackSelectState));
}

void GameModeSelectState::updateCardAnimations(float deltaTime) {
    for (auto& card : m_cards) {
        // Smooth scale animation
        float scaleDiff = card.targetScale - card.currentScale;
        card.currentScale += scaleDiff * 8.0f * deltaTime;
        
        // Glow animation for selected card
        if (card.isSelected) {
            card.hoverGlow += deltaTime * 3.0f;
            float glowIntensity = (std::sin(card.hoverGlow) + 1.0f) * 0.5f;
            
            sf::Color glowColor = card.primaryColor;
            glowColor.r = static_cast<std::uint8_t>(std::min(255, static_cast<int>(glowColor.r) + static_cast<int>(40 * glowIntensity)));
            glowColor.g = static_cast<std::uint8_t>(std::min(255, static_cast<int>(glowColor.g) + static_cast<int>(40 * glowIntensity)));
            glowColor.b = static_cast<std::uint8_t>(std::min(255, static_cast<int>(glowColor.b) + static_cast<int>(40 * glowIntensity)));
            
            card.cardShape->setOutlineColor(glowColor);
            card.cardShape->setOutlineThickness(4.0f);
        } else {
            card.hoverGlow = 0.0f;
            card.cardShape->setOutlineThickness(2.0f);
        }
    }
}

void GameModeSelectState::update(float deltaTime) {
    m_bgAnimTimer += deltaTime;
    updateCardAnimations(deltaTime);
}

void GameModeSelectState::drawBackground(sf::RenderWindow& window) {
    // Gradient background
    auto& settings = SettingsManager::getInstance();
    sf::RectangleShape bg(sf::Vector2f(
        static_cast<float>(settings.getWindowWidth()),
        static_cast<float>(settings.getWindowHeight())
    ));
    bg.setFillColor(sf::Color(15, 15, 25));
    window.draw(bg);

    // Animated decorative lines
    for (int i = 0; i < 5; ++i) {
        float yOffset = std::sin(m_bgAnimTimer * 0.5f + i * 0.5f) * 20.0f;

        sf::RectangleShape line(sf::Vector2f(static_cast<float>(settings.getWindowWidth()), 2.0f));
        line.setPosition(sf::Vector2f(0.0f, 100.0f + i * 150.0f + yOffset));
        line.setFillColor(sf::Color(40, 40, 60, 100));
        window.draw(line);
    }
}

void GameModeSelectState::drawCard(sf::RenderWindow& window, GameModeCard& card, size_t index) {
    // Apply scale transform
    sf::Vector2f cardCenter = card.cardShape->getPosition() + 
                               sf::Vector2f(card.cardShape->getSize().x * 0.5f, 
                                           card.cardShape->getSize().y * 0.5f);
    
    // Draw card background
    window.draw(*card.cardShape);
    
    // Draw icon background
    window.draw(*card.iconBg);
    
    // Draw icon
    window.draw(*card.iconText);
    
    // Draw title
    window.draw(*card.titleText);
    
    // Draw description
    window.draw(*card.descText);
    
    // Draw selection indicator
    if (card.isSelected) {
        sf::RectangleShape indicator(sf::Vector2f(60.0f, 4.0f));
        indicator.setOrigin(sf::Vector2f(30.0f, 2.0f));
        indicator.setPosition(sf::Vector2f(
            card.cardShape->getPosition().x + card.cardShape->getSize().x * 0.5f,
            card.cardShape->getPosition().y + card.cardShape->getSize().y - 30.0f
        ));
        indicator.setFillColor(card.primaryColor);
        window.draw(indicator);
    }
}

void GameModeSelectState::render(sf::RenderWindow& window) {
    drawBackground(window);
    
    // Draw header
    window.draw(*m_headerText);
    
    // Draw cards
    for (size_t i = 0; i < m_cards.size(); ++i) {
        drawCard(window, m_cards[i], i);
    }
    
    // Draw hint
    window.draw(*m_hintText);
}

void GameModeSelectState::onEnter() {
    std::cout << "[GameModeSelect] Entered" << std::endl;
    m_selectedIndex = 0;
    updateSelection(0);
}
