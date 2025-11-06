#include "StateManager.h"
#include "State.h"
#include "Core/Game.h"
#include <iostream>

StateManager::StateManager(Game* game)
    : m_game(game)
{
    std::cout << "StateManager initialized" << std::endl;
}

StateManager::~StateManager() {
    std::cout << "StateManager shutting down..." << std::endl;
    clearStates();
}

void StateManager::pushState(std::unique_ptr<State> state) {
    if (!m_states.empty()) {
        m_states.top()->onExit();
    }
    m_states.push(std::move(state));
    m_states.top()->onEnter();
}

void StateManager::popState() {
    if (!m_states.empty()) {
        m_states.top()->onExit();
        m_states.pop();

        if (!m_states.empty()) {
            m_states.top()->onEnter();
        }
    }
}

void StateManager::changeState(std::unique_ptr<State> state) {
    clearStates();
    pushState(std::move(state));
}

void StateManager::clearStates() {
    while (!m_states.empty()) {
        popState();
    }
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
    return m_states.empty() ? nullptr : m_states.top().get();
}