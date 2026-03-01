/*
 * Smoke tests for rsMath library (rsVec, rsVec4, rsMatrix, rsQuat)
 */

#include <gtest/gtest.h>
#include <cmath>
#include <rsMath/rsMath.h>

static constexpr float kEps = 1e-5f;

// ---------------------------------------------------------------------------
// rsVec tests
// ---------------------------------------------------------------------------

TEST(rsVec, DefaultConstructor) {
    rsVec v;
    // Default constructor does not zero-initialize; verify usability via set()
    v.set(1.0f, 2.0f, 3.0f);
    EXPECT_FLOAT_EQ(v[0], 1.0f);
    EXPECT_FLOAT_EQ(v[1], 2.0f);
    EXPECT_FLOAT_EQ(v[2], 3.0f);
}

TEST(rsVec, ParameterizedConstructor) {
    rsVec v(1.0f, 2.0f, 3.0f);
    EXPECT_FLOAT_EQ(v[0], 1.0f);
    EXPECT_FLOAT_EQ(v[1], 2.0f);
    EXPECT_FLOAT_EQ(v[2], 3.0f);
}

TEST(rsVec, Set) {
    rsVec v;
    v.set(4.0f, 5.0f, 6.0f);
    EXPECT_FLOAT_EQ(v[0], 4.0f);
    EXPECT_FLOAT_EQ(v[1], 5.0f);
    EXPECT_FLOAT_EQ(v[2], 6.0f);
}

TEST(rsVec, Length) {
    rsVec v(3.0f, 4.0f, 0.0f);
    EXPECT_NEAR(v.length(), 5.0f, kEps);
}

TEST(rsVec, Normalize) {
    rsVec v(0.0f, 3.0f, 4.0f);
    float len = v.normalize();
    EXPECT_NEAR(len, 5.0f, kEps);
    EXPECT_NEAR(v.length(), 1.0f, kEps);
}

TEST(rsVec, Dot) {
    rsVec a(1.0f, 0.0f, 0.0f);
    rsVec b(0.0f, 1.0f, 0.0f);
    EXPECT_NEAR(a.dot(b), 0.0f, kEps);

    rsVec c(1.0f, 2.0f, 3.0f);
    rsVec d(4.0f, 5.0f, 6.0f);
    EXPECT_NEAR(c.dot(d), 32.0f, kEps);
}

TEST(rsVec, Cross) {
    rsVec a(1.0f, 0.0f, 0.0f);
    rsVec b(0.0f, 1.0f, 0.0f);
    rsVec result;
    result.cross(a, b);
    EXPECT_NEAR(result[0], 0.0f, kEps);
    EXPECT_NEAR(result[1], 0.0f, kEps);
    EXPECT_NEAR(result[2], 1.0f, kEps);
}

TEST(rsVec, Scale) {
    rsVec v(1.0f, 2.0f, 3.0f);
    v.scale(2.0f);
    EXPECT_FLOAT_EQ(v[0], 2.0f);
    EXPECT_FLOAT_EQ(v[1], 4.0f);
    EXPECT_FLOAT_EQ(v[2], 6.0f);
}

TEST(rsVec, OperatorAdd) {
    rsVec a(1.0f, 2.0f, 3.0f);
    rsVec b(4.0f, 5.0f, 6.0f);
    rsVec c = a + b;
    EXPECT_FLOAT_EQ(c[0], 5.0f);
    EXPECT_FLOAT_EQ(c[1], 7.0f);
    EXPECT_FLOAT_EQ(c[2], 9.0f);
}

TEST(rsVec, OperatorSub) {
    rsVec a(4.0f, 5.0f, 6.0f);
    rsVec b(1.0f, 2.0f, 3.0f);
    rsVec c = a - b;
    EXPECT_FLOAT_EQ(c[0], 3.0f);
    EXPECT_FLOAT_EQ(c[1], 3.0f);
    EXPECT_FLOAT_EQ(c[2], 3.0f);
}

TEST(rsVec, OperatorMul) {
    rsVec v(1.0f, 2.0f, 3.0f);
    rsVec r = v * 3.0f;
    EXPECT_FLOAT_EQ(r[0], 3.0f);
    EXPECT_FLOAT_EQ(r[1], 6.0f);
    EXPECT_FLOAT_EQ(r[2], 9.0f);
}

TEST(rsVec, OperatorDiv) {
    rsVec v(2.0f, 4.0f, 6.0f);
    rsVec r = v / 2.0f;
    EXPECT_FLOAT_EQ(r[0], 1.0f);
    EXPECT_FLOAT_EQ(r[1], 2.0f);
    EXPECT_FLOAT_EQ(r[2], 3.0f);
}

TEST(rsVec, OperatorPlusEqual) {
    rsVec a(1.0f, 2.0f, 3.0f);
    rsVec b(4.0f, 5.0f, 6.0f);
    a += b;
    EXPECT_FLOAT_EQ(a[0], 5.0f);
    EXPECT_FLOAT_EQ(a[1], 7.0f);
    EXPECT_FLOAT_EQ(a[2], 9.0f);
}

TEST(rsVec, AlmostEqual) {
    rsVec a(1.0f, 2.0f, 3.0f);
    rsVec b(1.0f, 2.0f, 3.0000001f);
    EXPECT_TRUE(a.almostEqual(b, 0.001f));
    EXPECT_FALSE(a.almostEqual(rsVec(0.0f, 0.0f, 0.0f), 0.001f));
}

// ---------------------------------------------------------------------------
// rsVec4 tests
// ---------------------------------------------------------------------------

TEST(rsVec4, DefaultConstructor) {
    rsVec4 v;
    // Default constructor does not zero-initialize; verify usability via set()
    v.set(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_FLOAT_EQ(v[0], 1.0f);
    EXPECT_FLOAT_EQ(v[1], 2.0f);
    EXPECT_FLOAT_EQ(v[2], 3.0f);
    EXPECT_FLOAT_EQ(v[3], 4.0f);
}

TEST(rsVec4, ParameterizedConstructor) {
    rsVec4 v(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_FLOAT_EQ(v[0], 1.0f);
    EXPECT_FLOAT_EQ(v[1], 2.0f);
    EXPECT_FLOAT_EQ(v[2], 3.0f);
    EXPECT_FLOAT_EQ(v[3], 4.0f);
}

TEST(rsVec4, Length) {
    rsVec4 v(1.0f, 2.0f, 2.0f, 0.0f);
    EXPECT_NEAR(v.length(), 3.0f, kEps);
}

TEST(rsVec4, Normalize) {
    rsVec4 v(0.0f, 0.0f, 3.0f, 4.0f);
    float len = v.normalize();
    EXPECT_NEAR(len, 5.0f, kEps);
    EXPECT_NEAR(v.length(), 1.0f, kEps);
}

TEST(rsVec4, Dot) {
    rsVec4 a(1.0f, 2.0f, 3.0f, 4.0f);
    rsVec4 b(5.0f, 6.0f, 7.0f, 8.0f);
    // 5 + 12 + 21 + 32 = 70
    EXPECT_NEAR(a.dot(b), 70.0f, kEps);
}

TEST(rsVec4, OperatorAdd) {
    rsVec4 a(1.0f, 2.0f, 3.0f, 4.0f);
    rsVec4 b(5.0f, 6.0f, 7.0f, 8.0f);
    rsVec4 c = a + b;
    EXPECT_FLOAT_EQ(c[0], 6.0f);
    EXPECT_FLOAT_EQ(c[1], 8.0f);
    EXPECT_FLOAT_EQ(c[2], 10.0f);
    EXPECT_FLOAT_EQ(c[3], 12.0f);
}

// ---------------------------------------------------------------------------
// rsMatrix tests
// ---------------------------------------------------------------------------

TEST(rsMatrix, Identity) {
    rsMatrix m;
    m.identity();
    // Diagonal should be 1
    EXPECT_FLOAT_EQ(m[0], 1.0f);
    EXPECT_FLOAT_EQ(m[5], 1.0f);
    EXPECT_FLOAT_EQ(m[10], 1.0f);
    EXPECT_FLOAT_EQ(m[15], 1.0f);
    // Off-diagonal should be 0
    EXPECT_FLOAT_EQ(m[1], 0.0f);
    EXPECT_FLOAT_EQ(m[4], 0.0f);
    EXPECT_FLOAT_EQ(m[12], 0.0f);
}

TEST(rsMatrix, MakeTranslate) {
    rsMatrix m;
    m.makeTranslate(1.0f, 2.0f, 3.0f);
    // OpenGL column-major: translation in m[12], m[13], m[14]
    EXPECT_FLOAT_EQ(m[12], 1.0f);
    EXPECT_FLOAT_EQ(m[13], 2.0f);
    EXPECT_FLOAT_EQ(m[14], 3.0f);
    // Diagonal should be 1
    EXPECT_FLOAT_EQ(m[0], 1.0f);
    EXPECT_FLOAT_EQ(m[5], 1.0f);
    EXPECT_FLOAT_EQ(m[10], 1.0f);
    EXPECT_FLOAT_EQ(m[15], 1.0f);
}

TEST(rsMatrix, MakeScale) {
    rsMatrix m;
    m.makeScale(2.0f, 3.0f, 4.0f);
    EXPECT_FLOAT_EQ(m[0], 2.0f);
    EXPECT_FLOAT_EQ(m[5], 3.0f);
    EXPECT_FLOAT_EQ(m[10], 4.0f);
    EXPECT_FLOAT_EQ(m[15], 1.0f);
}

TEST(rsMatrix, MakeRotateZ90) {
    // Rotate 90 degrees (PI/2) around Z axis
    rsMatrix m;
    m.makeRotate(RS_PIo2, 0.0f, 0.0f, 1.0f);
    // Point (1,0,0) should become (0,1,0)
    rsVec v(1.0f, 0.0f, 0.0f);
    v.transPoint(m);
    EXPECT_NEAR(v[0], 0.0f, kEps);
    EXPECT_NEAR(v[1], 1.0f, kEps);
    EXPECT_NEAR(v[2], 0.0f, kEps);
}

TEST(rsMatrix, Invert) {
    // Create a translation matrix and invert it
    rsMatrix m;
    m.makeTranslate(5.0f, 10.0f, 15.0f);
    rsMatrix inv;
    bool ok = inv.invert(m);
    EXPECT_TRUE(ok);
    // Multiplying m * inv should give identity
    rsMatrix result;
    result.copy(m);
    result.postMult(inv);
    EXPECT_NEAR(result[12], 0.0f, kEps);
    EXPECT_NEAR(result[13], 0.0f, kEps);
    EXPECT_NEAR(result[14], 0.0f, kEps);
    EXPECT_NEAR(result[0], 1.0f, kEps);
    EXPECT_NEAR(result[5], 1.0f, kEps);
    EXPECT_NEAR(result[10], 1.0f, kEps);
}

// ---------------------------------------------------------------------------
// rsQuat tests
// ---------------------------------------------------------------------------

TEST(rsQuat, DefaultConstructor) {
    rsQuat q;
    EXPECT_FLOAT_EQ(q[0], 0.0f);
    EXPECT_FLOAT_EQ(q[1], 0.0f);
    EXPECT_FLOAT_EQ(q[2], 0.0f);
    EXPECT_FLOAT_EQ(q[3], 1.0f);
}

TEST(rsQuat, MakeFromAngleAxis) {
    rsQuat q;
    // 90 degrees around Z axis
    q.make(RS_PIo2, 0.0f, 0.0f, 1.0f);
    // Convert to matrix and verify rotation
    float mat[16];
    q.toMat(mat);
    // Apply to (1,0,0): column-major mat * vec
    float x = mat[0] * 1.0f + mat[4] * 0.0f + mat[8] * 0.0f;
    float y = mat[1] * 1.0f + mat[5] * 0.0f + mat[9] * 0.0f;
    EXPECT_NEAR(x, 0.0f, kEps);
    EXPECT_NEAR(y, 1.0f, kEps);
}

TEST(rsQuat, Slerp) {
    rsQuat a, b, result;
    // Identity quaternion (no rotation)
    a.make(0.0f, 0.0f, 0.0f, 1.0f);
    // 90 degrees around Z
    b.make(RS_PIo2, 0.0f, 0.0f, 1.0f);
    // Halfway interpolation should give 45 degrees
    result.slerp(a, b, 0.5f);
    float mat[16];
    result.toMat(mat);
    // Rotate (1,0,0) by 45 degrees -> (cos45, sin45, 0)
    float x = mat[0] * 1.0f + mat[4] * 0.0f + mat[8] * 0.0f;
    float y = mat[1] * 1.0f + mat[5] * 0.0f + mat[9] * 0.0f;
    float expected = sqrtf(2.0f) / 2.0f;
    EXPECT_NEAR(x, expected, 0.01f);
    EXPECT_NEAR(y, expected, 0.01f);
}

// ---------------------------------------------------------------------------
// rsMath utility functions
// ---------------------------------------------------------------------------

TEST(rsMathUtils, SqrtFloat) {
    EXPECT_NEAR(rsSqrtf(4.0f), 2.0f, kEps);
    EXPECT_NEAR(rsSqrtf(9.0f), 3.0f, kEps);
    EXPECT_NEAR(rsSqrtf(0.0f), 0.0f, kEps);
}

TEST(rsMathUtils, InvSqrtFloat) {
    EXPECT_NEAR(rsInvSqrtf(4.0f), 0.5f, 0.001f);
    EXPECT_NEAR(rsInvSqrtf(1.0f), 1.0f, 0.001f);
}
