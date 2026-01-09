#include "MenuState.h"
#include "Core/Game.h"
#include "Core/Constants.h"
#include "States/StateManager.h"
#include "States/MainMenuState.h"
#include "Core/SettingsManager.h"
#include <iostream>

// Constructor - Initializes the main menu
MenuState::MenuState(Game *game)
	: State(game)
	, m_blinkTimer(0.0f)
	, m_showStartText(true)
{
	auto& settings = SettingsManager::getInstance();

	m_titleText = std::make_unique<sf::Text>(m_font, sf::String());
	m_startText = std::make_unique<sf::Text>(m_font, sf::String());
	m_versionText = std::make_unique<sf::Text>(m_font, sf::String());

	m_backgroundTexture = std::make_unique<sf::Texture>();

	if (m_backgroundTexture->loadFromFile("assets/textures/mainmenubg.png")) {
		m_backgroundSprite = std::make_unique<sf::Sprite>(*m_backgroundTexture);

		auto textureSize = m_backgroundTexture->getSize();
		
		float scaleX = static_cast<float>(settings.getWindowWidth()) / textureSize.x;
		float scaleY = static_cast<float>(settings.getWindowHeight()) / textureSize.y;
		m_backgroundSprite->setScale(sf::Vector2f(scaleX, scaleY));
	}
	else {
		std::cerr << "Failed to load background texture!" << std::endl;
		m_backgroundTexture.reset();
	}

	if (!m_font.openFromFile("assets/fonts/PressStart2P-Regular.ttf")) {
		std::cerr << "Failed to load font, using default" << std::endl;
	}

	m_titleText->setString(sf::String("PIXELRACER"));
	m_titleText->setCharacterSize(80);
	m_titleText->setFillColor(sf::Color(30, 30, 30));
	m_titleText->setStyle(sf::Text::Bold);

	auto titleBounds = m_titleText->getLocalBounds();
	m_titleText->setOrigin(titleBounds.position + titleBounds.size * 0.5f);
	m_titleText->setPosition(sf::Vector2f(settings.getWindowWidth() * 0.5f, settings.getWindowHeight() / 3.0f));

	m_startText->setString(sf::String("Press ENTER to Start"));
	m_startText->setCharacterSize(24);
	m_startText->setFillColor(Config::TEXT_COLOR);

	auto startBounds = m_startText->getGlobalBounds();
	m_startText->setOrigin(sf::Vector2f(startBounds.size.x * 0.5f, startBounds.size.y * 0.5f));
	m_startText->setPosition(sf::Vector2f(settings.getWindowWidth() * 0.5f, settings.getWindowHeight() * 0.66f));

	m_versionText->setString(sf::String("Alpha0.1 - Development Build"));
	m_versionText->setCharacterSize(16);
	m_versionText->setFillColor(sf::Color(150, 150, 150));
	m_versionText->setPosition(sf::Vector2f(10.0f, settings.getWindowHeight() - 30.0f));

	m_titleAnimation = std::make_unique<TitleAnimation>(
		m_font,
		sf::Vector2f(settings.getWindowWidth() * 0.5f, settings.getWindowHeight() / 3.0f),
		settings.getWindowHeight()
	);
}


// Handles user input for Enter and Escape keys
void MenuState::handleInput(const sf::Event &event)
{
	if (event.is<sf::Event::KeyPressed>())
	{
		const auto *key = event.getIf<sf::Event::KeyPressed>();

		if (key->code == sf::Keyboard::Key::Enter)
		{
			std::cout << "Moving to Main Menu..." << std::endl;
			m_game->getStateManager()->changeState(std::make_unique<MainMenuState>(m_game));
		}
		else if (key->code == sf::Keyboard::Key::Escape)
		{
			std::cout << "Exiting game..." << std::endl;
			m_game->quit();
		}
	}
}

// Updates the menu state every frame
void MenuState::update(float deltaTime)
{
	m_titleAnimation->update(deltaTime);

	if (m_titleAnimation->isComplete())
	{
		m_blinkTimer += deltaTime;

		if (m_blinkTimer >= 0.5f)  
		{
			m_showStartText = !m_showStartText;
			m_blinkTimer = 0.0f;
		}
	}
}

// Draws the menu on the screen
void MenuState::render(sf::RenderWindow &window)
{
	if (m_backgroundSprite) {
		window.draw(*m_backgroundSprite);
	}

	m_titleAnimation->render(window);

	if (m_titleAnimation->isComplete() && m_showStartText)
	{
		window.draw(*m_startText);
	}

	window.draw(*m_versionText);
}

// Called when entering the menu state
void MenuState::onEnter()
{
	std::cout << "Entered Menu State" << std::endl;
}
