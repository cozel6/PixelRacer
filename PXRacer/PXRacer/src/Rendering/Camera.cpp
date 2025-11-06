#include "Camera.h"

Camera::Camera()
    : m_position(0.0f, 0.0f), m_zoom(1.0f), m_rotation(0.0f)
{
}

void Camera::setPosition(const sf::Vector2f& position)
{
    m_position = position;
}

void Camera::setZoom(float zoom)
{
    m_zoom = zoom;
}

void Camera::setRotation(float rotation)
{
    m_rotation = rotation;
}

const sf::Vector2f& Camera::getPosition() const
{
    return m_position;
}

float Camera::getZoom() const
{
    return m_zoom;
}

float Camera::getRotation() const
{
    return m_rotation;
}

void Camera::applyToWindow(sf::RenderWindow& window)
{
    // SFML 3.0 - folosește constructorul cu center și size
    m_view = sf::View(m_position, sf::Vector2f(800.0f / m_zoom, 600.0f / m_zoom));
    m_view.setRotation(sf::degrees(m_rotation));
    window.setView(m_view);
}

void Camera::move(const sf::Vector2f& offset)
{
    m_position += offset;
}

void Camera::zoom(float factor)
{
    m_zoom *= factor;
}

void Camera::rotate(float angle)
{
    m_rotation += angle;
}

sf::Vector2f Camera::screenToWorld(const sf::Vector2f& screenPoint) const
{
    // SFML 3.0 - folosește mapPixelToCoords pe window, nu pe view
    // Pentru acum, returnăm direct (poți implementa mai târziu)
    return screenPoint;
}

sf::Vector2f Camera::worldToScreen(const sf::Vector2f& worldPoint) const
{
    // SFML 3.0 - folosește mapCoordsToPixel pe window, nu pe view  
    // Pentru acum, returnăm direct (poți implementa mai târziu)
    return worldPoint;
}