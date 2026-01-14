#include "Player.h"
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cmath>
#include <filesystem>

Player::Player()
    : m_positionX(0.0f), m_positionZ(0.0f), m_speed(0.0f), m_rotation(0.0f)
    , m_steeringVisualState(0), m_isSpinning(false), m_spinTimer(0.0f)
    , m_offRoadTimer(0.0f), m_wobbleOffset(0.0f), m_wobbleTimer(0.0f), m_totalDamage(0.0f)
{
    loadTextures();
}

void Player::loadTextures() {
    namespace fs = std::filesystem;
    std::string path = "assets/textures/";
    auto tryLoad = [&](sf::Texture& tex, const std::string& name) {
        return tex.loadFromFile(path + name);
        };

    if (tryLoad(m_textureStraight, "car_straight.png")) {
        m_texturesLoaded = true;
        if (!tryLoad(m_textureLeft, "car_left.png")) m_textureLeft = m_textureStraight;
        if (!tryLoad(m_textureRight, "car_right.png")) m_textureRight = m_textureStraight;
    }
}

void Player::getWheelPositions(float& flX, float& flZ, float& frX, float& frZ,
    float& rlX, float& rlZ, float& rrX, float& rrZ) const {
    flX = m_positionX - PlayerConfig::WHEEL_OFFSET_X; flZ = m_positionZ + PlayerConfig::WHEEL_OFFSET_Z;
    frX = m_positionX + PlayerConfig::WHEEL_OFFSET_X; frZ = m_positionZ + PlayerConfig::WHEEL_OFFSET_Z;
    rlX = m_positionX - PlayerConfig::WHEEL_OFFSET_X; rlZ = m_positionZ - PlayerConfig::WHEEL_OFFSET_Z;
    rrX = m_positionX + PlayerConfig::WHEEL_OFFSET_X; rrZ = m_positionZ - PlayerConfig::WHEEL_OFFSET_Z;
}

void Player::update(float deltaTime, const WheelSurfaces& wheelSurfaces, float roadCurve) {
    if (isDestroyed()) {
        m_speed *= 0.95f;
        m_rotation += 180.0f * deltaTime;
        m_positionZ += m_speed * deltaTime;
        return;
    }
    if (m_isSpinning) { handleSpinOut(deltaTime); return; }
    applyPhysics(deltaTime, wheelSurfaces, roadCurve);
}

void Player::handleInput(float deltaTime, float gripMultiplier) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up))
        m_speed += PlayerConfig::ACCELERATION * deltaTime * gripMultiplier;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down))
        m_speed -= PlayerConfig::BRAKING * deltaTime;
    else
        m_speed = std::max(0.0f, m_speed - PlayerConfig::DECELERATION * deltaTime);

    m_speed = std::min(m_speed, PlayerConfig::MAX_SPEED);
    float steerPower = PlayerConfig::STEER_SPEED * gripMultiplier;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Left)) {
        m_positionX -= steerPower * deltaTime; m_steeringVisualState = -1;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Right)) {
        m_positionX += steerPower * deltaTime; m_steeringVisualState = 1;
    }
    else {
        m_steeringVisualState = 0;
    }
}

void Player::render(sf::RenderWindow& window, float screenX, float screenY, float scale) {
    // MODIFICARE: Factor 1.3f pentru lățime (corecție aspect ratio)
    float drawW = PlayerConfig::CAR_WIDTH * scale * 1.3f;
    float drawH = PlayerConfig::CAR_HEIGHT * scale;

    if (m_texturesLoaded) {
        sf::Texture* tex = &m_textureStraight;
        if (m_steeringVisualState == -1) tex = &m_textureLeft;
        else if (m_steeringVisualState == 1) tex = &m_textureRight;

        sf::Sprite spr(*tex);
        sf::Vector2u size = tex->getSize();
        spr.setOrigin(sf::Vector2f(static_cast<float>(size.x) / 2.0f, static_cast<float>(size.y)));
        spr.setScale(sf::Vector2f(drawW / size.x, drawH / size.y));
        spr.setPosition(sf::Vector2f(screenX + m_wobbleOffset, screenY));
        spr.setRotation(sf::degrees(m_rotation));
        window.draw(spr);
    }
}

void Player::applyPhysics(float deltaTime, const WheelSurfaces& ws, float roadCurve) {
    float targetGrip = (ws.wheelsOnRoad() / 4.0f);
    if (ws.anyOnGrass()) {
        m_speed *= PlayerConfig::GRASS_FRICTION;
        m_offRoadTimer += deltaTime;
        if (m_offRoadTimer > 0.5f && m_speed > 35.0f) { m_isSpinning = true; m_spinTimer = 0.0f; return; }
    }
    else m_offRoadTimer = 0.0f;

    handleInput(deltaTime, std::max(0.2f, targetGrip));
    m_positionX += roadCurve * (m_speed / PlayerConfig::MAX_SPEED) * 45.0f * deltaTime;

    float ratio = m_speed / PlayerConfig::MAX_SPEED;
    if (ratio >= 0.9f) {
        m_wobbleTimer += deltaTime * 20.0f;
        m_wobbleOffset = std::sin(m_wobbleTimer) * 4.0f;
    }
    else m_wobbleOffset *= 0.9f;

    m_positionZ += m_speed * deltaTime;
    m_positionX = std::clamp(m_positionX, -PlayerConfig::MAX_LATERAL_POSITION, PlayerConfig::MAX_LATERAL_POSITION);
}

void Player::handleSpinOut(float dt) {
    m_spinTimer += dt; m_rotation += PlayerConfig::SPIN_SPEED * dt; m_speed *= 0.96f;
    if (m_spinTimer >= PlayerConfig::RECOVERY_TIME) { m_isSpinning = false; m_rotation = 0.0f; m_speed *= 0.4f; }
}

void Player::addDamage(float d) { m_totalDamage = std::min(m_totalDamage + d, PlayerConfig::MAX_DAMAGE); }
void Player::repair(float a) { m_totalDamage = std::max(0.0f, m_totalDamage - a); }
void Player::resetPosition() { m_positionX = 0.0f; m_speed = 0.0f; m_rotation = 0.0f; m_isSpinning = false; }