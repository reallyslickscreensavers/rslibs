/*
 * Tests for rsWin32Saver settings logic.
 *
 * rsWin32SaverSettings.h is deliberately free of windows.h, so everything here
 * runs on Linux too. The Win32 calls around it are not unit testable and are
 * not mocked; this covers the decidable part.
 */

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <rsWin32Saver/rsWin32SaverSettings.h>

using rsWin32Saver::clampFrameRateLimit;
using rsWin32Saver::kMaxFrameRateLimit;
using rsWin32Saver::kMinFrameRateLimit;
using rsWin32Saver::parseCommandLine;
using rsWin32Saver::parseUnsigned;
using rsWin32Saver::runPacedFrame;
using rsWin32Saver::CommandLineAction;

TEST(FrameRateLimit, InRangeValuesPassThrough) {
    EXPECT_EQ(clampFrameRateLimit(1UL), 1u);
    EXPECT_EQ(clampFrameRateLimit(60UL), 60u);
    EXPECT_EQ(clampFrameRateLimit(144UL), 144u);
    EXPECT_EQ(clampFrameRateLimit((unsigned long)kMaxFrameRateLimit),
              kMaxFrameRateLimit);
}

TEST(FrameRateLimit, ZeroIsLegalAndMeansUnlimited) {
    // 0 is the on-disk contract for "no limit", not an out-of-range value, so
    // it must survive the clamp untouched.
    EXPECT_EQ(clampFrameRateLimit(0UL), kMinFrameRateLimit);
    EXPECT_EQ(clampFrameRateLimit(0UL), 0u);
}

TEST(FrameRateLimit, OversizedValuesClampToTheMaximum) {
    EXPECT_EQ(clampFrameRateLimit((unsigned long)kMaxFrameRateLimit + 1),
              kMaxFrameRateLimit);
    EXPECT_EQ(clampFrameRateLimit(100000UL), kMaxFrameRateLimit);
}

TEST(FrameRateLimit, NegativeLookingDwordDoesNotBecomeUnlimited) {
    // The bug this guards: a registry value of 0xFFFFFFFF read into an int is
    // -1, and assigning that to the unsigned dFrameRateLimit produced
    // 4294967295, which the saver then divided into 1.0f.
    EXPECT_EQ(clampFrameRateLimit(0xFFFFFFFFUL), kMaxFrameRateLimit);
    EXPECT_EQ(clampFrameRateLimit(0x80000000UL), kMaxFrameRateLimit);

    // Specifically, it must not land on 0, which would silently mean unlimited
    // - the opposite of what an oversized limit asks for.
    EXPECT_NE(clampFrameRateLimit(0xFFFFFFFFUL), 0u);
}

TEST(FramePacing, UnlimitedDrawsWithoutWaiting) {
    int draws = 0;
    std::vector<double> waits;

    runPacedFrame(0,
        [&waits](double seconds) { waits.push_back(seconds); },
        [&draws]() { ++draws; });

    EXPECT_TRUE(waits.empty());
    EXPECT_EQ(draws, 1);
}

TEST(FramePacing, LimitedRatesWaitBeforeDrawing) {
    const unsigned int rates[] = { 30, 100, 120, 1000 };
    for(unsigned int rate : rates){
        std::vector<char> order;
        double waited = 0.0;

        runPacedFrame(rate,
            [&order, &waited](double seconds) {
                order.push_back('w');
                waited = seconds;
            },
            [&order]() { order.push_back('d'); });

        ASSERT_EQ(order.size(), 2u);
        EXPECT_EQ(order[0], 'w');
        EXPECT_EQ(order[1], 'd');
        EXPECT_DOUBLE_EQ(waited, 1.0 / double(rate));
    }
}

TEST(CommandLine, RecognisesEachMode) {
    EXPECT_EQ(parseCommandLine("/c").mode, rsWin32Saver::kSaverConfigureWithParent);
    EXPECT_EQ(parseCommandLine("/s").mode, rsWin32Saver::kSaverRun);
    EXPECT_EQ(parseCommandLine("/w").mode, rsWin32Saver::kSaverWindowed);
    EXPECT_EQ(parseCommandLine("/p 12345").mode, rsWin32Saver::kSaverPreview);
}

TEST(CommandLine, IsCaseInsensitive) {
    EXPECT_EQ(parseCommandLine("/C").mode, rsWin32Saver::kSaverConfigureWithParent);
    EXPECT_EQ(parseCommandLine("/S").mode, rsWin32Saver::kSaverRun);
    EXPECT_EQ(parseCommandLine("/W").mode, rsWin32Saver::kSaverWindowed);
    EXPECT_EQ(parseCommandLine("/P 1").mode, rsWin32Saver::kSaverPreview);
}

TEST(CommandLine, AcceptsDashAndBareLetter) {
    // Windows passes '/', but '-' and a bare letter have always worked too.
    EXPECT_EQ(parseCommandLine("-c").mode, rsWin32Saver::kSaverConfigureWithParent);
    EXPECT_EQ(parseCommandLine("s").mode, rsWin32Saver::kSaverRun);
}

TEST(CommandLine, SkipsLeadingSeparators) {
    EXPECT_EQ(parseCommandLine("   /s").mode, rsWin32Saver::kSaverRun);
    EXPECT_EQ(parseCommandLine(" - / s").mode, rsWin32Saver::kSaverRun);
}

TEST(CommandLine, EmptyAndNullConfigureWithoutAParent) {
    // The two configure forms differ in the parent they pass to the dialog, so
    // they must not collapse into one.
    EXPECT_EQ(parseCommandLine("").mode, rsWin32Saver::kSaverConfigureNoParent);
    EXPECT_EQ(parseCommandLine(0).mode, rsWin32Saver::kSaverConfigureNoParent);
    EXPECT_EQ(parseCommandLine("   ").mode, rsWin32Saver::kSaverConfigureNoParent);
    EXPECT_NE(parseCommandLine("/c").mode, parseCommandLine("").mode);
}

TEST(CommandLine, UnknownSwitchIsInvalid) {
    EXPECT_EQ(parseCommandLine("/x").mode, rsWin32Saver::kSaverInvalid);
    EXPECT_EQ(parseCommandLine("nonsense").mode, rsWin32Saver::kSaverInvalid);
}

TEST(CommandLine, OnlyTheFirstLetterMatters) {
    // Long-standing behaviour: "/config" is /c. Pinned so a future tightening
    // is a deliberate decision rather than an accident.
    EXPECT_EQ(parseCommandLine("/config").mode, rsWin32Saver::kSaverConfigureWithParent);
    EXPECT_EQ(parseCommandLine("/saver").mode, rsWin32Saver::kSaverRun);
}

TEST(CommandLine, PreviewArgPointsAtTheWindowId) {
    CommandLineAction a = parseCommandLine("/p 12345");
    ASSERT_EQ(a.mode, rsWin32Saver::kSaverPreview);
    ASSERT_NE(a.arg, (const char*)0);
    EXPECT_EQ(std::string(a.arg), "12345");
    EXPECT_EQ(parseUnsigned(a.arg), 12345u);
}

TEST(CommandLine, PreviewToleratesExtraSpacesAndNoSpace) {
    EXPECT_EQ(std::string(parseCommandLine("/p     987").arg), "987");
    EXPECT_EQ(std::string(parseCommandLine("/p987").arg), "987");
}

TEST(CommandLine, PreviewWithNoIdYieldsZeroWhichIsRejectedLater) {
    // startSaverPreview hands the result to IsWindow(), so a missing id must
    // parse to 0 rather than to something that looks like a real handle.
    CommandLineAction a = parseCommandLine("/p");
    ASSERT_EQ(a.mode, rsWin32Saver::kSaverPreview);
    EXPECT_EQ(parseUnsigned(a.arg), 0u);
}

// --- dispatch -------------------------------------------------------------
//
// runCommandLine reaches Win32 only through SaverOps, so a table of recording
// stubs stands in for the real entry points. Plain function pointers cannot
// capture, hence the file-scope log.

namespace {

int g_fakeWindowObject = 0;  // its address stands in for an HWND

struct DispatchLog {
    int foregroundCalls;
    int configureCalls;
    void* configureParent;
    int previewCalls;
    const char* previewArg;
    int runCalls;
    int windowedCalls;

    void reset() {
        foregroundCalls = configureCalls = previewCalls = 0;
        runCalls = windowedCalls = 0;
        configureParent = 0;
        previewArg = 0;
    }
    int totalOperations() const {
        return configureCalls + previewCalls + runCalls + windowedCalls;
    }
};

DispatchLog g_log;

void* fakeForeground() { g_log.foregroundCalls++; return &g_fakeWindowObject; }
int fakeConfigure(void* p) { g_log.configureCalls++; g_log.configureParent = p; return 11; }
int fakePreview(const char* a) { g_log.previewCalls++; g_log.previewArg = a; return 22; }
int fakeRun() { g_log.runCalls++; return 33; }
int fakeWindowed() { g_log.windowedCalls++; return 44; }

rsWin32Saver::SaverOps recordingOps() {
    g_log.reset();
    rsWin32Saver::SaverOps ops = {
        fakeForeground, fakeConfigure, fakePreview, fakeRun, fakeWindowed
    };
    return ops;
}

int dispatch(const char* cmdLine, rsWin32Saver::SaverOps& ops) {
    return rsWin32Saver::runCommandLine(parseCommandLine(cmdLine), ops);
}

}  // namespace

TEST(Dispatch, ConfigureWithParentUsesTheForegroundWindow) {
    rsWin32Saver::SaverOps ops = recordingOps();
    EXPECT_EQ(dispatch("/c", ops), 11);
    EXPECT_EQ(g_log.configureCalls, 1);
    EXPECT_EQ(g_log.foregroundCalls, 1);
    EXPECT_EQ(g_log.configureParent, &g_fakeWindowObject);
}

TEST(Dispatch, ConfigureWithoutArgumentsPassesNoParent) {
    // The one rule in the dispatch that is not a plain one-to-one mapping. If
    // these two modes ever collapse, the settings dialog silently changes which
    // window it belongs to.
    rsWin32Saver::SaverOps ops = recordingOps();
    EXPECT_EQ(dispatch("", ops), 11);
    EXPECT_EQ(g_log.configureCalls, 1);
    EXPECT_EQ(g_log.configureParent, (void*)0);
    EXPECT_EQ(g_log.foregroundCalls, 0) << "no parent means the foreground window is never asked for";
}

TEST(Dispatch, PreviewReceivesTheWindowId) {
    rsWin32Saver::SaverOps ops = recordingOps();
    EXPECT_EQ(dispatch("/p 12345", ops), 22);
    EXPECT_EQ(g_log.previewCalls, 1);
    ASSERT_NE(g_log.previewArg, (const char*)0);
    EXPECT_EQ(std::string(g_log.previewArg), "12345");
}

TEST(Dispatch, RunAndWindowedRouteToTheirOwnOperations) {
    rsWin32Saver::SaverOps ops = recordingOps();
    EXPECT_EQ(dispatch("/s", ops), 33);
    EXPECT_EQ(g_log.runCalls, 1);
    EXPECT_EQ(g_log.windowedCalls, 0);

    ops = recordingOps();
    EXPECT_EQ(dispatch("/w", ops), 44);
    EXPECT_EQ(g_log.windowedCalls, 1);
    EXPECT_EQ(g_log.runCalls, 0);
}

TEST(Dispatch, InvalidRunsNothingAndReturnsMinusOne) {
    rsWin32Saver::SaverOps ops = recordingOps();
    EXPECT_EQ(dispatch("/x", ops), -1);
    EXPECT_EQ(g_log.totalOperations(), 0);
}

TEST(Dispatch, EveryModeIsHandled) {
    // Walks the enum through the sentinel, so a mode added without a matching
    // case fails here instead of silently falling through to -1. Neither MSVC
    // at /W3 nor GCC without -Wall would warn about it.
    for (int m = 0; m < rsWin32Saver::kSaverModeCount; ++m) {
        rsWin32Saver::SaverOps ops = recordingOps();
        rsWin32Saver::CommandLineAction action;
        action.mode = (rsWin32Saver::SaverMode)m;
        action.arg = "0";

        const int result = rsWin32Saver::runCommandLine(action, ops);

        if (m == rsWin32Saver::kSaverInvalid) {
            EXPECT_EQ(result, -1) << "invalid must stay invalid";
            EXPECT_EQ(g_log.totalOperations(), 0);
        } else {
            EXPECT_EQ(g_log.totalOperations(), 1)
                << "mode " << m << " reached no operation - is there a case for it?";
            EXPECT_NE(result, -1) << "mode " << m << " fell through the switch";
        }
    }
}

TEST(ParseUnsigned, ReadsLeadingDigitsOnly) {
    EXPECT_EQ(parseUnsigned("0"), 0u);
    EXPECT_EQ(parseUnsigned("7"), 7u);
    EXPECT_EQ(parseUnsigned("65536"), 65536u);
    EXPECT_EQ(parseUnsigned("123abc"), 123u);
}

TEST(ParseUnsigned, NonNumericAndEmptyGiveZero) {
    EXPECT_EQ(parseUnsigned(""), 0u);
    EXPECT_EQ(parseUnsigned(0), 0u);
    EXPECT_EQ(parseUnsigned("abc"), 0u);
    EXPECT_EQ(parseUnsigned(" 12"), 0u);   // no leading-space tolerance, by design
    EXPECT_EQ(parseUnsigned("-5"), 0u);    // no sign handling, by design
}

TEST(FrameRateLimit, ResultIsAlwaysUsableAsADivisor) {
    // dFrameRateLimit is only ever divided into 1.0f after a non-zero check, so
    // every clamped value must be either 0 or a sane positive rate.
    const unsigned long inputs[] = {0UL, 1UL, 60UL, 1000UL, 1001UL, 0xFFFFFFFFUL};
    for (unsigned long in : inputs) {
        unsigned int out = clampFrameRateLimit(in);
        EXPECT_LE(out, kMaxFrameRateLimit);
        if (out != 0)
            EXPECT_GT(1.0f / float(out), 0.0f);
    }
}
