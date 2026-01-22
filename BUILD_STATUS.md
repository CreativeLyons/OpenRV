# OpenRV macOS Build Status

## Current Status

✅ **Qt Detection Fixed**: CMake now correctly prioritizes the specified Qt installation (`/Users/tonylyons/Qt/6.5.3/macos`) over Homebrew Qt.

✅ **Build Environment Ready**: All required Homebrew packages installed, CMake configured, Python virtual environment set up.

❌ **QtWebEngine Missing**: Qt 6.5.3 installation is missing the QtWebEngine component, which is required for building OpenRV.

## Required Action: Install QtWebEngine

QtWebEngine must be installed via the Qt Maintenance Tool. Follow these steps:

1. **Open Qt Maintenance Tool:**
   ```bash
   open ~/Qt/MaintenanceTool
   ```
   (If not found there, check `~/Qt/` directory or re-download from https://www.qt.io/download-open-source)

2. **In Qt Maintenance Tool:**
   - Click "Add or remove components"
   - Select your Qt 6.5.3 installation
   - Expand "Qt 6.5.3" → "Additional Libraries"
   - Check "Qt WebEngine" component
   - Click "Next" and complete the installation

3. **Verify Installation:**
   ```bash
   ls -d /Users/tonylyons/Qt/6.5.3/macos/lib/cmake/Qt6WebEngineCore
   ```
   Should show the directory exists.

## Build Commands (After Installing QtWebEngine)

Once QtWebEngine is installed, run:

```bash
cd /Users/tonylyons/Dropbox/Public/GitHub/OpenRV
export RV_VFX_PLATFORM=CY2024
export QT_HOME=/Users/tonylyons/Qt/6.5.3/macos
source rvcmds.sh
rvbootstrap
```

Or step by step:
```bash
source rvcmds.sh
rvsetup    # Install Python dependencies (already done)
rvcfg      # Configure CMake
rvbuild    # Build the project
```

## Final Output Location

After successful build:
- **Release build:** `_build/stage/app/RV.app/Contents/MacOS/RV`
- **Debug build:** `_build_debug/stage/app/RV.app/Contents/MacOS/RV`

**To launch:**
```bash
open _build/stage/app/RV.app
# or
_build/stage/app/RV.app/Contents/MacOS/RV
```

## Fixes Applied

1. **Qt Path Priority** (`cmake/dependencies/qt6.cmake`):
   - Prepend Qt location to `CMAKE_PREFIX_PATH` to prioritize over Homebrew
   - Clear cached Qt6 variables to force re-discovery
   - Updated error message for Qt 6.5.3

2. **QtWebEngine Installation Guidance**:
   - Clear error message directing users to install via Qt Maintenance Tool
   - Qt 6.5.3 WebEngine cannot be auto-downloaded (unlike 6.8.3)

## Verification

To verify Qt detection is working correctly (not picking up Homebrew Qt):

```bash
cd /Users/tonylyons/Dropbox/Public/GitHub/OpenRV
export RV_VFX_PLATFORM=CY2024
export QT_HOME=/Users/tonylyons/Qt/6.5.3/macos
source rvcmds.sh
rvcfg 2>&1 | grep -i "qt\|webengine"
```

Should show:
- `Using Qt 6.5 installation already set at /Users/tonylyons/Qt/6.5.3/macos`
- No references to `/opt/homebrew/lib/cmake/Qt6`
