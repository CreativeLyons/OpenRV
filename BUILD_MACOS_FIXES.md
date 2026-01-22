# macOS Build Fixes for OpenRV

This document summarizes the fixes applied to enable building OpenRV on macOS (Apple Silicon).

## Issues Identified and Fixed

### 1. Qt Path Priority Issue (FIXED)
**Problem:** CMake was finding Qt6 from Homebrew (`/opt/homebrew/lib/cmake/Qt6`) instead of the user-specified Qt installation, causing build failures because Homebrew Qt lacks QtWebEngineCore.

**Fix Applied:** Modified `cmake/dependencies/qt6.cmake` to:
- Prepend the specified Qt location to `CMAKE_PREFIX_PATH` (instead of appending)
- Clear cached Qt6 variables to force re-discovery with the specified location
- Updated error message to reference Qt 6.5.3 instead of Qt 5.15

**Files Changed:**
- `cmake/dependencies/qt6.cmake`

### 2. Missing Dependencies

#### Required but Missing:
1. **Qt 6.5.3** - Must be installed from official Qt installer (not Homebrew)
   - Download from: https://www.qt.io/download-open-source
   - Install Qt 6.5.3 to `~/Qt/6.5.3/macos` or similar location
   - Set `QT_HOME` environment variable or let `rvcmds.sh` auto-detect

2. **Xcode Developer Directory** - Currently pointing to Command Line Tools
   - Run: `sudo xcode-select -s /Applications/Xcode.app/Contents/Developer`
   - Verify with: `xcode-select -p` (should return `/Applications/Xcode.app/Contents/Developer`)

#### Already Installed:
- ✅ CMake 3.31.2 (meets requirement of 3.31.X+)
- ✅ Ninja build system
- ✅ Most Homebrew packages (ninja, readline, sqlite, xz, zlib, autoconf, automake, libtool, python@3.11, yasm, clang-format, black, meson, nasm, pkg-config, glew, rust)

#### Potentially Missing:
- ✅ `tcl-tk` - Now installed

## Build Instructions

### Prerequisites Setup

1. **Install Qt 6.5.3:**
   ```bash
   # Download Qt 6.5.3 from https://www.qt.io/download-open-source
   # Install to default location: ~/Qt/6.5.3/macos
   ```

2. **Fix Xcode Developer Directory:**
   ```bash
   sudo xcode-select -s /Applications/Xcode.app/Contents/Developer
   xcode-select -p  # Verify it returns /Applications/Xcode.app/Contents/Developer
   ```

3. **Install Missing Homebrew Packages (if needed):**
   ```bash
   brew install tcl-tk  # Already installed
   ```

### Building OpenRV

1. **Navigate to OpenRV directory:**
   ```bash
   cd /Users/tonylyons/Dropbox/Public/GitHub/OpenRV
   ```

2. **Source the build commands:**
   ```bash
   export RV_VFX_PLATFORM=CY2024
   source rvcmds.sh
   ```

3. **First-time build:**
   ```bash
   rvbootstrap
   ```

4. **Subsequent builds:**
   ```bash
   rvmk
   ```

### Build Output Location

- **Release build:** `_build/stage/app/RV.app/Contents/MacOS/RV`
- **Debug build:** `_build_debug/stage/app/RV.app/Contents/MacOS/RV`

## CMake Configuration

The build system will automatically:
- Download and build most dependencies (Boost, OpenEXR, OCIO, Python, etc.)
- Use the specified Qt installation via `RV_DEPS_QT_LOCATION`
- Configure for Apple Silicon (arm64) architecture

## Notes

- The fix ensures that when `RV_DEPS_QT_LOCATION` is specified, CMake will prioritize that location over system-installed Qt (like Homebrew)
- All other dependencies are built from source during the build process
- The build process uses a Python virtual environment (`.venv`) for Python dependencies
