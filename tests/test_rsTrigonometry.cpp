/*
 * Tests for fast trigonometry functions (rsCosf, rsSinf, rsAtan2f)
 */

#include <gtest/gtest.h>
#include <cmath>
#include <rsMath/rsMath.h>

// The fast trig functions are table-based approximations.
// Use a generous epsilon compared to standard library functions.
static constexpr float kTrigEps = 0.01f;

// ---------------------------------------------------------------------------
// rsCosf tests
// ---------------------------------------------------------------------------

TEST(rsTrigonometry, CosfZero) {
    EXPECT_NEAR(rsCosf(0.0f), std::cos(0.0f), kTrigEps);
}

TEST(rsTrigonometry, CosfPiOver2) {
    EXPECT_NEAR(rsCosf(RS_PIo2), std::cos(RS_PIo2), kTrigEps);
}

TEST(rsTrigonometry, CosfPi) {
    EXPECT_NEAR(rsCosf(RS_PI), std::cos(RS_PI), kTrigEps);
}

TEST(rsTrigonometry, Cosf3PiOver2) {
    EXPECT_NEAR(rsCosf(RS_PI + RS_PIo2), std::cos(RS_PI + RS_PIo2), kTrigEps);
}

TEST(rsTrigonometry, CosfTwoPi) {
    EXPECT_NEAR(rsCosf(RS_PIx2), std::cos(RS_PIx2), kTrigEps);
}

TEST(rsTrigonometry, CosfPiOver4) {
    EXPECT_NEAR(rsCosf(RS_PI * 0.25f), std::cos(RS_PI * 0.25f), kTrigEps);
}

TEST(rsTrigonometry, CosfPiOver6) {
    const float angle = RS_PI / 6.0f;
    EXPECT_NEAR(rsCosf(angle), std::cos(angle), kTrigEps);
}

TEST(rsTrigonometry, CosfPiOver3) {
    const float angle = RS_PI / 3.0f;
    EXPECT_NEAR(rsCosf(angle), std::cos(angle), kTrigEps);
}

// ---------------------------------------------------------------------------
// rsSinf tests
// ---------------------------------------------------------------------------

TEST(rsTrigonometry, SinfZero) {
    EXPECT_NEAR(rsSinf(0.0f), std::sin(0.0f), kTrigEps);
}

TEST(rsTrigonometry, SinfPiOver2) {
    EXPECT_NEAR(rsSinf(RS_PIo2), std::sin(RS_PIo2), kTrigEps);
}

TEST(rsTrigonometry, SinfPi) {
    EXPECT_NEAR(rsSinf(RS_PI), std::sin(RS_PI), kTrigEps);
}

TEST(rsTrigonometry, Sinf3PiOver2) {
    EXPECT_NEAR(rsSinf(RS_PI + RS_PIo2), std::sin(RS_PI + RS_PIo2), kTrigEps);
}

TEST(rsTrigonometry, SinfTwoPi) {
    EXPECT_NEAR(rsSinf(RS_PIx2), std::sin(RS_PIx2), kTrigEps);
}

TEST(rsTrigonometry, SinfPiOver4) {
    EXPECT_NEAR(rsSinf(RS_PI * 0.25f), std::sin(RS_PI * 0.25f), kTrigEps);
}

TEST(rsTrigonometry, SinfPiOver6) {
    const float angle = RS_PI / 6.0f;
    EXPECT_NEAR(rsSinf(angle), std::sin(angle), kTrigEps);
}

TEST(rsTrigonometry, SinfPiOver3) {
    const float angle = RS_PI / 3.0f;
    EXPECT_NEAR(rsSinf(angle), std::sin(angle), kTrigEps);
}

// Verify fundamental identity: sin^2 + cos^2 = 1
TEST(rsTrigonometry, PythagoreanIdentity) {
    for (int i = 0; i < 64; ++i) {
        const float angle = RS_PIx2 * static_cast<float>(i) / 64.0f;
        const float s = rsSinf(angle);
        const float c = rsCosf(angle);
        EXPECT_NEAR(s * s + c * c, 1.0f, 0.05f)
            << "Failed at angle = " << angle;
    }
}

// ---------------------------------------------------------------------------
// rsAtan2f tests
// ---------------------------------------------------------------------------

TEST(rsTrigonometry, Atan2fPositiveXAxis) {
    // atan2(0, 1) = 0
    EXPECT_NEAR(rsAtan2f(0.0f, 1.0f), std::atan2(0.0f, 1.0f), kTrigEps);
}

TEST(rsTrigonometry, Atan2fPositiveYAxis) {
    // atan2(1, 0) = pi/2
    EXPECT_NEAR(rsAtan2f(1.0f, 0.0f), std::atan2(1.0f, 0.0f), kTrigEps);
}

TEST(rsTrigonometry, Atan2fNegativeXAxis) {
    // atan2(0, -1) = pi
    EXPECT_NEAR(rsAtan2f(0.0f, -1.0f), std::atan2(0.0f, -1.0f), kTrigEps);
}

TEST(rsTrigonometry, Atan2fNegativeYAxis) {
    // atan2(-1, 0) = -pi/2
    EXPECT_NEAR(rsAtan2f(-1.0f, 0.0f), std::atan2(-1.0f, 0.0f), kTrigEps);
}

TEST(rsTrigonometry, Atan2fDiagonal) {
    // atan2(1, 1) = pi/4
    EXPECT_NEAR(rsAtan2f(1.0f, 1.0f), std::atan2(1.0f, 1.0f), kTrigEps);
}

TEST(rsTrigonometry, Atan2fNegativeDiagonal) {
    // atan2(-1, -1) = -3pi/4
    EXPECT_NEAR(rsAtan2f(-1.0f, -1.0f), std::atan2(-1.0f, -1.0f), kTrigEps);
}

TEST(rsTrigonometry, Atan2fQuadrant2) {
    // atan2(1, -1) = 3pi/4
    EXPECT_NEAR(rsAtan2f(1.0f, -1.0f), std::atan2(1.0f, -1.0f), kTrigEps);
}

TEST(rsTrigonometry, Atan2fQuadrant4) {
    // atan2(-1, 1) = -pi/4
    EXPECT_NEAR(rsAtan2f(-1.0f, 1.0f), std::atan2(-1.0f, 1.0f), kTrigEps);
}
