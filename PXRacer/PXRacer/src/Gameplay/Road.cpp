#include "Road.h"
#include "CurveProcessor.h"
#include "Core/Constants.h"
#include <cmath>
#include <iostream>

// ============================================================================
// RoadSegment Implementation
// ============================================================================

RoadSegment::RoadSegment()
    : worldZ(0.0f)
    , curve(0.0f)
    , worldY(0.0f)
    , roadColor(sf::Color(100, 100, 100))
    , grassColor(sf::Color(16, 200, 16))
    , rumbleColor(sf::Color::White)
    , laneColor(sf::Color::White)
    , screenX(0.0f)
    , screenY(0.0f)
    , screenW(0.0f)
    , scale(0.0f)
{
}

// ============================================================================
// Road Implementation
// ============================================================================

Road::Road()
    : m_playerZ(0.0f)
{
}

void Road::generate(int segmentCount) {
    m_segments.clear();
    m_segments.reserve(segmentCount);

    // ✅ FIX: Ensure seamless color loop by making total segments divisible by pattern length
    // Pattern repeats every 3 segments for rumble, every 6 for road/grass
    const int COLOR_PATTERN_LENGTH = 6;
    int adjustedCount = ((segmentCount + COLOR_PATTERN_LENGTH - 1) / COLOR_PATTERN_LENGTH) * COLOR_PATTERN_LENGTH;

    for (int i = 0; i < adjustedCount; ++i) {
        RoadSegment segment;
        segment.worldZ = i * RoadConfig::SEGMENT_LENGTH;
        segment.curve = 0.0f;
        segment.worldY = 0.0f;

        bool isDark = ((i / 3) % 2) == 0;
        segment.roadColor = isDark ? sf::Color(107, 107, 107) : sf::Color(105, 105, 105);
        segment.grassColor = isDark ? sf::Color(16, 200, 16) : sf::Color(0, 154, 0);
        segment.rumbleColor = (i % 3 == 0) ? sf::Color::Red : sf::Color::White;
        segment.laneColor = sf::Color::White;

        m_segments.push_back(segment);
    }

    int pos = 0;
    addStraight(pos, adjustedCount / 6);     pos += adjustedCount / 6;
    addCurve(pos, adjustedCount / 8, 8.0f);  pos += adjustedCount / 8;
    addStraight(pos, adjustedCount / 10);    pos += adjustedCount / 10;
    addCurve(pos, adjustedCount / 8, -8.0f); pos += adjustedCount / 8;
    addHill(pos, adjustedCount / 6, 600.0f); pos += adjustedCount / 6;
    addStraight(pos, adjustedCount / 6);

    std::cout << "Road generated (procedural): " << m_segments.size() 
              << " segments (adjusted from " << segmentCount << " for seamless loop)" << std::endl;
}

void Road::init(int segmentCount) {
    m_segments.clear();
    m_segments.reserve(segmentCount);

    // ✅ FIX: Ensure seamless color loop by making total segments divisible by pattern length
    // Pattern repeats every 3 segments for rumble, every 6 for road/grass
    const int COLOR_PATTERN_LENGTH = 6;
    int adjustedCount = ((segmentCount + COLOR_PATTERN_LENGTH - 1) / COLOR_PATTERN_LENGTH) * COLOR_PATTERN_LENGTH;

    for (int i = 0; i < adjustedCount; ++i) {
        RoadSegment segment;
        segment.worldZ = i * RoadConfig::SEGMENT_LENGTH;
        segment.curve = 0.0f;
        segment.worldY = 0.0f;

        bool isDark = ((i / 3) % 2) == 0;
        segment.roadColor = isDark ? sf::Color(107, 107, 107) : sf::Color(105, 105, 105);
        segment.grassColor = isDark ? sf::Color(16, 200, 16) : sf::Color(0, 154, 0);
        segment.rumbleColor = (i % 3 == 0) ? sf::Color::Red : sf::Color::White;
        segment.laneColor = sf::Color::White;

        m_segments.push_back(segment);
    }

    std::cout << "Road initialized (flat): " << m_segments.size() 
              << " segments (adjusted from " << segmentCount << " for seamless loop)" << std::endl;
}

void Road::addStraight(int startIndex, int count) {
    for (int i = 0; i < count && startIndex + i < m_segments.size(); ++i) {
        m_segments[startIndex + i].curve = 0.0f;
        m_segments[startIndex + i].worldY = 0.0f;
    }
}

void Road::addCurve(int startIndex, int count, float curvature) {
    for (int i = 0; i < count && startIndex + i < m_segments.size(); ++i) {
        m_segments[startIndex + i].curve = curvature;
    }
}

void Road::addHill(int startIndex, int count, float height) {
    std::cout << "   ⛰️  ADDING HILL: startIndex=" << startIndex << ", count=" << count << ", height=" << height << std::endl;
    
    for (int i = 0; i < count && startIndex + i < m_segments.size(); ++i) {
        float t = static_cast<float>(i) / count;
        float calculatedY = std::sin(t * 3.14159f) * height;
        m_segments[startIndex + i].worldY = calculatedY;
        
        if (i < 5) {
            std::cout << "      seg[" << (startIndex + i) << "].worldY = " << calculatedY << std::endl;
        }
    }
}

void Road::update(float playerZ) {
    m_playerZ = playerZ;
}

void Road::render(sf::RenderWindow& window, float cameraZ) {
    const float windowWidth = static_cast<float>(window.getSize().x);
    const float windowHeight = static_cast<float>(window.getSize().y);
    
    const float cameraY = RoadConfig::CAMERA_HEIGHT;
    
    // ✅ FIX: Handle wrap-around properly for seamless loop
    const int trackSegmentCount = static_cast<int>(m_segments.size());
    const float trackLength = getLength();
    
    // Normalize camera Z to track bounds
    float normalizedCameraZ = std::fmod(cameraZ, trackLength);
    if (normalizedCameraZ < 0.0f) normalizedCameraZ += trackLength;
    
    int baseIndex = static_cast<int>(normalizedCameraZ / RoadConfig::SEGMENT_LENGTH);
    
    // Background
    sf::RectangleShape sky(sf::Vector2f(windowWidth, windowHeight * 0.5f));
    sky.setFillColor(sf::Color(135, 206, 250));
    window.draw(sky);

    sf::RectangleShape ground(sf::Vector2f(windowWidth, windowHeight * 0.5f));
    ground.setPosition(sf::Vector2f(0.0f, windowHeight * 0.5f));
    ground.setFillColor(sf::Color(16, 200, 16));
    window.draw(ground);

    // ✅ CurveProcessor pentru curbe smooth
    std::vector<float> segmentCurves(m_segments.size());
    for (size_t i = 0; i < m_segments.size(); ++i) {
        segmentCurves[i] = m_segments[i].curve;
    }
    
    float cameraSegmentPos = (normalizedCameraZ - (baseIndex * RoadConfig::SEGMENT_LENGTH)) / RoadConfig::SEGMENT_LENGTH;
    
    auto curveData = CurveProcessor::processSegmentCurves(
        segmentCurves,
        baseIndex,
        RoadConfig::DRAW_DISTANCE,
        cameraSegmentPos
    );

    // Orizont static
    sf::RectangleShape horizon(sf::Vector2f(windowWidth, 2.0f));
    horizon.setPosition(sf::Vector2f(0.0f, windowHeight * 0.5f));
    horizon.setFillColor(sf::Color(100, 150, 200, 128));
    window.draw(horizon);

    const float CURVE_AMPLIFICATION = 2.5f;
    
    for (int y = static_cast<int>(windowHeight); y >= static_cast<int>(windowHeight * 0.5f); --y) {
        float screenYNorm = (y - windowHeight * 0.5f) / windowHeight;
        if (screenYNorm <= 0.0001f) continue;

        float z = normalizedCameraZ + RoadConfig::CAMERA_DEPTH * RoadConfig::CAMERA_HEIGHT / screenYNorm;
        float relativeZ = z - (baseIndex * RoadConfig::SEGMENT_LENGTH);
        float segmentOffsetFloat = relativeZ / RoadConfig::SEGMENT_LENGTH;

        // Interpolate curve cu Catmull-Rom
        float highResIndex = segmentOffsetFloat * 4.0f;
        int idx0 = std::max(0, std::min(static_cast<int>(highResIndex), curveData.baseSamples - 1));
        int idx_1 = std::max(0, idx0 - 1);
        int idx1 = std::min(idx0 + 1, curveData.baseSamples);
        int idx2 = std::min(idx0 + 2, curveData.baseSamples);

        float t = highResIndex - idx0;
        
        float accumulatedCurve = CurveProcessor::catmullRomInterpolate(
            curveData.accumulatedCurves[idx_1],
            curveData.accumulatedCurves[idx0],
            curveData.accumulatedCurves[idx1],
            curveData.accumulatedCurves[idx2],
            t
        );

        float relativeCurve = accumulatedCurve - curveData.cameraOffset;

        // ✅ FIX: Get segment with proper wrap-around - NO CLAMPING!
        int segmentOffset = static_cast<int>(segmentOffsetFloat);
        
        // ✅ Direct modulo wrap-around, no clamping
        int segmentIndex = (baseIndex + segmentOffset) % trackSegmentCount;
        if (segmentIndex < 0) segmentIndex += trackSegmentCount;

        RoadSegment& seg = m_segments[segmentIndex];
        float scale = RoadConfig::CAMERA_DEPTH / (z - normalizedCameraZ);
        
        float roadCenterX = windowWidth * 0.5f + (relativeCurve * CURVE_AMPLIFICATION * scale * windowWidth * 0.5f);
        float roadWidth = RoadConfig::ROAD_WIDTH * scale * windowWidth * 0.5f;

        // Grass
        sf::RectangleShape grassLine(sf::Vector2f(windowWidth, 1.0f));
        grassLine.setPosition(sf::Vector2f(0.0f, static_cast<float>(y)));
        grassLine.setFillColor(seg.grassColor);
        window.draw(grassLine);

        // Road
        if (roadWidth > 0.5f) {
            sf::RectangleShape roadLine(sf::Vector2f(roadWidth * 2.0f, 1.0f));
            roadLine.setPosition(sf::Vector2f(roadCenterX - roadWidth, static_cast<float>(y)));
            roadLine.setFillColor(seg.roadColor);
            window.draw(roadLine);

            float rumbleWidth = roadWidth * 0.15f;

            sf::RectangleShape leftRumble(sf::Vector2f(rumbleWidth, 1.0f));
            leftRumble.setPosition(sf::Vector2f(roadCenterX - roadWidth, static_cast<float>(y)));
            leftRumble.setFillColor(seg.rumbleColor);
            window.draw(leftRumble);

            sf::RectangleShape rightRumble(sf::Vector2f(rumbleWidth, 1.0f));
            rightRumble.setPosition(sf::Vector2f(roadCenterX + roadWidth - rumbleWidth, static_cast<float>(y)));
            rightRumble.setFillColor(seg.rumbleColor);
            window.draw(rightRumble);
        }
    }
}

void Road::setSegmentCurve(int index, float curve) {
    if (index >= 0 && index < m_segments.size()) {
        m_segments[index].curve = curve;
        
        static int debugCount = 0;
        if (std::abs(curve) > 0.1f && debugCount < 10) {
            std::cout << "      🔍 seg[" << index << "].curve = " << curve << std::endl;
            debugCount++;
        }
    }
}

void Road::project(RoadSegment& segment, float cameraX, float cameraY, float cameraZ) {
    const float windowWidth = static_cast<float>(Config::WINDOW_WIDTH);
    const float windowHeight = static_cast<float>(Config::WINDOW_HEIGHT);

    float relativeZ = segment.worldZ - cameraZ;

    if (relativeZ <= 0) {
        segment.scale = -1.0f;
        return;
    }

    segment.scale = RoadConfig::CAMERA_DEPTH / relativeZ;
    segment.screenX = windowWidth * 0.5f + (segment.curve - cameraX) * segment.scale * windowWidth;
    segment.screenY = windowHeight * 0.5f + (cameraY - segment.worldY) * segment.scale * windowHeight;
    segment.screenW = RoadConfig::ROAD_WIDTH * segment.scale * windowWidth * 0.5f;
}

float Road::getCurveAt(float z) const {
    const RoadSegment* segment = getSegmentAt(z);
    return segment ? segment->curve : 0.0f;
}

RoadSegment* Road::getSegmentAt(float z) {
    if (m_segments.empty()) return nullptr;

    // ✅ FIX: Proper wrap-around with normalized Z
    const float trackLength = getLength();
    float normalizedZ = std::fmod(z, trackLength);
    if (normalizedZ < 0.0f) normalizedZ += trackLength;

    int index = static_cast<int>(normalizedZ / RoadConfig::SEGMENT_LENGTH) % m_segments.size();
    if (index < 0) index += static_cast<int>(m_segments.size());

    return &m_segments[index];
}

const RoadSegment* Road::getSegmentAt(float z) const {
    if (m_segments.empty()) return nullptr;

    // ✅ FIX: Proper wrap-around with normalized Z
    const float trackLength = getLength();
    float normalizedZ = std::fmod(z, trackLength);
    if (normalizedZ < 0.0f) normalizedZ += trackLength;

    int index = static_cast<int>(normalizedZ / RoadConfig::SEGMENT_LENGTH) % m_segments.size();
    if (index < 0) index += static_cast<int>(m_segments.size());

    return &m_segments[index];
}