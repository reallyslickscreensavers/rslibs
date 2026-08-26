# rslibs

A collection of C++ utility libraries used by [ReallySlickScreensavers](https://reallyslickscreensavers.com).

## Modules

| Module | Description |
|---|---|
| **rsMath** | Vectors (`rsVec`, `rsVec4`), 4×4 matrices (`rsMatrix`), quaternions (`rsQuat`), and fast trigonometry approximations. |
| **Rgbhsl** | RGB ↔ HSL color space conversion and tweening. |
| **Implicit** | Implicit surface primitives (sphere, ellipsoid, torus, knot, capsule, hexahedron, etc.) with marching-cubes support. |
| **rsText** | Bitmap font text rendering. |
| **rsWin32Saver** | Win32 screen saver framework. |
| **rsXScreenSaver** | XScreenSaver (Linux/X11) framework. |
| **rsUtility** | Miscellaneous utilities (command-line arguments, timer). |

## Building & Testing

The test suite uses [GoogleTest](https://github.com/google/googletest) fetched automatically via CMake's `FetchContent`.

```bash
# Configure (from repo root)
cmake -S tests -B build

# Build
cmake --build build

# Run tests
./build/Debug/rslibs_tests          # Windows (MSVC)
./build/rslibs_tests                # Linux / macOS
```
