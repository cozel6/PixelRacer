#include "TrackSelectState.h"
#include "PlayState.h"
#include "GameModeSelectState.h"
#include "Core/Constants.h"
#include "Core/Game.h"
#include "States/StateManager.h"
#include "Gameplay/TrackBuilder.h"
#include <iostream>

TrackSelectState::TrackSelectState(Game* game, GameMode gameMode)
    : State(game)
    , m_gameMode(gameMode)
    , m_selectedIndex(0)
{
    // ✅ Verifică încărcarea fontului
    if (!m_font.openFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "❌ ERROR: Failed to load font for TrackSelectState!" << std::endl;
        // Încearcă alt font ca fallback
        if (!m_font.openFromFile("assets/fonts/PressStart2P-Regular.ttf")) {
            std::cerr << "❌ ERROR: Failed to load fallback font too!" << std::endl;
        }
    }
    else {
        std::cout << "✅ Font loaded successfully in TrackSelectState" << std::endl;
    }

    // ✅ Creăm textele
    m_titleText = std::make_unique<sf::Text>(m_font);
    m_trackInfoText = std::make_unique<sf::Text>(m_font);

    std::cout << "✅ Text objects created" << std::endl;

    loadTracksForMode();
    createUI();
}

void TrackSelectState::loadTracksForMode() {
    switch (m_gameMode) {
    case GameMode::Campaign:
        m_availableTracks = TrackLibrary::getTracksByDifficulty(TrackDifficulty::Easy);
        {
            auto medium = TrackLibrary::getTracksByDifficulty(TrackDifficulty::Medium);
            m_availableTracks.insert(m_availableTracks.end(), medium.begin(), medium.end());
        }
        break;

    case GameMode::TimeTrial:
    case GameMode::Endless:
        m_availableTracks = TrackLibrary::getAllTracks();
        break;

    default:
        m_availableTracks = TrackLibrary::getAllTracks();
        break;
    }

    std::cout << "✅ Loaded " << m_availableTracks.size() << " tracks" << std::endl;
}

void TrackSelectState::createUI() {
    const float windowWidth = static_cast<float>(Config::WINDOW_WIDTH);
    const float windowHeight = static_cast<float>(Config::WINDOW_HEIGHT);

    // ✅ Setăm textul de titlu
    m_titleText->setString("Select Track");
    m_titleText->setCharacterSize(48);
    m_titleText->setFillColor(sf::Color::White);
    m_titleText->setPosition(sf::Vector2f(50.0f, 30.0f));

    std::cout << "✅ Title text: '" << m_titleText->getString().toAnsiString() << "'" << std::endl;
    std::cout << "   Position: (" << m_titleText->getPosition().x << ", " << m_titleText->getPosition().y << ")" << std::endl;
    std::cout << "   Size: " << m_titleText->getCharacterSize() << std::endl;

    float startY = 120.0f;
    float buttonHeight = 60.0f;
    float spacing = 10.0f;

    for (size_t i = 0; i < m_availableTracks.size(); ++i) {
        SimpleButton btn;

        btn.shape.setSize(sf::Vector2f(400.0f, buttonHeight));
        btn.shape.setPosition(sf::Vector2f(50.0f, startY + i * (buttonHeight + spacing)));
        btn.shape.setFillColor(sf::Color(50, 50, 50));
        btn.shape.setOutlineColor(sf::Color::White);
        btn.shape.setOutlineThickness(2.0f);
        btn.label = m_availableTracks[i].name;

        std::cout << "   Track " << i << ": " << btn.label << std::endl;

        m_trackButtons.push_back(btn);
    }

    m_infoPanel.setSize(sf::Vector2f(500.0f, 500.0f));
    m_infoPanel.setPosition(sf::Vector2f(windowWidth - 550.0f, 120.0f));
    m_infoPanel.setFillColor(sf::Color(30, 30, 30, 200));
    m_infoPanel.setOutlineColor(sf::Color::White);
    m_infoPanel.setOutlineThickness(2.0f);

    m_trackInfoText->setCharacterSize(20);
    m_trackInfoText->setFillColor(sf::Color::White);
    m_trackInfoText->setPosition(sf::Vector2f(windowWidth - 530.0f, 140.0f));

    m_backButton.shape.setSize(sf::Vector2f(150.0f, 50.0f));
    m_backButton.shape.setPosition(sf::Vector2f(50.0f, windowHeight - 80.0f));
    m_backButton.shape.setFillColor(sf::Color(100, 50, 50));
    m_backButton.shape.setOutlineColor(sf::Color::White);
    m_backButton.shape.setOutlineThickness(2.0f);
    m_backButton.label = "Back";

    m_confirmButton.shape.setSize(sf::Vector2f(150.0f, 50.0f));
    m_confirmButton.shape.setPosition(sf::Vector2f(windowWidth - 200.0f, windowHeight - 80.0f));
    m_confirmButton.shape.setFillColor(sf::Color(50, 100, 50));
    m_confirmButton.shape.setOutlineColor(sf::Color::White);
    m_confirmButton.shape.setOutlineThickness(2.0f);
    m_confirmButton.label = "Race!";

    if (!m_availableTracks.empty()) {
        selectTrack(0);
    }

    std::cout << "✅ UI created successfully" << std::endl;
}

void TrackSelectState::handleInput(const sf::Event& event) {
    if (const auto* mousePress = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePress->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos(static_cast<float>(mousePress->position.x),
                static_cast<float>(mousePress->position.y));

            for (size_t i = 0; i < m_trackButtons.size(); ++i) {
                if (m_trackButtons[i].shape.getGlobalBounds().contains(mousePos)) {
                    selectTrack(static_cast<int>(i));
                    break;
                }
            }

            if (m_backButton.shape.getGlobalBounds().contains(mousePos)) {
                m_game->getStateManager()->changeState(
                    std::make_unique<GameModeSelectState>(m_game)
                );
            }

            if (m_confirmButton.shape.getGlobalBounds().contains(mousePos)) {
                confirmSelection();
            }
        }
    }

    if (const auto* keyPress = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPress->code == sf::Keyboard::Key::Escape) {
            m_game->getStateManager()->changeState(
                std::make_unique<GameModeSelectState>(m_game)
            );
        }
    }
}

void TrackSelectState::update(float deltaTime) {
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(m_game->getWindow());
    sf::Vector2f mousePos = m_game->getWindow().mapPixelToCoords(mousePixelPos);
    updateButtonStates(mousePos);
}

void TrackSelectState::render(sf::RenderWindow& window) {
    window.clear(sf::Color(20, 20, 20));

    // ✅ Debug: Desenăm un pătrat mare roșu pentru a vedea dacă render-ul funcționează
    sf::RectangleShape debugRect(sf::Vector2f(200.0f, 100.0f));
    debugRect.setPosition(sf::Vector2f(100.0f, 100.0f));
    debugRect.setFillColor(sf::Color::Red);
    window.draw(debugRect);

    // ✅ Desenăm titlul
    if (m_titleText) {
        window.draw(*m_titleText);
    }

    for (size_t i = 0; i < m_trackButtons.size(); ++i) {
        if (static_cast<int>(i) == m_selectedIndex) {
            m_trackButtons[i].shape.setFillColor(sf::Color(100, 100, 150));
        }
        else if (m_trackButtons[i].isHovered) {
            m_trackButtons[i].shape.setFillColor(sf::Color(70, 70, 70));
        }
        else {
            m_trackButtons[i].shape.setFillColor(sf::Color(50, 50, 50));
        }

        window.draw(m_trackButtons[i].shape);

        // ✅ Creăm text pentru fiecare buton
        sf::Text btnText(m_font, m_trackButtons[i].label, 24);
        btnText.setFillColor(sf::Color::White);

        sf::FloatRect textBounds = btnText.getLocalBounds();
        sf::FloatRect shapeBounds = m_trackButtons[i].shape.getGlobalBounds();

        btnText.setOrigin(sf::Vector2f(textBounds.size.x / 2.0f, textBounds.size.y / 2.0f));
        btnText.setPosition(sf::Vector2f(shapeBounds.position.x + shapeBounds.size.x / 2.0f,
            shapeBounds.position.y + shapeBounds.size.y / 2.0f));
        window.draw(btnText);
    }

    window.draw(m_infoPanel);

    if (m_trackInfoText) {
        window.draw(*m_trackInfoText);
    }

    // Back button
    if (m_backButton.isHovered) {
        m_backButton.shape.setFillColor(sf::Color(120, 60, 60));
    }
    else {
        m_backButton.shape.setFillColor(sf::Color(100, 50, 50));
    }
    window.draw(m_backButton.shape);

    sf::Text backText(m_font, m_backButton.label, 24);
    backText.setFillColor(sf::Color::White);
    sf::FloatRect backBounds = m_backButton.shape.getGlobalBounds();
    sf::FloatRect backTextBounds = backText.getLocalBounds();
    backText.setOrigin(sf::Vector2f(backTextBounds.size.x / 2.0f, backTextBounds.size.y / 2.0f));
    backText.setPosition(sf::Vector2f(backBounds.position.x + backBounds.size.x / 2.0f,
        backBounds.position.y + backBounds.size.y / 2.0f));
    window.draw(backText);

    // Confirm button
    if (m_confirmButton.isHovered) {
        m_confirmButton.shape.setFillColor(sf::Color(60, 120, 60));
    }
    else {
        m_confirmButton.shape.setFillColor(sf::Color(50, 100, 50));
    }
    window.draw(m_confirmButton.shape);

    sf::Text confirmText(m_font, m_confirmButton.label, 24);
    confirmText.setFillColor(sf::Color::White);
    sf::FloatRect confirmBounds = m_confirmButton.shape.getGlobalBounds();
    sf::FloatRect confirmTextBounds = confirmText.getLocalBounds();
    confirmText.setOrigin(sf::Vector2f(confirmTextBounds.size.x / 2.0f, confirmTextBounds.size.y / 2.0f));
    confirmText.setPosition(sf::Vector2f(confirmBounds.position.x + confirmBounds.size.x / 2.0f,
        confirmBounds.position.y + confirmBounds.size.y / 2.0f));
    window.draw(confirmText);
}

void TrackSelectState::selectTrack(int index) {
    if (index >= 0 && index < static_cast<int>(m_availableTracks.size())) {
        m_selectedIndex = index;
        std::cout << "✅ Selected track: " << m_availableTracks[index].name << std::endl;
        displayTrackInfo(m_availableTracks[index]);
    }
}

void TrackSelectState::confirmSelection() {
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_availableTracks.size())) {
        std::cout << "✅ Confirming track: " << m_availableTracks[m_selectedIndex].name << std::endl;
        
        // ✅ Transmitem pointer către traseul selectat
        m_game->getStateManager()->changeState(
            std::make_unique<PlayState>(m_game, m_gameMode, &m_availableTracks[m_selectedIndex])
        );
    }
}

void TrackSelectState::displayTrackInfo(const TrackDefinition& track) {
    std::string info = track.name + "\n\n";
    info += "Country: " + track.country + "\n";
    info += "Length: " + std::to_string(track.lengthKm) + " km\n";
    info += "Laps: " + std::to_string(track.recommendedLaps) + "\n\n";
    info += track.description + "\n\n";

    std::string diffStr;
    switch (track.difficulty) {
    case TrackDifficulty::Easy:   diffStr = "Easy";   break;
    case TrackDifficulty::Medium: diffStr = "Medium"; break;
    case TrackDifficulty::Hard:   diffStr = "Hard";   break;
    case TrackDifficulty::Expert: diffStr = "Expert"; break;
    }
    info += "Difficulty: " + diffStr + "\n";

    if (track.year > 0) {
        info += "First used: " + std::to_string(track.year);
    }

    std::cout << "✅ Track info:\n" << info << std::endl;

    if (m_trackInfoText) {
        m_trackInfoText->setString(info);
    }
}

void TrackSelectState::updateButtonStates(const sf::Vector2f& mousePos) {
    for (auto& btn : m_trackButtons) {
        btn.isHovered = btn.shape.getGlobalBounds().contains(mousePos);
    }

    m_backButton.isHovered = m_backButton.shape.getGlobalBounds().contains(mousePos);
    m_confirmButton.isHovered = m_confirmButton.shape.getGlobalBounds().contains(mousePos);
}