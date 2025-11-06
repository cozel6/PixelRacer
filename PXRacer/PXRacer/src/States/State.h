#pragma once
#include <SFML/Graphics.hpp>

//Forward declaration
class Game;

class State {
public:
		explicit State(Game* game) : m_game(game) {}
		virtual ~State() = default;

		//Core state functions
		virtual void handleInput(const sf::Event& event) = 0;
		virtual void update(float deltaTime) = 0;
		virtual void render(sf::RenderWindow& window) = 0;

		//Lifecycle hooks
		virtual void onEnter(){}
		virtual void onExit(){}
		virtual void onPause(){}
		virtual void onResume(){}

	protected:
		Game* m_game;
	};
