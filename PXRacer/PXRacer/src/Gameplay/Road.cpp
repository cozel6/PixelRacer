#include "Road.h"
#include "Core/SettingsManager.h"
#include "CurveProcessor.h"
#include "Core/Constants.h"
#include <cmath>
#include <iostream>
#include <random>
#include <algorithm>

RoadSegment::RoadSegment()
    : worldZ(0.0f), curve(0.0f), worldY(0.0f), roadColor(sf::Color(100, 100, 100)), grassColor(sf::Color(16, 200, 16)), rumbleColor(sf::Color::White), laneColor(sf::Color::White), screenX(0.0f), screenY(0.0f), screenW(0.0f), scale(0.0f)
{
    pothole.exists = false;
    pothole.wasHit = false;
    repairPickup.exists = false;
    repairPickup.collected = false;
}

Road::Road()
    : m_playerZ(0.0f), m_rng(std::random_device{}()), m_potholeChance(RoadConfig::POTHOLE_SPAWN_CHANCE), m_repairChance(RoadConfig::REPAIR_SPAWN_CHANCE), m_potholeCount(0)
{
}

void Road::generate(int segmentCount)
{
    generateWithDifficulty(segmentCount, EndlessDifficultyLevel::Medium);
}

void Road::generateWithDifficulty(int segmentCount, EndlessDifficultyLevel difficulty)
{
    m_segments.clear();
    m_segments.reserve(segmentCount);

    auto settings = EndlessDifficultySettings::getSettings(difficulty);
    m_potholeChance = settings.potholeChance;
    m_repairChance = settings.repairPickupChance;

    const int COLOR_PATTERN_LENGTH = 6;
    int adjustedCount = ((segmentCount + COLOR_PATTERN_LENGTH - 1) / COLOR_PATTERN_LENGTH) * COLOR_PATTERN_LENGTH;

    for (int i = 0; i < adjustedCount; ++i)
    {
        RoadSegment segment;
        segment.worldZ = i * RoadConfig::SEGMENT_LENGTH;
        segment.curve = 0.0f;
        segment.worldY = 0.0f;

        bool isDark = ((i / 3) % 2) == 0;
        segment.roadColor = isDark ? sf::Color(107, 107, 107) : sf::Color(105, 105, 105);
        segment.grassColor = isDark ? sf::Color(16, 200, 16) : sf::Color(0, 154, 0);
        segment.rumbleColor = (i % 3 == 0) ? sf::Color::Red : sf::Color::White;
        segment.laneColor = sf::Color::White;

        segment.repairPickup.animTimer = static_cast<float>(i) * 0.1f;

        m_segments.push_back(segment);
    }

    int pos = 0;
    addStraight(pos, adjustedCount / 6);
    pos += adjustedCount / 6;
    addCurve(pos, adjustedCount / 8, 8.0f);
    pos += adjustedCount / 8;
    addStraight(pos, adjustedCount / 10);
    pos += adjustedCount / 10;
    addCurve(pos, adjustedCount / 8, -8.0f);
    pos += adjustedCount / 8;
    addHill(pos, adjustedCount / 6, 600.0f);
    pos += adjustedCount / 6;
    addStraight(pos, adjustedCount / 6);

    generatePotholesWithChance(m_potholeChance);

    // Initial pickups - use Low Damage (player starts with 0 Dmg)
    generateRepairPickupsFixed(RoadConfig::PICKUPS_LOW_DAMAGE);

    std::cout << "[ROAD] Generated with " << settings.name << " difficulty" << std::endl;
    std::cout << "[ROAD] Potholes: " << m_potholeCount << std::endl;
}

void Road::init(int segmentCount)
{
    m_segments.clear();
    m_segments.reserve(segmentCount);

    const int COLOR_PATTERN_LENGTH = 6;
    int adjustedCount = ((segmentCount + COLOR_PATTERN_LENGTH - 1) / COLOR_PATTERN_LENGTH) * COLOR_PATTERN_LENGTH;

    for (int i = 0; i < adjustedCount; ++i)
    {
        RoadSegment segment;
        segment.worldZ = i * RoadConfig::SEGMENT_LENGTH;
        segment.curve = 0.0f;
        segment.worldY = 0.0f;

        bool isDark = ((i / 3) % 2) == 0;
        segment.roadColor = isDark ? sf::Color(107, 107, 107) : sf::Color(105, 105, 105);
        segment.grassColor = isDark ? sf::Color(16, 200, 16) : sf::Color(0, 154, 0);
        segment.rumbleColor = (i % 3 == 0) ? sf::Color::Red : sf::Color::White;
        segment.laneColor = sf::Color::White;

        segment.repairPickup.animTimer = static_cast<float>(i) * 0.1f;

        m_segments.push_back(segment);
    }

    generatePotholes();
    generateRepairPickupsFixed(RoadConfig::PICKUPS_LOW_DAMAGE);

    std::cout << "Road initialized (flat): " << m_segments.size() << " segments" << std::endl;
}

// NEW: Regenerate pickups based on current damage
void Road::regeneratePickupsForDamage(float currentDamage)
{
    // Clear all existing pickups
    for (auto &seg : m_segments)
    {
        seg.repairPickup.exists = false;
        seg.repairPickup.collected = false;
    }

    // Decide how many pickups to spawn
    int pickupCount;
    if (currentDamage < 30.0f)
    {
        pickupCount = RoadConfig::PICKUPS_LOW_DAMAGE; // 5 pickups
    }
    else if (currentDamage < 50.0f)
    {
        pickupCount = RoadConfig::PICKUPS_MED_DAMAGE; // 3 pickups
    }
    else
    {
        pickupCount = RoadConfig::PICKUPS_HIGH_DAMAGE; // 1 pickup
    }

    generateRepairPickupsFixed(pickupCount);

    std::cout << "[ROAD] Regenerated " << pickupCount << " pickups for "
              << currentDamage << " Dmg" << std::endl;
}

// Modify generateRepairPickupsFixed for uniform random placement:
void Road::generateRepairPickupsFixed(int count)
{
    std::uniform_real_distribution<float> positionDist(-350.0f, 350.0f);

    // Distribute pickups uniformly along the track
    int segmentCount = static_cast<int>(m_segments.size());
    int spacing = segmentCount / (count + 1); // uniform spacing

    // Add variation so placement is not predictable
    std::uniform_int_distribution<int> offsetDist(-spacing / 3, spacing / 3);

    int pickupCount = 0;

    for (int i = 0; i < count; ++i)
    {
        int basePos = spacing * (i + 1);
        int offset = offsetDist(m_rng);
        int segIdx = std::clamp(basePos + offset, 50, segmentCount - 1);

        // Find a valid segment (no pothole and no pickup)
        int attempts = 0;
        while (attempts < 20 &&
               (m_segments[segIdx].pothole.exists || m_segments[segIdx].repairPickup.exists))
        {
            segIdx = (segIdx + 1) % segmentCount;
            if (segIdx < 50)
                segIdx = 50;
            attempts++;
        }

        if (!m_segments[segIdx].pothole.exists && !m_segments[segIdx].repairPickup.exists)
        {
            m_segments[segIdx].repairPickup.exists = true;
            m_segments[segIdx].repairPickup.offsetX = positionDist(m_rng);
            m_segments[segIdx].repairPickup.width = 80.0f;
            m_segments[segIdx].repairPickup.healAmount = RoadConfig::REPAIR_HEAL_AMOUNT;
            m_segments[segIdx].repairPickup.collected = false;
            pickupCount++;
        }
    }

    std::cout << "[ROAD] Generated " << pickupCount << " repair pickups" << std::endl;
}

void Road::generatePotholes()
{
    generatePotholesWithChance(RoadConfig::POTHOLE_SPAWN_CHANCE);
}

// Replaces generatePotholesWithChance:
void Road::generatePotholesWithChance(float chance)
{
    std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> positionDist(-400.0f, 400.0f);
    std::uniform_real_distribution<float> widthDist(RoadConfig::POTHOLE_MIN_WIDTH, RoadConfig::POTHOLE_MAX_WIDTH);

    m_potholeCount = 0;

    int lastPotholeSegment = -10;

    for (size_t i = 50; i < m_segments.size(); ++i)
    {
        if (static_cast<int>(i) - lastPotholeSegment < 5)
        {
            continue;
        }

        if (chanceDist(m_rng) < chance)
        {
            m_segments[i].pothole.exists = true;
            m_segments[i].pothole.offsetX = positionDist(m_rng);
            m_segments[i].pothole.width = widthDist(m_rng); // Fix: was using wrong variable
            m_segments[i].pothole.wasHit = false;
            m_potholeCount++;
            lastPotholeSegment = static_cast<int>(i);
        }
    }

    std::cout << "[ROAD] Generated " << m_potholeCount << " potholes" << std::endl;
}

bool Road::checkPotholeCollision(float wheelX, float wheelZ, float &damageOut)
{
    RoadSegment *segment = getSegmentAt(wheelZ);
    if (!segment || !segment->pothole.exists || segment->pothole.wasHit)
    {
        return false;
    }

    float potholeLeft = segment->pothole.offsetX - segment->pothole.width / 2.0f;
    float potholeRight = segment->pothole.offsetX + segment->pothole.width / 2.0f;

    if (wheelX >= potholeLeft && wheelX <= potholeRight)
    {
        segment->pothole.wasHit = true;
        damageOut = RoadConfig::POTHOLE_DAMAGE;
        return true;
    }

    return false;
}

bool Road::checkRepairPickupCollision(float playerX, float playerZ, float &healOut)
{
    RoadSegment *segment = getSegmentAt(playerZ);
    if (!segment || !segment->repairPickup.exists || segment->repairPickup.collected)
    {
        return false;
    }

    float pickupLeft = segment->repairPickup.offsetX - segment->repairPickup.width / 2.0f;
    float pickupRight = segment->repairPickup.offsetX + segment->repairPickup.width / 2.0f;

    if (playerX >= pickupLeft - 60.0f && playerX <= pickupRight + 60.0f)
    {
        segment->repairPickup.collected = true;
        healOut = segment->repairPickup.healAmount;
        return true;
    }

    return false;
}

void Road::resetPotholes()
{
    for (auto &seg : m_segments)
    {
        seg.pothole.wasHit = false;
    }
}

void Road::resetPickups()
{
    for (auto &seg : m_segments)
    {
        seg.repairPickup.collected = false;
    }
}

void Road::addStraight(int startIndex, int count)
{
    for (int i = 0; i < count && startIndex + i < static_cast<int>(m_segments.size()); ++i)
    {
        m_segments[startIndex + i].curve = 0.0f;
        m_segments[startIndex + i].worldY = 0.0f;
    }
}

void Road::addCurve(int startIndex, int count, float curvature)
{
    for (int i = 0; i < count && startIndex + i < static_cast<int>(m_segments.size()); ++i)
    {
        m_segments[startIndex + i].curve = curvature;
    }
}

void Road::addHill(int startIndex, int count, float height)
{
    for (int i = 0; i < count && startIndex + i < static_cast<int>(m_segments.size()); ++i)
    {
        float t = static_cast<float>(i) / count;
        float calculatedY = std::sin(t * 3.14159f) * height;
        m_segments[startIndex + i].worldY = calculatedY;
    }
}

void Road::update(float playerZ, float deltaTime)
{
    m_playerZ = playerZ;

    for (auto &seg : m_segments)
    {
        if (seg.repairPickup.exists && !seg.repairPickup.collected)
        {
            seg.repairPickup.animTimer += deltaTime;
            seg.repairPickup.bobOffset = std::sin(seg.repairPickup.animTimer * RoadConfig::PICKUP_BOB_SPEED) * RoadConfig::PICKUP_BOB_AMOUNT;
        }
    }
}

void Road::render(sf::RenderWindow &window, float cameraZ)
{
    const float windowWidth = static_cast<float>(window.getSize().x);
    const float windowHeight = static_cast<float>(window.getSize().y);

    const int trackSegmentCount = static_cast<int>(m_segments.size());
    const float trackLength = getLength();

    float normalizedCameraZ = std::fmod(cameraZ, trackLength);
    if (normalizedCameraZ < 0.0f)
        normalizedCameraZ += trackLength;

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
    for (size_t i = 0; i < m_segments.size(); ++i)
    {
        segmentCurves[i] = m_segments[i].curve;
    }

    float cameraSegmentPos = (normalizedCameraZ - (baseIndex * RoadConfig::SEGMENT_LENGTH)) / RoadConfig::SEGMENT_LENGTH;

    auto curveData = CurveProcessor::processSegmentCurves(
        segmentCurves,
        baseIndex,
        RoadConfig::DRAW_DISTANCE,
        cameraSegmentPos);

    const float CURVE_AMPLIFICATION = 2.5f;

    // Collect pickups for rendering
    struct PickupRenderData
    {
        float screenX, screenY;
        float scale;
        float pulse;
        int segmentIndex;
    };
    std::vector<PickupRenderData> pickupsToRender;

    for (int y = static_cast<int>(windowHeight); y >= static_cast<int>(windowHeight * 0.5f); --y)
    {
        float screenYNorm = (y - windowHeight * 0.5f) / windowHeight;
        if (screenYNorm <= 0.0001f)
            continue;

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
            t);

        float relativeCurve = accumulatedCurve - curveData.cameraOffset;

        int segmentOffset = static_cast<int>(segmentOffsetFloat);
        int segmentIndex = (baseIndex + segmentOffset) % trackSegmentCount;
        if (segmentIndex < 0)
            segmentIndex += trackSegmentCount;

        RoadSegment &seg = m_segments[segmentIndex];
        float scale = RoadConfig::CAMERA_DEPTH / (z - normalizedCameraZ);

        float roadCenterX = windowWidth * 0.5f + (relativeCurve * CURVE_AMPLIFICATION * scale * windowWidth * 0.5f);
        float roadWidth = RoadConfig::ROAD_WIDTH * scale * windowWidth * 0.5f;

        // Grass
        sf::RectangleShape grassLine(sf::Vector2f(windowWidth, 1.0f));
        grassLine.setPosition(sf::Vector2f(0.0f, static_cast<float>(y)));
        grassLine.setFillColor(seg.grassColor);
        window.draw(grassLine);

        // Road
        if (roadWidth > 0.5f)
        {
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

            // Start/Finish line - checkered pattern (single segment)
            if (segmentIndex == 0)
            {
                float checkerWidth = roadWidth * 0.15f;
                int numCheckers = static_cast<int>((roadWidth * 2.0f) / checkerWidth) + 1;

                for (int c = 0; c < numCheckers; ++c)
                {
                    float checkerX = roadCenterX - roadWidth + c * checkerWidth;
                    bool isWhite = (c % 2 == 0);
                    sf::Color checkerColor = isWhite ? sf::Color::White : sf::Color(15, 15, 15);

                    sf::RectangleShape checker(sf::Vector2f(checkerWidth + 1.0f, 1.0f));
                    checker.setPosition(sf::Vector2f(checkerX, static_cast<float>(y)));
                    checker.setFillColor(checkerColor);
                    window.draw(checker);
                }
            }

            // Pothole
            if (seg.pothole.exists)
            {
                float potholeScreenX = roadCenterX + (seg.pothole.offsetX / static_cast<float>(RoadConfig::ROAD_WIDTH)) * roadWidth * 2.0f;
                float potholeScreenWidth = (seg.pothole.width / static_cast<float>(RoadConfig::ROAD_WIDTH)) * roadWidth * 2.0f;

                potholeScreenWidth = std::max(potholeScreenWidth, 8.0f);

                sf::Color potholeColor = seg.pothole.wasHit ? sf::Color(60, 55, 50) : sf::Color(30, 25, 20);

                sf::RectangleShape potholeLine(sf::Vector2f(potholeScreenWidth, 1.0f));
                potholeLine.setPosition(sf::Vector2f(potholeScreenX - potholeScreenWidth / 2.0f, static_cast<float>(y)));
                potholeLine.setFillColor(potholeColor);
                window.draw(potholeLine);
            }

            // Collect pickup for rendering
            if (seg.repairPickup.exists && !seg.repairPickup.collected)
            {
                bool alreadyAdded = false;
                for (const auto &p : pickupsToRender)
                {
                    if (p.segmentIndex == segmentIndex)
                    {
                        alreadyAdded = true;
                        break;
                    }
                }

                if (!alreadyAdded)
                {
                    float pickupScreenX = roadCenterX + (seg.repairPickup.offsetX / static_cast<float>(RoadConfig::ROAD_WIDTH)) * roadWidth * 2.0f;

                    // Fix: calculate pickup Y relative to the road
                    // Use same logic as road, but shift upward
                    float floatHeight = RoadConfig::PICKUP_FLOAT_HEIGHT + seg.repairPickup.bobOffset;
                    float pickupScreenY = static_cast<float>(y) - floatHeight * scale * 50.0f;

                    float pulse = 0.8f + 0.2f * std::sin(seg.repairPickup.animTimer * 4.0f);

                    pickupsToRender.push_back({pickupScreenX,
                                               pickupScreenY,
                                               scale, // use same scale as the road
                                               pulse,
                                               segmentIndex});
                }
            }
        }
    }

    // Render pickups - floating green crosses (larger)
    std::sort(pickupsToRender.begin(), pickupsToRender.end(),
              [](const PickupRenderData &a, const PickupRenderData &b)
              {
                  return a.scale < b.scale; // far -> near
              });

    for (const auto &pickup : pickupsToRender)
    {
        if (pickup.scale < 0.00001f)
            continue;

        // Enlarged: scale factor increased from 3500 to 6000
        float baseSize = 6000.0f;
        float size = pickup.scale * baseSize;

        // Enlarged: larger clamps
        size = std::clamp(size, 5.0f, 120.0f);

        if (size < 5.0f)
            continue;

        // Cross proportions thicker
        float crossWidth = size * 0.4f;
        float crossLength = size;

        // Green color with pulsing
        sf::Color crossColor(
            static_cast<std::uint8_t>(40 * pickup.pulse),
            static_cast<std::uint8_t>(230 * pickup.pulse),
            static_cast<std::uint8_t>(40 * pickup.pulse));

        float outlineThickness = std::max(1.5f, size * 0.08f);

        // Cross - vertical bar
        sf::RectangleShape verticalBar(sf::Vector2f(crossWidth, crossLength));
        verticalBar.setOrigin(sf::Vector2f(crossWidth / 2.0f, crossLength / 2.0f));
        verticalBar.setPosition(sf::Vector2f(pickup.screenX, pickup.screenY));
        verticalBar.setFillColor(crossColor);
        verticalBar.setOutlineColor(sf::Color(120, 255, 120));
        verticalBar.setOutlineThickness(outlineThickness);

        // Cross - horizontal bar
        sf::RectangleShape horizontalBar(sf::Vector2f(crossLength, crossWidth));
        horizontalBar.setOrigin(sf::Vector2f(crossLength / 2.0f, crossWidth / 2.0f));
        horizontalBar.setPosition(sf::Vector2f(pickup.screenX, pickup.screenY));
        horizontalBar.setFillColor(crossColor);
        horizontalBar.setOutlineColor(sf::Color(120, 255, 120));
        horizontalBar.setOutlineThickness(outlineThickness);

        // Glow effect - larger
        float glowRadius = size * 1.0f;
        sf::CircleShape glow(glowRadius);
        glow.setOrigin(sf::Vector2f(glowRadius, glowRadius));
        glow.setPosition(sf::Vector2f(pickup.screenX, pickup.screenY));
        glow.setFillColor(sf::Color(50, 255, 50, static_cast<std::uint8_t>(50 * pickup.pulse)));

        window.draw(glow);
        window.draw(verticalBar);
        window.draw(horizontalBar);

        // Bright center - larger
        float centerRadius = crossWidth * 0.5f;
        sf::CircleShape center(centerRadius);
        center.setOrigin(sf::Vector2f(centerRadius, centerRadius));
        center.setPosition(sf::Vector2f(pickup.screenX, pickup.screenY));
        center.setFillColor(sf::Color(220, 255, 220, static_cast<std::uint8_t>(240 * pickup.pulse)));
        window.draw(center);
    }
}

void Road::setSegmentCurve(int index, float curve)
{
    if (index >= 0 && index < static_cast<int>(m_segments.size()))
    {
        m_segments[index].curve = curve;
    }
}

void Road::project(RoadSegment &segment, float cameraX, float cameraY, float cameraZ)
{
    auto &settings = SettingsManager::getInstance();
    const float windowWidth = static_cast<float>(settings.getWindowWidth());
    const float windowHeight = static_cast<float>(settings.getWindowHeight());
    ;

    float relativeZ = segment.worldZ - cameraZ;

    if (relativeZ <= 0)
    {
        segment.scale = -1.0f;
        return;
    }

    segment.scale = RoadConfig::CAMERA_DEPTH / relativeZ;
    segment.screenX = windowWidth * 0.5f + (segment.curve - cameraX) * segment.scale * windowWidth;
    segment.screenY = windowHeight * 0.5f + (cameraY - segment.worldY) * segment.scale * windowHeight;
    segment.screenW = RoadConfig::ROAD_WIDTH * segment.scale * windowWidth * 0.5f;
}

float Road::getCurveAt(float z) const
{
    const RoadSegment *segment = getSegmentAt(z);
    return segment ? segment->curve : 0.0f;
}

RoadSegment *Road::getSegmentAt(float z)
{
    if (m_segments.empty())
        return nullptr;

    const float trackLength = getLength();
    float normalizedZ = std::fmod(z, trackLength);
    if (normalizedZ < 0.0f)
        normalizedZ += trackLength;

    int index = static_cast<int>(normalizedZ / RoadConfig::SEGMENT_LENGTH) % m_segments.size();
    if (index < 0)
        index += static_cast<int>(m_segments.size());

    return &m_segments[index];
}

const RoadSegment *Road::getSegmentAt(float z) const
{
    if (m_segments.empty())
        return nullptr;

    const float trackLength = getLength();
    float normalizedZ = std::fmod(z, trackLength);
    if (normalizedZ < 0.0f)
        normalizedZ += trackLength;

    int index = static_cast<int>(normalizedZ / RoadConfig::SEGMENT_LENGTH) % m_segments.size();
    if (index < 0)
        index += static_cast<int>(m_segments.size());

    return &m_segments[index];
}

// Add a new function for Campaign:

void Road::generateForCampaign(int segmentCount)
{
    m_segments.clear();
    m_segments.reserve(segmentCount);

    const int COLOR_PATTERN_LENGTH = 6;
    int adjustedCount = ((segmentCount + COLOR_PATTERN_LENGTH - 1) / COLOR_PATTERN_LENGTH) * COLOR_PATTERN_LENGTH;

    for (int i = 0; i < adjustedCount; ++i)
    {
        RoadSegment segment;
        segment.worldZ = i * RoadConfig::SEGMENT_LENGTH;
        segment.curve = 0.0f;
        segment.worldY = 0.0f;

        bool isDark = ((i / 3) % 2) == 0;
        segment.roadColor = isDark ? sf::Color(107, 107, 107) : sf::Color(105, 105, 105);
        segment.grassColor = isDark ? sf::Color(16, 200, 16) : sf::Color(0, 154, 0);
        segment.rumbleColor = (i % 3 == 0) ? sf::Color::Red : sf::Color::White;
        segment.laneColor = sf::Color::White;

        // No potholes or pickups in Campaign
        segment.pothole.exists = false;
        segment.repairPickup.exists = false;

        m_segments.push_back(segment);
    }

    // Road layout (curves, hills)
    int pos = 0;
    addStraight(pos, adjustedCount / 6);
    pos += adjustedCount / 6;
    addCurve(pos, adjustedCount / 8, 6.0f);
    pos += adjustedCount / 8;
    addStraight(pos, adjustedCount / 10);
    pos += adjustedCount / 10;
    addCurve(pos, adjustedCount / 8, -6.0f);
    pos += adjustedCount / 8;
    addHill(pos, adjustedCount / 6, 400.0f);
    pos += adjustedCount / 6;
    addStraight(pos, adjustedCount / 6);

    std::cout << "[ROAD] Generated for Campaign mode (no obstacles)" << std::endl;
}

// Add this function after init():
void Road::initClean(int segmentCount)
{
    m_segments.clear();
    m_segments.reserve(segmentCount);

    const int COLOR_PATTERN_LENGTH = 6;
    int adjustedCount = ((segmentCount + COLOR_PATTERN_LENGTH - 1) / COLOR_PATTERN_LENGTH) * COLOR_PATTERN_LENGTH;

    for (int i = 0; i < adjustedCount; ++i)
    {
        RoadSegment segment;
        segment.worldZ = i * RoadConfig::SEGMENT_LENGTH;
        segment.curve = 0.0f;
        segment.worldY = 0.0f;

        bool isDark = ((i / 3) % 2) == 0;
        segment.roadColor = isDark ? sf::Color(107, 107, 107) : sf::Color(105, 105, 105);
        segment.grassColor = isDark ? sf::Color(16, 200, 16) : sf::Color(0, 154, 0);
        segment.rumbleColor = (i % 3 == 0) ? sf::Color::Red : sf::Color::White;
        segment.laneColor = sf::Color::White;

        // Do not generate potholes or pickups
        segment.pothole.exists = false;
        segment.pothole.wasHit = false;
        segment.repairPickup.exists = false;
        segment.repairPickup.collected = false;

        m_segments.push_back(segment);
    }

    m_potholeCount = 0;
    std::cout << "[ROAD] Initialized clean (no obstacles): " << m_segments.size() << " segments" << std::endl;
}
