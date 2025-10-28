#pragma once
#include "States/State.h"
#include <SFML/Graphics.hpp>

class MenuState : public State {
	public:
		explicit MenuState(Game* game);
		~MenuState() override = default;

		void handleInput(const sf::Event& event) override;
		void update(float deltaTime) override;
		void render(sf::RenderWindow& window) override;

		void onEnter() override;

		private:
			sf::Text m_titleText;
			sf::Text m_startText;
			sf::Text m_versionText;
			sf::Font m_font;

			float m_blinkTimer;
			bool m_showStartText;
};