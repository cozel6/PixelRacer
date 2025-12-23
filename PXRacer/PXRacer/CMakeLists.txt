# CMakeList.txt : CMake project for PXRacer, include source and define
# project specific logic here.
cmake_minimum_required(VERSION 3.16)


project(PXRacer)

# Add source to this project's executable.
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")
add_executable(PXRacer ${SOURCES}
    "src/States/MainMenuState.cpp"
    "src/States/MainMenuState.h"
    "src/States/PlayState.h"
    "src/States/PlayState.cpp"
    "src/States/SettingsState.h"
    "src/States/SettingsState.cpp"
    "src/States/CreditsState.h"
    "src/States/CreditsState.cpp"
    "src/Gameplay/Road.h"
    "src/Gameplay/Player.h"
    "src/Gameplay/Player.cpp"
    "src/Gameplay/Road.cpp"
    "src/Gameplay/GameplayManager.h"
    "src/Gameplay/GameplayManager.cpp"
    "src/Gameplay/TrackDefinition.cpp"
    "src/Gameplay/GameModeConfig.h"
    "src/States/TrackSelectState.h"
    "src/States/TrackSelectState.cpp"
    "src/Gameplay/TrackBuilder.h"
    "src/Gameplay/TrackBuilder.cpp"
    "src/Gameplay/CurveProcessor.h"
    "src/Gameplay/CurveProcessor.cpp"
    "src/UI/GameHUD.h"
    "src/UI/GameHUD.cpp"
)

target_include_directories(PXRacer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# === Set working directory for IDEs ===
# When running from Visual Studio (F5) or Xcode (Run),
# the working directory will be the project root (where assets/ is located)
set_target_properties(PXRacer PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"  # Visual Studio (Windows)
    XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" # Xcode (macOS)
)
# === Find and link SFML library ===
if(WIN32 AND NOT DEFINED ENV{SFML_DIR})
    set(POSSIBLE_SFML_PATHS
        "C:/Users/user/Documents/Rersources/SFML-3.0.2-windows-vc17-64-bit/SFML-3.0.2/lib/cmake/SFML"
        "S:/SFML/SFML-3.0.2/lib/cmake/SFML"
    )
    
    foreach(SFML_PATH ${POSSIBLE_SFML_PATHS})
        message(STATUS "Checking SFML path: ${SFML_PATH}")
        if(EXISTS ${SFML_PATH})
            message(STATUS "Found SFML at: ${SFML_PATH}")
            set(SFML_DIR ${SFML_PATH} CACHE PATH "Path to SFML")
            break()
        else()
            message(STATUS "Path not found: ${SFML_PATH}")
        endif()
    endforeach()
endif()

find_package(SFML 3 COMPONENTS Graphics Window REQUIRED)
target_link_libraries(PXRacer PRIVATE SFML::Graphics SFML::Window)

# Set C++ standard
set_target_properties(PXRacer PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# === Windows-specific: Copy SFML DLLs after build ===
if(WIN32)
    add_custom_command(TARGET PXRacer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SFML_DIR}/../../../bin"
        $<TARGET_FILE_DIR:PXRacer>
        COMMENT "Copying SFML DLLs to output directory"
    )
endif()

# === macOS-specific: Handle frameworks ===
if(APPLE)
    set_target_properties(PXRacer PROPERTIES
        INSTALL_RPATH "@executable_path/../Frameworks"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()

# === Asset management: Symlink (macOS Dev) vs Copy (Windows/Release) ===
# Strategy:
#   - macOS Debug: Symlink → instant, 0 copy time
#   - Windows Debug: Copy → only modified files (fast)
#   - Release (both): Copy → for distribution

if(UNIX AND CMAKE_BUILD_TYPE MATCHES "Debug")
    # macOS/Linux in Debug mode: Create symlink (instant, no copying!)
    add_custom_command(TARGET PXRacer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:PXRacer>/assets
        COMMENT "Creating symlink to assets (Dev mode - instant!)"
    )
    message(STATUS "Development mode (macOS): Symlinking assets → instant, no copy! ⚡")

elseif(WIN32 AND CMAKE_BUILD_TYPE MATCHES "Debug")
    # Windows in Debug mode: Copy (symlink requires admin rights)
    # Note: copy_directory only copies modified files (incremental)
    add_custom_command(TARGET PXRacer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:PXRacer>/assets
        COMMENT "Copying assets (Dev mode - only modified files)"
    )
    message(STATUS "Development mode (Windows): Copying assets (incremental) 🔄")

else()
    # Release mode (both platforms): Copy for distribution
    add_custom_command(TARGET PXRacer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:PXRacer>/assets
        COMMENT "Copying assets (Release mode - for distribution)"
    )
    message(STATUS "Release mode: Copying assets for distribution 📦")
endif()