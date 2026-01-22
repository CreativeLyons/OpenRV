# OpenRV macOS Build Requirements

## Current Status

✅ **Qt 6.6.3 detected** at `/Users/tonylyons/Qt/6.6.3/macos`
✅ **QtWebEngine confirmed present**
❌ **Qt6Multimedia missing** - Required for audio functionality

## Required Qt Components

OpenRV requires the following Qt 6.6.3 components:

1. ✅ **Qt WebEngine** - Already installed
2. ❌ **Qt Multimedia** - **MISSING** - Required for audio playback

## Fix: Install Qt Multimedia

**Install via Qt Maintenance Tool:**

```bash
open ~/Qt/MaintenanceTool
```

Then:
1. Click "Add or remove components"
2. Select your Qt 6.6.3 installation
3. Expand "Qt 6.6.3" → "Additional Libraries"
4. Check "Qt Multimedia" component
5. Click "Next" and complete the installation

**Verify installation:**
```bash
ls -d /Users/tonylyons/Qt/6.6.3/macos/lib/cmake/Qt6Multimedia
```
Should show the directory exists.

## Build Commands (After Installing Qt Multimedia)

```bash
cd /Users/tonylyons/Dropbox/Public/GitHub/OpenRV
export RV_VFX_PLATFORM=CY2024
export QT_HOME=/Users/tonylyons/Qt/6.6.3/macos
source rvcmds.sh
rvbootstrap
```

## Verification

To verify Qt detection is working (not using Homebrew Qt):
- Check that `QT_HOME` points to `/Users/tonylyons/Qt/6.6.3/macos`
- CMake should find Qt components from that location, not `/opt/homebrew/lib/cmake/Qt6`
