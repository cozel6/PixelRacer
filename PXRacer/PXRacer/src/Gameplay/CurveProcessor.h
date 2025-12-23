#pragma once
#include <vector>

// ============================================================================
// CurveProcessor - Responsabil pentru matematica și acumularea curbelor
// ============================================================================
class CurveProcessor {
public:
    struct CurveData {
        std::vector<float> accumulatedCurves;  // Curbe acumulate high-res
        float cameraOffset;                     // Offset pentru camera centering
        int baseSamples;                        // Număr de sample-uri
    };

    // Calculează curbe acumulate cu Catmull-Rom interpolation
    static CurveData processSegmentCurves(
        const std::vector<float>& segmentCurves,
        int baseSegmentIndex,
        int drawDistance,
        float cameraPosition
    );

    // ✅ MUTATĂ ÎN PUBLIC - necesară pentru Road.cpp
    static float catmullRomInterpolate(float p0, float p1, float p2, float p3, float t);

private:
    // High-resolution sampling pentru smooth curves
    static constexpr int SAMPLES_PER_SEGMENT = 4;
};