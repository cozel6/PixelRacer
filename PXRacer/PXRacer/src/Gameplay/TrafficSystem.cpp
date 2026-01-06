#include "TrafficSystem.h"
#include <random>
#include <algorithm>
#include <iostream>
#include <cmath>

// Configuration constants for traffic rendering
namespace TrafficConfig {
    constexpr float ROAD_WIDTH = 2000.0f;    
    constexpr float CAMERA_HEIGHT = 1000.0f;
    constexpr float CAMERA_DEPTH = 1.0f / 150.0f;
    constexpr float DRAW_DISTANCE = 4000.0f;
    constexpr float MIN_SCALE_MULTIPLIER = 0.10f;  
    constexpr float SCALE_CURVE_EXPONENT = 2.0f;
    constexpr float REFERENCE_DISTANCE = 300.0f; 
}
// TrafficSystem class implementation
TrafficSystem::TrafficSystem() {}

void TrafficSystem::init(float trackLength) {
    // Initialize random number generator and distribute cars

    m_trackLength = trackLength;
    m_cars.clear();
    
    // Random generators
    std::mt19937 rng(std::random_device{}()); // Seeded RNG
    std::uniform_real_distribution<float> distZ(300.0f, trackLength);  // Start cars ahead of player
    std::vector<float> lanes = { -500.0f, 0.0f, 500.0f };  // Three lanes
    std::uniform_int_distribution<int> laneDist(0, 2); // Lane selection
    std::uniform_real_distribution<float> speedDist(30.0f, 85.0f); // Speed between 30 and 85 units/sec

    // Determine dynamic car count based on track length
    int dynamicCount = static_cast<int>(trackLength / 400.0f); // Roughly one car every 400 units
    dynamicCount = std::clamp(dynamicCount, 10, 40); // Clamp between 10 and 40 cars

    // Create cars
    for (int i = 0; i < dynamicCount; ++i) {
        TrafficCar car;
        car.worldZ = distZ(rng);
        car.worldX = lanes[laneDist(rng)];
        car.speed = speedDist(rng);
        
        car.width = 160.0f;
        car.height = 220.0f;
        
        int c = i % 4;
        if (c == 0) car.color = sf::Color(220, 40, 40);
        else if (c == 1) car.color = sf::Color(40, 80, 220);
        else if (c == 2) car.color = sf::Color(220, 200, 40);
        else car.color = sf::Color(230, 230, 230);

        m_cars.push_back(car);
    }
    std::cout << "[TRAFFIC] System initialized. Cars: " << m_cars.size() << std::endl;
}
// Update car positions
void TrafficSystem::update(float deltaTime, float trackLength, float playerZ, float playerSpeed) {
    m_trackLength = trackLength;
    
    for (auto& car : m_cars) {
        car.worldZ += car.speed * deltaTime;

        if (car.worldZ >= trackLength) car.worldZ -= trackLength;
        if (car.worldZ < 0) car.worldZ += trackLength;
    }
}

// Render traffic cars
void TrafficSystem::render(sf::RenderWindow& window, float cameraZ, float cameraX) {

    // Get window dimensions
    const float windowWidth = static_cast<float>(window.getSize().x);
    const float windowHeight = static_cast<float>(window.getSize().y); 

    // Precompute half dimensions
    const float halfWidth = windowWidth * 0.5f;
    const float halfHeight = windowHeight * 0.5f;

    // Prepare list of cars to render
    std::vector<std::pair<float, TrafficCar*>> renderList;

    // Collect cars within draw distance
    for (auto& car : m_cars) {
        float dz = car.worldZ - cameraZ;

        if (dz < -m_trackLength / 2.0f) dz += m_trackLength;
        else if (dz > m_trackLength / 2.0f) dz -= m_trackLength;

        if (dz > 1.0f && dz < TrafficConfig::DRAW_DISTANCE) {
            renderList.push_back({dz, &car});
        }
    }
    // Sort cars by depth (furthest first)
    std::sort(renderList.begin(), renderList.end(), 
        [](const auto& a, const auto& b) { return a.first > b.first; });

    // Render each car
    for (auto& item : renderList) {
        float dz = item.first;
        TrafficCar* car = item.second;

        float baseScale = TrafficConfig::CAMERA_DEPTH / dz;

        float normalizedDistance = std::clamp(
            TrafficConfig::REFERENCE_DISTANCE / dz,
            0.0f,
            1.0f
        );
        // Apply curve to scaling
        float curvedDistance = std::pow(normalizedDistance, TrafficConfig::SCALE_CURVE_EXPONENT);

        // Final scale multiplier
        float scaleMultiplier = TrafficConfig::MIN_SCALE_MULTIPLIER +
                        (1.0f - TrafficConfig::MIN_SCALE_MULTIPLIER) * curvedDistance;

        // Compute final scale and screen position
        float scale = baseScale * scaleMultiplier;
        float screenY = halfHeight + (TrafficConfig::CAMERA_HEIGHT * scale * windowHeight);
        float screenX = halfWidth + (car->worldX - cameraX) * scale * windowWidth * 0.5f;

        // Draw the car
        float drawW = car->width * scale * windowWidth * 0.5f;
        float drawH = car->height * scale * windowHeight * 0.5f;

        // Cull cars that are too small or off-screen
        if (drawW < 1.0f || screenY > windowHeight) continue;

        // Create rectangle shape for the car
        sf::RectangleShape shape(sf::Vector2f(drawW, drawH));
        shape.setOrigin(sf::Vector2f(drawW / 2.0f, drawH));
        shape.setPosition(sf::Vector2f(screenX, screenY));
        shape.setFillColor(car->color);

        float outlineThick = std::max(1.0f, scale * 100.0f);
        shape.setOutlineColor(sf::Color::Black);
        shape.setOutlineThickness(outlineThick);

        window.draw(shape);

        // Draw shadow
        sf::RectangleShape shadow(sf::Vector2f(drawW * 0.9f, drawH * 0.15f));
        shadow.setOrigin(sf::Vector2f(drawW * 0.45f, 0));
        shadow.setPosition(sf::Vector2f(screenX, screenY));
        shadow.setFillColor(sf::Color(0, 0, 0, 80));
        window.draw(shadow);
    }
}
