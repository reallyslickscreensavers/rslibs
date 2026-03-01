/*
 * Tests for the Implicit library (impShape hierarchy, impCrawlPoint)
 *
 * All shapes under test are pure math — no OpenGL dependency.
 * impSurface and impCubeVolume are excluded because they require OpenGL.
 */

#include <gtest/gtest.h>
#include <cmath>
#include <cstring>

#include <rsMath/rsMath.h>

#include "impShape.h"
#include "impSphere.h"
#include "impEllipsoid.h"
#include "impTorus.h"
#include "impKnot.h"
#include "impCapsule.h"
#include "impRoundedHexahedron.h"
#include "impHexahedron.h"
#include "impCrawlPoint.h"

static constexpr float kEps = 1e-4f;

// Helper: build an identity matrix as a float[16]
static void makeIdentity(float m[16]) {
    std::memset(m, 0, 16 * sizeof(float));
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

// Helper: build a translation matrix as a float[16] (column-major)
static void makeTranslation(float m[16], float x, float y, float z) {
    makeIdentity(m);
    m[12] = x;
    m[13] = y;
    m[14] = z;
}

// Helper: build a scale matrix as a float[16] (column-major)
static void makeScaleMatrix(float m[16], float sx, float sy, float sz) {
    std::memset(m, 0, 16 * sizeof(float));
    m[0] = sx;
    m[5] = sy;
    m[10] = sz;
    m[15] = 1.0f;
}

// ===========================================================================
// impCrawlPoint tests
// ===========================================================================

TEST(impCrawlPoint, ParameterizedConstructor) {
    impCrawlPoint cp(1.0f, 2.0f, 3.0f);
    EXPECT_FLOAT_EQ(cp.position[0], 1.0f);
    EXPECT_FLOAT_EQ(cp.position[1], 2.0f);
    EXPECT_FLOAT_EQ(cp.position[2], 3.0f);
}

TEST(impCrawlPoint, ConstructFromPointer) {
    float p[3] = {4.0f, 5.0f, 6.0f};
    impCrawlPoint cp(p);
    EXPECT_FLOAT_EQ(cp.position[0], 4.0f);
    EXPECT_FLOAT_EQ(cp.position[1], 5.0f);
    EXPECT_FLOAT_EQ(cp.position[2], 6.0f);
}

TEST(impCrawlPoint, SetXYZ) {
    impCrawlPoint cp(0.0f, 0.0f, 0.0f);
    cp.set(7.0f, 8.0f, 9.0f);
    EXPECT_FLOAT_EQ(cp.position[0], 7.0f);
    EXPECT_FLOAT_EQ(cp.position[1], 8.0f);
    EXPECT_FLOAT_EQ(cp.position[2], 9.0f);
}

TEST(impCrawlPoint, SetFromPointer) {
    float p[3] = {10.0f, 11.0f, 12.0f};
    impCrawlPoint cp(0.0f, 0.0f, 0.0f);
    cp.set(p);
    EXPECT_FLOAT_EQ(cp.position[0], 10.0f);
    EXPECT_FLOAT_EQ(cp.position[1], 11.0f);
    EXPECT_FLOAT_EQ(cp.position[2], 12.0f);
}

// ===========================================================================
// impShape base class tests
// ===========================================================================

TEST(impShape, DefaultConstructor) {
    impShape shape;
    // mat should be identity
    EXPECT_FLOAT_EQ(shape.mat[0], 1.0f);
    EXPECT_FLOAT_EQ(shape.mat[5], 1.0f);
    EXPECT_FLOAT_EQ(shape.mat[10], 1.0f);
    EXPECT_FLOAT_EQ(shape.mat[15], 1.0f);
    EXPECT_FLOAT_EQ(shape.mat[1], 0.0f);
    EXPECT_FLOAT_EQ(shape.mat[4], 0.0f);
    EXPECT_FLOAT_EQ(shape.mat[12], 0.0f);
    // Default thickness
    EXPECT_FLOAT_EQ(shape.thickness, 0.1f);
    EXPECT_FLOAT_EQ(shape.thicknessSquared, 0.01f);
}

TEST(impShape, SetThickness) {
    impShape shape;
    shape.setThickness(2.0f);
    EXPECT_FLOAT_EQ(shape.getThickness(), 2.0f);
    EXPECT_FLOAT_EQ(shape.thicknessSquared, 4.0f);
}

TEST(impShape, SetPosition) {
    impShape shape;
    shape.setPosition(5.0f, 10.0f, 15.0f);
    EXPECT_FLOAT_EQ(shape.mat[12], 5.0f);
    EXPECT_FLOAT_EQ(shape.mat[13], 10.0f);
    EXPECT_FLOAT_EQ(shape.mat[14], 15.0f);
    // invmat translation should be negated
    EXPECT_FLOAT_EQ(shape.invmat[12], -5.0f);
    EXPECT_FLOAT_EQ(shape.invmat[13], -10.0f);
    EXPECT_FLOAT_EQ(shape.invmat[14], -15.0f);
}

TEST(impShape, SetPositionFromPointer) {
    impShape shape;
    float pos[3] = {1.0f, 2.0f, 3.0f};
    shape.setPosition(pos);
    EXPECT_FLOAT_EQ(shape.mat[12], 1.0f);
    EXPECT_FLOAT_EQ(shape.mat[13], 2.0f);
    EXPECT_FLOAT_EQ(shape.mat[14], 3.0f);
}

TEST(impShape, SetMatrixIdentity) {
    impShape shape;
    float id[16];
    makeIdentity(id);
    shape.setMatrix(id);
    // After identity, invmat should also be identity
    for (int i = 0; i < 16; ++i) {
        EXPECT_NEAR(shape.invmat[i], id[i], kEps) << "invmat[" << i << "]";
    }
}

TEST(impShape, SetMatrixTranslation) {
    impShape shape;
    float m[16];
    makeTranslation(m, 3.0f, 4.0f, 5.0f);
    shape.setMatrix(m);
    // mat should reflect the translation
    EXPECT_FLOAT_EQ(shape.mat[12], 3.0f);
    EXPECT_FLOAT_EQ(shape.mat[13], 4.0f);
    EXPECT_FLOAT_EQ(shape.mat[14], 5.0f);
    // invmat translation should negate
    EXPECT_NEAR(shape.invmat[12], -3.0f, kEps);
    EXPECT_NEAR(shape.invmat[13], -4.0f, kEps);
    EXPECT_NEAR(shape.invmat[14], -5.0f, kEps);
}

TEST(impShape, InvertMatrixRoundTrip) {
    impShape shape;
    // Set a non-trivial matrix (scale + translation)
    float m[16];
    makeIdentity(m);
    m[0] = 2.0f;  m[5] = 3.0f;  m[10] = 4.0f;
    m[12] = 1.0f; m[13] = 2.0f; m[14] = 3.0f;
    shape.setMatrix(m);

    // Multiply mat * invmat, expect identity
    rsMatrix original, inverse, result;
    original.set(shape.mat);
    inverse.set(shape.invmat);
    result.copy(original);
    result.postMult(inverse);

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) {
            float expected = (i == j) ? 1.0f : 0.0f;
            EXPECT_NEAR(result[i + j * 4], expected, kEps)
                << "result[" << i << "," << j << "]";
        }
}

TEST(impShape, ValueReturnsZero) {
    impShape shape;
    float pos[3] = {0.0f, 0.0f, 0.0f};
    EXPECT_FLOAT_EQ(shape.value(pos), 0.0f);
}

TEST(impShape, Center) {
    impShape shape;
    shape.setPosition(1.0f, 2.0f, 3.0f);
    float center[3] = {0.0f, 0.0f, 0.0f};
    shape.center(center);
    EXPECT_FLOAT_EQ(center[0], 1.0f);
    EXPECT_FLOAT_EQ(center[1], 2.0f);
    EXPECT_FLOAT_EQ(center[2], 3.0f);
}

TEST(impShape, AddCrawlPoint) {
    impShape shape;
    shape.setPosition(5.0f, 6.0f, 7.0f);
    impCrawlPointVector cpv;
    shape.addCrawlPoint(cpv);
    ASSERT_EQ(cpv.size(), 1u);
    EXPECT_FLOAT_EQ(cpv[0].position[0], 5.0f);
    EXPECT_FLOAT_EQ(cpv[0].position[1], 6.0f);
    EXPECT_FLOAT_EQ(cpv[0].position[2], 7.0f);
}

// ===========================================================================
// impSphere tests
// ===========================================================================

TEST(impSphere, ValueAtCenter) {
    impSphere sphere;
    sphere.setThickness(1.0f);
    // At origin the sphere is at origin by default
    float pos[3] = {0.0f, 0.0f, 0.0f};
    float val = sphere.value(pos);
    // value = thicknessSquared / (0 + 0 + 0 + IMP_MIN_DIVISOR)
    // = 1.0 / 0.0001 = 10000
    EXPECT_NEAR(val, 1.0f / IMP_MIN_DIVISOR, 1.0f);
}

TEST(impSphere, SphericalSymmetry) {
    impSphere sphere;
    sphere.setThickness(1.0f);
    float px[3] = {1.0f, 0.0f, 0.0f};
    float py[3] = {0.0f, 1.0f, 0.0f};
    float pz[3] = {0.0f, 0.0f, 1.0f};
    float vx = sphere.value(px);
    float vy = sphere.value(py);
    float vz = sphere.value(pz);
    EXPECT_NEAR(vx, vy, kEps);
    EXPECT_NEAR(vy, vz, kEps);
}

TEST(impSphere, ValueDecaysWithDistance) {
    impSphere sphere;
    sphere.setThickness(1.0f);
    float near_pos[3] = {0.5f, 0.0f, 0.0f};
    float far_pos[3]  = {2.0f, 0.0f, 0.0f};
    EXPECT_GT(sphere.value(near_pos), sphere.value(far_pos));
}

TEST(impSphere, ValueAlwaysPositive) {
    impSphere sphere;
    sphere.setThickness(0.5f);
    float pos[3] = {10.0f, -5.0f, 3.0f};
    EXPECT_GT(sphere.value(pos), 0.0f);
}

TEST(impSphere, TranslatedSphere) {
    impSphere sphere;
    sphere.setThickness(1.0f);
    sphere.setPosition(5.0f, 0.0f, 0.0f);
    float at_center[3] = {5.0f, 0.0f, 0.0f};
    float at_origin[3] = {0.0f, 0.0f, 0.0f};
    float val_center = sphere.value(at_center);
    float val_origin = sphere.value(at_origin);
    // Value at the translated center should be much larger
    EXPECT_GT(val_center, val_origin * 10.0f);
}

// ===========================================================================
// impEllipsoid tests
// ===========================================================================

TEST(impEllipsoid, ValueAtCenterWithIdentity) {
    impEllipsoid ellipsoid;
    float m[16];
    makeIdentity(m);
    ellipsoid.setMatrix(m);
    ellipsoid.setThickness(1.0f);
    float pos[3] = {0.0f, 0.0f, 0.0f};
    float val = ellipsoid.value(pos);
    // Should be thicknessSquared / IMP_MIN_DIVISOR
    EXPECT_NEAR(val, 1.0f / IMP_MIN_DIVISOR, 1.0f);
}

TEST(impEllipsoid, SymmetryOnAxes) {
    impEllipsoid ellipsoid;
    float m[16];
    makeIdentity(m);
    ellipsoid.setMatrix(m);
    ellipsoid.setThickness(1.0f);
    float px[3] = {1.0f, 0.0f, 0.0f};
    float nx[3] = {-1.0f, 0.0f, 0.0f};
    EXPECT_NEAR(ellipsoid.value(px), ellipsoid.value(nx), kEps);
}

TEST(impEllipsoid, NonUniformScale) {
    impEllipsoid ellipsoid;
    ellipsoid.setThickness(1.0f);
    // Scale x by 2 — points on x-axis should map to half in local space
    float m[16];
    makeScaleMatrix(m, 2.0f, 1.0f, 1.0f);
    ellipsoid.setMatrix(m);
    // Test that value at (2,0,0) != value at (0,2,0) due to non-uniform scale
    float px[3] = {2.0f, 0.0f, 0.0f};
    float py[3] = {0.0f, 2.0f, 0.0f};
    float vx = ellipsoid.value(px);
    float vy = ellipsoid.value(py);
    // (2,0,0) maps to (1,0,0) in local space, (0,2,0) maps to (0,2,0)
    EXPECT_GT(vx, vy);
}

// ===========================================================================
// impTorus tests
// ===========================================================================

TEST(impTorus, GetSetRadius) {
    impTorus torus;
    EXPECT_FLOAT_EQ(torus.getRadius(), 1.0f);  // default
    torus.setRadius(2.5f);
    EXPECT_FLOAT_EQ(torus.getRadius(), 2.5f);
}

TEST(impTorus, ValueOnRing) {
    impTorus torus;
    float m[16];
    makeIdentity(m);
    torus.setMatrix(m);
    torus.setThickness(0.5f);
    torus.setRadius(2.0f);
    // A point on the ring: (2, 0, 0) is on the ring of radius 2 in XY
    float on_ring[3] = {2.0f, 0.0f, 0.0f};
    float off_ring[3] = {0.0f, 0.0f, 2.0f};
    float val_on = torus.value(on_ring);
    float val_off = torus.value(off_ring);
    // Value on the ring should be large (near center of tube)
    EXPECT_GT(val_on, val_off);
}

TEST(impTorus, ValueDecaysAwayFromRing) {
    impTorus torus;
    float m[16];
    makeIdentity(m);
    torus.setMatrix(m);
    torus.setThickness(0.5f);
    torus.setRadius(2.0f);
    // Slightly off the ring
    float near_ring[3] = {2.1f, 0.0f, 0.0f};
    float far_ring[3] = {5.0f, 0.0f, 0.0f};
    EXPECT_GT(torus.value(near_ring), torus.value(far_ring));
}

TEST(impTorus, ValueAlwaysPositive) {
    impTorus torus;
    float m[16];
    makeIdentity(m);
    torus.setMatrix(m);
    torus.setThickness(0.5f);
    float pos[3] = {10.0f, -5.0f, 3.0f};
    EXPECT_GT(torus.value(pos), 0.0f);
}

TEST(impTorus, CenterOnRing) {
    impTorus torus;
    float m[16];
    makeIdentity(m);
    torus.setMatrix(m);
    torus.setRadius(3.0f);
    float center[3];
    torus.center(center);
    // center should be at (radius, 0, 0) for identity matrix
    // mat[0]*radius+mat[12] = 1*3+0 = 3
    EXPECT_NEAR(center[0], 3.0f, kEps);
    EXPECT_NEAR(center[1], 0.0f, kEps);
    EXPECT_NEAR(center[2], 0.0f, kEps);
}

TEST(impTorus, AddCrawlPointOnRing) {
    impTorus torus;
    float m[16];
    makeIdentity(m);
    torus.setMatrix(m);
    torus.setRadius(2.0f);
    impCrawlPointVector cpv;
    torus.addCrawlPoint(cpv);
    ASSERT_EQ(cpv.size(), 1u);
    EXPECT_NEAR(cpv[0].position[0], 2.0f, kEps);
    EXPECT_NEAR(cpv[0].position[1], 0.0f, kEps);
    EXPECT_NEAR(cpv[0].position[2], 0.0f, kEps);
}

// ===========================================================================
// impKnot tests
// ===========================================================================

TEST(impKnot, GetSetProperties) {
    impKnot knot;
    // Defaults
    EXPECT_FLOAT_EQ(knot.getRadius1(), 1.0f);
    EXPECT_FLOAT_EQ(knot.getRadius2(), 0.5f);
    EXPECT_EQ(knot.getNumCoils(), 3);
    EXPECT_EQ(knot.getNumTwists(), 2);

    knot.setRadius1(2.0f);
    knot.setRadius2(0.8f);
    knot.setNumCoils(4);
    knot.setNumTwists(5);
    EXPECT_FLOAT_EQ(knot.getRadius1(), 2.0f);
    EXPECT_FLOAT_EQ(knot.getRadius2(), 0.8f);
    EXPECT_EQ(knot.getNumCoils(), 4);
    EXPECT_EQ(knot.getNumTwists(), 5);
}

TEST(impKnot, NumCoilsMinimum) {
    impKnot knot;
    knot.setNumCoils(0);
    EXPECT_EQ(knot.getNumCoils(), 1);  // clamped to 1
    knot.setNumCoils(-5);
    EXPECT_EQ(knot.getNumCoils(), 1);
}

TEST(impKnot, ValueAtOrigin) {
    impKnot knot;
    float m[16];
    makeIdentity(m);
    knot.setMatrix(m);
    knot.setThickness(0.5f);
    float pos[3] = {0.0f, 0.0f, 0.0f};
    float val = knot.value(pos);
    // The knot wraps around — at origin it should be nonzero and positive
    EXPECT_GT(val, 0.0f);
}

TEST(impKnot, ValueAlwaysPositive) {
    impKnot knot;
    float m[16];
    makeIdentity(m);
    knot.setMatrix(m);
    knot.setThickness(0.5f);
    float pos[3] = {5.0f, -3.0f, 2.0f};
    EXPECT_GT(knot.value(pos), 0.0f);
}

TEST(impKnot, CenterPoint) {
    impKnot knot;
    float m[16];
    makeIdentity(m);
    knot.setMatrix(m);
    knot.setRadius1(1.0f);
    knot.setRadius2(0.5f);
    float center[3];
    knot.center(center);
    // center = mat[0]*(r1+r2) + mat[12] = 1*1.5 + 0 = 1.5
    EXPECT_NEAR(center[0], 1.5f, kEps);
    EXPECT_NEAR(center[1], 0.0f, kEps);
    EXPECT_NEAR(center[2], 0.0f, kEps);
}

TEST(impKnot, AddCrawlPointsMatchesCoilCount) {
    impKnot knot;
    float m[16];
    makeIdentity(m);
    knot.setMatrix(m);
    knot.setNumCoils(5);
    impCrawlPointVector cpv;
    knot.addCrawlPoint(cpv);
    EXPECT_EQ(cpv.size(), 5u);
}

// ===========================================================================
// impCapsule tests
// ===========================================================================

TEST(impCapsule, ValueAtOrigin) {
    impCapsule capsule;
    float m[16];
    makeIdentity(m);
    capsule.setMatrix(m);
    capsule.setThickness(1.0f);
    capsule.setLength(2.0f);
    float pos[3] = {0.0f, 0.0f, 0.0f};
    float val = capsule.value(pos);
    // At origin, tz = 0, |tz|-length = -2.0, clamped to 0 → sz = 0
    // value = 1.0 / (0 + 0 + 0 + IMP_MIN_DIVISOR)
    EXPECT_NEAR(val, 1.0f / IMP_MIN_DIVISOR, 1.0f);
}

TEST(impCapsule, ValueAtEndpoint) {
    impCapsule capsule;
    float m[16];
    makeIdentity(m);
    capsule.setMatrix(m);
    capsule.setThickness(1.0f);
    capsule.setLength(2.0f);
    // Z = 2.0 is the endpoint — |tz|-length = 0, clamped to 0
    float pos[3] = {0.0f, 0.0f, 2.0f};
    float val = capsule.value(pos);
    EXPECT_NEAR(val, 1.0f / IMP_MIN_DIVISOR, 1.0f);
}

TEST(impCapsule, ValueAtMidpoint) {
    impCapsule capsule;
    float m[16];
    makeIdentity(m);
    capsule.setMatrix(m);
    capsule.setThickness(1.0f);
    capsule.setLength(2.0f);
    // Z = 1.0 is inside the segment — |tz|-length = -1.0, clamped to 0
    float pos[3] = {0.0f, 0.0f, 1.0f};
    float val = capsule.value(pos);
    EXPECT_NEAR(val, 1.0f / IMP_MIN_DIVISOR, 1.0f);
}

TEST(impCapsule, ValueDecaysRadially) {
    impCapsule capsule;
    float m[16];
    makeIdentity(m);
    capsule.setMatrix(m);
    capsule.setThickness(1.0f);
    capsule.setLength(2.0f);
    float near_pos[3] = {0.5f, 0.0f, 0.0f};
    float far_pos[3]  = {3.0f, 0.0f, 0.0f};
    EXPECT_GT(capsule.value(near_pos), capsule.value(far_pos));
}

TEST(impCapsule, ValueBeyondEndpoint) {
    impCapsule capsule;
    float m[16];
    makeIdentity(m);
    capsule.setMatrix(m);
    capsule.setThickness(1.0f);
    capsule.setLength(2.0f);
    // Z = 5.0 is well beyond the endpoint — value should be smaller
    float at_center[3] = {0.0f, 0.0f, 0.0f};
    float beyond[3] = {0.0f, 0.0f, 5.0f};
    EXPECT_GT(capsule.value(at_center), capsule.value(beyond));
}

// ===========================================================================
// impRoundedHexahedron tests
// ===========================================================================

TEST(impRoundedHexahedron, ValueAtCenter) {
    impRoundedHexahedron hex;
    float m[16];
    makeIdentity(m);
    hex.setMatrix(m);
    hex.setThickness(1.0f);
    hex.setSize(1.0f, 1.0f, 1.0f);
    float pos[3] = {0.0f, 0.0f, 0.0f};
    float val = hex.value(pos);
    // At center: |tx| - width < 0 → clamped to 0, same for y, z
    EXPECT_NEAR(val, 1.0f / IMP_MIN_DIVISOR, 1.0f);
}

TEST(impRoundedHexahedron, ValueOnFaceCenter) {
    impRoundedHexahedron hex;
    float m[16];
    makeIdentity(m);
    hex.setMatrix(m);
    hex.setThickness(1.0f);
    hex.setSize(2.0f, 2.0f, 2.0f);
    // Right at the face boundary: x = 2.0, y = 0, z = 0
    // sx = |2|-2 = 0, sy = 0, sz = 0 → still max value
    float face[3] = {2.0f, 0.0f, 0.0f};
    float val = hex.value(face);
    EXPECT_NEAR(val, 1.0f / IMP_MIN_DIVISOR, 1.0f);
}

TEST(impRoundedHexahedron, ValueAtCornerLessThanFace) {
    impRoundedHexahedron hex;
    float m[16];
    makeIdentity(m);
    hex.setMatrix(m);
    hex.setThickness(1.0f);
    hex.setSize(1.0f, 1.0f, 1.0f);
    // Beyond the corner: all axes contribute
    float corner[3] = {2.0f, 2.0f, 2.0f};
    float face[3] = {2.0f, 0.0f, 0.0f};
    EXPECT_GT(hex.value(face), hex.value(corner));
}

TEST(impRoundedHexahedron, ValueDecaysFromCenter) {
    impRoundedHexahedron hex;
    float m[16];
    makeIdentity(m);
    hex.setMatrix(m);
    hex.setThickness(1.0f);
    hex.setSize(1.0f, 1.0f, 1.0f);
    float near_pos[3] = {0.5f, 0.0f, 0.0f};  // inside the box
    float far_pos[3] = {5.0f, 0.0f, 0.0f};   // far outside
    EXPECT_GT(hex.value(near_pos), hex.value(far_pos));
}

// ===========================================================================
// impHexahedron tests
// ===========================================================================

TEST(impHexahedron, ValueAtOriginIsLarge) {
    impHexahedron hex;
    float m[16];
    makeIdentity(m);
    hex.setMatrix(m);
    float pos[3] = {0.0f, 0.0f, 0.0f};
    float val = hex.value(pos);
    // At origin: 1/(0+eps) for all axes, min of three very large values
    EXPECT_GT(val, 1000.0f);
}

TEST(impHexahedron, ValueOnAxes) {
    impHexahedron hex;
    float m[16];
    makeIdentity(m);
    hex.setMatrix(m);
    // At (1,0,0): xx = 1/(1+eps) ≈ 1, yy = 1/eps, zz = 1/eps → min = xx ≈ 1
    float px[3] = {1.0f, 0.0f, 0.0f};
    float val = hex.value(px);
    EXPECT_NEAR(val, 1.0f / (1.0f + IMP_MIN_DIVISOR), 0.01f);
}

TEST(impHexahedron, SymmetryXY) {
    impHexahedron hex;
    float m[16];
    makeIdentity(m);
    hex.setMatrix(m);
    float px[3] = {0.5f, 0.0f, 0.0f};
    float py[3] = {0.0f, 0.5f, 0.0f};
    float pz[3] = {0.0f, 0.0f, 0.5f};
    // With identity matrix, all axis directions should give same value
    float vx = hex.value(px);
    float vy = hex.value(py);
    float vz = hex.value(pz);
    EXPECT_NEAR(vx, vy, kEps);
    EXPECT_NEAR(vy, vz, kEps);
}

TEST(impHexahedron, ValueDecaysFromCenter) {
    impHexahedron hex;
    float m[16];
    makeIdentity(m);
    hex.setMatrix(m);
    float near_pos[3] = {0.1f, 0.0f, 0.0f};
    float far_pos[3] = {3.0f, 0.0f, 0.0f};
    EXPECT_GT(hex.value(near_pos), hex.value(far_pos));
}

TEST(impHexahedron, ValueAlwaysPositive) {
    impHexahedron hex;
    float m[16];
    makeIdentity(m);
    hex.setMatrix(m);
    float pos[3] = {10.0f, -5.0f, 3.0f};
    EXPECT_GT(hex.value(pos), 0.0f);
}
