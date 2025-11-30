#include "TrackBuilder.h"
#include <cmath>
#include <iostream>

void TrackBuilder::buildTrack(Road& road, const TrackDefinition& trackDef) {
    int totalSegments = 0;
    for (const auto& section : trackDef.sections) {
        totalSegments += section.length;
    }

    std::cout << "\n🏗️ ===== BUILDING TRACK: " << trackDef.name << " =====" << std::endl;
    road.init(totalSegments);

    int currentIndex = 0;
    
    for (size_t i = 0; i < trackDef.sections.size(); ++i) {
        const auto& section = trackDef.sections[i];

        switch (section.type) {
        case SectionType::Straight:
            applyStraight(road, currentIndex, section.length);
            break;
        case SectionType::LeftCurve:
            applyLeftCurve(road, currentIndex, section.length, section.intensity);
            break;
        case SectionType::RightCurve:
            applyRightCurve(road, currentIndex, section.length, section.intensity);
            break;
        case SectionType::LeftHairpin:
            applyHairpin(road, currentIndex, section.length, section.intensity, true);
            break;
        case SectionType::RightHairpin:
            applyHairpin(road, currentIndex, section.length, section.intensity, false);
            break;
        case SectionType::Hill:
            applyHill(road, currentIndex, section.length, section.intensity);
            break;
        case SectionType::Valley:
            applyValley(road, currentIndex, section.length, section.intensity);
            break;
        case SectionType::Chicane:
            applyChicane(road, currentIndex, section.length, section.intensity);
            break;
        }
        currentIndex += section.length;
    }

    // ✅ NEW: Smooth the loop transition
    smoothLoopTransition(road, totalSegments);

    std::cout << "✅ Track built: " << road.getSegmentCount() << " segments\n" << std::endl;
}

void TrackBuilder::smoothLoopTransition(Road& road, int totalSegments) {
    // ✅ Smooth the transition between last and first segments
    const int TRANSITION_LENGTH = 20; // Number of segments to blend
    
    if (totalSegments < TRANSITION_LENGTH * 2) {
        return; // Track too short to blend
    }
    
    std::cout << "   🔄 Smoothing loop transition..." << std::endl;
    
    // Calculate total curve accumulation over the entire track
    float totalCurveSum = 0.0f;
    for (int i = 0; i < totalSegments; ++i) {
        totalCurveSum += road.getSegmentAt(i * 10.0f)->curve;
    }
    
    // If the track doesn't loop back (total curve != 0), apply correction
    if (std::abs(totalCurveSum) > 0.01f) {
        float correction = -totalCurveSum / totalSegments;
        
        // Apply gradual correction across transition zones
        for (int i = 0; i < TRANSITION_LENGTH; ++i) {
            float t = static_cast<float>(i) / TRANSITION_LENGTH;
            float smoothFactor = 0.5f * (1.0f - std::cos(t * 3.14159f)); // Smooth S-curve
            
            // Correct start of track
            RoadSegment* startSeg = road.getSegmentAt(i * 10.0f);
            if (startSeg) {
                startSeg->curve += correction * smoothFactor;
            }
            
            // Correct end of track
            RoadSegment* endSeg = road.getSegmentAt((totalSegments - TRANSITION_LENGTH + i) * 10.0f);
            if (endSeg) {
                endSeg->curve += correction * (1.0f - smoothFactor);
            }
        }
        
        std::cout << "      Applied curve correction: " << correction << " per segment" << std::endl;
    }
}

void TrackBuilder::applyStraight(Road& road, int startIndex, int length) {
    std::cout << "   ➡️  STRAIGHT: segments=" << length << std::endl;
    for (int i = 0; i < length; ++i) {
        road.setSegmentCurve(startIndex + i, 0.0f);
    }
}

void TrackBuilder::applyLeftCurve(Road& road, int startIndex, int length, float intensity) {
    std::cout << "   ↩️  LEFT CURVE: intensity=" << intensity << ", segments=" << length << std::endl;
    
    const float INTENSITY_BOOST = 3.0f;
    
    for (int i = 0; i < length; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(length);
        float curve = -(intensity * INTENSITY_BOOST) * std::sin(t * 3.14159f);
        road.setSegmentCurve(startIndex + i, curve);
    }
}

void TrackBuilder::applyRightCurve(Road& road, int startIndex, int length, float intensity) {
    std::cout << "   ↪️  RIGHT CURVE: intensity=" << intensity << ", segments=" << length << std::endl;
    
    const float INTENSITY_BOOST = 3.0f;
    
    for (int i = 0; i < length; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(length);
        float curve = (intensity * INTENSITY_BOOST) * std::sin(t * 3.14159f);
        road.setSegmentCurve(startIndex + i, curve);
    }
}

void TrackBuilder::applyHairpin(Road& road, int startIndex, int length, float intensity, bool left) {
    std::cout << "   🔄 HAIRPIN " << (left ? "LEFT" : "RIGHT") << ": intensity=" << intensity << ", segments=" << length << std::endl;
    
    float sign = left ? -1.0f : 1.0f;
    const float INTENSITY_BOOST = 3.5f;
    
    for (int i = 0; i < length; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(length);
        float curve = sign * (intensity * INTENSITY_BOOST) * std::sin(t * 3.14159f);
        road.setSegmentCurve(startIndex + i, curve);
    }
}

void TrackBuilder::applyHill(Road& road, int startIndex, int length, float intensity) {
    std::cout << "   ⛰️  HILL: intensity=" << intensity << ", segments=" << length << std::endl;
    road.addHill(startIndex, length, 2000.0f * intensity);
}

void TrackBuilder::applyValley(Road& road, int startIndex, int length, float intensity) {
    std::cout << "   🕳️  VALLEY: intensity=" << intensity << ", segments=" << length << std::endl;
    road.addHill(startIndex, length, -2000.0f * intensity);
}

void TrackBuilder::applyChicane(Road& road, int startIndex, int length, float intensity) {
    std::cout << "   ⚡ CHICANE: intensity=" << intensity << ", segments=" << length << std::endl;
    
    const float INTENSITY_BOOST = 3.0f;
    int halfLength = length / 2;
    
    // Prima jumătate - left
    for (int i = 0; i < halfLength; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(halfLength);
        float curve = -(intensity * INTENSITY_BOOST) * std::sin(t * 3.14159f);
        road.setSegmentCurve(startIndex + i, curve);
    }
    
    // A doua jumătate - right
    for (int i = 0; i < (length - halfLength); ++i) {
        float t = static_cast<float>(i) / static_cast<float>(length - halfLength);
        float curve = (intensity * INTENSITY_BOOST) * std::sin(t * 3.14159f);
        road.setSegmentCurve(startIndex + halfLength + i, curve);
    }
}