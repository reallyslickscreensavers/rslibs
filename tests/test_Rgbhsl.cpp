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
