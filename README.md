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

## Known Bugs

### `rsQuat::fromMat()` produces incorrect results for some rotations

**File:** `rsMath/rsQuat.cpp`, lines 218–270

The `fromMat()` method converts a rotation matrix back to a quaternion. The first branch (trace > 0) is correct, but the three else-branches contain assignment bugs: they use `q[i] *= 0.5f` instead of `q[i] = b * 0.5f`. With a default-constructed quaternion (x/y/z = 0), the multiplication leaves those components at zero, causing `toMat()` to hit its "no axis" early-return and produce the identity matrix.

A test (`rsQuat.FromMatRoundTrip` in `tests/test_rsMath.cpp`) verifies the correct round-trip behavior and is skipped until the implementation is corrected.
