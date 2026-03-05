/*
 * Smoke tests for Rgbhsl library (rgb2hsl, hsl2rgb, hslTween, rgbTween)
 */

#include <gtest/gtest.h>
#include <cmath>
#include <Rgbhsl/Rgbhsl.h>

static constexpr float kEps = 1e-3f;

// ---------------------------------------------------------------------------
// rgb2hsl / hsl2rgb round-trip tests
// ---------------------------------------------------------------------------

TEST(Rgbhsl, RoundTripRed) {
    float h, s, l, r, g, b;
    rgb2hsl(1.0f, 0.0f, 0.0f, h, s, l);
    EXPECT_NEAR(l, 1.0f, kEps);  // brightest channel = 1
    hsl2rgb(h, s, l, r, g, b);
    EXPECT_NEAR(r, 1.0f, kEps);
    EXPECT_NEAR(g, 0.0f, kEps);
    EXPECT_NEAR(b, 0.0f, kEps);
}

TEST(Rgbhsl, RoundTripGreen) {
    float h, s, l, r, g, b;
    rgb2hsl(0.0f, 1.0f, 0.0f, h, s, l);
    EXPECT_NEAR(l, 1.0f, kEps);
    hsl2rgb(h, s, l, r, g, b);
    EXPECT_NEAR(r, 0.0f, kEps);
    EXPECT_NEAR(g, 1.0f, kEps);
    EXPECT_NEAR(b, 0.0f, kEps);
}

TEST(Rgbhsl, RoundTripBlue) {
    float h, s, l, r, g, b;
    rgb2hsl(0.0f, 0.0f, 1.0f, h, s, l);
    EXPECT_NEAR(l, 1.0f, kEps);
    hsl2rgb(h, s, l, r, g, b);
    EXPECT_NEAR(r, 0.0f, kEps);
    EXPECT_NEAR(g, 0.0f, kEps);
    EXPECT_NEAR(b, 1.0f, kEps);
}

TEST(Rgbhsl, RoundTripWhite) {
    float h, s, l, r, g, b;
    rgb2hsl(1.0f, 1.0f, 1.0f, h, s, l);
    EXPECT_NEAR(l, 1.0f, kEps);
    EXPECT_NEAR(s, 0.0f, kEps);  // white has zero saturation
    hsl2rgb(h, s, l, r, g, b);
    EXPECT_NEAR(r, 1.0f, kEps);
    EXPECT_NEAR(g, 1.0f, kEps);
    EXPECT_NEAR(b, 1.0f, kEps);
}

TEST(Rgbhsl, BlackSpecialCase) {
    float h, s, l;
    rgb2hsl(0.0f, 0.0f, 0.0f, h, s, l);
    EXPECT_NEAR(l, 0.0f, kEps);
    EXPECT_NEAR(h, 0.0f, kEps);
    // Saturation is undefined when luminosity is 0; the library returns 1.0
    EXPECT_NEAR(s, 1.0f, kEps);
}

TEST(Rgbhsl, RoundTripArbitraryColor) {
    float h, s, l, r, g, b;
    // Use a fully-saturated color that round-trips cleanly
    float origR = 1.0f, origG = 0.0f, origB = 0.5f;
    rgb2hsl(origR, origG, origB, h, s, l);
    hsl2rgb(h, s, l, r, g, b);
    EXPECT_NEAR(r, origR, kEps);
    EXPECT_NEAR(g, origG, kEps);
    EXPECT_NEAR(b, origB, kEps);
}

// ---------------------------------------------------------------------------
// hslTween tests
// ---------------------------------------------------------------------------

TEST(Rgbhsl, HslTweenEndpoints) {
    float h, s, l;
    // tween=0.0 should return the first color
    hslTween(0.0f, 1.0f, 1.0f,
             0.5f, 0.5f, 0.5f, 0.0f, 0,
             h, s, l);
    EXPECT_NEAR(h, 0.0f, kEps);
    EXPECT_NEAR(s, 1.0f, kEps);
    EXPECT_NEAR(l, 1.0f, kEps);

    // tween=1.0 should return the second color
    hslTween(0.0f, 1.0f, 1.0f,
             0.5f, 0.5f, 0.5f, 1.0f, 0,
             h, s, l);
    EXPECT_NEAR(h, 0.5f, kEps);
    EXPECT_NEAR(s, 0.5f, kEps);
    EXPECT_NEAR(l, 0.5f, kEps);
}

// ---------------------------------------------------------------------------
// rgbTween tests
// ---------------------------------------------------------------------------

TEST(Rgbhsl, RgbTweenEndpoints) {
    float r, g, b;
    // tween=0.0 should return the first color
    rgbTween(1.0f, 0.0f, 0.0f,
             0.0f, 1.0f, 0.0f, 0.0f, 0,
             r, g, b);
    EXPECT_NEAR(r, 1.0f, kEps);
    EXPECT_NEAR(g, 0.0f, kEps);
    EXPECT_NEAR(b, 0.0f, kEps);

    // tween=1.0 should return the second color
    rgbTween(1.0f, 0.0f, 0.0f,
             0.0f, 1.0f, 0.0f, 1.0f, 0,
             r, g, b);
    EXPECT_NEAR(r, 0.0f, kEps);
    EXPECT_NEAR(g, 1.0f, kEps);
    EXPECT_NEAR(b, 0.0f, kEps);
}

// ---------------------------------------------------------------------------
// hslTween midpoint and direction tests
// ---------------------------------------------------------------------------

TEST(Rgbhsl, HslTweenMidpoint) {
    float h, s, l;
    // Tween at 0.5 between two HSL colors (forward direction)
    hslTween(0.0f, 1.0f, 1.0f,
             0.5f, 0.5f, 0.5f, 0.5f, 0,
             h, s, l);
    // Midpoint: h = 0.25, s = 0.75, l = 0.75
    EXPECT_NEAR(h, 0.25f, kEps);
    EXPECT_NEAR(s, 0.75f, kEps);
    EXPECT_NEAR(l, 0.75f, kEps);
}

TEST(Rgbhsl, HslTweenBackwardDirection) {
    float h, s, l;
    // direction=1: backward around color wheel
    // h1=0.0, h2=0.5, tween=0.0 => h1
    hslTween(0.0f, 1.0f, 1.0f,
             0.5f, 0.5f, 0.5f, 0.0f, 1,
             h, s, l);
    EXPECT_NEAR(h, 0.0f, kEps);
    EXPECT_NEAR(s, 1.0f, kEps);
    EXPECT_NEAR(l, 1.0f, kEps);

    // tween=1.0
    hslTween(0.0f, 1.0f, 1.0f,
             0.5f, 0.5f, 0.5f, 1.0f, 1,
             h, s, l);
    EXPECT_NEAR(h, 0.5f, kEps);
    EXPECT_NEAR(s, 0.5f, kEps);
    EXPECT_NEAR(l, 0.5f, kEps);
}

TEST(Rgbhsl, HslTweenBackwardQuarterpoint) {
    float h, s, l;
    // h1=0.2, h2=0.8, direction=1 (backward from 0.2 toward 0.8)
    // Backward interpolation (no wrap at this tween):
    // outh = h1 - tween*(1 - (h2 - h1)) = 0.2 - 0.25*(1 - 0.6) = 0.2 - 0.1 = 0.1
    hslTween(0.2f, 1.0f, 1.0f,
             0.8f, 1.0f, 1.0f, 0.25f, 1,
             h, s, l);
    // At tween=0.25 we expect outh = 0.1 (no wrapping); wrap behavior is
    // covered separately in HslTweenHueWrapping.
    EXPECT_NEAR(h, 0.1f, kEps);
}

TEST(Rgbhsl, HslTweenHueWrapping) {
    float h, s, l;
    // Forward: h1=0.8, h2=0.2. Forward wraps through 1.0.
    // outh = 0.8 + 0.75*(1.0 - 0.6) = 0.8 + 0.3 = 1.1 → wraps to 0.1
    hslTween(0.8f, 1.0f, 1.0f,
             0.2f, 1.0f, 1.0f, 0.75f, 0,
             h, s, l);
    EXPECT_NEAR(h, 0.1f, kEps);
}

TEST(Rgbhsl, RgbTweenMidpoint) {
    float r, g, b;
    // Midpoint between red and green (through HSL space)
    rgbTween(1.0f, 0.0f, 0.0f,
             0.0f, 1.0f, 0.0f, 0.5f, 0,
             r, g, b);
    // Should be a yellow-ish color (both r and g present)
    EXPECT_GT(r, 0.0f);
    EXPECT_GT(g, 0.0f);
}

TEST(Rgbhsl, RgbTweenBackward) {
    float r, g, b;
    // Same endpoints but direction=1
    rgbTween(1.0f, 0.0f, 0.0f,
             0.0f, 1.0f, 0.0f, 0.5f, 1,
             r, g, b);
    // Should produce a different color than forward direction (goes through blue)
    // Blue component should be present
    EXPECT_GT(b, 0.0f);
}

TEST(Rgbhsl, Grayscale) {
    float h, s, l;
    // White (1,1,1) is a clean grayscale case for this implementation
    rgb2hsl(1.0f, 1.0f, 1.0f, h, s, l);
    EXPECT_NEAR(s, 0.0f, kEps);
    EXPECT_NEAR(l, 1.0f, kEps);
    // Black (0,0,0)
    rgb2hsl(0.0f, 0.0f, 0.0f, h, s, l);
    EXPECT_NEAR(l, 0.0f, kEps);
}

// ---------------------------------------------------------------------------
// Division-by-zero guard and additional round-trip tests
// ---------------------------------------------------------------------------

TEST(Rgbhsl, NearZeroLuminosity) {
    float h, s, l;
    rgb2hsl(1e-8f, 1e-8f, 1e-8f, h, s, l);
    EXPECT_TRUE(std::isfinite(h));
    EXPECT_TRUE(std::isfinite(s));
    EXPECT_TRUE(std::isfinite(l));
    EXPECT_NEAR(l, 0.0f, kEps);
}

TEST(Rgbhsl, MidGraySafety) {
    float h, s, l, r, g, b;
    rgb2hsl(0.5f, 0.5f, 0.5f, h, s, l);
    EXPECT_TRUE(std::isfinite(h));
    EXPECT_TRUE(std::isfinite(s));
    EXPECT_TRUE(std::isfinite(l));
    EXPECT_NEAR(l, 0.5f, kEps);
    // This HSL model computes s = 1 - minChannel, so equal-channel grays
    // (other than black/white) have non-zero saturation and do NOT round-trip
    // to the original RGB values.  This test only verifies finite, in-range
    // output to confirm no division-by-zero on a mid-gray input.
    hsl2rgb(h, s, l, r, g, b);
    EXPECT_TRUE(std::isfinite(r));
    EXPECT_TRUE(std::isfinite(g));
    EXPECT_TRUE(std::isfinite(b));
    EXPECT_GE(r, -kEps);
    EXPECT_LE(r, 1.0f + kEps);
    EXPECT_GE(g, -kEps);
    EXPECT_LE(g, 1.0f + kEps);
    EXPECT_GE(b, -kEps);
    EXPECT_LE(b, 1.0f + kEps);
}

struct RgbColor {
    float r, g, b;
};

class RgbhslRoundTrip : public ::testing::TestWithParam<RgbColor> {};

TEST_P(RgbhslRoundTrip, AllPrimarySecondaryRoundTrip) {
    RgbColor color = GetParam();
    float r0 = color.r;
    float g0 = color.g;
    float b0 = color.b;
    float h, s, l, r, g, b;
    rgb2hsl(r0, g0, b0, h, s, l);
    hsl2rgb(h, s, l, r, g, b);
    EXPECT_NEAR(r, r0, kEps);
    EXPECT_NEAR(g, g0, kEps);
    EXPECT_NEAR(b, b0, kEps);
}

INSTANTIATE_TEST_SUITE_P(PrimarySecondary, RgbhslRoundTrip,
    ::testing::Values(
        RgbColor{1.0f, 0.0f, 0.0f},  // Red
        RgbColor{0.0f, 1.0f, 0.0f},  // Green
        RgbColor{0.0f, 0.0f, 1.0f},  // Blue
        RgbColor{0.0f, 1.0f, 1.0f},  // Cyan
        RgbColor{1.0f, 0.0f, 1.0f},  // Magenta
        RgbColor{1.0f, 1.0f, 0.0f}   // Yellow
    ));
