#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <stack>

class State;
class Game;

class StateManager {
public: 
		explicit StateManager(Game* game);
		~StateManager();

		//State management
		void pushState(std::unique_ptr<State> state);
		void popState();
		void changeState(std::unique_ptr<State> state);
		void clearStates();

		//Game loop functions
		void handleInput(const sf::Event& event);
		void update(float deltaTime);
		void render(sf::RenderWindow& window);

		//Queries
		bool isEmpty() const { return m_states.empty();  }
		State* getCurrentState() const;

private: 
		Game* m_game;
		std::stack<std::unique_ptr<State>>m_states;
};


