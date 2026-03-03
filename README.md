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

### `rsMatrix::rotationInvert()` produces incorrect results

**File:** `rsMath/rsMatrix.cpp`, lines 527–548

The `rotationInvert()` method is intended to compute the inverse of a 3×3 rotation matrix embedded in a 4×4 matrix. However, the cofactor elements are assigned to the wrong positions — the method computes **cofactor / determinant** instead of **adjugate / determinant** (the adjugate is the *transpose* of the cofactor matrix). As a result, the output equals the original rotation matrix rather than its inverse.

For a correct inverse the row/column indices of the cofactor assignments need to be swapped. For example, the current code assigns:

```cpp
m[1] = (mat[6] * mat[8] - mat[4] * mat[10]) / det;  // cofactor(0,1)
m[4] = (mat[9] * mat[2] - mat[10] * mat[1]) / det;  // cofactor(1,0)
```

These should be transposed so that `m[1]` receives `cofactor(1,0)` and `m[4]` receives `cofactor(0,1)`.

A regression test (`rsMatrix.RotationInvert` in `tests/test_rsMath.cpp`) exercises `rotationInvert()` and should be reviewed and potentially updated once the implementation is corrected.

### `rsQuat::fromMat()` produces incorrect results for some rotations

**File:** `rsMath/rsQuat.cpp`, lines 218–270

The `fromMat()` method converts a rotation matrix back to a quaternion. The first branch (trace > 0) is correct, but the three else-branches contain assignment bugs: they use `q[i] *= 0.5f` instead of `q[i] = b * 0.5f`. With a default-constructed quaternion (x/y/z = 0), the multiplication leaves those components at zero, causing `toMat()` to hit its "no axis" early-return and produce the identity matrix.

A test (`rsQuat.FromMatRoundTrip` in `tests/test_rsMath.cpp`) verifies the correct round-trip behavior and is skipped until the implementation is corrected.
