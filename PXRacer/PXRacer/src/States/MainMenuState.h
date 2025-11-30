#pragma once
#include "State.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class Game;

class MainMenuState : public State {

public:
		explicit MainMenuState(Game* game);
		~MainMenuState() override = default;

		void handleInput(const sf::Event& event) override;
		void update(float deltaTime) override;
		void render(sf::RenderWindow& window) override;

		void onEnter() override;

private:
		std::unique_ptr<sf::Text> m_titleText;
		std::vector<std::unique_ptr<sf::Text>> m_menuItems;
		sf::Font m_font;

		std::unique_ptr<sf::Sprite> m_backgroundSprite;
		std::unique_ptr<sf::Texture> m_backgroundTexture;
		

		int m_selectedIndex;
		float m_blinkTimer;
		bool m_showSelector;

		void updateMenuDisplay();
};
