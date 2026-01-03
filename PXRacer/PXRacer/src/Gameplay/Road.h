#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

class CurveProcessor;

namespace RoadConfig {
    constexpr float SEGMENT_LENGTH = 10.0f;
    constexpr int ROAD_WIDTH = 2000;
    constexpr int LANES = 3;
    constexpr float CAMERA_HEIGHT = 1000.0f;
    constexpr float CAMERA_DEPTH = 1.0f / 300.0f;
    constexpr int DRAW_DISTANCE = 300;
    
    // ✅ POTHOLE SETTINGS - Frecvență redusă
    constexpr float POTHOLE_SPAWN_CHANCE = 0.06f;   // 6% (un sfert din 25%)
    constexpr float POTHOLE_DAMAGE = 5.0f;
}

struct Pothole {
    float offsetX = 0.0f;
    float width = 150.0f;
    bool exists = false;
    bool wasHit = false;
};

class RoadSegment {
public:
    RoadSegment();

    float worldZ;
    float curve;
    float worldY;

    sf::Color roadColor;
    sf::Color grassColor;
    sf::Color rumbleColor;
    sf::Color laneColor;

    float screenX;
    float screenY;
    float screenW;
    float scale;
    
    Pothole pothole;
};

class Road {
public:
    Road();

    void generate(int segmentCount);
    void init(int segmentCount);
    void update(float playerZ);
    void render(sf::RenderWindow& window, float cameraZ);

    float getCurveAt(float z) const;
    RoadSegment* getSegmentAt(float z);
    const RoadSegment* getSegmentAt(float z) const;

    int getSegmentCount() const { return static_cast<int>(m_segments.size()); }
    float getLength() const { return m_segments.size() * RoadConfig::SEGMENT_LENGTH; }

    void addStraight(int startIndex, int count);
    void addCurve(int startIndex, int count, float curvature);
    void addHill(int startIndex, int count, float height);
    void setSegmentCurve(int index, float curve);
    
    void generatePotholes();
    bool checkPotholeCollision(float wheelX, float wheelZ, float& damageOut);
    void resetPotholes();

private:
    std::vector<RoadSegment> m_segments;
    float m_playerZ;
    std::mt19937 m_rng;

    void project(RoadSegment& segment, float cameraX, float cameraY, float cameraZ);
};