#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

struct TrafficCar {
    float worldZ; float worldX; float speed;
    float width = 160.0f; float height = 220.0f;
    sf::Color color; int texIndex = 0;
};

class TrafficSystem {
public:
    TrafficSystem();
    void init(float trackLength);
    void update(float deltaTime, float trackLength, float playerZ, float playerSpeed);
    void render(sf::RenderWindow& window, float cameraZ, float cameraX);

private:
    std::vector<TrafficCar> m_cars;
    float m_trackLength = 0.0f;
    std::vector<sf::Texture> m_trafficTextures;
    bool m_trafficTexturesLoaded = false;
};