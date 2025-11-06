#pragma once
#include <SFML/Graphics.hpp>

class Camera
{
private:
    sf::Vector2f m_position;
    float m_zoom;
    float m_rotation;
    sf::View m_view;

public:
    Camera();

    void setPosition(const sf::Vector2f& position);
    void setZoom(float zoom);
    void setRotation(float rotation);

    const sf::Vector2f& getPosition() const;
    float getZoom() const;
    float getRotation() const;

    void applyToWindow(sf::RenderWindow& window);
    void move(const sf::Vector2f& offset);
    void zoom(float factor);
    void rotate(float angle);

    sf::Vector2f screenToWorld(const sf::Vector2f& screenPoint) const;
    sf::Vector2f worldToScreen(const sf::Vector2f& worldPoint) const;
};