#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// Forward declaration
class CurveProcessor;

namespace RoadConfig {
    // ✅ FIX MAJOR: Reducem SEGMENT_LENGTH pentru smooth visual speed!
    // 200.0 → 10.0 = drumul scrollează de 20x mai repede vizual!
    constexpr float SEGMENT_LENGTH = 10.0f;  // ✅ MODIFICAT de la 200.0f!
    
    constexpr int ROAD_WIDTH = 2000;
    constexpr int LANES = 3;
    constexpr float CAMERA_HEIGHT = 1000.0f;
    constexpr float CAMERA_DEPTH = 1.0f / 300.0f;
    constexpr int DRAW_DISTANCE = 300;
}

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
    void setSegmentCurve(int index, float curve); // ✅ ADĂUGAT

private:
    std::vector<RoadSegment> m_segments;
    float m_playerZ;

    void project(RoadSegment& segment, float cameraX, float cameraY, float cameraZ);
};