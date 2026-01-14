#include "TrafficSystem.h"
#include <random>
#include <algorithm>
#include <cmath>
#include <filesystem>

namespace TrafficConfig {
    constexpr float CAMERA_HEIGHT = 1500.0f;
    constexpr float CAMERA_DEPTH = 1.0f / 180.0f;
    constexpr float DRAW_DISTANCE = 4000.0f;
    constexpr float PLAYER_Z_OFFSET = 85.0f;
    constexpr float TRAFFIC_SIZE_MULT = 2.1f;
    constexpr float CLIP_BEHIND_DISTANCE = -150.0f;
}

TrafficSystem::TrafficSystem() {}

void TrafficSystem::init(float trackLength) {
    m_trackLength = trackLength;
    m_cars.clear();
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> distZ(0.0f, trackLength);
    std::uniform_real_distribution<float> speedDist(40.0f, 85.0f);
    float lanes[] = { -550.0f, 0.0f, 550.0f };

    for (int i = 0; i < 25; ++i) {
        TrafficCar car;
        car.worldZ = distZ(rng);
        car.worldX = lanes[i % 3];
        car.speed = speedDist(rng);
        car.width = 180.0f; car.height = 240.0f;
        m_cars.push_back(car);
    }

    namespace fs = std::filesystem;
    for (int i = 1; i <= 4; ++i) {
        sf::Texture tex;
        if (tex.loadFromFile("assets/textures/traffic_car_" + std::to_string(i) + ".png"))
            m_trafficTextures.push_back(std::move(tex));
    }
    m_trafficTexturesLoaded = !m_trafficTextures.empty();
    if (m_trafficTexturesLoaded) {
        std::uniform_int_distribution<int> tDist(0, (int)m_trafficTextures.size() - 1);
        for (auto& c : m_cars) c.texIndex = tDist(rng);
    }
}

void TrafficSystem::update(float deltaTime, float trackLength, float playerZ, float playerSpeed) {
    m_trackLength = trackLength;
    for (auto& car : m_cars) {
        car.worldZ += car.speed * deltaTime;
        if (car.worldZ >= trackLength) car.worldZ -= trackLength;
        if (car.worldZ < 0) car.worldZ += trackLength;
    }
}

void TrafficSystem::render(sf::RenderWindow& window, float cameraZ, float cameraX) {
    sf::Vector2u winSize = window.getSize();
    float halfW = static_cast<float>(winSize.x) / 2.0f;
    float halfH = static_cast<float>(winSize.y) / 2.0f;

    std::vector<std::pair<float, TrafficCar*>> renderList;
    for (auto& car : m_cars) {
        float dz = car.worldZ - cameraZ;
        if (dz < -m_trackLength / 2.0f) dz += m_trackLength;
        else if (dz > m_trackLength / 2.0f) dz -= m_trackLength;

        if (dz > TrafficConfig::CLIP_BEHIND_DISTANCE && dz < TrafficConfig::DRAW_DISTANCE)
            renderList.push_back({ dz, &car });
    }

    std::sort(renderList.begin(), renderList.end(), [](const auto& a, const auto& b) {
        return a.first > b.first;
        });

    for (auto& item : renderList) {
        float dz = item.first;
        TrafficCar* car = item.second;
        float projectedZ = dz + TrafficConfig::PLAYER_Z_OFFSET;
        if (projectedZ <= 1.0f) projectedZ = 1.0f;

        float scale = TrafficConfig::CAMERA_DEPTH * (static_cast<float>(winSize.y) / projectedZ);
        float finalScale = scale * TrafficConfig::TRAFFIC_SIZE_MULT;

        float screenY = halfH + (TrafficConfig::CAMERA_HEIGHT * scale);
        float screenX = halfW + (car->worldX - cameraX) * scale * 0.85f;

        // Lățime corectată pentru trafic
        float drawW = car->width * finalScale * 1.4f;
        float drawH = car->height * finalScale;

        if (m_trafficTexturesLoaded) {
            sf::Sprite spr(m_trafficTextures[car->texIndex]);
            sf::Vector2u ts = m_trafficTextures[car->texIndex].getSize();
            spr.setOrigin(sf::Vector2f(static_cast<float>(ts.x) / 2.0f, static_cast<float>(ts.y)));
            spr.setScale(sf::Vector2f(drawW / ts.x, drawH / ts.y));
            spr.setPosition(sf::Vector2f(screenX, screenY));
            window.draw(spr);
        }
    }
}