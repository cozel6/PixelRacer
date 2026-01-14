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
    executeClearStates();
}

// Public interface - queue operations instead of executing immediately
void StateManager::pushState(std::unique_ptr<State> state) {
    m_pendingOperations.push_back({StateOperation::Push, std::move(state)});
}

void StateManager::popState() {
    m_pendingOperations.push_back({StateOperation::Pop, nullptr});
}

void StateManager::changeState(std::unique_ptr<State> state) {
    m_pendingOperations.push_back({StateOperation::Change, std::move(state)});
}

void StateManager::clearStates() {
    m_pendingOperations.push_back({StateOperation::Clear, nullptr});
}

// Process all pending state changes - called once per frame after render
void StateManager::processStateChanges() {
    if (m_pendingOperations.empty()) {
        return; // Early exit if nothing to process
    }

    std::cout << "[StateManager] Processing " << m_pendingOperations.size()
              << " pending operation(s)" << std::endl;

    for (auto& operation : m_pendingOperations) {
        switch (operation.operation) {
            case StateOperation::Pop:
                std::cout << "[StateManager] Executing Pop operation" << std::endl;
                executePopState();
                break;
            case StateOperation::Push:
                std::cout << "[StateManager] Executing Push operation" << std::endl;
                executePushState(std::move(operation.state));
                break;
            case StateOperation::Change:
                std::cout << "[StateManager] Executing Change operation" << std::endl;
                executeChangeState(std::move(operation.state));
                break;
            case StateOperation::Clear:
                std::cout << "[StateManager] Executing Clear operation" << std::endl;
                executeClearStates();
                break;
        }
    }
    m_pendingOperations.clear();
    std::cout << "[StateManager] All operations processed" << std::endl;
}

// Private execution methods - these do the actual state management
void StateManager::executePushState(std::unique_ptr<State> state) {
    if (!m_states.empty()) {
        m_states.top()->onExit();
    }
    m_states.push(std::move(state));
    m_states.top()->onEnter();
}

void StateManager::executePopState() {
    if (!m_states.empty()) {
        m_states.top()->onExit();
        m_states.pop();

        if (!m_states.empty()) {
            m_states.top()->onEnter();
        }
    }
}

void StateManager::executeChangeState(std::unique_ptr<State> state) {
    executeClearStates();
    executePushState(std::move(state));
}

void StateManager::executeClearStates() {
    while (!m_states.empty()) {
        executePopState();
    }
}

// Game loop functions - these interact with current state
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
