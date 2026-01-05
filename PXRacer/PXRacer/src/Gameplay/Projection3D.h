#pragma once
#include "Road.h"

// Unified 3D to 2D projection system for pseudo-3D racing
class Projection3D {
public:
    struct ProjectedPoint {
        float screenX;
        float screenY;
        float scale;
        bool visible;
    };
    
    // Project world position (worldX, worldZ) to screen coordinates
    // cameraZ = player's Z position
    static ProjectedPoint project(
        float worldX, 
        float worldZ, 
        float cameraZ,
        float windowWidth,
        float windowHeight
    );
    
    // Check if a Z position is visible on screen
    static bool isVisible(float worldZ, float cameraZ);
    
    // Get screen Y for a given world Z (for rendering order)
    static float getScreenY(float worldZ, float cameraZ, float windowHeight);
};