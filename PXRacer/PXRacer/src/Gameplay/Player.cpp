#include "Player.h"
#include "Road.h"
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cmath>

Player::Player()
    : m_positionX(0.0f)
    , m_positionZ(0.0f)
    , m_speed(0.0f)
    , m_rotation(0.0f)
    , m_isSpinning(false)
    , m_spinTimer(0.0f)
    , m_currentGrip(PlayerConfig::FULL_GRIP)
    , m_offRoadTimer(0.0f)
{
}

void Player::getWheelPositions(float& flX, float& flZ, 
                                float& frX, float& frZ,
                                float& rlX, float& rlZ,
                                float& rrX, float& rrZ) const {
    flX = m_positionX - PlayerConfig::WHEEL_OFFSET_X;
    flZ = m_positionZ + PlayerConfig::WHEEL_OFFSET_Z;
    
    frX = m_positionX + PlayerConfig::WHEEL_OFFSET_X;
    frZ = m_positionZ + PlayerConfig::WHEEL_OFFSET_Z;
    
    rlX = m_positionX - PlayerConfig::WHEEL_OFFSET_X;
    rlZ = m_positionZ - PlayerConfig::WHEEL_OFFSET_Z;
    
    rrX = m_positionX + PlayerConfig::WHEEL_OFFSET_X;
    rrZ = m_positionZ - PlayerConfig::WHEEL_OFFSET_Z;
}

void Player::update(float deltaTime, const WheelSurfaces& wheelSurfaces) {
    if (m_isSpinning) {
        handleSpinOut(deltaTime);
        return;
    }
    
    applyPhysics(deltaTime, wheelSurfaces); // ❌ Fără roadCurvature!
}

float Player::calculateGripFromWheels(const WheelSurfaces& wheelSurfaces) const {
    int wheelsOnRoad = wheelSurfaces.wheelsOnRoad();
    
    switch (wheelsOnRoad) {
        case 4: return PlayerConfig::FULL_GRIP;
        case 3: return PlayerConfig::PARTIAL_GRIP_3;
        case 2: return PlayerConfig::PARTIAL_GRIP_2;
        case 1: return PlayerConfig::PARTIAL_GRIP_1;
        default: return PlayerConfig::NO_GRIP;
    }
}

void Player::handleInput(float deltaTime, float gripMultiplier) { // ❌ ELIMINAT roadCurvature!
    // Acceleration
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        m_speed += PlayerConfig::ACCELERATION * deltaTime * gripMultiplier;
    }
    // Braking
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || 
             sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        m_speed -= PlayerConfig::BRAKING * deltaTime;
    }
    // Natural deceleration
    else {
        if (m_speed > 0.0f) {
            m_speed -= PlayerConfig::DECELERATION * deltaTime;
            if (m_speed < 0.0f) m_speed = 0.0f;
        } else if (m_speed < 0.0f) {
            m_speed += PlayerConfig::DECELERATION * deltaTime;
            if (m_speed > 0.0f) m_speed = 0.0f;
        }
    }

    // ❌ ELIMINAT complet roadPushForce - nu mai există împingere!
    
    // ✅ SIMPLU: Doar mișcare laterală pură!
    float steerPower = PlayerConfig::STEER_SPEED * gripMultiplier;
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        m_positionX -= steerPower * deltaTime;
        m_rotation = std::max(m_rotation - 90.0f * deltaTime, -25.0f);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        m_positionX += steerPower * deltaTime;
        m_rotation = std::min(m_rotation + 90.0f * deltaTime, 25.0f);
    }
    else {
        // Return to neutral rotation
        if (m_rotation > 0) {
            m_rotation = std::max(0.0f, m_rotation - 180.0f * deltaTime);
        } else if (m_rotation < 0) {
            m_rotation = std::min(0.0f, m_rotation + 180.0f * deltaTime);
        }
    }
}

void Player::applyPhysics(float deltaTime, const WheelSurfaces& wheelSurfaces) {
    float targetGrip = calculateGripFromWheels(wheelSurfaces);
    
    if (wheelSurfaces.anyOnGrass()) {
        m_speed *= PlayerConfig::GRASS_FRICTION;
        m_offRoadTimer += deltaTime;
        
        if (m_offRoadTimer > 0.5f && targetGrip < PlayerConfig::SPIN_THRESHOLD) {
            if (std::abs(m_speed) > PlayerConfig::MAX_SPEED * 0.3f) {
                m_isSpinning = true;
                m_spinTimer = 0.0f;
                return;
            }
        }
    } else {
        m_offRoadTimer = 0.0f;
    }
    
    if (wheelSurfaces.allOffTrack()) {
        m_speed *= 0.85f;
        m_offRoadTimer += deltaTime;
        
        if (m_offRoadTimer > 2.0f) {
            resetPosition();
            return;
        }
    }
    
    m_currentGrip += (targetGrip - m_currentGrip) * 5.0f * deltaTime;
    m_currentGrip = std::clamp(m_currentGrip, 0.1f, 1.0f);
    
    handleInput(deltaTime, m_currentGrip); // ❌ Fără roadCurvature!
    
    m_speed = std::clamp(m_speed, -PlayerConfig::MAX_SPEED * 0.5f, PlayerConfig::MAX_SPEED);
    m_positionX = std::clamp(m_positionX, -PlayerConfig::MAX_LATERAL_POSITION, PlayerConfig::MAX_LATERAL_POSITION);
    m_positionZ += m_speed * deltaTime;
}

void Player::handleSpinOut(float deltaTime) {
    m_spinTimer += deltaTime;
    m_rotation += PlayerConfig::SPIN_SPEED * deltaTime;
    if (m_rotation > 360.0f) m_rotation -= 360.0f;
    
    m_speed *= 0.9f;
    
    if (m_spinTimer >= PlayerConfig::RECOVERY_TIME) {
        m_isSpinning = false;
        m_rotation = 0.0f;
        m_spinTimer = 0.0f;
        m_speed *= 0.5f;
        
        if (std::abs(m_positionX) > PlayerConfig::MAX_LATERAL_POSITION * 0.5f) {
            resetPosition();
        }
    }
}

void Player::resetPosition() {
    m_positionX = 0.0f;
    m_speed *= 0.3f;
    m_rotation = 0.0f;
    m_isSpinning = false;
    m_spinTimer = 0.0f;
    m_offRoadTimer = 0.0f;
    m_currentGrip = PlayerConfig::ROAD_GRIP;
}

void Player::render(sf::RenderWindow& window, float screenX, float screenY, float scale) {
    const float carWidth = PlayerConfig::CAR_WIDTH * scale;
    const float carHeight = PlayerConfig::CAR_HEIGHT * scale;

    sf::RectangleShape car(sf::Vector2f(carWidth, carHeight));
    car.setOrigin(sf::Vector2f(carWidth * 0.5f, carHeight * 0.5f));
    car.setPosition(sf::Vector2f(screenX, screenY));
    car.setRotation(sf::degrees(m_rotation));
    
    if (m_isSpinning) {
        car.setFillColor(sf::Color::Yellow);
    } else if (m_currentGrip < PlayerConfig::SPIN_THRESHOLD) {
        car.setFillColor(sf::Color(255, 150, 0));
    } else {
        car.setFillColor(sf::Color::Red);
    }
    
    car.setOutlineColor(sf::Color::White);
    car.setOutlineThickness(2.0f * scale);

    window.draw(car);
}