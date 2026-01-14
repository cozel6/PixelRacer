#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <stack>
#include <vector>

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
	void processStateChanges(); // Process pending state changes

	//Queries
	bool isEmpty() const { return m_states.empty();  }
	State* getCurrentState() const;


private:
	enum class StateOperation {
		Push,
		Pop,
		Change,
		Clear
	};

	struct PendingOperation {
		StateOperation operation;
		std::unique_ptr<State> state; // Only used for Push/Change
	};

	// Private execution methods (moved from public)
	void executePushState(std::unique_ptr<State> state);
	void executePopState();
	void executeChangeState(std::unique_ptr<State> state);
	void executeClearStates();

	Game* m_game;
	std::stack<std::unique_ptr<State>> m_states;
	std::vector<PendingOperation> m_pendingOperations; // Deferred operations queue
};

