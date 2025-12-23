#include "UI/TitleAnimation.h"
#include "Core/Constants.h"
#include <iostream>

// Constructor for TitleAnimation
TitleAnimation::TitleAnimation(const sf::Font& font, const sf::Vector2f& targetCenter, float windowHeight)
    : m_animationComplete(false)
    , m_animationPhase(0)
    , m_settleTimer(0.0f)
    , m_targetCenter(targetCenter)
{
    // Initialize car (rectangle placeholder)
    m_car.setSize(sf::Vector2f(80.0f, 40.0f));
    m_car.setFillColor(Config::ACCENT_COLOR);           // color
    m_car.setOutlineColor(sf::Color::White);
    m_car.setOutlineThickness(2.0f);
    
    // Position the car off-screen (left)
    m_carPosition = sf::Vector2f(-200.0f, windowHeight / 3.0f);
    
    // Set car speed (250 pixels/second to the right)
    m_carVelocity = sf::Vector2f(250.0f, 0.0f);
    
    // Create letters for "PIXELRACER"
    std::string word = "PIXELRACER";
    
    // Distance between letters on the rope (in pixels)
    float letterSpacing = 55.0f;
    
    // Create each letter
    for (size_t i = 0; i < word.length(); ++i)
    {
        // Create text object for current letter
        sf::Text letterText(font, sf::String(std::string(1, word[i])));
        letterText.setCharacterSize(80);                // Font size: 80px
        letterText.setFillColor(sf::Color(30,30,30));  // Black color
        letterText.setStyle(sf::Text::Bold);            // Bold font
        
        // Set origin to center of letter (for correct rotation/positioning)
        auto bounds = letterText.getLocalBounds();
        letterText.setOrigin(bounds.position + bounds.size * 0.5f);
        
        // Calculate distance of this letter from the car
        // First letter (P) is closest, last (R) is farthest
        float offsetFromCar = letterSpacing * (word.length() - i);
        
        // Position the letter behind the car
        sf::Vector2f initialPos(m_carPosition.x - offsetFromCar, m_carPosition.y);
        
        // Add letter to vector
        m_letters.push_back({
            letterText,                     // text
            initialPos,                     // position
            sf::Vector2f(0.0f, 0.0f),      // velocity (initially static)
            offsetFromCar,                  // offsetFromCar
            sf::Vector2f(0.0f, 0.0f)       // finalPosition (calculated later)
        });
    }
    // Calculate final positions for PERFECT centering
    // Use a temporary text with the full word to get accurate positioning
    sf::Text fullWordText(font, sf::String(word));
    fullWordText.setCharacterSize(80);
    fullWordText.setStyle(sf::Text::Bold);

    // Find the leftmost and rightmost positions of the word
    sf::Vector2f firstCharPos = fullWordText.findCharacterPos(0);
    sf::Vector2f lastCharPos = fullWordText.findCharacterPos(word.length() - 1);

    // Calculate the width of the last character
    sf::Text lastCharText(font, sf::String(std::string(1, word[word.length() - 1])));
    lastCharText.setCharacterSize(80);
    lastCharText.setStyle(sf::Text::Bold);
    float lastCharWidth = lastCharText.getGlobalBounds().size.x;

    // Total visual width = from start of first char to end of last char
    float totalWidth = (lastCharPos.x - firstCharPos.x) + lastCharWidth;

    // Calculate starting X for perfect centering
    float startX = m_targetCenter.x - (totalWidth / 2.0f);

    // Assign final positions based on character positions
    for (size_t i = 0; i < m_letters.size(); ++i)
    {
        // Get position of this character in the full word
        sf::Vector2f charPos = fullWordText.findCharacterPos(i);

        // Calculate offset from first character
        float offsetX = charPos.x - firstCharPos.x;

        // Get this letter's width for centering
        auto letterBounds = m_letters[i].text.getLocalBounds();

        // Final position with centered origin
        m_letters[i].finalPosition = sf::Vector2f(
            startX + offsetX + letterBounds.size.x / 2.0f,
            m_targetCenter.y
        );
    }
}  // 

// update() - Updates the animation each frame
void TitleAnimation::update(float deltaTime)
{
    // Do not update if animation is complete
    if (m_animationComplete) {
        return;
    }
    
    // PHASE 0: Car pulls letters with ropes
    if (m_animationPhase == 0)
    {
        // Move car to the right
        m_carPosition += m_carVelocity * deltaTime;
        
        // Update each letter to follow the car
        for (Letter& letter : m_letters)
        {
            // Calculate where the letter SHOULD be (behind the car)
            sf::Vector2f targetPos(m_carPosition.x - letter.offsetFromCar, m_carPosition.y);
            
            // Spring physics for natural movement
            float stiffness = 6.0f;     // How "stiff" the spring is (higher = more rigid)
            float damping = 0.85f;      // Damping (higher = less oscillation)
            
            // Calculate force towards target position
            sf::Vector2f displacement = targetPos - letter.position;
            sf::Vector2f springForce = displacement * stiffness;
            
            // Apply force and damping
            letter.velocity += springForce * deltaTime;
            letter.velocity *= damping;
            
            // Update letter position
            letter.position += letter.velocity * deltaTime;
        }
        
        // Check if FIRST letter (P) reached the center
        // When it does, switch to phase 1 (settling)
        if (!m_letters.empty() && m_letters[0].position.x >= m_targetCenter.x - 300.0f)
        {
            m_animationPhase = 1;   // Switch to settling phase
            m_settleTimer = 0.0f;   // Reset timer
        }
    }
    // PHASE 1: Letters settle, car disappears
    else if (m_animationPhase == 1)
    {
        // Increment settling timer
        m_settleTimer += deltaTime;
        
        // Car continues moving (exits screen)
        m_carPosition += m_carVelocity * deltaTime;
        
        // Letters continue moving with inertia (settle naturally)
        for (Letter& letter : m_letters)
        {
            // Softer spring physics for settling
            float stiffness = 6.0f;
            float damping = 0.93f;      // More damping = stop faster
            
            // Calculate where the letter is now (no longer following car)
            // Letters keep current position and stop gradually
            sf::Vector2f displacement = sf::Vector2f(0.0f, 0.0f) - letter.velocity;
            sf::Vector2f springForce = displacement * stiffness * 0.1f; // Small force for natural stop
            
            letter.velocity += springForce * deltaTime;
            letter.velocity *= damping;
            letter.position += letter.velocity * deltaTime;
        }
        
        // After 1.2 seconds, animation is complete
        if (m_settleTimer >= 1.2f)
        {
            m_animationComplete = true;
            m_animationPhase = 2;
        }
    }
}


void TitleAnimation::render(sf::RenderWindow& window)
{
    // Draw car (only if animation is not complete)
    if (!m_animationComplete)
    {
        m_car.setPosition(m_carPosition);
        window.draw(m_car);
        
        // Draw ropes (white lines) ONLY in phase 0
        if (m_animationPhase == 0)
        {
            // Attachment point on car (its center)
            sf::Vector2f carRopeAttachment = m_carPosition;
            
            // Draw a line from car to each letter
            for (const auto& letter : m_letters)
            {
                // Create a line (2 points: from car to letter)
                sf::Vertex line[] = {
                    {carRopeAttachment, sf::Color::White},  // Start point (car)
                    {letter.position, sf::Color::White}     // End point (letter)
                };
                
                // Draw the line
                window.draw(line, 2, sf::PrimitiveType::Lines);
            }
        }
    }
    
    // Draw letters (ALWAYS, in both phases and after)
    for (const auto& letter : m_letters)
    {
        // Create a temporary copy to set position
        sf::Text tempText = letter.text;
        tempText.setPosition(letter.position);
        window.draw(tempText);
    }
}