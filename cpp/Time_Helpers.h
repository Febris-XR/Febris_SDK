// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "pch.h"
#ifndef TIME_HELPERS_H
#define TIME_HELPERS_H
#define STOPWATCH_H
//#include "pch.h";
#endif

/// <summary>
/// To-Do: Need to convert commandlineargs to something useful
/// </summary>
class TIME_HELPERS_H Time_Helpers
{
public:

};

// EXPORT-SURFACE PORT (2026-08-29): System.Diagnostics.Stopwatch stand-in,
// header-only (the dead namespaced duplicate in Time_Helpers.cpp is gone; the
// old header declared methods with no state and no definitions anywhere).
// steady_clock on purpose: Stopwatch is monotonic, wall-clock adjustments must
// not bend durations. ElapsedMilliseconds returns long long to mirror the C#
// 'long Stopwatch.ElapsedMilliseconds' (the old declaration said double).
class STOPWATCH_H Stopwatch
{
public:
	void Start()
	{
		startTime = std::chrono::steady_clock::now();
		isRunning = true;
	}

	void Stop()
	{
		endTime = std::chrono::steady_clock::now();
		isRunning = false;
	}

	long long ElapsedMilliseconds()
	{
		auto elapsed = isRunning
			? std::chrono::steady_clock::now() - startTime
			: endTime - startTime;
		return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
	}

private:
	std::chrono::steady_clock::time_point startTime{};
	std::chrono::steady_clock::time_point endTime{};
	bool isRunning = false;
};
