#pragma once
#include <vector>

// CurveProcessor - Responsible for curve mathematics and accumulation
class CurveProcessor {
public:
    struct CurveData {
        std::vector<float> accumulatedCurves;  // Accumulated high-res curves
        float cameraOffset;                     // Offset for camera centering
        int baseSamples;                        // Number of samples
    };

    // Calculates accumulated curves with Catmull-Rom interpolation
    static CurveData processSegmentCurves(
        const std::vector<float>& segmentCurves,
        int baseSegmentIndex,
        int drawDistance,
        float cameraPosition
    );

    
    static float catmullRomInterpolate(float p0, float p1, float p2, float p3, float t);

private:
    // High-resolution sampling for smooth curves
    static constexpr int SAMPLES_PER_SEGMENT = 4;
};