#include "States/StateManager.h"
#include "States/State.h"
#include <iostream>

StateManager::StateManager(Game* game)
	: m_game(game)
{

}

StateManager::~StateManager() {
	clearStates();
}

void StateManager::pushState(std::unique_ptr<State> state) {
	//Pause current state if exists
	if (!m_states.empty()) {
		m_states.top()->onPause();
	}

	//Add new state
	state->onEnter();
	m_states.push(std::move(state));

	std::cout<<" State pushed (stack size: "<< m_states.size() << ")" <<std::endl;
}

void StateManager::popState() {
	if (!m_states.empty()) {
		//Exit current state
		m_states.top()->onExit();
		m_states,pop();

		//Resume previous state if exists
		if (!m_states.empty()) {
			m_states.top()->onResume();
		}

		std::cout<<"State popped (stack size: "<<m_states.size()<<")"<<std::endl;
	}
	}

void StateManager::changeState(std::unique_ptr<State> state) {
	//Remove current state
	if (!m_states.empty()) {
		m_states.top()->onExit();
		m_states.pop();
	}
	//Add new state
	state->onEnter();
	m_states.push(std::move(state));

	std::cout<<"State changed (stack size: "<< m_states.size()<<")"<< srd::endl;

	}

void StateManager::clearStates() {
	while (!m_states.empty()) {
		m_states.top()->onExit();
		m_states.pop();
	}
	std::cout<<"All states cleared"<<std::endl;
	}

void StateManager::handleInput(const sf::Event& event) {
	if (!m_states.empty()) {
		m_states.top()->handleInput(event);
	}

}

void StateManager::update(float deltaTime) {
	if (!m_states.empty()) {
		m_states.top()->update(deltaTime);
	}
}

void StateManager::render(sf::RenderWindow& window) {

	if (!m_states.empty()) {
		m_states.top()->render(window);
   }
}

State* StateManager::getCurrentState() const {
	if (!m_states.empty()) {
		return m_states.top().get();
	}
	return nullptr;
}
