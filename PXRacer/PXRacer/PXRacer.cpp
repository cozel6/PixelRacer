#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std::literals;

int main()
{
    std::cout << "PixelRacer - Test with Red Square!" << std::endl;

    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "PixelRacer - Square Test");
    window.setFramerateLimit(60);

    // Red square setup for testing
    sf::RectangleShape testSquare({ 100.f, 100.f });
    testSquare.setFillColor(sf::Color::Red);
    testSquare.setPosition({ 50.f, 50.f });
    testSquare.setOutlineThickness(3.f);
    testSquare.setOutlineColor(sf::Color::White);

    sf::CircleShape face(150.f);
    face.setFillColor(sf::Color::Yellow);
    face.setPosition({ 250.f, 150.f });
    face.setOutlineThickness(5.f);
    face.setOutlineColor(sf::Color::Black);

    sf::CircleShape leftEye(20.f);
    leftEye.setFillColor(sf::Color::Black);
    leftEye.setPosition({ 330.f, 240.f });

    sf::CircleShape rightEye(20.f);
    rightEye.setFillColor(sf::Color::Black);
    rightEye.setPosition({ 420.f, 240.f });

    sf::CircleShape smile(80.f);
    smile.setFillColor(sf::Color::Transparent);
    smile.setOutlineThickness(5.f);
    smile.setOutlineColor(sf::Color::Black);
    smile.setPosition({ 320.f, 280.f });

    sf::RectangleShape smileCutter({ 160.f, 80.f });
    smileCutter.setFillColor(sf::Color::Yellow);
    smileCutter.setPosition({ 320.f, 280.f });

    // Game loop
    while (window.isOpen())
    {
        // Event handling
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->code == sf::Keyboard::Key::Escape)
                    window.close();
            }
        }

        // Render
        window.clear(sf::Color(100, 149, 237));

        window.draw(testSquare);

        window.draw(face);
        window.draw(leftEye);
        window.draw(rightEye);
        window.draw(smile);
        window.draw(smileCutter);

        window.display();
    }

    std::cout << "PixelRacer - Closing..." << std::endl;
    return 0;
}
