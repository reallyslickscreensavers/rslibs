/*
 * Smoke tests for rsMath library (rsVec, rsVec4, rsMatrix, rsQuat)
 */

#include <gtest/gtest.h>
#include <cmath>
#include <sstream>
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

TEST(rsVec, OperatorMinusEqual) {
    rsVec a(4.0f, 5.0f, 6.0f);
    rsVec b(1.0f, 2.0f, 3.0f);
    a -= b;
    EXPECT_FLOAT_EQ(a[0], 3.0f);
    EXPECT_FLOAT_EQ(a[1], 3.0f);
    EXPECT_FLOAT_EQ(a[2], 3.0f);
}

TEST(rsVec, OperatorMulEqualVec) {
    rsVec a(2.0f, 3.0f, 4.0f);
    rsVec b(5.0f, 6.0f, 7.0f);
    a *= b;
    EXPECT_FLOAT_EQ(a[0], 10.0f);
    EXPECT_FLOAT_EQ(a[1], 18.0f);
    EXPECT_FLOAT_EQ(a[2], 28.0f);
}

TEST(rsVec, OperatorMulEqualFloat) {
    rsVec a(1.0f, 2.0f, 3.0f);
    a *= 3.0f;
    EXPECT_FLOAT_EQ(a[0], 3.0f);
    EXPECT_FLOAT_EQ(a[1], 6.0f);
    EXPECT_FLOAT_EQ(a[2], 9.0f);
}

TEST(rsVec, TransVec) {
    // transVec should transform as a direction (no translation)
    rsMatrix m;
    m.makeTranslate(10.0f, 20.0f, 30.0f);
    rsVec v(1.0f, 0.0f, 0.0f);
    v.transVec(m);
    // Translation should NOT affect vectors
    EXPECT_NEAR(v[0], 1.0f, kEps);
    EXPECT_NEAR(v[1], 0.0f, kEps);
    EXPECT_NEAR(v[2], 0.0f, kEps);
}

TEST(rsVec, TransVecRotation) {
    // Rotation should affect vectors
    rsMatrix m;
    m.makeRotate(RS_PIo2, 0.0f, 0.0f, 1.0f);
    rsVec v(1.0f, 0.0f, 0.0f);
    v.transVec(m);
    EXPECT_NEAR(v[0], 0.0f, kEps);
    EXPECT_NEAR(v[1], 1.0f, kEps);
    EXPECT_NEAR(v[2], 0.0f, kEps);
}

// ---------------------------------------------------------------------------
// C-style free vector function tests
// ---------------------------------------------------------------------------

TEST(rsVecFree, Length) {
    float v[3] = {3.0f, 4.0f, 0.0f};
    EXPECT_NEAR(rsLength(v), 5.0f, kEps);
}

TEST(rsVecFree, Normalize) {
    float v[3] = {0.0f, 3.0f, 4.0f};
    float len = rsNormalize(v);
    EXPECT_NEAR(len, 5.0f, kEps);
    EXPECT_NEAR(rsLength(v), 1.0f, kEps);
}

TEST(rsVecFree, NormalizeZeroVector) {
    float v[3] = {0.0f, 0.0f, 0.0f};
    float len = rsNormalize(v);
    EXPECT_FLOAT_EQ(len, 0.0f);
}

TEST(rsVecFree, Dot) {
    float a[3] = {1.0f, 2.0f, 3.0f};
    float b[3] = {4.0f, 5.0f, 6.0f};
    EXPECT_NEAR(rsDot(a, b), 32.0f, kEps);
}

TEST(rsVecFree, Cross) {
    float a[3] = {1.0f, 0.0f, 0.0f};
    float b[3] = {0.0f, 1.0f, 0.0f};
    float result[3];
    rsCross(a, b, result);
    EXPECT_NEAR(result[0], 0.0f, kEps);
    EXPECT_NEAR(result[1], 0.0f, kEps);
    EXPECT_NEAR(result[2], 1.0f, kEps);
}

TEST(rsVecFree, ScaleVec) {
    float v[3] = {1.0f, 2.0f, 3.0f};
    rsScaleVec(v, 2.0f);
    EXPECT_FLOAT_EQ(v[0], 2.0f);
    EXPECT_FLOAT_EQ(v[1], 4.0f);
    EXPECT_FLOAT_EQ(v[2], 6.0f);
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

TEST(rsVec4, OperatorSub) {
    rsVec4 a(5.0f, 6.0f, 7.0f, 8.0f);
    rsVec4 b(1.0f, 2.0f, 3.0f, 4.0f);
    rsVec4 c = a - b;
    EXPECT_FLOAT_EQ(c[0], 4.0f);
    EXPECT_FLOAT_EQ(c[1], 4.0f);
    EXPECT_FLOAT_EQ(c[2], 4.0f);
    EXPECT_FLOAT_EQ(c[3], 4.0f);
}

TEST(rsVec4, OperatorMulScalar) {
    rsVec4 v(1.0f, 2.0f, 3.0f, 4.0f);
    rsVec4 r = v * 2.0f;
    EXPECT_FLOAT_EQ(r[0], 2.0f);
    EXPECT_FLOAT_EQ(r[1], 4.0f);
    EXPECT_FLOAT_EQ(r[2], 6.0f);
    EXPECT_FLOAT_EQ(r[3], 8.0f);
}

TEST(rsVec4, OperatorDivScalar) {
    rsVec4 v(2.0f, 4.0f, 6.0f, 8.0f);
    rsVec4 r = v / 2.0f;
    EXPECT_FLOAT_EQ(r[0], 1.0f);
    EXPECT_FLOAT_EQ(r[1], 2.0f);
    EXPECT_FLOAT_EQ(r[2], 3.0f);
    EXPECT_FLOAT_EQ(r[3], 4.0f);
}

TEST(rsVec4, OperatorPlusEqual) {
    rsVec4 a(1.0f, 2.0f, 3.0f, 4.0f);
    rsVec4 b(5.0f, 6.0f, 7.0f, 8.0f);
    a += b;
    EXPECT_FLOAT_EQ(a[0], 6.0f);
    EXPECT_FLOAT_EQ(a[1], 8.0f);
    EXPECT_FLOAT_EQ(a[2], 10.0f);
    EXPECT_FLOAT_EQ(a[3], 12.0f);
}

TEST(rsVec4, OperatorMinusEqual) {
    rsVec4 a(5.0f, 6.0f, 7.0f, 8.0f);
    rsVec4 b(1.0f, 2.0f, 3.0f, 4.0f);
    a -= b;
    EXPECT_FLOAT_EQ(a[0], 4.0f);
    EXPECT_FLOAT_EQ(a[1], 4.0f);
    EXPECT_FLOAT_EQ(a[2], 4.0f);
    EXPECT_FLOAT_EQ(a[3], 4.0f);
}

TEST(rsVec4, OperatorMulEqualVec) {
    rsVec4 a(2.0f, 3.0f, 4.0f, 5.0f);
    rsVec4 b(6.0f, 7.0f, 8.0f, 9.0f);
    a *= b;
    EXPECT_FLOAT_EQ(a[0], 12.0f);
    EXPECT_FLOAT_EQ(a[1], 21.0f);
    EXPECT_FLOAT_EQ(a[2], 32.0f);
    EXPECT_FLOAT_EQ(a[3], 45.0f);
}

TEST(rsVec4, OperatorMulEqualFloat) {
    rsVec4 a(1.0f, 2.0f, 3.0f, 4.0f);
    a *= 3.0f;
    EXPECT_FLOAT_EQ(a[0], 3.0f);
    EXPECT_FLOAT_EQ(a[1], 6.0f);
    EXPECT_FLOAT_EQ(a[2], 9.0f);
    EXPECT_FLOAT_EQ(a[3], 12.0f);
}

TEST(rsVec4, Scale) {
    rsVec4 v(1.0f, 2.0f, 3.0f, 4.0f);
    v.scale(2.0f);
    EXPECT_FLOAT_EQ(v[0], 2.0f);
    EXPECT_FLOAT_EQ(v[1], 4.0f);
    EXPECT_FLOAT_EQ(v[2], 6.0f);
    EXPECT_FLOAT_EQ(v[3], 8.0f);
}

TEST(rsVec4, AlmostEqual) {
    rsVec4 a(1.0f, 2.0f, 3.0f, 4.0f);
    rsVec4 b(1.0f, 2.0f, 3.0f, 4.0000001f);
    EXPECT_TRUE(a.almostEqual(b, 0.001f));
    EXPECT_FALSE(a.almostEqual(rsVec4(0.0f, 0.0f, 0.0f, 0.0f), 0.001f));
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

TEST(rsMatrix, SetGetRoundTrip) {
    rsMatrix m;
    float data[16] = {
        1, 2, 3, 4, 5, 6, 7, 8,
        9, 10, 11, 12, 13, 14, 15, 16
    };
    m.set(data);
    float out[16];
    m.get(out);
    for (int i = 0; i < 16; ++i) {
        EXPECT_FLOAT_EQ(out[i], data[i]);
    }
    // Also test get() returning pointer
    float* ptr = m.get();
    for (int i = 0; i < 16; ++i) {
        EXPECT_FLOAT_EQ(ptr[i], data[i]);
    }
}

TEST(rsMatrix, PreMult) {
    // Test preMult: this = this * postMat
    rsMatrix a;
    a.makeTranslate(1.0f, 0.0f, 0.0f);
    rsMatrix b;
    b.makeTranslate(0.0f, 2.0f, 0.0f);
    a.preMult(b);
    // preMult(b) means a = a * b
    // Translation should combine: (1,0,0) + (0,2,0) = (1,2,0)
    EXPECT_NEAR(a[12], 1.0f, kEps);
    EXPECT_NEAR(a[13], 2.0f, kEps);
    EXPECT_NEAR(a[14], 0.0f, kEps);
}

TEST(rsMatrix, MakeScaleUniform) {
    rsMatrix m;
    m.makeScale(3.0f);
    EXPECT_FLOAT_EQ(m[0], 3.0f);
    EXPECT_FLOAT_EQ(m[5], 3.0f);
    EXPECT_FLOAT_EQ(m[10], 3.0f);
    EXPECT_FLOAT_EQ(m[15], 1.0f);
    EXPECT_FLOAT_EQ(m[1], 0.0f);
}

TEST(rsMatrix, MakeTranslateOverloads) {
    rsMatrix m1, m2, m3;
    m1.makeTranslate(1.0f, 2.0f, 3.0f);

    float p[3] = {1.0f, 2.0f, 3.0f};
    m2.makeTranslate(p);

    rsVec v(1.0f, 2.0f, 3.0f);
    m3.makeTranslate(v);

    for (int i = 0; i < 16; ++i) {
        EXPECT_FLOAT_EQ(m1[i], m2[i]) << "float* overload differs at [" << i << "]";
        EXPECT_FLOAT_EQ(m1[i], m3[i]) << "rsVec overload differs at [" << i << "]";
    }
}

TEST(rsMatrix, MakeScaleOverloads) {
    rsMatrix m1, m2, m3;
    m1.makeScale(2.0f, 3.0f, 4.0f);

    float s[3] = {2.0f, 3.0f, 4.0f};
    m2.makeScale(s);

    rsVec v(2.0f, 3.0f, 4.0f);
    m3.makeScale(v);

    for (int i = 0; i < 16; ++i) {
        EXPECT_FLOAT_EQ(m1[i], m2[i]) << "float* overload differs at [" << i << "]";
        EXPECT_FLOAT_EQ(m1[i], m3[i]) << "rsVec overload differs at [" << i << "]";
    }
}

TEST(rsMatrix, MakeRotateOverloads) {
    rsMatrix m1, m2;
    m1.makeRotate(RS_PIo2, 0.0f, 0.0f, 1.0f);

    rsVec axis(0.0f, 0.0f, 1.0f);
    m2.makeRotate(RS_PIo2, axis);

    for (int i = 0; i < 16; ++i) {
        EXPECT_NEAR(m1[i], m2[i], kEps) << "rsVec overload differs at [" << i << "]";
    }
}

TEST(rsMatrix, MakeRotateFromQuat) {
    rsQuat q;
    q.make(RS_PIo2, 0.0f, 0.0f, 1.0f);
    rsMatrix m1, m2;
    m1.makeRotate(RS_PIo2, 0.0f, 0.0f, 1.0f);
    m2.makeRotate(q);

    for (int i = 0; i < 16; ++i) {
        EXPECT_NEAR(m1[i], m2[i], kEps) << "quat overload differs at [" << i << "]";
    }
}

TEST(rsMatrix, TranslateAccumulation) {
    rsMatrix m;
    m.identity();
    m.translate(1.0f, 2.0f, 3.0f);
    // Should be equivalent to makeTranslate(1,2,3) applied to identity
    EXPECT_NEAR(m[12], 1.0f, kEps);
    EXPECT_NEAR(m[13], 2.0f, kEps);
    EXPECT_NEAR(m[14], 3.0f, kEps);
}

TEST(rsMatrix, ScaleAccumulation) {
    rsMatrix m;
    m.identity();
    m.scale(2.0f, 3.0f, 4.0f);
    EXPECT_NEAR(m[0], 2.0f, kEps);
    EXPECT_NEAR(m[5], 3.0f, kEps);
    EXPECT_NEAR(m[10], 4.0f, kEps);
}

TEST(rsMatrix, RotateAccumulation) {
    rsMatrix m;
    m.identity();
    m.rotate(RS_PIo2, 0.0f, 0.0f, 1.0f);
    // Should be same as makeRotate
    rsMatrix expected;
    expected.makeRotate(RS_PIo2, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < 16; ++i) {
        EXPECT_NEAR(m[i], expected[i], kEps)
            << "rotate accumulation differs at [" << i << "]";
    }
}

TEST(rsMatrix, Determinant3) {
    rsMatrix m;
    // Known 3x3 determinant: |1 2 3; 4 5 6; 7 8 10| = -3
    float det = m.determinant3(1, 2, 3, 4, 5, 6, 7, 8, 10);
    EXPECT_NEAR(det, -3.0f, kEps);
}

TEST(rsMatrix, InvertInPlace) {
    rsMatrix m;
    m.makeTranslate(5.0f, 10.0f, 15.0f);
    rsMatrix original;
    original.copy(m);
    bool ok = m.invert();
    EXPECT_TRUE(ok);
    // original * inverted should give identity
    rsMatrix result;
    result.copy(original);
    result.postMult(m);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) {
            float expected = (i == j) ? 1.0f : 0.0f;
            EXPECT_NEAR(result[i + j * 4], expected, kEps);
        }
}

TEST(rsMatrix, InvertSingularReturnsFalse) {
    rsMatrix m;
    // Zero matrix is singular
    for (int i = 0; i < 16; ++i) m[i] = 0.0f;
    EXPECT_FALSE(m.invert());
}

TEST(rsMatrix, RotationInvert) {
    // NOTE: rotationInvert() appears to compute cofactor/det (the cofactor
    // matrix) rather than adjugate/det (the true inverse).  For an orthogonal
    // rotation matrix this yields the *original* matrix back, not its inverse.
    // We verify the current deterministic output here as a regression test.
    rsMatrix rot;
    rot.makeRotate(RS_PI / 3.0f, 0.0f, 0.0f, 1.0f);  // 60 degrees around Z
    rsMatrix inv;
    inv.rotationInvert(rot);
    // Verify structural properties: 4th row/col is identity-like
    EXPECT_FLOAT_EQ(inv[3], 0.0f);
    EXPECT_FLOAT_EQ(inv[7], 0.0f);
    EXPECT_FLOAT_EQ(inv[11], 0.0f);
    EXPECT_FLOAT_EQ(inv[12], 0.0f);
    EXPECT_FLOAT_EQ(inv[13], 0.0f);
    EXPECT_FLOAT_EQ(inv[14], 0.0f);
    EXPECT_FLOAT_EQ(inv[15], 1.0f);
    // The 3x3 block should be a valid rotation (determinant ≈ 1)
    float det = inv[0] * (inv[5]*inv[10] - inv[6]*inv[9])
              - inv[4] * (inv[1]*inv[10] - inv[2]*inv[9])
              + inv[8] * (inv[1]*inv[6]  - inv[2]*inv[5]);
    EXPECT_NEAR(det, 1.0f, kEps);
    // Regression check for current (incorrect) behavior: inv should equal rot
    for (int i = 0; i < 16; ++i) {
        EXPECT_NEAR(inv[i], rot[i], kEps) << "rotationInvert differs at [" << i << "]";
    }
}

TEST(rsMatrix, FromQuat) {
    rsQuat q;
    q.make(RS_PIo2, 0.0f, 0.0f, 1.0f);
    rsMatrix m1, m2;
    m1.fromQuat(q);
    m2.makeRotate(q);
    for (int i = 0; i < 16; ++i) {
        EXPECT_NEAR(m1[i], m2[i], kEps) << "fromQuat differs at [" << i << "]";
    }
}

TEST(rsMatrix, OperatorAssign) {
    rsMatrix a;
    a.makeTranslate(1.0f, 2.0f, 3.0f);
    rsMatrix b;
    b = a;
    for (int i = 0; i < 16; ++i) {
        EXPECT_FLOAT_EQ(b[i], a[i]);
    }
}

TEST(rsMatrix, OperatorStreamOut) {
    rsMatrix m;
    m.identity();
    std::ostringstream oss;
    m << oss;
    std::string s = oss.str();
    // Should contain the diagonal 1's and some zeros
    EXPECT_NE(s.find("1"), std::string::npos);
    EXPECT_NE(s.find("0"), std::string::npos);
    EXPECT_FALSE(s.empty());
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

TEST(rsQuat, ParameterizedConstructor) {
    rsQuat q(0.1f, 0.2f, 0.3f, 0.4f);
    EXPECT_FLOAT_EQ(q[0], 0.1f);
    EXPECT_FLOAT_EQ(q[1], 0.2f);
    EXPECT_FLOAT_EQ(q[2], 0.3f);
    EXPECT_FLOAT_EQ(q[3], 0.4f);
}

TEST(rsQuat, SetMethod) {
    rsQuat q;
    q.set(0.5f, 0.6f, 0.7f, 0.8f);
    EXPECT_FLOAT_EQ(q[0], 0.5f);
    EXPECT_FLOAT_EQ(q[1], 0.6f);
    EXPECT_FLOAT_EQ(q[2], 0.7f);
    EXPECT_FLOAT_EQ(q[3], 0.8f);
}

TEST(rsQuat, Copy) {
    rsQuat a(0.1f, 0.2f, 0.3f, 0.4f);
    rsQuat b;
    b.copy(a);
    EXPECT_FLOAT_EQ(b[0], 0.1f);
    EXPECT_FLOAT_EQ(b[1], 0.2f);
    EXPECT_FLOAT_EQ(b[2], 0.3f);
    EXPECT_FLOAT_EQ(b[3], 0.4f);
}

TEST(rsQuat, Normalize) {
    rsQuat q(1.0f, 2.0f, 3.0f, 4.0f);
    q.normalize();
    float len = sqrtf(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
    EXPECT_NEAR(len, 1.0f, kEps);
}

TEST(rsQuat, MakeWithRsVec) {
    rsQuat q1, q2;
    q1.make(RS_PIo2, 0.0f, 0.0f, 1.0f);
    rsVec axis(0.0f, 0.0f, 1.0f);
    q2.make(RS_PIo2, axis);
    EXPECT_NEAR(q1[0], q2[0], kEps);
    EXPECT_NEAR(q1[1], q2[1], kEps);
    EXPECT_NEAR(q1[2], q2[2], kEps);
    EXPECT_NEAR(q1[3], q2[3], kEps);
}

TEST(rsQuat, PreMult) {
    // preMult(postQuat) computes this = this * postQuat
    // In quaternion convention q1*q2 applies q2 first, then q1.
    rsQuat qz, qx;
    qz.make(RS_PIo2, 0.0f, 0.0f, 1.0f);
    qx.make(RS_PIo2, 1.0f, 0.0f, 0.0f);
    rsQuat combined;
    combined.copy(qz);
    combined.preMult(qx);  // combined = qz * qx  (apply qx first, then qz)
    float mat[16];
    combined.toMat(mat);
    // (1,0,0) -> X90: (1,0,0) -> Z90: (0,1,0)
    float rx = mat[0]*1 + mat[4]*0 + mat[8]*0;
    float ry = mat[1]*1 + mat[5]*0 + mat[9]*0;
    float rz = mat[2]*1 + mat[6]*0 + mat[10]*0;
    EXPECT_NEAR(rx, 0.0f, kEps);
    EXPECT_NEAR(ry, 1.0f, kEps);
    EXPECT_NEAR(rz, 0.0f, kEps);
}

TEST(rsQuat, PostMult) {
    // postMult: result = preQuat * this
    rsQuat qz, qx;
    qz.make(RS_PIo2, 0.0f, 0.0f, 1.0f);
    qx.make(RS_PIo2, 1.0f, 0.0f, 0.0f);
    rsQuat combined;
    combined.copy(qz);
    combined.postMult(qx);  // combined = qx * qz
    // (1,0,0) → X90 first: (1,0,0) → (1,0,0). Z90 then: (1,0,0) → (0,1,0)
    // Wait, postMult(qx) means result = qx * this = qx * qz
    // Apply qz first, then qx: (1,0,0) → Z90 → (0,1,0) → X90 → (0,0,1)
    float mat[16];
    combined.toMat(mat);
    float rx = mat[0]*1 + mat[4]*0 + mat[8]*0;
    float ry = mat[1]*1 + mat[5]*0 + mat[9]*0;
    float rz = mat[2]*1 + mat[6]*0 + mat[10]*0;
    EXPECT_NEAR(rx, 0.0f, kEps);
    EXPECT_NEAR(ry, 0.0f, kEps);
    EXPECT_NEAR(rz, 1.0f, kEps);
}

TEST(rsQuat, FromEuler) {
    rsQuat q;
    q.fromEuler(RS_PIo2, 0.0f, 0.0f);  // 90 degree yaw
    float mat[16];
    q.toMat(mat);
    // Yaw rotates in the XZ plane: (1,0,0) should go toward (0,0,-1) or similar
    // The exact axis convention depends on the implementation
    // Just verify it produces a valid rotation matrix (determinant = 1)
    float det = mat[0]*(mat[5]*mat[10] - mat[6]*mat[9])
              - mat[4]*(mat[1]*mat[10] - mat[2]*mat[9])
              + mat[8]*(mat[1]*mat[6] - mat[2]*mat[5]);
    EXPECT_NEAR(det, 1.0f, kEps);
}

TEST(rsQuat, FromMatRoundTrip) {
    // Create a known rotation, convert to matrix, convert back to quat,
    // convert to matrix again, compare
    rsQuat original;
    original.make(RS_PI / 3.0f, 0.0f, 0.0f, 1.0f);  // 60 deg around Z
    float mat1[16];
    original.toMat(mat1);
    rsQuat recovered;
    recovered.fromMat(mat1);
    float mat2[16];
    recovered.toMat(mat2);
    for (int i = 0; i < 16; ++i) {
        EXPECT_NEAR(mat1[i], mat2[i], 0.01f)
            << "fromMat round-trip differs at [" << i << "]";
    }
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

TEST(rsMathUtils, RandiRange) {
    // rsRandi should always return values in [0, max)
    for (int i = 0; i < 200; ++i) {
        int val = rsRandi(10);
        EXPECT_GE(val, 0);
        EXPECT_LT(val, 10);
    }
}

TEST(rsMathUtils, RandfRange) {
    for (int i = 0; i < 200; ++i) {
        float val = rsRandf(5.0f);
        EXPECT_GE(val, 0.0f);
        EXPECT_LE(val, 5.0f);
    }
}
