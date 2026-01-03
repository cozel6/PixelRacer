#include "Projection3D.h"
#include <cmath>
#include <iostream>

Projection3D::ProjectedPoint Projection3D::project(
    float worldX,
    float worldZ,
    float cameraZ,
    float windowWidth,
    float windowHeight
) {
    ProjectedPoint result;
    result.visible = false;
    result.screenX = 0.0f;
    result.screenY = 0.0f;
    result.scale = 0.0f;

    const float relativeZ = worldZ - cameraZ;

    // Behind camera or too far
    if (relativeZ <= 10.0f || relativeZ > 5000.0f) {
        return result;
    }

    // ✅ SIMPLE PERSPECTIVE FORMULA (no RoadConfig dependency!)
    // Scale: objects get smaller with distance
    // At relativeZ = 100, scale = 1.0 (full size)
    // At relativeZ = 1000, scale = 0.1 (10% size)
    // At relativeZ = 5000, scale = 0.02 (2% size - at horizon)
    
    const float perspectiveDistance = 100.0f;  // Reference distance for scale=1.0
    result.scale = perspectiveDistance / relativeZ;

    // Screen Y: map Z distance to screen position
    // Close objects (relativeZ small) -> bottom of screen (Y = 0.85)
    // Far objects (relativeZ large) -> horizon (Y = 0.5)
    
    // Normalize distance: 0.0 = at camera, 1.0 = at max render distance
    const float maxRenderZ = 5000.0f;
    const float normalizedZ = std::clamp(relativeZ / maxRenderZ, 0.0f, 1.0f);
    
    // Interpolate Y: close = 0.85, far = 0.5
    const float horizonY = 0.5f;
    const float nearY = 0.85f;
    
    // Use inverse relationship for perspective (far objects cluster near horizon)
    const float yFactor = 1.0f - std::pow(normalizedZ, 0.5f);  // Square root for non-linear
    result.screenY = windowHeight * (horizonY + yFactor * (nearY - horizonY));

    // Screen X: lateral position with perspective
    // At scale=1.0, worldX maps directly to screen
    // As scale decreases, objects move toward center
    result.screenX = windowWidth * 0.5f + (worldX * result.scale * 0.5f);

    // Clamp scale for safety
    if (result.scale < 0.01f || result.scale > 2.0f) {
        return result;
    }

    // Check if on screen
    if (result.screenY < windowHeight * 0.45f || result.screenY > windowHeight) {
        return result;
    }
    
    if (result.screenX < -100.0f || result.screenX > windowWidth + 100.0f) {
        return result;
    }

    result.visible = true;
    return result;
}

bool Projection3D::isVisible(float worldZ, float cameraZ) {
    const float relativeZ = worldZ - cameraZ;
    return (relativeZ > 10.0f && relativeZ <= 5000.0f);
}

float Projection3D::getScreenY(float worldZ, float cameraZ, float windowHeight) {
    const float relativeZ = worldZ - cameraZ;

    if (relativeZ <= 10.0f) {
        return windowHeight * 2.0f; // Off-screen
    }

    const float maxRenderZ = 5000.0f;
    const float normalizedZ = std::clamp(relativeZ / maxRenderZ, 0.0f, 1.0f);
    const float horizonY = 0.5f;
    const float nearY = 0.85f;
    const float yFactor = 1.0f - std::pow(normalizedZ, 0.5f);
    
    return windowHeight * (horizonY + yFactor * (nearY - horizonY));
}