// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"

// EXPORT-SURFACE PORT (2026-08-29): real global-scope definitions for the
// AndroidHandler declared in AndroidHandler.h, ported from the CURRENT C#
// Service/AndroidHandler.cs (SDKV-19/20 fix series). This TU previously held a
// dead same-named duplicate class inside namespace Service -- pre-refactor
// drift that was accidentally abstract (wrong signatures, no override) and
// could never even have been instantiated. Deleted. Not a semantic source.
//
// PORT NOTE on the extras payload: C# serializes the RawStatementJson extra via
// JObject.ToString(), which is INDENTED Newtonsoft output (2-space indent,
// ": " separators, Environment.NewLine = CRLF on Windows). The lifecycle
// parity harness byte-compares the extras, and the wire-parity rule
// (docs/OSS_SDK_DISTRIBUTION.md) demands byte-equivalent statement JSON, so
// the payload mirrors those bytes exactly: nlohmann dump(2) matches the
// Newtonsoft indented layout except for the newline character, which
// IndentedLikeNewtonsoft() rewrites to CRLF.

namespace
{
	std::string IndentedLikeNewtonsoft(const XApiJson::ojson& statement)
	{
		std::string text = statement.dump(2);
		std::string out;
		out.reserve(text.size() + text.size() / 16);
		for (char c : text)
		{
			if (c == '\n')
			{
				out += "\r\n";
			}
			else
			{
				out += c;
			}
		}
		return out;
	}
}

HandlerResult AndroidHandler::CreateInitialPost(XApiJson::ojson& statementFromDataModel)
{
	HandlerResult result{};
	try
	{
		// C# reads the lazy ReferenceUUID property here (no fresh GUID on the
		// Android path -- the getter creates one only on first touch).
		std::string referenceKey = StaticDetails::GetReferenceUUID();
		// PORT NOTE (JObject reference semantics): in C# this assignment ALIASES.
		// CurrentStatement and the caller's object become one JObject, so the id
		// stamped below lands on both. ojson assignment COPIES. The stamp still
		// reaches the caller's object (the ojson& parameter) and the intent extra
		// built from it, and StampStatementId is idempotent against the unchanged
		// ReferenceUUID, so the first UpdatePost (always called with
		// CurrentStatement itself, StatementHandler.cs line 69) re-stamps the
		// SAME id onto CurrentStatement.
		StaticDetails::CurrentStatement = statementFromDataModel;
		// FIX (SDKV-19/20): stamp the wire id before the JSON goes into
		// the intent extra so it matches the ReferenceUUID extra key.
		StatementHandler::StampStatementId(statementFromDataModel);
		// [Historical - C# region "this is one way but I don't think it is the
		// best way": a commented-out KeyValuePair<string,string>[3] variant.]

		// { key, value } -- rows in the same order the C# string[,] is filled.
		result.Extras.emplace_back(AndroidIntentConst::ReferenceUUIDIntentExtraTag, referenceKey);
		result.Extras.emplace_back(AndroidIntentConst::StatementJsonIntentExtraTag, IndentedLikeNewtonsoft(statementFromDataModel));
		result.HasExtras = true;
		// [C# testing region: Console.WriteLine(outputArray) prints only the
		// array TYPE NAME in C#, no data. Not mirrored as output.]
		result.Ready = true;
	}
	catch (const std::exception& e)
	{
		// SIM-T13 G4: route through ISimulationLogger so the host game
		// can see the failure. Re-throw preserved -- callers depend on it.
		StaticDetails::Log(SimulationLogLevel::Error, "AndroidHandler: failed building Intent extras for outgoing broadcast.", e.what());
		throw;
	}
	return result;
}

HandlerResult AndroidHandler::UpdatePost(XApiJson::ojson& statementFromDataModel)
{
	HandlerResult result{};
	try
	{
		std::string referenceKey = StaticDetails::GetReferenceUUID();
		// FIX (SDKV-19/20): stamp the wire id before the JSON goes into
		// the intent extra. Idempotent, so every update re-emission of
		// the run carries the SAME id (retry-safe on the ingest node).
		StatementHandler::StampStatementId(statementFromDataModel);

		// [Historical - C# region "this is one way but I don't think it is the
		// best way": a commented-out KeyValuePair<string,string>[3] variant.]

		// This is really just a key value pair { key, value }
		result.Extras.emplace_back(AndroidIntentConst::ReferenceUUIDIntentExtraTag, referenceKey);
		result.Extras.emplace_back(AndroidIntentConst::StatementJsonIntentExtraTag, IndentedLikeNewtonsoft(statementFromDataModel));
		result.HasExtras = true;

		// [C# testing region: Console.WriteLine(outputArray), type name only. Not mirrored.]

		result.Ready = true;
	}
	catch (const std::exception& e)
	{
		// SIM-T13 G4: route through ISimulationLogger so the host game
		// can see the failure. Re-throw preserved -- callers depend on it.
		StaticDetails::Log(SimulationLogLevel::Error, "AndroidHandler: failed building Intent extras for outgoing broadcast.", e.what());
		throw;
	}
	return result;
}

HandlerResult AndroidHandler::ErrorPost(XApiJson::ojson& statementFromDataModel)
{
	HandlerResult result{};
	try
	{
		std::string referenceKey = StaticDetails::GetReferenceUUID();
		// FIX (SDKV-19/20): error emissions carry the stamped id too.
		StatementHandler::StampStatementId(statementFromDataModel);
		// Key and value
		result.Extras.emplace_back(AndroidIntentConst::ReferenceUUIDIntentExtraTag, referenceKey);
		result.Extras.emplace_back(AndroidIntentConst::StatementJsonIntentExtraTag, IndentedLikeNewtonsoft(statementFromDataModel));
		result.HasExtras = true;

		// [C# testing region: Console.WriteLine(outputArray), type name only. Not mirrored.]

		result.Ready = true;
	}
	catch (const std::exception& e)
	{
		// SIM-T13 G4: route error-emission failures through the logger.
		StaticDetails::Log(SimulationLogLevel::Error, "AndroidHandler.ErrorPost: failed building error-intent extras.", e.what());
		throw;
	}
	return result;
}
