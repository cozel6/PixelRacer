#include "Player.h"
#include "Road.h"
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>

Player::Player()
    : m_positionX(0.0f)
    , m_positionZ(0.0f)
    , m_speed(0.0f)
    , m_rotation(0.0f)
    , m_isSpinning(false)
    , m_spinTimer(0.0f)
    , m_currentGrip(PlayerConfig::FULL_GRIP)
    , m_offRoadTimer(0.0f)
    , m_isDrifting(false)
    , m_driftAngle(0.0f)
    , m_wobbleOffset(0.0f)
    , m_wobbleTimer(0.0f)
    , m_isBraking(false)
    , m_totalDamage(0.0f)
    , m_lastDamageThreshold(0)
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

// ═══════════════════════════════════════════════════════════════
// ✅ DAMAGE SYSTEM
// ═══════════════════════════════════════════════════════════════
void Player::addDamage(float damage) {
    float oldDamage = m_totalDamage;
    m_totalDamage += damage;
    
    // Clamp la max
    m_totalDamage = std::min(m_totalDamage, PlayerConfig::MAX_DAMAGE);
    
    // Verificăm dacă am trecut un prag de 10 DMG
    int oldThreshold = static_cast<int>(oldDamage / PlayerConfig::DAMAGE_SPEED_PENALTY_THRESHOLD);
    int newThreshold = static_cast<int>(m_totalDamage / PlayerConfig::DAMAGE_SPEED_PENALTY_THRESHOLD);
    
    if (newThreshold > oldThreshold) {
        // Am trecut un prag! Aplicăm penalizare de viteză
        m_speed = std::max(0.0f, m_speed - PlayerConfig::DAMAGE_SPEED_PENALTY);
        std::cout << "[DAMAGE] Threshold " << newThreshold << "0 DMG reached! Speed penalty applied. Current speed: " 
                  << static_cast<int>(m_speed * 3.6f) << " km/h" << std::endl;
    }
    
    m_lastDamageThreshold = newThreshold;
    
    // Game over check
    if (m_totalDamage >= PlayerConfig::MAX_DAMAGE) {
        std::cout << "[DAMAGE] CAR DESTROYED! Total damage: " << m_totalDamage << std::endl;
    }
}

float Player::getMaxSpeedWithDamage() const {
    // Reducem viteza maximă bazat pe damage
    int damageLevel = static_cast<int>(m_totalDamage / PlayerConfig::DAMAGE_SPEED_PENALTY_THRESHOLD);
    float speedReduction = damageLevel * PlayerConfig::DAMAGE_SPEED_PENALTY;
    return std::max(10.0f, PlayerConfig::MAX_SPEED - speedReduction);  // Minim 10 m/s (~36 km/h)
}

void Player::update(float deltaTime, const WheelSurfaces& wheelSurfaces, float roadCurve) {
    // ✅ Dacă mașina e distrusă, nu mai răspunde la input
    if (isDestroyed()) {
        // Mașina se oprește încet
        m_speed *= 0.95f;
        if (m_speed < 1.0f) m_speed = 0.0f;
        
        // Continuă să se învârtă
        m_rotation += 180.0f * deltaTime;
        if (m_rotation > 360.0f) m_rotation -= 360.0f;
        
        m_positionZ += m_speed * deltaTime;
        return;
    }
    
    if (m_isSpinning) {
        handleSpinOut(deltaTime);
        return;
    }
    
    applyPhysics(deltaTime, wheelSurfaces, roadCurve);
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

float Player::calculateCornerGrip(float roadCurve) const {
    if (std::abs(roadCurve) < 0.01f) {
        return 1.0f;
    }
    
    float speedRatio = m_speed / PlayerConfig::MAX_SPEED;
    float curveIntensity = std::min(std::abs(roadCurve) * 2.0f, 1.0f);
    float safeSpeed = PlayerConfig::SAFE_CORNER_SPEED;
    
    if (speedRatio <= safeSpeed) {
        return 1.0f;
    }
    
    float overSpeed = (speedRatio - safeSpeed) / (1.0f - safeSpeed);
    float gripLoss = overSpeed * curveIntensity * (1.0f - PlayerConfig::CORNER_SPEED_PENALTY);
    
    return std::max(0.3f, 1.0f - gripLoss);
}

void Player::applyCentrifugalForce(float deltaTime, float roadCurve) {
    if (std::abs(roadCurve) < 0.01f || m_speed < 5.0f) {
        m_isDrifting = false;
        return;
    }
    
    float speedRatio = m_speed / PlayerConfig::MAX_SPEED;
    float centrifugalPush = roadCurve * speedRatio * speedRatio * PlayerConfig::CENTRIFUGAL_FORCE * deltaTime;
    
    if (m_isBraking) {
        centrifugalPush *= (1.0f + PlayerConfig::BRAKE_IN_CORNER_PENALTY);
    }
    
    m_positionX += centrifugalPush;
    
    float cornerGrip = calculateCornerGrip(roadCurve);
    m_isDrifting = (cornerGrip < PlayerConfig::DRIFT_THRESHOLD);
    
    if (m_isDrifting) {
        float targetDriftAngle = roadCurve * 15.0f * speedRatio;
        m_driftAngle += (targetDriftAngle - m_driftAngle) * 3.0f * deltaTime;
    } else {
        m_driftAngle *= 0.9f;
    }
}

void Player::applySpeedWobble(float deltaTime) {
    float speedRatio = m_speed / PlayerConfig::MAX_SPEED;
    
    // ✅ Wobble mai puternic cu damage mare
    float damageWobbleBonus = m_totalDamage / PlayerConfig::MAX_DAMAGE * 0.3f;
    float effectiveThreshold = PlayerConfig::SPEED_WOBBLE_THRESHOLD - damageWobbleBonus;
    
    if (speedRatio >= effectiveThreshold) {
        float wobbleStrength = (speedRatio - effectiveThreshold) / (1.0f - effectiveThreshold);
        
        // Wobble mai intens cu damage
        float damageIntensity = 1.0f + (m_totalDamage / PlayerConfig::MAX_DAMAGE);
        
        m_wobbleTimer += deltaTime * 15.0f;
        
        float wobble = std::sin(m_wobbleTimer) * wobbleStrength * PlayerConfig::SPEED_WOBBLE_INTENSITY * damageIntensity * deltaTime;
        m_positionX += wobble;
        m_wobbleOffset = wobble * 5.0f;
    } else {
        m_wobbleTimer = 0.0f;
        m_wobbleOffset *= 0.9f;
    }
}

void Player::handleInput(float deltaTime, float gripMultiplier, float roadCurve) {
    m_isBraking = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || 
                  sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down);
    
    // ✅ Viteza maximă redusă de damage
    float maxSpeed = getMaxSpeedWithDamage();
    
    // ACCELERATION
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        
        float accelMod = m_isDrifting ? 0.6f : 1.0f;
        m_speed += PlayerConfig::ACCELERATION * deltaTime * gripMultiplier * accelMod;
    }
    // BRAKING
    else if (m_isBraking) {
        float brakeEfficiency = 1.0f;
        
        if (std::abs(roadCurve) > 0.1f) {
            float speedRatio = m_speed / PlayerConfig::MAX_SPEED;
            if (speedRatio > PlayerConfig::SAFE_CORNER_SPEED) {
                brakeEfficiency = PlayerConfig::BRAKE_IN_CORNER_PENALTY;
                if (speedRatio > 0.8f && std::abs(roadCurve) > 0.3f) {
                    m_currentGrip *= 0.95f;
                }
            }
        }
        
        m_speed -= PlayerConfig::BRAKING * deltaTime * brakeEfficiency;
    }
    // Natural deceleration
    else {
        if (m_speed > 0.0f) {
            m_speed -= PlayerConfig::DECELERATION * deltaTime;
            if (m_speed < 0.0f) m_speed = 0.0f;
        }
    }
    
    // ✅ Clamp la viteza maximă bazată pe damage
    m_speed = std::min(m_speed, maxSpeed);

    // STEERING - mai greu cu damage mare
    float damageSteerPenalty = 1.0f - (m_totalDamage / PlayerConfig::MAX_DAMAGE * 0.3f);
    float steerPower = PlayerConfig::STEER_SPEED * gripMultiplier * damageSteerPenalty;
    
    if (m_isDrifting) {
        steerPower *= 0.7f;
    }
    
    float speedRatio = m_speed / PlayerConfig::MAX_SPEED;
    if (speedRatio > 0.7f) {
        steerPower *= (1.0f - (speedRatio - 0.7f) * 0.5f);
    }
    
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
        if (m_rotation > 0) {
            m_rotation = std::max(0.0f, m_rotation - 180.0f * deltaTime);
        } else if (m_rotation < 0) {
            m_rotation = std::min(0.0f, m_rotation + 180.0f * deltaTime);
        }
    }
}

void Player::applyPhysics(float deltaTime, const WheelSurfaces& wheelSurfaces, float roadCurve) {
    float targetGrip = calculateGripFromWheels(wheelSurfaces);
    
    float cornerGrip = calculateCornerGrip(roadCurve);
    targetGrip *= cornerGrip;
    
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
    
    if (m_currentGrip < 0.25f && m_speed > PlayerConfig::MAX_SPEED * 0.6f) {
        m_isSpinning = true;
        m_spinTimer = 0.0f;
        return;
    }
    
    handleInput(deltaTime, m_currentGrip, roadCurve);
    applyCentrifugalForce(deltaTime, roadCurve);
    applySpeedWobble(deltaTime);
    
    // ✅ Viteza maximă limitată de damage
    float maxSpeed = getMaxSpeedWithDamage();
    m_speed = std::clamp(m_speed, -PlayerConfig::MAX_SPEED * 0.5f, maxSpeed);
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
        m_isDrifting = false;
        m_driftAngle = 0.0f;
        
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
    m_isDrifting = false;
    m_driftAngle = 0.0f;
    m_wobbleOffset = 0.0f;
    // ✅ NU resetăm damage-ul!
}

void Player::render(sf::RenderWindow& window, float screenX, float screenY, float scale) {
    const float carWidth = PlayerConfig::CAR_WIDTH * scale;
    const float carHeight = PlayerConfig::CAR_HEIGHT * scale;

    sf::RectangleShape car(sf::Vector2f(carWidth, carHeight));
    car.setOrigin(sf::Vector2f(carWidth * 0.5f, carHeight * 0.5f));
    car.setPosition(sf::Vector2f(screenX + m_wobbleOffset, screenY));
    car.setRotation(sf::degrees(m_rotation + m_driftAngle));
    
    // ✅ Culoare bazată pe damage
    if (isDestroyed()) {
        car.setFillColor(sf::Color(50, 50, 50));  // Gri închis - distrusă
    } else if (m_totalDamage >= 75.0f) {
        car.setFillColor(sf::Color(100, 30, 30));  // Roșu închis - critică
    } else if (m_totalDamage >= 50.0f) {
        car.setFillColor(sf::Color(180, 80, 0));   // Portocaliu - avariată
    } else if (m_totalDamage >= 25.0f) {
        car.setFillColor(sf::Color(200, 150, 0));  // Galben - zgâriată
    } else if (m_isSpinning) {
        car.setFillColor(sf::Color::Yellow);
    } else if (m_isDrifting) {
        car.setFillColor(sf::Color(255, 100, 50));
    } else if (m_currentGrip < PlayerConfig::SPIN_THRESHOLD) {
        car.setFillColor(sf::Color(255, 150, 0));
    } else {
        car.setFillColor(sf::Color::Red);
    }
    
    car.setOutlineColor(sf::Color::White);
    car.setOutlineThickness(2.0f * scale);

    window.draw(car);
}