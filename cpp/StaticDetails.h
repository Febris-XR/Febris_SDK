// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef STATICDETAILS_H
#define STATICDETAILS_H
#endif

// EXPORT-SURFACE PORT (2026-08-29): mirror of the C# StaticDetails.StaticDetails,
// rebuilt against the CURRENT C# semantics (in-memory CurrentStatement, lazy
// ReferenceUUID, SIM-T13 G4 logger). The previous header declared bare statics
// with storage defined nowhere; C++17 inline statics give every member real
// storage from the header alone.

// SIM-T13 G4 mirror: process-wide log sink. C-ABI-friendly shape (plain function
// pointer) so a host game can route library failures into its own diagnostics
// through the exported surface. Implementations must never throw.
enum class SimulationLogLevel
{
	Debug = 0,
	Info = 1,
	Warn = 2,
	Error = 3,
};

using SimulationLogFn = void(*)(SimulationLogLevel level, const char* message, const char* detail);

class STATICDETAILS_H StaticDetails
{
public:
	//running the statement writing loop
	static constexpr int writeFrequency = 30000;//ms

	//timer
	inline static Stopwatch timer{};

	// Default sink, same format as the C# ConsoleSimulationLogger:
	// [FebrisSim][Level] message
	static void DefaultConsoleLog(SimulationLogLevel level, const char* message, const char* detail)
	{
		static const char* names[] = { "Debug", "Info", "Warn", "Error" };
		int idx = (int)level;
		if (idx < 0 || idx > 3) { idx = 3; }
		std::cout << "[FebrisSim][" << names[idx] << "] " << (message ? message : "");
		if (detail && detail[0] != '\0')
		{
			std::cout << " | " << detail;
		}
		std::cout << std::endl;
	}

	inline static SimulationLogFn Logger = &StaticDetails::DefaultConsoleLog;

	// C# Logger setter semantics: a null reset reverts to the console fallback
	// rather than leaving a null deref hazard for every catch in the library.
	static void SetLogger(SimulationLogFn fn)
	{
		Logger = fn ? fn : &StaticDetails::DefaultConsoleLog;
	}

	// Convenience wrapper the catch blocks use; swallows sink failures (the C#
	// logger contract is "never throw").
	static void Log(SimulationLogLevel level, const std::string& message, const std::string& detail = std::string())
	{
		try
		{
			Logger(level, message.c_str(), detail.c_str());
		}
		catch (...) {}
	}

	// The working statement (C# JObject CurrentStatement). ordered_json on
	// purpose: insertion order IS the wire key order, and the lifecycle write
	// path serializes this object straight into the {uuid}.json handoff file.
	inline static XApiJson::ojson CurrentStatement = XApiJson::ojson::object();

	// Guid.NewGuid mirror (boost random uuid, lowercase-hyphenated like C#).
	static std::string NewUuidString()
	{
		static boost::uuids::random_generator gen;
		return boost::uuids::to_string(gen());
	}

	// C# ReferenceUUID property mirror: lazy Guid on first read, the setter
	// only stores a changed value.
	static std::string GetReferenceUUID()
	{
		if (_referenceUUID.empty() || _referenceUUID == "00000000-0000-0000-0000-000000000000")
		{
			_referenceUUID = NewUuidString();
		}
		return _referenceUUID;
	}

	static void SetReferenceUUID(const std::string& value)
	{
		if (value != _referenceUUID)
		{
			_referenceUUID = value;
		}
	}

	inline static std::string _referenceUUID{};

	inline static std::shared_ptr<IEnvironmentHandler> Handler{};

	static void SetHandler(std::shared_ptr<IEnvironmentHandler> newHandler)
	{
		Handler = newHandler;
	}
};

#ifndef ANDROIDSTATEMENTPASSINGSTATICDETAILS_H
#define ANDROIDSTATEMENTPASSINGSTATICDETAILS_H
//#include "pch.h";
#endif
class ANDROIDSTATEMENTPASSINGSTATICDETAILS_H AndroidStatementPassingStaticDetails
{
public:
	///Intents for companion interactions
	static constexpr const char* StatementCreation = "com.febris.STATEMENT_CREATE";
	static constexpr const char* StatementUpdate = "com.febris.STATEMENT_UPDATE";
	static constexpr const char* StatementError = "com.febris.STATEMENT_ERROR";

	// The trailing typo is load-bearing API on the C# side; keep it verbatim.
	static constexpr const char* StatementBraodcastIntentUri = "com.febris.";
};

#ifndef ANDROIDINTENTCONST_H
#define ANDROIDINTENTCONST_H
//#include "pch.h";
#endif
class ANDROIDINTENTCONST_H AndroidIntentConst
{
public:
	static constexpr const char* IntentClass = "com.unity3d.player.UnityPlayer";
	static constexpr const char* IntentObject = "android.content.Intent";
	static constexpr const char* IntentSetAction = "setAction";
	static constexpr const char* IntentSetData = "setData";
	static constexpr const char* IntentStartActivity = "startActivity";
	static constexpr const char* SendBroadcast = "sendBroadcast";

	static constexpr const char* AndroidUriClass = "android.net.Uri";
	static constexpr const char* AndroidParseTag = "parse";
	static constexpr const char* PutExtraTag = "putExtra";

	/// <summary>
	/// Gathering arguments
	/// </summary>
	static constexpr const char* ArgumentExtraTag = "arguments";
	static constexpr const char* HasExtrasTag = "hasExtras";
	static constexpr const char* GetExtrasTag = "getExtras";
	static constexpr const char* GetStringTag = "getString";
	static constexpr const char* GetIntentTag = "getIntent";
	static constexpr const char* GetCurrentActivityTag = "currentActivity";
	static constexpr const char* UnityPlayerTag = "com.unity3d.player.UnityPlayer";

	//credentials
	static constexpr const char* IntentTag = "Intent";
	static constexpr const char* ReferenceUUIDIntentExtraTag = "ReferenceUUID";
	static constexpr const char* StatementJsonIntentExtraTag = "RawStatementJson";

	///Intents for companion interactions
	static constexpr const char* StatementCreation = "com.febris.STATEMENT_CREATE";
	static constexpr const char* StatementUpdate = "com.febris.STATEMENT_UPDATE";
	static constexpr const char* StatementError = "com.febris.STATEMENT_ERROR";

	static constexpr const char* StatementBroadcastIntentUri = "com.febris.";
};
