/*
 * Copyright (C) 1999-2010  Terence M. Welsh
 *
 * This file is part of rsWin32Saver.
 *
 * rsWin32Saver is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * rsWin32Saver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


 /*
  * Settings bounds and conversions for rsWin32Saver.
  *
  * Deliberately free of windows.h, so this logic can be unit tested without an
  * HWND or a registry. The registry reader and the frame rate slider both take
  * their bounds from here, so the two cannot drift apart.
  */

#ifndef RSWIN32SAVERSETTINGS_H
#define RSWIN32SAVERSETTINGS_H

namespace rsWin32Saver {

// 0 is the documented on-disk contract for "no limit", so it is a legal stored
// value rather than an out-of-range one. The upper bound matches what the frame
// rate slider can produce.
const unsigned int kMinFrameRateLimit = 0;
const unsigned int kMaxFrameRateLimit = 1000;


// Clamp an untrusted frame rate limit read from the registry.
//
// Takes unsigned long because the value arrives as a DWORD. Narrowing to int
// first would turn 0xFFFFFFFF into -1, which slips past a naive lower-bound
// check and then converts straight back to 4294967295 on assignment to the
// unsigned dFrameRateLimit.
//
// An oversized value clamps to the maximum rather than to 0, because 0 is not
// the smallest limit here - it means unlimited, which is the opposite of what
// an oversized limit is asking for.
inline unsigned int
clampFrameRateLimit(unsigned long v)
{
	if (v > (unsigned long)kMaxFrameRateLimit)
		return kMaxFrameRateLimit;
	return (unsigned int)v;
}


// What the command line asked the saver to do.
//
// Configure has two forms because they differ in the parent window they pass
// to the dialog: an explicit /c parents it to the foreground window, while a
// bare command line has no parent at all.
enum SaverMode
{
	kSaverConfigureWithParent,
	kSaverConfigureNoParent,
	kSaverPreview,
	kSaverRun,
	kSaverWindowed,
	kSaverInvalid,
	// Sentinel, keep last. Lets a test walk every mode, so a mode added here
	// without a matching case in runCommandLine fails rather than silently
	// falling through to -1 - neither MSVC at /W3 nor GCC without -Wall warns
	// about an unhandled enumerator.
	kSaverModeCount
};

struct CommandLineAction
{
	SaverMode mode;
	// For kSaverPreview only: points into the command line at the parent window
	// id. Null for every other mode.
	const char* arg;
};


// Decide what a screensaver command line is asking for.
//
// Windows passes these, so the accepted forms are not ours to choose: /c, /p
// <hwnd>, /s and /w, with '-' accepted for '/' and leading separators skipped.
// Only the first letter is examined, so "/config" is /c - that is long-standing
// behaviour and callers depend on it.
inline CommandLineAction
parseCommandLine(const char* cmdLine)
{
	CommandLineAction action;
	action.mode = kSaverInvalid;
	action.arg = 0;

	if (!cmdLine)
	{
		action.mode = kSaverConfigureNoParent;
		return action;
	}

	while (1)
	{
		switch (*cmdLine)
		{
			case ' ':  // skip over whitespace and switch characters
			case '-':
			case '/':
				cmdLine++;
				break;
			case 'c':  // open "settings" dialog box
			case 'C':
				action.mode = kSaverConfigureWithParent;
				return action;
			case '\0':
				action.mode = kSaverConfigureNoParent;
				return action;
			case 'p':  // run the preview
			case 'P':
				do
					cmdLine++;
				while (*cmdLine == ' ');
				action.mode = kSaverPreview;
				action.arg = cmdLine;
				return action;
			case 's':  // run the saver
			case 'S':
				action.mode = kSaverRun;
				return action;
			case 'w':  // run windowed saver, for debugging purposes
			case 'W':
				action.mode = kSaverWindowed;
				return action;
			default:
				action.mode = kSaverInvalid;
				return action;
		}
	}
}


// The operations a command line can dispatch to.
//
// Function pointers rather than direct calls, so the dispatch below can be
// exercised without a window station. Parent windows travel as void* to keep
// this header free of windows.h; WinMain casts them back to HWND.
struct SaverOps
{
	void* (*foregroundWindow)();
	int (*configure)(void* parent);
	int (*preview)(const char* windowId);
	int (*run)();
	int (*windowed)();
};


// Carry out what the command line asked for.
//
// The only rule here that is not a plain one-to-one mapping: an explicit /c
// parents the dialog to the foreground window, while a bare command line passes
// no parent at all. Collapsing those two would change which window the settings
// dialog belongs to.
inline int
runCommandLine(const CommandLineAction& action, const SaverOps& ops)
{
	switch (action.mode)
	{
		case kSaverConfigureWithParent:
			return ops.configure(ops.foregroundWindow());
		case kSaverConfigureNoParent:
			return ops.configure(0);
		case kSaverPreview:
			return ops.preview(action.arg);
		case kSaverRun:
			return ops.run();
		case kSaverWindowed:
			return ops.windowed();
		case kSaverInvalid:
		case kSaverModeCount:
			break;
	}

	return -1;
}


// Read a leading run of decimal digits, stopping at the first character that is
// not one. Empty and non-numeric input give 0.
//
// This is how the preview parent window id arrives. There is no overflow check
// and never was; the result is handed to IsWindow() before anything is done
// with it, which rejects a wrapped value along with every other bad one.
inline unsigned int
parseUnsigned(const char* str)
{
	unsigned int returnval = 0;

	if (!str)
		return 0;

	while ((*str >= '0') && (*str <= '9'))
		returnval = ((returnval * 10) + (unsigned int)(*str++ - '0'));

	return returnval;
}

}

#endif
