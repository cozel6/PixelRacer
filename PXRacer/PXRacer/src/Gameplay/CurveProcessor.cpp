#include "CurveProcessor.h"
#include <algorithm>
#include <cmath>

CurveProcessor::CurveData CurveProcessor::processSegmentCurves(
    const std::vector<float>& segmentCurves,
    int baseSegmentIndex,
    int drawDistance,
    float cameraPosition
) {
    CurveData result;
    
    const int totalSamples = drawDistance * SAMPLES_PER_SEGMENT;
    result.accumulatedCurves.resize(totalSamples + 1, 0.0f);
    result.baseSamples = totalSamples;
    
    const int trackLength = static_cast<int>(segmentCurves.size());
    
    // Wrap-around smoothing for seamless looping!
    // Calculate accumulated curve absolute from track start
    float baseAccumulation = 0.0f;
    for (int i = 0; i < baseSegmentIndex % trackLength; ++i) {
        baseAccumulation += segmentCurves[i];
    }
    
    // Accumulate with wrap-around for seamless looping
    float runningCurve = 0.0f;
    for (int n = 0; n <= totalSamples; ++n) {
        float segmentPos = static_cast<float>(n) / SAMPLES_PER_SEGMENT;
        int segmentIdx = static_cast<int>(segmentPos);
        
        // Correct wrap-around for index
        int absoluteIndex = (baseSegmentIndex + segmentIdx) % trackLength;
        if (absoluteIndex < 0) absoluteIndex += trackLength;
        
        if (n > 0) {
            int prevSegmentIdx = static_cast<int>((n - 1) / SAMPLES_PER_SEGMENT);
            int prevAbsoluteIndex = (baseSegmentIndex + prevSegmentIdx) % trackLength;
            if (prevAbsoluteIndex < 0) prevAbsoluteIndex += trackLength;
            
            runningCurve += segmentCurves[prevAbsoluteIndex];
        }
        
        result.accumulatedCurves[n] = runningCurve;
    }
    
    // Calculate camera offset for perfect centering
    float cameraHighResIndex = cameraPosition * SAMPLES_PER_SEGMENT;
    int camIdx0 = std::max(0, std::min(static_cast<int>(cameraHighResIndex), totalSamples - 1));
    int camIdx_1 = std::max(0, camIdx0 - 1);
    int camIdx1 = std::min(camIdx0 + 1, totalSamples);
    int camIdx2 = std::min(camIdx0 + 2, totalSamples);
    
    float camT = cameraHighResIndex - camIdx0;
    
    result.cameraOffset = catmullRomInterpolate(
        result.accumulatedCurves[camIdx_1],
        result.accumulatedCurves[camIdx0],
        result.accumulatedCurves[camIdx1],
        result.accumulatedCurves[camIdx2],
        camT
    );
    
    return result;
}

float CurveProcessor::catmullRomInterpolate(float p0, float p1, float p2, float p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;
    
    return 0.5f * (
        (2.0f * p1) +
        (-p0 + p2) * t +
        (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
        (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
    );
}