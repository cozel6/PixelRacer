#pragma once
#include "Road.h"
#include "TrackDefinition.h"

class TrackBuilder {
public:
    static void buildTrack(Road& road, const TrackDefinition& trackDef);

private:
    static void applyStraight(Road& road, int startIndex, int length);
    static void applyLeftCurve(Road& road, int startIndex, int length, float intensity);
    static void applyRightCurve(Road& road, int startIndex, int length, float intensity);
    static void applyHairpin(Road& road, int startIndex, int length, float intensity, bool left);
    static void applyHill(Road& road, int startIndex, int length, float intensity);
    static void applyValley(Road& road, int startIndex, int length, float intensity);
    static void applyChicane(Road& road, int startIndex, int length, float intensity);
    
    // Smooth the loop transition for seamless circuit
    static void smoothLoopTransition(Road& road, int totalSegments);
};