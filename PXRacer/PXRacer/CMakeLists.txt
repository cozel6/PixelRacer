# CMakeList.txt : CMake project for PXRacer, include source and define
# project specific logic here.
#

cmake_minimum_required(VERSION 3.16)

# Add source to this project's executable.
add_executable (PXRacer "PXRacer.cpp" "PXRacer.h" "New Folder Test/Test.cpp")

# Cross-platform SFML detection
if(WIN32 AND NOT DEFINED ENV{SFML_DIR})
    # Optional: Set a default Windows path if SFML_DIR is not set
    set(SFML_DIR "C:/Users/user/Documents/Rersources/SFML-3.0.2-windows-vc17-64-bit/SFML-3.0.2/lib/cmake/SFML" CACHE PATH "Path to SFML")
endif()

find_package(SFML 3 COMPONENTS Graphics Window REQUIRED)

target_link_libraries(PXRacer PRIVATE SFML::Graphics)

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
    # SFML on macOS uses frameworks which are handled automatically by CMake
    set_target_properties(PXRacer PROPERTIES
        INSTALL_RPATH "@executable_path/../Frameworks"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()
