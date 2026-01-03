#include "Road.h"
#include "CurveProcessor.h"
#include "Core/Constants.h"
#include <cmath>
#include <iostream>
#include <random>

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
    pothole.exists = false;
    pothole.wasHit = false;
}

// ============================================================================
// Road Implementation
// ============================================================================

Road::Road()
    : m_playerZ(0.0f)
    , m_rng(std::random_device{}())
{
}

void Road::generate(int segmentCount) {
    m_segments.clear();
    m_segments.reserve(segmentCount);

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

    generatePotholes();

    std::cout << "Road generated (procedural): " << m_segments.size() << " segments" << std::endl;
}

void Road::init(int segmentCount) {
    m_segments.clear();
    m_segments.reserve(segmentCount);

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

    generatePotholes();

    std::cout << "Road initialized (flat): " << m_segments.size() << " segments" << std::endl;
}

// ✅ Generează gropi aleatoriu pe segmente
void Road::generatePotholes() {
    std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> positionDist(-500.0f, 500.0f);
    std::uniform_real_distribution<float> widthDist(100.0f, 250.0f);
    
    int potholeCount = 0;
    
    // Skip primele 30 de segmente (zona de start)
    for (size_t i = 30; i < m_segments.size(); ++i) {
        if (chanceDist(m_rng) < RoadConfig::POTHOLE_SPAWN_CHANCE) {
            m_segments[i].pothole.exists = true;
            m_segments[i].pothole.offsetX = positionDist(m_rng);
            m_segments[i].pothole.width = widthDist(m_rng);
            m_segments[i].pothole.wasHit = false;
            potholeCount++;
        }
    }
    
    std::cout << "[ROAD] Generated " << potholeCount << " potholes (25% chance)" << std::endl;
}

// ✅ Verifică coliziunea cu groapa
bool Road::checkPotholeCollision(float wheelX, float wheelZ, float& damageOut) {
    RoadSegment* segment = getSegmentAt(wheelZ);
    if (!segment || !segment->pothole.exists || segment->pothole.wasHit) {
        return false;
    }
    
    // Verifică dacă roata e în groapa (pe X)
    float potholeLeft = segment->pothole.offsetX - segment->pothole.width / 2.0f;
    float potholeRight = segment->pothole.offsetX + segment->pothole.width / 2.0f;
    
    if (wheelX >= potholeLeft && wheelX <= potholeRight) {
        segment->pothole.wasHit = true;
        damageOut = RoadConfig::POTHOLE_DAMAGE;
        return true;
    }
    
    return false;
}

void Road::resetPotholes() {
    for (auto& seg : m_segments) {
        seg.pothole.wasHit = false;
    }
}

void Road::addStraight(int startIndex, int count) {
    for (int i = 0; i < count && startIndex + i < static_cast<int>(m_segments.size()); ++i) {
        m_segments[startIndex + i].curve = 0.0f;
        m_segments[startIndex + i].worldY = 0.0f;
    }
}

void Road::addCurve(int startIndex, int count, float curvature) {
    for (int i = 0; i < count && startIndex + i < static_cast<int>(m_segments.size()); ++i) {
        m_segments[startIndex + i].curve = curvature;
    }
}

void Road::addHill(int startIndex, int count, float height) {
    for (int i = 0; i < count && startIndex + i < static_cast<int>(m_segments.size()); ++i) {
        float t = static_cast<float>(i) / count;
        float calculatedY = std::sin(t * 3.14159f) * height;
        m_segments[startIndex + i].worldY = calculatedY;
    }
}

void Road::update(float playerZ) {
    m_playerZ = playerZ;
}

void Road::render(sf::RenderWindow& window, float cameraZ) {
    const float windowWidth = static_cast<float>(window.getSize().x);
    const float windowHeight = static_cast<float>(window.getSize().y);
    
    const int trackSegmentCount = static_cast<int>(m_segments.size());
    const float trackLength = getLength();
    
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

    const float CURVE_AMPLIFICATION = 2.5f;
    
    for (int y = static_cast<int>(windowHeight); y >= static_cast<int>(windowHeight * 0.5f); --y) {
        float screenYNorm = (y - windowHeight * 0.5f) / windowHeight;
        if (screenYNorm <= 0.0001f) continue;

        float z = normalizedCameraZ + RoadConfig::CAMERA_DEPTH * RoadConfig::CAMERA_HEIGHT / screenYNorm;
        float relativeZ = z - (baseIndex * RoadConfig::SEGMENT_LENGTH);
        float segmentOffsetFloat = relativeZ / RoadConfig::SEGMENT_LENGTH;

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

        int segmentOffset = static_cast<int>(segmentOffsetFloat);
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
            
            // ═══════════════════════════════════════════════════════════════
            // ✅ POTHOLE - PATĂ ÎNCHISĂ PE ASFALT (PARTE DIN TILE)
            // ═══════════════════════════════════════════════════════════════
            if (seg.pothole.exists) {
                // Calculăm poziția și lățimea gropii pe ecran
                float potholeScreenX = roadCenterX + (seg.pothole.offsetX / static_cast<float>(RoadConfig::ROAD_WIDTH)) * roadWidth * 2.0f;
                float potholeScreenWidth = (seg.pothole.width / static_cast<float>(RoadConfig::ROAD_WIDTH)) * roadWidth * 2.0f;
                
                // Minim vizibil
                potholeScreenWidth = std::max(potholeScreenWidth, 8.0f);
                
                // Culoarea gropii - mult mai închisă decât asfaltul!
                sf::Color potholeColor = seg.pothole.wasHit ? 
                    sf::Color(60, 55, 50) :     // Mai deschis dacă a fost lovită
                    sf::Color(30, 25, 20);      // Foarte închis - groapă activă
                
                // Desenăm groapa peste asfalt
                sf::RectangleShape potholeLine(sf::Vector2f(potholeScreenWidth, 1.0f));
                potholeLine.setPosition(sf::Vector2f(potholeScreenX - potholeScreenWidth / 2.0f, static_cast<float>(y)));
                potholeLine.setFillColor(potholeColor);
                window.draw(potholeLine);
            }
        }
    }
}

void Road::setSegmentCurve(int index, float curve) {
    if (index >= 0 && index < static_cast<int>(m_segments.size())) {
        m_segments[index].curve = curve;
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

    const float trackLength = getLength();
    float normalizedZ = std::fmod(z, trackLength);
    if (normalizedZ < 0.0f) normalizedZ += trackLength;

    int index = static_cast<int>(normalizedZ / RoadConfig::SEGMENT_LENGTH) % m_segments.size();
    if (index < 0) index += static_cast<int>(m_segments.size());

    return &m_segments[index];
}

const RoadSegment* Road::getSegmentAt(float z) const {
    if (m_segments.empty()) return nullptr;

    const float trackLength = getLength();
    float normalizedZ = std::fmod(z, trackLength);
    if (normalizedZ < 0.0f) normalizedZ += trackLength;

    int index = static_cast<int>(normalizedZ / RoadConfig::SEGMENT_LENGTH) % m_segments.size();
    if (index < 0) index += static_cast<int>(m_segments.size());

    return &m_segments[index];
}