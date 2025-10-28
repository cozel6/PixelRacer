#include "States/MenuState.h"
#include "Core/Game.h"
#include "Core/Constants.h"
#include <iostream>

MenuState::MenuState(Game* game)
	: State(game)
	, m_blinkTimer(0.0f)
	, m_showStartText(true)
{

//Note: For now we're using the default font
//We can load custom fonts from assets/fonts/

	if (!m_fontloadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {

		std::cerr << "Failed to load font, using default"<<std::endl;

		//Title text
		m_titleText.setFont(m_font);
		m_titleText.setString("PIXELRACER");
		m_titleText.setCharacterSize(80);
		m_titleText.setFillColor(Config::ACCENT_COLOR);
		m_titleText.setStyle(sf::Text::Bold);

		//Center title
		sf::FloatRect titleBounds = m_titleText.getLocalBounds();
		m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.0f,
			titleBounds.top + titleBounds.height / 2.0f);
		m_titleText.setPosition(Config::WINDOW_WIDTH / 2.0f, Config::WINDOW_HEIGHT / 3.0f);

		//Version text
		m_versionText.setFont(m_font);
		m_versionText.setString("Alpha 0.1 - Development Build");
		m_versionText.setCharacterSize(16);
		m_versionText.setFillColor(sf::Color(150, 150, 150));
		m_versionText.setPosition(10.0f, Config::WINDOW_HEIGHT - 30.0f);
}

	void MenuState::handleInput(const sf::Event & event) {
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Enter) {
				std::cout<<"Starting gaem..."<<std::endl;
				//TODO: Change to PlayState when implemented
				//For now, just quit
				m_game->quit();
			}
			else if (event.key.code == sf::Keyboard::Escape) {
				std::cout<<"Exiting gaem..."<<std::endl;
				m_game->quit();
			}
		}
}

	void MenuState::render(sf::RenderWindow& window) {
		//Draw title
		window.draw(m_titleText);

		//Draw blinking start text
		if (m_showStartText) {
			window.draw(m_startText);
		}

		//Draw version

		window.draw(m_versionText);
}

	void MenuState::onEnter() {
		std::cout<<"Entered Menu State"<<std::endl;
}


}