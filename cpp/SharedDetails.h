// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef SHAREDDETAILS_H
#define SHAREDDETAILS_H
//#include "pch.h";
#endif 

// EXPORT-SURFACE PORT (2026-08-29): mirror of the C# SharedDetails.SharedDetails,
// header-only. The previous header declared bare statics whose storage existed
// nowhere (the .cpp defined a dead namespaced duplicate instead), so nothing
// referencing these members could ever have linked. C++17 inline statics /
// constexpr give every member real storage from the header alone. Member order
// follows the C# source.

class SHAREDDETAILS_H SharedDetails
{
public:
	//public static string videoName;
	inline static bool SimulationIsRunning = false;

	//argument constants (C# const string / const int pairs; each length is the
	//character count of the preface right above it)
	inline static const std::string StatementPreface = "-febrisData=";
	static constexpr int StatementPrefaceLength = 12;
	inline static const std::string VideoDataPreface = "-videoData=";
	static constexpr int VideoDataPrefaceLength = 11;
	inline static const std::string SimulationProcessIdPreface = "-simulationProcessId=";
	static constexpr int SimulationProcessIdPrefaceLength = 21;
	inline static const std::string SimulationProcessName = "-simulationProcessName=";
	static constexpr int SimulationProcessNameLength = 23;
};
