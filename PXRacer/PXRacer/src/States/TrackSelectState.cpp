#include "TrackSelectState.h"
#include "../Core/SettingsManager.h"
#include "PlayState.h"
#include "TaskSelectState.h"
#include "GameModeSelectState.h"
#include "Core/Constants.h"
#include "Core/Game.h"
#include "Core/AudioManager.h"
#include "States/StateManager.h"
#include <iostream>
#include <cmath>
#include <cstdint>

TrackSelectState::TrackSelectState(Game* game, GameMode gameMode)
    : State(game)
    , m_gameMode(gameMode)
    , m_selectedIndex(0)
    , m_scrollOffset(0)
    , m_backHovered(false)
    , m_startHovered(false)
    , m_animTimer(0.0f)
{
    if (!m_font.openFromFile("assets/fonts/PressStart2P-Regular.ttf")) {
        std::cerr << "[TrackSelect] Failed to load font!" << std::endl;
    }

    loadTracksForMode();
    createUI();
    createTrackCards();
    
    if (!m_availableTracks.empty()) {
        updateSelection(0);
    }
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
        default:
            m_availableTracks = TrackLibrary::getAllTracks();
            break;
    }
    
    std::cout << "[TrackSelect] Loaded " << m_availableTracks.size() << " tracks for " << getModeString() << std::endl;
}

std::string TrackSelectState::getModeString() {
    switch (m_gameMode) {
        case GameMode::Endless: return "ENDLESS";
        case GameMode::Campaign: return "CAMPAIGN";
        case GameMode::TimeTrial: return "TIME TRIAL";
        default: return "UNKNOWN";
    }
}

sf::Color TrackSelectState::getDifficultyColor(TrackDifficulty diff) {
    switch (diff) {
        case TrackDifficulty::Easy:   return sf::Color(76, 175, 80);
        case TrackDifficulty::Medium: return sf::Color(255, 193, 7);
        case TrackDifficulty::Hard:   return sf::Color(255, 87, 34);
        case TrackDifficulty::Expert: return sf::Color(244, 67, 54);
        default: return sf::Color::White;
    }
}

std::string TrackSelectState::getDifficultyString(TrackDifficulty diff) {
    switch (diff) {
        case TrackDifficulty::Easy:   return "EASY";
        case TrackDifficulty::Medium: return "MEDIUM";
        case TrackDifficulty::Hard:   return "HARD";
        case TrackDifficulty::Expert: return "EXPERT";
        default: return "???";
    }
}

void TrackSelectState::createUI() {

    auto& settings = SettingsManager::getInstance();
    float windowWidth = static_cast<float>(settings.getWindowWidth());
    float windowHeight = static_cast<float>(settings.getWindowHeight());


    m_headerText = std::make_unique<sf::Text>(m_font);
    m_headerText->setString("SELECT TRACK");
    m_headerText->setCharacterSize(32);
    m_headerText->setFillColor(sf::Color::White);
    m_headerText->setStyle(sf::Text::Bold);
    auto headerBounds = m_headerText->getLocalBounds();
    m_headerText->setOrigin(sf::Vector2f(
        headerBounds.position.x + headerBounds.size.x * 0.5f,
        headerBounds.position.y + headerBounds.size.y * 0.5f
    ));
    m_headerText->setPosition(sf::Vector2f(windowWidth * 0.5f, 40.0f));
    
    m_modeText = std::make_unique<sf::Text>(m_font);
    m_modeText->setString("Mode: " + getModeString());
    m_modeText->setCharacterSize(14);
    m_modeText->setFillColor(sf::Color(150, 150, 150));
    auto modeBounds = m_modeText->getLocalBounds();
    m_modeText->setOrigin(sf::Vector2f(
        modeBounds.position.x + modeBounds.size.x * 0.5f,
        modeBounds.position.y + modeBounds.size.y * 0.5f
    ));
    m_modeText->setPosition(sf::Vector2f(windowWidth * 0.5f, 75.0f));
    
    m_hintText = std::make_unique<sf::Text>(m_font);
    m_hintText->setString("UP/DOWN to select - ENTER to race - ESC to go back");
    m_hintText->setCharacterSize(10);
    m_hintText->setFillColor(sf::Color(120, 120, 120));
    auto hintBounds = m_hintText->getLocalBounds();
    m_hintText->setOrigin(sf::Vector2f(
        hintBounds.position.x + hintBounds.size.x * 0.5f,
        hintBounds.position.y + hintBounds.size.y * 0.5f
    ));
    m_hintText->setPosition(sf::Vector2f(windowWidth * 0.5f, windowHeight - 25.0f));
    
    m_detailPanel = std::make_unique<sf::RectangleShape>(sf::Vector2f(450.0f, 500.0f));
    m_detailPanel->setPosition(sf::Vector2f(windowWidth - 480.0f, 110.0f));
    m_detailPanel->setFillColor(sf::Color(20, 20, 30, 240));
    m_detailPanel->setOutlineColor(sf::Color(60, 60, 80));
    m_detailPanel->setOutlineThickness(2.0f);
    
    m_trackPreview = std::make_unique<sf::RectangleShape>(sf::Vector2f(410.0f, 180.0f));
    m_trackPreview->setPosition(sf::Vector2f(windowWidth - 460.0f, 130.0f));
    m_trackPreview->setFillColor(sf::Color(40, 40, 50));
    m_trackPreview->setOutlineColor(sf::Color(80, 80, 100));
    m_trackPreview->setOutlineThickness(1.0f);
    
    float detailX = windowWidth - 460.0f;
    float detailY = 330.0f;
    
    m_detailName = std::make_unique<sf::Text>(m_font);
    m_detailName->setCharacterSize(22);
    m_detailName->setFillColor(sf::Color::White);
    m_detailName->setStyle(sf::Text::Bold);
    m_detailName->setPosition(sf::Vector2f(detailX, detailY));
    
    m_detailCountry = std::make_unique<sf::Text>(m_font);
    m_detailCountry->setCharacterSize(14);
    m_detailCountry->setFillColor(sf::Color(180, 180, 180));
    m_detailCountry->setPosition(sf::Vector2f(detailX, detailY + 40.0f));
    
    m_detailLength = std::make_unique<sf::Text>(m_font);
    m_detailLength->setCharacterSize(14);
    m_detailLength->setFillColor(sf::Color(150, 200, 255));
    m_detailLength->setPosition(sf::Vector2f(detailX, detailY + 70.0f));
    
    m_detailLaps = std::make_unique<sf::Text>(m_font);
    m_detailLaps->setCharacterSize(14);
    m_detailLaps->setFillColor(sf::Color(150, 200, 255));
    m_detailLaps->setPosition(sf::Vector2f(detailX + 180.0f, detailY + 70.0f));
    
    m_detailDifficulty = std::make_unique<sf::Text>(m_font);
    m_detailDifficulty->setCharacterSize(14);
    m_detailDifficulty->setPosition(sf::Vector2f(detailX, detailY + 100.0f));
    
    m_detailDescription = std::make_unique<sf::Text>(m_font);
    m_detailDescription->setCharacterSize(11);
    m_detailDescription->setFillColor(sf::Color(140, 140, 140));
    m_detailDescription->setLineSpacing(1.4f);
    m_detailDescription->setPosition(sf::Vector2f(detailX, detailY + 140.0f));
    
    m_backButton = std::make_unique<sf::RectangleShape>(sf::Vector2f(140.0f, 45.0f));
    m_backButton->setPosition(sf::Vector2f(30.0f, windowHeight - 70.0f));
    m_backButton->setFillColor(sf::Color(60, 40, 40));
    m_backButton->setOutlineColor(sf::Color(120, 80, 80));
    m_backButton->setOutlineThickness(2.0f);
    
    m_backText = std::make_unique<sf::Text>(m_font);
    m_backText->setString("< BACK");
    m_backText->setCharacterSize(14);
    m_backText->setFillColor(sf::Color::White);
    auto backBounds = m_backText->getLocalBounds();
    m_backText->setOrigin(sf::Vector2f(
        backBounds.position.x + backBounds.size.x * 0.5f,
        backBounds.position.y + backBounds.size.y * 0.5f
    ));
    m_backText->setPosition(sf::Vector2f(100.0f, windowHeight - 47.5f));
    
    m_startButton = std::make_unique<sf::RectangleShape>(sf::Vector2f(180.0f, 50.0f));
    m_startButton->setPosition(sf::Vector2f(windowWidth - 210.0f, windowHeight - 75.0f));
    m_startButton->setFillColor(sf::Color(40, 120, 40));
    m_startButton->setOutlineColor(sf::Color(80, 200, 80));
    m_startButton->setOutlineThickness(3.0f);
    
    m_startText = std::make_unique<sf::Text>(m_font);
    m_startText->setString("START RACE");
    m_startText->setCharacterSize(16);
    m_startText->setFillColor(sf::Color::White);
    m_startText->setStyle(sf::Text::Bold);
    auto startBounds = m_startText->getLocalBounds();
    m_startText->setOrigin(sf::Vector2f(
        startBounds.position.x + startBounds.size.x * 0.5f,
        startBounds.position.y + startBounds.size.y * 0.5f
    ));
    m_startText->setPosition(sf::Vector2f(windowWidth - 120.0f, windowHeight - 50.0f));
}

void TrackSelectState::createTrackCards() {
    const float cardWidth = 350.0f;
    const float cardHeight = 80.0f;
    const float cardSpacing = 10.0f;
    const float startX = 30.0f;
    const float startY = 110.0f;
    
    for (size_t i = 0; i < m_availableTracks.size(); ++i) {
        TrackCard card;
        card.track = &m_availableTracks[i];
        card.isSelected = false;
        card.hoverGlow = 0.0f;
        card.difficultyColor = getDifficultyColor(m_availableTracks[i].difficulty);
        
        float cardY = startY + i * (cardHeight + cardSpacing);
        
        card.cardBg = std::make_unique<sf::RectangleShape>(sf::Vector2f(cardWidth, cardHeight));
        card.cardBg->setPosition(sf::Vector2f(startX, cardY));
        card.cardBg->setFillColor(sf::Color(30, 30, 40, 230));
        card.cardBg->setOutlineColor(sf::Color(50, 50, 60));
        card.cardBg->setOutlineThickness(2.0f);
        
        card.difficultyBadge = std::make_unique<sf::RectangleShape>(sf::Vector2f(6.0f, cardHeight));
        card.difficultyBadge->setPosition(sf::Vector2f(startX, cardY));
        card.difficultyBadge->setFillColor(card.difficultyColor);
        
        card.nameText = std::make_unique<sf::Text>(m_font);
        card.nameText->setString(m_availableTracks[i].name);
        card.nameText->setCharacterSize(16);
        card.nameText->setFillColor(sf::Color::White);
        card.nameText->setStyle(sf::Text::Bold);
        card.nameText->setPosition(sf::Vector2f(startX + 20.0f, cardY + 12.0f));
        
        card.countryText = std::make_unique<sf::Text>(m_font);
        card.countryText->setString(m_availableTracks[i].country);
        card.countryText->setCharacterSize(11);
        card.countryText->setFillColor(sf::Color(150, 150, 150));
        card.countryText->setPosition(sf::Vector2f(startX + 20.0f, cardY + 38.0f));
        
        card.lengthText = std::make_unique<sf::Text>(m_font);
        std::string lengthStr = std::to_string(static_cast<int>(m_availableTracks[i].lengthKm * 10) / 10.0f);
        lengthStr = lengthStr.substr(0, lengthStr.find('.') + 2) + " km";
        card.lengthText->setString(lengthStr);
        card.lengthText->setCharacterSize(11);
        card.lengthText->setFillColor(sf::Color(100, 180, 255));
        card.lengthText->setPosition(sf::Vector2f(startX + 200.0f, cardY + 38.0f));
        
        card.difficultyText = std::make_unique<sf::Text>(m_font);
        card.difficultyText->setString(getDifficultyString(m_availableTracks[i].difficulty));
        card.difficultyText->setCharacterSize(10);
        card.difficultyText->setFillColor(card.difficultyColor);
        card.difficultyText->setPosition(sf::Vector2f(startX + 280.0f, cardY + 15.0f));
        
        m_trackCards.push_back(std::move(card));
    }
}

void TrackSelectState::updateSelection(int newIndex) {
    if (newIndex < 0 || newIndex >= static_cast<int>(m_trackCards.size())) {
        return;
    }
    
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_trackCards.size())) {
        m_trackCards[m_selectedIndex].isSelected = false;
        m_trackCards[m_selectedIndex].cardBg->setOutlineColor(sf::Color(50, 50, 60));
        m_trackCards[m_selectedIndex].cardBg->setFillColor(sf::Color(30, 30, 40, 230));
    }
    
    m_selectedIndex = newIndex;
    m_trackCards[m_selectedIndex].isSelected = true;
    m_trackCards[m_selectedIndex].cardBg->setOutlineColor(sf::Color(100, 150, 255));
    m_trackCards[m_selectedIndex].cardBg->setFillColor(sf::Color(40, 50, 70, 240));
    
    updateDetailPanel();
}

void TrackSelectState::updateDetailPanel() {
    if (m_selectedIndex < 0 || m_selectedIndex >= static_cast<int>(m_availableTracks.size())) {
        return;
    }
    
    const auto& track = m_availableTracks[m_selectedIndex];
    
    m_detailName->setString(track.name);
    m_detailCountry->setString("Location: " + track.country);
    
    std::string lengthStr = std::to_string(track.lengthKm);
    lengthStr = lengthStr.substr(0, lengthStr.find('.') + 2);
    m_detailLength->setString("Length: " + lengthStr + " km");
    
    m_detailLaps->setString("Laps: " + std::to_string(track.recommendedLaps));
    
    m_detailDifficulty->setString("Difficulty: " + getDifficultyString(track.difficulty));
    m_detailDifficulty->setFillColor(getDifficultyColor(track.difficulty));
    
    m_detailDescription->setString(track.description);
    
    m_trackPreview->setFillColor(sf::Color(
        static_cast<std::uint8_t>(30 + getDifficultyColor(track.difficulty).r / 10),
        static_cast<std::uint8_t>(30 + getDifficultyColor(track.difficulty).g / 10),
        static_cast<std::uint8_t>(40 + getDifficultyColor(track.difficulty).b / 10)
    ));
}

void TrackSelectState::handleInput(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        const auto* key = event.getIf<sf::Event::KeyPressed>();

        switch (key->code) {
            case sf::Keyboard::Key::Up:
            case sf::Keyboard::Key::W:
                if (m_selectedIndex > 0) {
                    AudioManager::getInstance().playSfx("menu_select");
                    updateSelection(m_selectedIndex - 1);
                }
                break;

            case sf::Keyboard::Key::Down:
            case sf::Keyboard::Key::S:
                if (m_selectedIndex < static_cast<int>(m_trackCards.size()) - 1) {
                    AudioManager::getInstance().playSfx("menu_select");
                    updateSelection(m_selectedIndex + 1);
                }
                break;

            case sf::Keyboard::Key::Enter:
            case sf::Keyboard::Key::Space:
                AudioManager::getInstance().playSfx("gamemode_select");
                confirmSelection();
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

        for (size_t i = 0; i < m_trackCards.size(); ++i) {
            if (m_trackCards[i].cardBg->getGlobalBounds().contains(mousePos)) {
                if (m_selectedIndex != static_cast<int>(i)) {
                    AudioManager::getInstance().playSfx("menu_select");
                    updateSelection(static_cast<int>(i));
                }
                break;
            }
        }

        m_backHovered = m_backButton->getGlobalBounds().contains(mousePos);
        m_startHovered = m_startButton->getGlobalBounds().contains(mousePos);
    }
    else if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos(static_cast<float>(mouseEvent->position.x),
                                  static_cast<float>(mouseEvent->position.y));

            for (size_t i = 0; i < m_trackCards.size(); ++i) {
                if (m_trackCards[i].cardBg->getGlobalBounds().contains(mousePos)) {
                    AudioManager::getInstance().playSfx("menu_select");
                    updateSelection(static_cast<int>(i));
                    break;
                }
            }

            if (m_backButton->getGlobalBounds().contains(mousePos)) {
                m_game->getStateManager()->popState();
            }
            if (m_startButton->getGlobalBounds().contains(mousePos)) {
                AudioManager::getInstance().playSfx("gamemode_select");
                confirmSelection();
            }
        }
    }
}

void TrackSelectState::confirmSelection() {
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_availableTracks.size())) {
        const auto& selectedTrack = m_availableTracks[m_selectedIndex];
        std::cout << "[TrackSelect] Selected track: " << selectedTrack.name << std::endl;
        
        if (m_gameMode == GameMode::Campaign) {
            auto campaignTracks = CampaignDataManager::getTracks();
            
            CampaignTrackData campaignTrack;
            bool found = false;
            
            for (const auto& ct : campaignTracks) {
                if (ct.trackId == selectedTrack.id || ct.name == selectedTrack.name) {
                    campaignTrack = ct;
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                campaignTrack.id = m_selectedIndex + 1;
                campaignTrack.trackId = selectedTrack.id;
                campaignTrack.name = selectedTrack.name;
                campaignTrack.description = selectedTrack.description;
                campaignTrack.unlocked = true;
                
                if (selectedTrack.name == "Test Oval" || selectedTrack.id == "test_oval") {
                    campaignTrack.requiredLaps = 5;
                } else {
                    campaignTrack.requiredLaps = selectedTrack.recommendedLaps;
                }
                
                switch (selectedTrack.difficulty) {
                    case TrackDifficulty::Easy:
                        campaignTrack.difficulty = CampaignTrackDifficulty::Easy;
                        break;
                    case TrackDifficulty::Medium:
                        campaignTrack.difficulty = CampaignTrackDifficulty::Medium;
                        break;
                    case TrackDifficulty::Hard:
                    case TrackDifficulty::Expert:
                        campaignTrack.difficulty = CampaignTrackDifficulty::Hard;
                        break;
                }
                
                campaignTrack.tasks[0] = CampaignTask(1, CampaignTaskType::FinishRace, 0.0f, "Complete the race");
                campaignTrack.tasks[1] = CampaignTask(2, CampaignTaskType::BeatTime, 180.0f, "Finish under 3:00");
                campaignTrack.tasks[2] = CampaignTask(3, CampaignTaskType::NoSpinouts, 0.0f, "No spinouts");
            } else {
                if (selectedTrack.name == "Test Oval" || selectedTrack.id == "test_oval") {
                    campaignTrack.requiredLaps = 5;
                }
            }
            
            std::cout << "[TrackSelect] Going to TaskSelectState for Campaign (Laps: " 
                      << campaignTrack.requiredLaps << ")" << std::endl;
            m_game->getStateManager()->changeState(
                std::make_unique<TaskSelectState>(m_game, campaignTrack, &selectedTrack)
            );
        } else {
            std::cout << "[TrackSelect] Starting race directly" << std::endl;
            m_game->getStateManager()->changeState(
                std::make_unique<PlayState>(m_game, m_gameMode, &selectedTrack)
            );
        }
    }
}
void TrackSelectState::onEnter(){
    std::cout << "[TaskSelect] Entered" << std::endl;
    AudioManager::getInstance().playMusic("main_menu", true);

}

void TrackSelectState::onExit(){
    std::cout << "[TaskSelect] Entered" << std::endl;
    AudioManager::getInstance().playMusic("main_menu", true);
}

void TrackSelectState::update(float deltaTime) {
    m_animTimer += deltaTime;
    
    for (auto& card : m_trackCards) {
        if (card.isSelected) {
            card.hoverGlow += deltaTime * 3.0f;
            float glowIntensity = (std::sin(card.hoverGlow) + 1.0f) * 0.5f;
            
            sf::Color glowColor(
                static_cast<std::uint8_t>(100 + 50 * glowIntensity),
                static_cast<std::uint8_t>(150 + 50 * glowIntensity),
                static_cast<std::uint8_t>(255)
            );
            card.cardBg->setOutlineColor(glowColor);
            card.cardBg->setOutlineThickness(3.0f);
        } else {
            card.hoverGlow = 0.0f;
            card.cardBg->setOutlineThickness(2.0f);
        }
    }
    
    if (m_backHovered) {
        m_backButton->setFillColor(sf::Color(80, 50, 50));
    } else {
        m_backButton->setFillColor(sf::Color(60, 40, 40));
    }
    
    if (m_startHovered) {
        m_startButton->setFillColor(sf::Color(50, 150, 50));
        m_startButton->setOutlineThickness(4.0f);
    } else {
        m_startButton->setFillColor(sf::Color(40, 120, 40));
        m_startButton->setOutlineThickness(3.0f);
    }
}

void TrackSelectState::render(sf::RenderWindow& window) {
    window.clear(sf::Color(15, 15, 20));
    
    for (int i = 0; i < 8; ++i) {
        float xOffset = std::sin(m_animTimer * 0.3f + i * 0.4f) * 30.0f;
        auto& settings = SettingsManager::getInstance();
        sf::RectangleShape line(sf::Vector2f(2.0f,  static_cast<float>(settings.getWindowHeight()))); 
        line.setPosition(sf::Vector2f(160.0f + i * 140.0f + xOffset, 0.0f));
        line.setFillColor(sf::Color(25, 25, 35, 80));
        window.draw(line);
    }
    
    window.draw(*m_headerText);
    window.draw(*m_modeText);
    
    for (auto& card : m_trackCards) {
        window.draw(*card.cardBg);
        window.draw(*card.difficultyBadge);
        window.draw(*card.nameText);
        window.draw(*card.countryText);
        window.draw(*card.lengthText);
        window.draw(*card.difficultyText);
    }
    
    window.draw(*m_detailPanel);
    window.draw(*m_trackPreview);
    window.draw(*m_detailName);
    window.draw(*m_detailCountry);
    window.draw(*m_detailLength);
    window.draw(*m_detailLaps);
    window.draw(*m_detailDifficulty);
    window.draw(*m_detailDescription);
    
    window.draw(*m_backButton);
    window.draw(*m_backText);
    window.draw(*m_startButton);
    window.draw(*m_startText);
    
    window.draw(*m_hintText);
}