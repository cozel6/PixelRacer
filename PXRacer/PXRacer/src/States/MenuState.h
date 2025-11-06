#pragma once

#include "States/State.h"
#include "UI/TitleAnimation.h"  // Include new animation class
#include <SFML/Graphics.hpp>
#include <memory>

// MenuState class declaration
class MenuState : public State
{
public:
	MenuState(Game *game);

	// Overrides from State
	void handleInput(const sf::Event &event) override;
	void update(float deltaTime) override;
	void render(sf::RenderWindow &window) override;
	void onEnter() override;

private:
	// Font and texts for menu
	sf::Font m_font;
	
	// Title text (used only for position calculation, not drawn)
	std::unique_ptr<sf::Text> m_titleText;
	
	// "Press ENTER to Start" text
	std::unique_ptr<sf::Text> m_startText;
	
	// Version text (bottom corner)
	std::unique_ptr<sf::Text> m_versionText;

	// Animation (delegated to TitleAnimation)
	std::unique_ptr<TitleAnimation> m_titleAnimation;
	
	// Old variables for blink (no longer used, kept for compatibility)
	float m_blinkTimer;
	bool m_showStartText;
};