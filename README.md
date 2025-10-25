# 🏎️ PixelRacer

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![SFML](https://img.shields.io/badge/SFML-3.0.2-brightgreen.svg)](https://www.sfml-dev.org/)
[![CMake](https://img.shields.io/badge/CMake-3.16+-red.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-All%20Rights%20Reserved-lightgrey.svg)]()
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20macOS-lightgrey.svg)]()

A retro-style 2D/2.5D racing game built with C++ and SFML, featuring isometric graphics, AI opponents, and cross-platform support.

---

## 🎮 About

PixelRacer is a racing game that combines classic arcade gameplay with modern programming techniques. Race against AI opponents on multiple tracks, choose from various vehicles, and compete for the best lap times.

The project is developed as a learning experience in game development, focusing on:
- 2D game physics and collision detection
- State machine architecture
- AI pathfinding and behavior
- Isometric rendering
- Cross-platform development

---

##  Features

### Current Features
-  **Racing Mechanics** - Complete lap system with checkpoints and timing
-  **AI Opponents** - Intelligent racing AI with difficulty levels
-  **Multiple Vehicles** - 5-6 cars with different stats (speed, handling, acceleration)
-  **Multiple Tracks** - 3-4 circuits with varying difficulty
-  **Isometric Graphics** - 2.5D perspective for visual depth
-  **Single Player Mode** - Race against AI opponents
-  **Settings Menu** - Customizable game options

### Planned Features
-  **LAN Multiplayer** - Local network racing (2-3 players)
-  **Power-ups** - Speed boosts, shields, and more
-  **Leaderboards** - Track best times and high scores
-  **Sound Effects** - Engine sounds, collisions, and UI feedback
-  **Visual Effects** - Particle systems for dust and smoke

---

##  Technologies

| Component | Technology |
|-----------|-----------|
| Language | C++17 |
| Graphics | SFML 3.0.2 |
| Build System | CMake 3.16+ |
| IDE | Visual Studio 2022 / VS Code |
| Version Control | Git & GitHub |
| Package Manager | vcpkg (Windows) / Homebrew (macOS) |

---

## 📋 Prerequisites

### Windows
- Visual Studio 2022 with C++ Desktop Development workload
- CMake 3.16 or higher
- vcpkg for dependency management
- Git

### macOS
- Xcode Command Line Tools: `xcode-select --install`
- Homebrew: [Install here](https://brew.sh/)
- CMake 3.16 or higher
- Git

---

## 🚀 Getting Started

### Windows Setup

#### 1. Install vcpkg and SFML
```bash
# Clone vcpkg (if not already installed)
cd C:\
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Install SFML
.\vcpkg install sfml:x64-windows

# Integrate with Visual Studio
.\vcpkg integrate install
```

#### 2. Clone and Build
```bash
# Clone the repository
git clone https://github.com/cozel6/PixelRacer.git
cd PixelRacer

# Run the build script
run.bat
```

#### 3. Build with Visual Studio (Alternative)
1. Open Visual Studio 2022
2. File → Open → Folder...
3. Select the `PixelRacer` folder
4. CMake will configure automatically
5. Press `F7` to build
6. Press `Ctrl+F5` to run

---

### macOS Setup

#### 1. Install Dependencies
```bash
# Install Homebrew (if not already installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install CMake and SFML
brew install cmake sfml
```

#### 2. Clone and Build
```bash
# Clone the repository
git clone https://github.com/cozel6/PixelRacer.git
cd PixelRacer

# Run the build script
./run.sh
```

#### 3. Manual Build (Alternative)
```bash
# Create build directory
cd PXRacer
mkdir -p build-macos && cd build-macos

# Configure with CMake
cmake ..

# Build
cmake --build . --config Release

# Run the game
./PXRacer/PXRacer
```

---

## 🎮 Controls

| Action | Keyboard |
|--------|----------|
| Accelerate | ↑ / W |
| Brake/Reverse | ↓ / S |
| Steer Left | ← / A |
| Steer Right | → / D |
| Pause | ESC |
| Menu Select | ENTER |

---

## 📁 Project Structure

```
PixelRacer/
├── PXRacer/
│   ├── CMakeLists.txt
│   └── PXRacer/
│       ├── src/              # Source code
│       │   ├── Core/         # Game engine core
│       │   ├── States/       # Game states (Menu, Playing, etc.)
│       │   ├── Entities/     # Game objects (Car, AI, etc.)
│       │   ├── Physics/      # Physics engine
│       │   ├── Track/        # Track/map system
│       │   ├── UI/           # User interface
│       │   ├── Rendering/    # Graphics rendering
│       │   └── AI/           # Artificial intelligence
│       │
│       └── assets/           # Game assets
│           ├── textures/     # Sprites and tiles
│           ├── fonts/        # Text fonts
│           ├── sounds/       # Audio files
│           └── tracks/       # Track definition files
│
├── run.sh                    # macOS build script
├── run.bat                   # Windows build script
└── README.md
```

---

## 🔧 Development

### Building from Source

**Debug Build:**
```bash
cmake --build build-<platform> --config Debug
```

**Release Build:**
```bash
cmake --build build-<platform> --config Release
```

### Clean Build
```bash
# Windows
rmdir /s /q PXRacer\build-windows
run.bat

# macOS
rm -rf PXRacer/build-macos
./run.sh
```

---

## 🐛 Troubleshooting

### Windows Issues

**SFML not found:**
```bash
# Ensure vcpkg is integrated
cd C:\vcpkg
.\vcpkg integrate install
```

**CMake errors:**
- Delete the `build-windows` folder and reconfigure
- Verify SFML is installed: `.\vcpkg list`

### macOS Issues

**SFML not found:**
```bash
# Specify SFML path explicitly
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix sfml)
```

**Permission denied:**
```bash
chmod +x run.sh
```

**Xcode issues:**
- Ensure Command Line Tools are installed: `xcode-select --install`

---

## 🤝 Contributing

This is currently a learning project for educational purposes. While we're not actively seeking contributions, feel free to:
- Report bugs via [Issues](https://github.com/cozel6/PixelRacer/issues)
- Suggest features or improvements
- Fork the project for your own learning

---

## 📝 License

All rights reserved. This project is currently unlicensed and is developed for educational purposes.

---

## 👥 Authors

**Popivici Cosmin**
- GitHub: [@Cosmin](https://github.com/cozel6)

**Novac Sergiu**
- GitHub: [@Sergiu](https://github.com/Serkiy)

---

## 📚 Resources

### Useful Links
- [SFML Documentation](https://www.sfml-dev.org/documentation/3.0.0/)
- [CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)
- [C++ Reference](https://en.cppreference.com/)
- [Game Programming Patterns](https://gameprogrammingpatterns.com/)

### Asset Resources
- [Kenney Game Assets](https://kenney.nl/assets)
- [OpenGameArt](https://opengameart.org/)
- [Freesound](https://freesound.org/)

---

## 🎯 Status

🚧 **Work in Progress** - Active Development

**Current Version:** Alpha 0.1
**Last Updated:** October 2025
**Target Release:** January 2026

---

## 📊 Roadmap

- [x] Project setup and cross-platform build system
- [x] Basic game loop and window management
- [ ] Core game physics
- [ ] Track system with tile-based rendering
- [ ] Vehicle selection and customization
- [ ] AI opponent implementation
- [ ] Multiple playable tracks
- [ ] Isometric rendering
- [ ] UI/UX polish
- [ ] LAN multiplayer (stretch goal)

---

## 📸 Screenshots

*Coming soon! Screenshots will be added as development progresses.*

---

## 🌟 Show Your Support

If you find this project interesting or useful for learning, please consider giving it a ⭐ star on GitHub!

---

<div align="center">

**[Report Bug](https://github.com/cozel6/PixelRacer/issues)** • **[Request Feature](https://github.com/cozel6/PixelRacer/issues)**

Made with ❤️ and C++

</div>
