#include "MenuState.h"
#include "Core/Game.h"
#include "Core/Constants.h"
#include "States/StateManager.h"
#include "States/MainMenuState.h"
#include <iostream>

/**
 * Constructor - Initializes the main menu
 * Creates all text objects and initializes the animation
 */
MenuState::MenuState(Game *game)
	: State(game)
	, m_blinkTimer(0.0f)
	, m_showStartText(true)
{
	// === Allocate text objects ===
	m_titleText = std::make_unique<sf::Text>(m_font, sf::String());
	m_startText = std::make_unique<sf::Text>(m_font, sf::String());
	m_versionText = std::make_unique<sf::Text>(m_font, sf::String());

	//Load background
	 m_backgroundTexture = std::make_unique<sf::Texture>();

    if (m_backgroundTexture->loadFromFile("assets/textures/mainmenubg.png")) {
        m_backgroundSprite = std::make_unique<sf::Sprite>(*m_backgroundTexture);

        auto textureSize = m_backgroundTexture->getSize();
        float scaleX = static_cast<float>(Config::WINDOW_WIDTH) / textureSize.x;
        float scaleY = static_cast<float>(Config::WINDOW_HEIGHT) / textureSize.y;
        m_backgroundSprite->setScale(sf::Vector2f(scaleX, scaleY));
    }
    else {
    std::cerr<< "Failed to load background texture!" <<std::endl;
    m_backgroundTexture.reset();
    }
    // Load font
    if (!m_font.openFromFile("assets/fonts/PressStart2P-Regular.ttf")) {
        std::cerr << "Failed to load font in MainMenuState" << std::endl;
    }
	// === Load font ===
	// Path is relative to working directory (set by CMake or run.sh)
	if (!m_font.openFromFile("assets/fonts/PressStart2P-Regular.ttf"))
	{
		std::cerr << "Failed to load font, using default" << std::endl;
	}

	// === Configure title text (used only for calculation) ===
	// Not drawn directly, but used by TitleAnimation for positioning
	m_titleText->setString(sf::String("PIXELRACER"));
	m_titleText->setCharacterSize(80);
	m_titleText->setFillColor(sf::Color(30, 30, 30));
	m_titleText->setStyle(sf::Text::Bold);

	// Center the title using Rect API (position + size)
	auto titleBounds = m_titleText->getLocalBounds();
	m_titleText->setOrigin(titleBounds.position + titleBounds.size * 0.5f);
	m_titleText->setPosition(sf::Vector2f(Config::WINDOW_WIDTH * 0.5f, Config::WINDOW_HEIGHT / 3.0f));

	// === Configure "Press ENTER to Start" text ===
	m_startText->setString(sf::String("Press ENTER to Start"));
	m_startText->setCharacterSize(24);
	m_startText->setFillColor(Config::TEXT_COLOR);

	// Center the text properly using global bounds
	auto startBounds = m_startText->getGlobalBounds();
	m_startText->setOrigin(sf::Vector2f(startBounds.size.x * 0.5f, startBounds.size.y * 0.5f));
	m_startText->setPosition(sf::Vector2f(Config::WINDOW_WIDTH * 0.5f, Config::WINDOW_HEIGHT * 0.66f));

	// === Configure version text ===
	m_versionText->setString(sf::String("Alpha0.1 - Development Build"));
	m_versionText->setCharacterSize(16);
	m_versionText->setFillColor(sf::Color(150, 150, 150));
	m_versionText->setPosition(sf::Vector2f(10.0f, Config::WINDOW_HEIGHT - 30.0f));

	// Title Animation
	m_titleAnimation = std::make_unique<TitleAnimation>(
		m_font,
		sf::Vector2f(Config::WINDOW_WIDTH * 0.5f, Config::WINDOW_HEIGHT / 3.0f),
		Config::WINDOW_HEIGHT
	);
}

/**
 * handleInput() - Handles user input
 * Enter = transitions to Main Menu, Escape = closes the application
 */
void MenuState::handleInput(const sf::Event &event)
{
	// Check if a key is pressed (SFML3 syntax)
	if (event.is<sf::Event::KeyPressed>())
	{
		const auto *key = event.getIf<sf::Event::KeyPressed>();

		// ENTER = Move to Main Menu state
		if (key->code == sf::Keyboard::Key::Enter)
		{
			std::cout << "Moving to Main Menu..." << std::endl;
			m_game->getStateManager()->changeState(std::make_unique<MainMenuState>(m_game));
		}
		// ESCAPE = shut down application
		else if (key->code == sf::Keyboard::Key::Escape)
		{
			std::cout << "Exiting game..." << std::endl;
			m_game->quit();
		}
	}
}

/**
 * update() - Updates the menu state every frame
 */
void MenuState::update(float deltaTime)
{
	// Update animation (if not finished, will move car and letters)
	m_titleAnimation->update(deltaTime);

	// Blink "Press ENTER" text after animation is complete
	if (m_titleAnimation->isComplete())
	{
		m_blinkTimer += deltaTime;

		// Toggle visibility every 0.5 seconds
		if (m_blinkTimer >= 0.5f)  
		{
			m_showStartText = !m_showStartText;
			m_blinkTimer = 0.0f;
		}
	}
}

/**
 * render() - Draws the menu on the screen
 * Delegates drawing animation to TitleAnimation
 */
void MenuState::render(sf::RenderWindow &window)
{


	if (m_backgroundSprite) {
		window.draw(*m_backgroundSprite);
	}
	// === Draw animation (car, ropes, letters) ===
	m_titleAnimation->render(window);

	// === Draw "Press ENTER to Start" only AFTER animation is complete ===
	// Blinks on/off every 0.5 seconds
	if (m_titleAnimation->isComplete() && m_showStartText)
	{
		window.draw(*m_startText);
	}

	// === Draw version text ===
	window.draw(*m_versionText);
}

/**
 * onEnter() - Called when entering this state
 */
void MenuState::onEnter()
{
	std::cout << "Entered Menu State" << std::endl;
}