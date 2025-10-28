#include "States/MenuState.h"
#include "Core/Game.h"
#include "Core/Constants.h"
#include <iostream>

MenuState::MenuState(Game* game)
	: State(game)
	, m_blinkTimer(0.0f)
	, m_showStartText(true)
{
	// Allocate texts
	m_titleText = std::make_unique<sf::Text>(m_font, sf::String());
	m_startText = std::make_unique<sf::Text>(m_font, sf::String());
	m_versionText = std::make_unique<sf::Text>(m_font, sf::String());

	// Load font (SFML3 uses openFromFile)
	if (!m_font.openFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
		std::cerr << "Failed to load font, using default" << std::endl;
	}

	// Title text
	m_titleText->setString(sf::String("PIXELRACER"));
	m_titleText->setCharacterSize(80);
	m_titleText->setFillColor(Config::ACCENT_COLOR);
	m_titleText->setStyle(sf::Text::Bold);

	// Center title using Rect API (position + size)
	auto titleBounds = m_titleText->getLocalBounds();
	m_titleText->setOrigin(titleBounds.position + titleBounds.size *0.5f);
	m_titleText->setPosition(sf::Vector2f(Config::WINDOW_WIDTH *0.5f, Config::WINDOW_HEIGHT /3.0f));

	// Start text
	m_startText->setString(sf::String("Press ENTER to Start"));
	m_startText->setCharacterSize(24);
	m_startText->setFillColor(Config::TEXT_COLOR);
	m_startText->setPosition(sf::Vector2f(Config::WINDOW_WIDTH *0.5f, Config::WINDOW_HEIGHT *0.66f));
	m_startText->setOrigin(m_startText->getLocalBounds().position + m_startText->getLocalBounds().size *0.5f);

	// Version text
	m_versionText->setString(sf::String("Alpha0.1 - Development Build"));
	m_versionText->setCharacterSize(16);
	m_versionText->setFillColor(sf::Color(150,150,150));
	m_versionText->setPosition(sf::Vector2f(10.0f, Config::WINDOW_HEIGHT -30.0f));
}

void MenuState::handleInput(const sf::Event& event) {
	// SFML3 event handling (simplified)
	if (event.is<sf::Event::KeyPressed>()) {
		const auto* key = event.getIf<sf::Event::KeyPressed>();
		if (key->code == sf::Keyboard::Key::Enter) {
			std::cout << "Starting game..." << std::endl;
			m_game->quit();
		}
		else if (key->code == sf::Keyboard::Key::Escape) {
			std::cout << "Exiting game..." << std::endl;
			m_game->quit();
		}
	}
}

void MenuState::update(float deltaTime) {
	m_blinkTimer += deltaTime;
	if (m_blinkTimer >0.5f) {
		m_showStartText = !m_showStartText;
		m_blinkTimer =0.0f;
	}
}

void MenuState::render(sf::RenderWindow& window) {
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