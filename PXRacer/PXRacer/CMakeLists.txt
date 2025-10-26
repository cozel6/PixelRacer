# CMakeList.txt : CMake project for PXRacer, include source and define
# project specific logic here.
cmake_minimum_required(VERSION 3.16)


project(PXRacer)

# Add source to this project's executable.
add_executable(PXRacer "PXRacer.cpp" "PXRacer.h")

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