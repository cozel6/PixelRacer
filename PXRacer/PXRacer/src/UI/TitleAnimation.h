#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

//Ttile Animation for pixel racer
class TitleAnimation {
public:
    TitleAnimation(const sf::Font& font, const sf::Vector2f& targetCenter, float windowHeight);

    // Call this every frame to update the animation
    void update(float deltaTime);

    // Call this to draw the animation
    void render(sf::RenderWindow& window);

    // Check if the animation is complete
    bool isComplete() const {return m_animationComplete; }

    private:
    
    // Letter struct representing each animated letter
    struct Letter {
        sf::Text text;              // Text object for the letter
        sf::Vector2f position;      // Current position
        sf::Vector2f velocity;      // Current velocity for physics
        float offsetFromCar;        // Min distance from car (horizontal)
        sf::Vector2f finalPosition; // Target final centered position

    };
    // List of letters in the title
    std::vector<Letter> m_letters;

    // Car representation
    sf::RectangleShape m_car;       // Simple rectangle as car placeholder
    sf::Vector2f m_carPosition;
    sf::Vector2f m_carVelocity;

    // Animation state
    bool m_animationComplete;
    int m_animationPhase;    // 0 = car pulling, 1 = letters settling, 2 = complete
    float m_settleTimer;     // Timer for phase 1

    sf::Vector2f m_targetCenter;
};