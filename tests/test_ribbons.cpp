/*
 * Tests for catmullRom.h (catmullRom, ensureMinDist)
 */

#include <gtest/gtest.h>
#include <cmath>
#include <limits>
#include <rsUtility/catmullRom.h>

static constexpr float kEps = 1e-4f;

// ---------------------------------------------------------------------------
// catmullRom interpolation tests
// ---------------------------------------------------------------------------

TEST(CatmullRom, ReturnsP1AtT0) {
    // At t=0, result should equal p1
    float result = catmullRom(0.0f, 1.0f, 2.0f, 3.0f, 0.0f);
    EXPECT_NEAR(result, 1.0f, kEps);
}

TEST(CatmullRom, ReturnsP2AtT1) {
    // At t=1, result should equal p2
    float result = catmullRom(0.0f, 1.0f, 2.0f, 3.0f, 1.0f);
    EXPECT_NEAR(result, 2.0f, kEps);
}

TEST(CatmullRom, MidpointSymmetry) {
    // For uniformly spaced points 0,1,2,3, midpoint should be 1.5
    float result = catmullRom(0.0f, 1.0f, 2.0f, 3.0f, 0.5f);
    EXPECT_NEAR(result, 1.5f, kEps);
}

TEST(CatmullRom, AllSamePoints) {
    // If all control points are identical, output should be that value for any t
    float result0 = catmullRom(5.0f, 5.0f, 5.0f, 5.0f, 0.0f);
    float result5 = catmullRom(5.0f, 5.0f, 5.0f, 5.0f, 0.5f);
    float result1 = catmullRom(5.0f, 5.0f, 5.0f, 5.0f, 1.0f);
    EXPECT_NEAR(result0, 5.0f, kEps);
    EXPECT_NEAR(result5, 5.0f, kEps);
    EXPECT_NEAR(result1, 5.0f, kEps);
}

TEST(CatmullRom, NonUniformSpacing) {
    // Non-uniform control points: p0=0, p1=0, p2=10, p3=10
    // At t=0, should be 0 (=p1); at t=1, should be 10 (=p2)
    EXPECT_NEAR(catmullRom(0.0f, 0.0f, 10.0f, 10.0f, 0.0f), 0.0f, kEps);
    EXPECT_NEAR(catmullRom(0.0f, 0.0f, 10.0f, 10.0f, 1.0f), 10.0f, kEps);
    // Midpoint for this symmetric case should be 5.0
    EXPECT_NEAR(catmullRom(0.0f, 0.0f, 10.0f, 10.0f, 0.5f), 5.0f, kEps);
}

TEST(CatmullRom, Extrapolation) {
    // t outside [0,1] extrapolates; ensure no crash and is finite
    float resultNeg = catmullRom(0.0f, 1.0f, 2.0f, 3.0f, -0.5f);
    float resultPos = catmullRom(0.0f, 1.0f, 2.0f, 3.0f, 1.5f);
    EXPECT_TRUE(std::isfinite(resultNeg));
    EXPECT_TRUE(std::isfinite(resultPos));
    // For uniformly spaced points, extrapolation continues linearly
    EXPECT_NEAR(resultNeg, 0.5f, kEps);
    EXPECT_NEAR(resultPos, 2.5f, kEps);
}

TEST(CatmullRom, EndpointTangentMatchesExpected) {
    // For Catmull-Rom, endpoint derivatives are:
    // d/dt at t=0 = 0.5 * (p2 - p0), d/dt at t=1 = 0.5 * (p3 - p1)
    const float p0 = -2.0f;
    const float p1 = 1.0f;
    const float p2 = 4.0f;
    const float p3 = 9.0f;
    const float h = 1e-4f;

    const float d0 = (catmullRom(p0, p1, p2, p3, h) - catmullRom(p0, p1, p2, p3, 0.0f)) / h;
    const float d1 = (catmullRom(p0, p1, p2, p3, 1.0f) - catmullRom(p0, p1, p2, p3, 1.0f - h)) / h;

    const float expectedD0 = 0.5f * (p2 - p0);
    const float expectedD1 = 0.5f * (p3 - p1);
    const float kSlopeEps = 1e-2f;
    EXPECT_NEAR(d0, expectedD0, kSlopeEps);
    EXPECT_NEAR(d1, expectedD1, kSlopeEps);
}

TEST(CatmullRom, AffineInvariance) {
    // Affine transform f(x)=a*x+b should commute with interpolation.
    const float p0 = -1.0f;
    const float p1 = 2.0f;
    const float p2 = 5.0f;
    const float p3 = 7.0f;
    const float a = -2.5f;
    const float b = 3.0f;

    const float t1 = 0.35f;
    const float t2 = 0.82f;

    const float base1 = catmullRom(p0, p1, p2, p3, t1);
    const float transformed1 = catmullRom(a * p0 + b, a * p1 + b, a * p2 + b, a * p3 + b, t1);
    EXPECT_NEAR(transformed1, a * base1 + b, kEps);

    const float base2 = catmullRom(p0, p1, p2, p3, t2);
    const float transformed2 = catmullRom(a * p0 + b, a * p1 + b, a * p2 + b, a * p3 + b, t2);
    EXPECT_NEAR(transformed2, a * base2 + b, kEps);
}

// ---------------------------------------------------------------------------
// ensureMinDist tests
// ---------------------------------------------------------------------------

TEST(EnsureMinDist, ZeroOffsetFallback) {
    // When pt == prev, should use fallback direction (minDist, 0, 0)
    float prev[3] = {1.0f, 2.0f, 3.0f};
    float pt[3] = {1.0f, 2.0f, 3.0f};
    ensureMinDist(pt, prev, 0.5f);
    EXPECT_NEAR(pt[0], 1.5f, kEps);
    EXPECT_NEAR(pt[1], 2.0f, kEps);
    EXPECT_NEAR(pt[2], 3.0f, kEps);
}

TEST(EnsureMinDist, TooCloseScalesUp) {
    // pt is 0.1 away from prev along X; minDist is 1.0
    float prev[3] = {0.0f, 0.0f, 0.0f};
    float pt[3] = {0.1f, 0.0f, 0.0f};
    ensureMinDist(pt, prev, 1.0f);
    // Should be scaled to 1.0 along X
    EXPECT_NEAR(pt[0], 1.0f, kEps);
    EXPECT_NEAR(pt[1], 0.0f, kEps);
    EXPECT_NEAR(pt[2], 0.0f, kEps);
}

TEST(EnsureMinDist, FarEnoughUnchanged) {
    // pt is 5.0 away; minDist is 1.0 — should not be modified
    float prev[3] = {0.0f, 0.0f, 0.0f};
    float pt[3] = {3.0f, 4.0f, 0.0f};  // distance = 5.0
    ensureMinDist(pt, prev, 1.0f);
    EXPECT_NEAR(pt[0], 3.0f, kEps);
    EXPECT_NEAR(pt[1], 4.0f, kEps);
    EXPECT_NEAR(pt[2], 0.0f, kEps);
}

TEST(EnsureMinDist, DiagonalScaling) {
    // pt is along diagonal (1,1,1) from prev, distance = sqrt(3) ≈ 1.732
    // minDist = 3.0, so must scale up while preserving direction
    float prev[3] = {0.0f, 0.0f, 0.0f};
    float pt[3] = {1.0f, 1.0f, 1.0f};
    ensureMinDist(pt, prev, 3.0f);
    float dist = std::sqrt(pt[0]*pt[0] + pt[1]*pt[1] + pt[2]*pt[2]);
    EXPECT_NEAR(dist, 3.0f, kEps);
    // Direction preserved: all components equal
    EXPECT_NEAR(pt[0], pt[1], kEps);
    EXPECT_NEAR(pt[1], pt[2], kEps);
}

TEST(EnsureMinDist, ExactlyAtMinDist) {
    // pt is exactly at minDist — should not be modified (dist == minDist, not <)
    float prev[3] = {0.0f, 0.0f, 0.0f};
    float pt[3] = {1.0f, 0.0f, 0.0f};
    ensureMinDist(pt, prev, 1.0f);
    EXPECT_NEAR(pt[0], 1.0f, kEps);
    EXPECT_NEAR(pt[1], 0.0f, kEps);
    EXPECT_NEAR(pt[2], 0.0f, kEps);
}

TEST(EnsureMinDist, ZeroMinDistNoOp) {
    float prev[3] = {2.0f, -1.0f, 4.0f};
    float pt[3] = {2.0f, -1.0f, 4.0f};
    ensureMinDist(pt, prev, 0.0f);
    EXPECT_NEAR(pt[0], 2.0f, kEps);
    EXPECT_NEAR(pt[1], -1.0f, kEps);
    EXPECT_NEAR(pt[2], 4.0f, kEps);
}

TEST(EnsureMinDist, NegativeMinDistNoOp) {
    float prev[3] = {0.0f, 0.0f, 0.0f};
    float pt[3] = {0.1f, 0.2f, 0.3f};
    ensureMinDist(pt, prev, -2.0f);
    EXPECT_NEAR(pt[0], 0.1f, kEps);
    EXPECT_NEAR(pt[1], 0.2f, kEps);
    EXPECT_NEAR(pt[2], 0.3f, kEps);
}

TEST(EnsureMinDist, NonFiniteMinDistNoOp) {
    float prev[3] = {0.0f, 0.0f, 0.0f};
    float ptInf[3] = {0.1f, 0.0f, 0.0f};
    float ptNaN[3] = {0.1f, 0.0f, 0.0f};

    ensureMinDist(ptInf, prev, std::numeric_limits<float>::infinity());
    EXPECT_NEAR(ptInf[0], 0.1f, kEps);
    EXPECT_NEAR(ptInf[1], 0.0f, kEps);
    EXPECT_NEAR(ptInf[2], 0.0f, kEps);

    ensureMinDist(ptNaN, prev, std::numeric_limits<float>::quiet_NaN());
    EXPECT_NEAR(ptNaN[0], 0.1f, kEps);
    EXPECT_NEAR(ptNaN[1], 0.0f, kEps);
    EXPECT_NEAR(ptNaN[2], 0.0f, kEps);
}

TEST(EnsureMinDist, NearZeroBoundary) {
    // Below threshold: use fallback (+X) and reset Y/Z to prev.
    float prevA[3] = {1.0f, 10.0f, -5.0f};
    float ptA[3] = {1.0f + 5e-9f, 10.0f + 2.5e-9f, -5.0f};
    ensureMinDist(ptA, prevA, 2.0f);
    EXPECT_NEAR(ptA[0], 3.0f, kEps);
    EXPECT_NEAR(ptA[1], 10.0f, kEps);
    EXPECT_NEAR(ptA[2], -5.0f, kEps);

    // Above threshold: scale while preserving direction.
    float prevB[3] = {0.0f, 0.0f, 0.0f};
    float ptB[3] = {2.0e-8f, 1.0e-8f, 0.0f};
    ensureMinDist(ptB, prevB, 2.0f);
    float distB = std::sqrt(ptB[0] * ptB[0] + ptB[1] * ptB[1] + ptB[2] * ptB[2]);
    EXPECT_NEAR(distB, 2.0f, kEps);
    EXPECT_NEAR(ptB[1] / ptB[0], 0.5f, kEps);
}

TEST(EnsureMinDist, NullPointersNoOp) {
    float prev[3] = {0.0f, 0.0f, 0.0f};
    float pt[3] = {1.0f, 0.0f, 0.0f};

    ensureMinDist(nullptr, prev, 1.0f);
    ensureMinDist(pt, nullptr, 1.0f);

    // Original point remains unchanged after nullptr call.
    EXPECT_NEAR(pt[0], 1.0f, kEps);
    EXPECT_NEAR(pt[1], 0.0f, kEps);
    EXPECT_NEAR(pt[2], 0.0f, kEps);
}
