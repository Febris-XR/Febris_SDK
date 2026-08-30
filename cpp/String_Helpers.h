// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef STRING_HELPERS_H
#define STRING_HELPERS_H
//#include "pch.h";
#endif 


/// <summary>
/// To-Do: Need to convert commandlineargs to something useful
/// </summary>
class STRING_HELPERS_H String_Helpers
{
public:

	static bool IsNullOrEmpty(const std::string& str);
	//static vector<string> GetCommandLineArgs(int argc, char* argv[]);
	static vector<string> GetCommandLineArgs(char* argv[]);
	// Inline definition hoisted from the .cpp (parity probe). The .cpp defines
	// its methods inside a namespaced duplicate class, so this header class was
	// never linkable -- the DLL only built because it exports nothing and the
	// linker dead-strips the lot. Same disease StatementFactoring had.
	static string Replace(std::string str, const std::string& old_str, const std::string& new_str)
	{
		size_t pos = 0;
		while ((pos = str.find(old_str, pos)) != std::string::npos) {
			str.replace(pos, old_str.length(), new_str);
			pos += new_str.length();
		}
		return str;
	};
	static bool StartsWith(const std::string& str, const std::string& prefix)
	{
		return str.size() >= prefix.size() &&
			str.compare(0, prefix.size(), prefix) == 0;
	};
	static void ChangePropertiesToLowerCase(json& jsonObject);
	static json ChangeStringToObject(string input);
	//static vector<string> GetCommandLineArgs();
	// Hoisted like Replace above (export-surface link, 2026-08-29). The dead
	// duplicate's body kept its result in a function-local static vector -- a
	// stale-state/reentrancy hazard with no benefit since the return is by
	// value -- so the hoist drops the static; callers see identical output.
	static vector<string> Split(string& input, char splitChar)
	{
		std::vector<std::string> result;

		std::string::size_type startPos = 0;
		std::string::size_type endPos = input.find(splitChar);

		while (endPos != std::string::npos) {
			std::string token = input.substr(startPos, endPos - startPos);
			result.push_back(token);

			startPos = endPos + 1;
			endPos = input.find(splitChar, startPos);
		}

		// add the last token (or the whole string if no splitChar is found)
		std::string token = input.substr(startPos);
		result.push_back(token);

		return result;
	};
};