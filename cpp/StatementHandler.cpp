// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
//#include "StatementHandler.h"
#include "pch.h"
// FIX (SIM-B7): cmath needed for std::isnan and std::isinf used in AutoUpdateScaledScore division guard. See docs/MODERNIZATION/SIM_MODERNIZATION.md.
#include <cmath>
#include <charconv>

// EXPORT-SURFACE PORT (2026-08-29): global-scope definitions for the class declared
// in StatementHandler.h, ported line-for-line from the CURRENT C#
// Statement\StatementHandler.cs (Android-11 in-memory state model + the SDKV /
// SIM-T13 / MDM-T2 fix series). The dead 'namespace FebrisCppStatement' duplicate
// that used to fill this file (2000 lines of pre-refactor disk-backed drift:
// GetJObject / SaveStatement / WriteToDataFile) is DELETED -- it predated the C#
// refactor and no call ever linked against it.
//
// FIX (SIM-B2): all catch clauses are std::exception (never std::runtime_error) so
// json::exception and the other std::exception derivatives are caught instead of
// escaping. C# swallow-and-log sites keep their exact Console/Logger text.
//
// Working statement type is XApiJson::ojson (nlohmann::ordered_json) -- key order
// IS the wire contract. REFERENCE SEMANTICS: every C# 'JObject statement =
// StaticDetails.CurrentStatement;' alias becomes 'XApiJson::ojson& statement =
// StaticDetails::CurrentStatement;' (reference, never a copy), and the C# trailing
// 'StaticDetails.CurrentStatement = statement;' write-backs become no-op comments
// (same shared instance).

// ---------------------------------------------------------------------------
// File-local Newtonsoft JObject/JToken behavior mirrors.
//
// nlohmann's non-const operator[] silently CREATES missing intermediates, but the
// C# JObject indexer chain throws (NullReferenceException on a missing hop,
// InvalidOperationException indexing a null/value token) into the surrounding
// catch -- that throw-into-catch is load-bearing (see the SDKV-4 note: the typo
// key made every context-activity update a silent no-op, not a node-creating
// write). The helpers below reproduce the C# outcomes.
//
// One residual difference, accepted: C# distinguishes a MISSING hop (NRE at the
// next index) from a PRESENT null hop (InvalidOperationException) -- both throw
// here too, just with one exception type.
// ---------------------------------------------------------------------------

// The C# 'null JToken' stand-in handed out for absent leaves.
static const XApiJson::ojson NullToken;

// C# JObject chained-indexer HOP mirror (write side): throws when the node is
// missing or not an object, exactly where the C# chain would throw.
static XApiJson::ojson& TokenHop(XApiJson::ojson& parent, const char* key)
{
	XApiJson::ojson& node = parent.at(key);  // missing key: throws, like the C# null-ref hop
	if (!node.is_object())
	{
		// C#: indexing a null/value token raises InvalidOperationException
		throw std::runtime_error(std::string("statement node '") + key + "' is not an object");
	}
	return node;
}

// Read-side overload of the same mirror.
static const XApiJson::ojson& TokenHop(const XApiJson::ojson& parent, const char* key)
{
	const XApiJson::ojson& node = parent.at(key);
	if (!node.is_object())
	{
		throw std::runtime_error(std::string("statement node '") + key + "' is not an object");
	}
	return node;
}

// C# leaf read mirror: JObject["key"] hands back a null JToken when the key is
// absent (no throw). Token-PRESENCE checks (the Newtonsoft '!= null' reference
// gotcha) map to !LeafToken(...).is_null() ... except where C# really relies on
// presence-only (see ActorIsCorrect), which uses contains() directly.
static const XApiJson::ojson& LeafToken(const XApiJson::ojson& parent, const char* key)
{
	if (!parent.is_object() || !parent.contains(key))
	{
		return NullToken;
	}
	return parent.at(key);
}

// Newtonsoft (string) cast mirror: null -> C# null (empty string here), string ->
// raw value, other scalars stringify, structured tokens throw (InvalidCastException).
static std::string CastStringOrEmpty(const XApiJson::ojson& token)
{
	if (token.is_null()) { return std::string(); }
	if (token.is_string()) { return token.get<std::string>(); }
	if (token.is_structured())
	{
		throw std::runtime_error("cannot cast a structured JSON token to string");
	}
	return token.dump();
}

// JToken.ToString() mirror: null token -> "", string -> raw value, other scalars ->
// their JSON text. (C# pretty-prints structured tokens; the working statement never
// feeds one through here, so dump() stands in.)
static std::string TokenText(const XApiJson::ojson& token)
{
	if (token.is_null()) { return std::string(); }
	if (token.is_string()) { return token.get<std::string>(); }
	return token.dump();
}

// C# float.ToString() mirror for log text: "NaN" / "Infinity" / shortest
// round-trip digits ("0", "1.5"), not printf's zero-padded "%f".
static std::string FloatText(float value)
{
	if (std::isnan(value)) { return "NaN"; }
	if (std::isinf(value)) { return value < 0.0f ? "-Infinity" : "Infinity"; }
	char buf[64];
	auto res = std::to_chars(buf, buf + sizeof(buf) - 1, value);
	*res.ptr = '\0';
	return std::string(buf);
}

// C# token?["key"] null-conditional mirror (AutoUpdateScaledScore's chain): an
// ABSENT hop yields nullptr and short-circuits; a PRESENT non-object hop still
// throws, because Newtonsoft's ?[] only guards null references, not invalid
// indexing -- a JSON-null result node lands in the catch on both sides.
static const XApiJson::ojson* ConditionalIndex(const XApiJson::ojson* parent, const char* key)
{
	if (parent == nullptr) { return nullptr; }
	if (!parent->is_object())
	{
		throw std::runtime_error(std::string("cannot index a non-object statement node with '") + key + "'");
	}
	if (!parent->contains(key)) { return nullptr; }
	return &parent->at(key);
}

// Int32.TryParse mirror: full-string match, sign allowed, no partial-prefix
// acceptance (std::stoi alone would accept "5abc"). C# also tolerates surrounding
// whitespace; extensionmap entries never carry any, so that nuance is dropped.
static bool TryParseInt32(const std::string& text, int& value)
{
	try
	{
		size_t consumed = 0;
		int parsed = std::stoi(text, &consumed);
		if (consumed != text.size()) { return false; }
		value = parsed;
		return true;
	}
	catch (const std::exception&)
	{
		return false;
	}
}

///Methods Developers can use to update Statements and trigger different actions
// #region External
// #region Returns values for use

/// <summary>
/// SIM-T13 G3 dispatch overload -- same as GetSendableUpdate but returns a typed
/// SimulationDispatch so the host (Unity glue, Unreal glue, future engine
/// integrations) does not have to remember the intent action by which library
/// method was called. The dispatch carries IntentAction =
/// "com.febris.STATEMENT_UPDATE" for Android; the host fires it mechanically.
/// (C# routes through the IEnvironmentHandler.UpdateDispatchAsync extension, which
/// wraps the legacy UpdatePost tuple + bakes in the StatementUpdate action; the
/// C++ handler surface is the synchronous HandlerResult UpdatePost, wrapped here.)
/// </summary>
SimulationDispatch StatementHandler::GetSendableDispatch()
{
	try
	{
		if (StaticDetails::Handler == nullptr)
		{
			// C#: a null handler raises NullReferenceException into this same catch;
			// calling through a null shared_ptr is undefined behavior, so surface it
			// as the exception the mirror expects.
			throw std::runtime_error("StaticDetails::Handler is null");
		}
		HandlerResult legacy = StaticDetails::Handler->UpdatePost(StaticDetails::CurrentStatement);
		SimulationDispatch dispatch;
		dispatch.Ready = legacy.Ready;
		dispatch.IntentAction = AndroidStatementPassingStaticDetails::StatementUpdate;
		dispatch.HasExtras = legacy.HasExtras;
		dispatch.Extras = legacy.Extras;
		return dispatch;
	}
	catch (const std::exception& e)
	{
		StaticDetails::Log(SimulationLogLevel::Error,
			"StatementHandler.GetSendableDispatch: handler.UpdateDispatchAsync failed.", e.what());
		SimulationDispatch dispatch;
		dispatch.Ready = false;
		dispatch.IntentAction = AndroidStatementPassingStaticDetails::StatementUpdate;
		// C#: extras null -> the SimulationDispatch ctor stores an empty dictionary;
		// HasExtras stays false to preserve the legacy null shape for the glue.
		return dispatch;
	}
}

/// <summary>
/// This needs to be used on update loops. This will cause the system to either
/// write to a shared file system or creates a way of posting to the controlling
/// application
/// </summary>
HandlerResult StatementHandler::GetSendableUpdate()
{
	HandlerResult result{};  // C#: bool ready = false; string[,] outputArray = default;
	try
	{
		if (StaticDetails::Handler == nullptr)
		{
			// C#: null handler -> NullReferenceException into this catch (see
			// GetSendableDispatch for why the guard is explicit here).
			throw std::runtime_error("StaticDetails::Handler is null");
		}
		result = StaticDetails::Handler->UpdatePost(StaticDetails::CurrentStatement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error getting sendable statement: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
	return result;
}

HandlerResult StatementHandler::EndSimulation()
{
	HandlerResult result{};  // C#: bool ready = false; string[,] outputArray = default;
	try
	{
		// REFERENCE SEMANTICS: C# 'JObject statement = CurrentStatement' aliases the
		// shared statement -- bind a reference, never copy.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		//calculate passing /complete?

		//update verb
		const XApiJson::ojson& cstatement = statement;
		string currentVerb = TokenText(TokenHop(cstatement, "verb").at("id"));  // C#: statement["verb"]["id"].ToString()
		VerbEnums currentVerbEnum = VerbIRIResolver::GetVerbEnum(currentVerb);
		VerbUpdate(statement, currentVerbEnum);  // C#: statement = VerbUpdate(statement, enum) -- same instance handed back
		// [Historical] duplicate verb-update + WriteToDataFile block elided (pre-refactor)
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance, no-op here

		result = GetSendableUpdate();
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing the final simulation statement: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
	return result;
}

HandlerResult StatementHandler::EndSimulation(bool success, bool complete, float rawScore, long long durationMs)
{
	HandlerResult result{};  // C#: bool ready = false; string[,] outputArray = default;
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;

		//update complete
		UpdateCompletionStatus(statement, complete);
		//update success
		UpdateSuccessStatus(statement, success);
		//update result
		UpdateDuration(statement, durationMs);
		//update score
		UpdateRawScore(statement, rawScore);
		//update verb
		const XApiJson::ojson& cstatement = statement;
		string currentVerb = TokenText(TokenHop(cstatement, "verb").at("id"));  // C#: statement["verb"]["id"].ToString()
		VerbEnums currentVerbEnum = VerbIRIResolver::GetVerbEnum(currentVerb);
		VerbUpdate(statement, currentVerbEnum);
		//write statement
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance, no-op here

		result = GetSendableUpdate();
		//bool written = JSONHandler.WriteToDataFile(statement); // [Historical] disk-era write
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing the final simulation statement: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
	return result;
}

// #endregion

// #region One way calls
// #region Specific calls

/// <summary>
/// this method is used to finish the simulation and finalize the JSON Statement.
/// FIX (SDKV-3/11): result.completion is written as a JSON Boolean --
/// the old lowercase-string "true" violated xAPI 1.0.3 section 4.1.5.
/// </summary>
void StatementHandler::SimulationComplete()
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		TokenHop(statement, "result")["completion"] = true;
		//statement["result"]["completion"] = "true"; // [Historical - SDKV-3] string-typed boolean
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance, no-op here
		//tally up score so it can set up success

		//
		// NOTE (SIM-B9): the C# blocks on GetSendableUpdate().Result here (sync-over-async
		// on the game thread), together with the caller rewiring in FebrisScriptManager.cs;
		// needs an async refactor of SimulationComplete and pairs with SIM-M3 (non-blocking
		// queue). Deferred per do-not-change-functionality. The C++ SDK is synchronous, so
		// the plain call carries the same behavior. See docs/MODERNIZATION/SIM_MODERNIZATION.md.
		HandlerResult sendable = GetSendableUpdate();
		(void)sendable;  // C# discards the (ready, outputArray) tuple the same way
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing passed simulation statement: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

/// <summary>
/// Marks the simulation's result.success flag.
/// FIX (SDKV-3/11): written as a JSON Boolean -- the old
/// lowercase-string "true"/"false" violated xAPI 1.0.3 section 4.1.5.
/// </summary>
void StatementHandler::SimulationPassed(bool input)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		TokenHop(statement, "result")["success"] = input;
		//statement["result"]["success"] = input.ToString().ToLower(); // [Historical - SDKV-3] string-typed boolean
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance, no-op here
		//tally up score so it can set up success

		//
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing passed simulation statement: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

/// <summary>
/// Updates the duration of the simulation (C# TimeSpan -> milliseconds)
/// </summary>
void StatementHandler::DurationUpdate(long long input)
{
	try
	{
		UpdateStatement(XAPIProperties::Result, ResultOptions::Duration, input);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error Duration Update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

/// <summary>
/// Every time a stage is restarted this function should be called.
/// </summary>
void StatementHandler::StageRestart()
{
	try
	{
		// REFERENCE SEMANTICS: shared statement; only read here (the write goes
		// through UpdateStatement below).
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		int count = 0;
		try
		{
			// The extensionmap is a STRING holding serialized entries, not a JSON object.
			const XApiJson::ojson& cstatement = statement;
			string extensionMap = CastStringOrEmpty(
				LeafToken(TokenHop(TokenHop(cstatement, "result"), "extensions"), "extensionmap"));
			if (extensionMap != "")
			{
				vector<string> extensionMapArray = String_Helpers::Split(extensionMap, ',');
				string key = ExtensionIRIResolver::ResolveExtensionIRI(ExtensionIRIOptions::RestartCounterIRI);
				for (size_t i = 0; i < extensionMapArray.size(); i++)
				{
					vector<string> extensionSingle = String_Helpers::Split(extensionMapArray[i], ':');
					// MDM-T2 C-03 / BUGS.md: bounds-guard the indexer access + int parse.
					// Previously `extensionSingle[2]` and `Int32.Parse` would throw
					// IndexOutOfRange / FormatException on any malformed entry,
					// killing the whole stage-restart counter update.
					if (extensionSingle.size() < 3)
					{
						StaticDetails::Log(SimulationLogLevel::Warn,
							"StageRestart: skipping malformed extensionmap entry (need 3 colon-separated parts, got " +
							std::to_string(extensionSingle.size()) + "): " + extensionMapArray[i]);
						continue;
					}
					if (extensionSingle[0] + ":" + extensionSingle[1] == key)
					{
						if (!TryParseInt32(extensionSingle[2], count))
						{
							StaticDetails::Log(SimulationLogLevel::Warn,
								"StageRestart: extensionmap counter value isn't a valid int32: " + extensionSingle[2]);
							count = 0;
						}
					}
				}
			}
		}
		catch (const std::exception& e)
		{
			StaticDetails::Log(SimulationLogLevel::Error,
				"StageRestart: unexpected exception parsing extensionmap.", e.what());
		}
		count++;
		UpdateStatement(XAPIProperties::Result, ResultOptions::Extensions, ResultExtensionOptions::RestartCounter, count);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on stage restart counter: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

/// <summary>
/// This is the simple way to add notes.
/// </summary>
void StatementHandler::AddResultNote(string note)
{
	try
	{
		// C# takes a CurrentStatement alias here and writes it back unchanged around
		// the call -- a reference-type no-op; nothing to copy in C++.
		UpdateStatement(XAPIProperties::Result, ResultOptions::Extensions, ResultExtensionOptions::Notes, note);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error add result note: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}
// #endregion

// #region Direct Statement Updates

void StatementHandler::UpdateStatement(XAPIProperties property, ResultOptions resultType, bool input)
{
	try
	{
		switch (property)
		{
		case XAPIProperties::Result:
			//need more information for this one
			Result(input, resultType);
			break;
		case XAPIProperties::Score:
			break;
		default:
			// SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
			// so silent no-ops become visible. The Result/Context/Attachments cases
			// above are the only branches that do anything; everything else hits
			// here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
			// a Warn line instead of a black hole.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on statement update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::UpdateStatement(XAPIProperties property, ResultOptions resultType, string input)
{
	try
	{
		switch (property)
		{
		case XAPIProperties::Result:
			//need more information for this one
			Result(input, resultType);
			break;
		case XAPIProperties::Score:
			break;
		default:
			// SIM-T13 G6: surface unsupported XAPIProperties routes through the logger.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on statement update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::UpdateStatement(XAPIProperties property, ResultOptions resultType, float input)
{
	try
	{
		switch (property)
		{
		case XAPIProperties::Result:
			Result(input, resultType);
			break;
		default:
			// SIM-T13 G6: surface unsupported XAPIProperties routes through the logger.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on statement update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

// C# UpdateStatement(XAPIProperties, ResultOptions, TimeSpan)
void StatementHandler::UpdateStatement(XAPIProperties property, ResultOptions resultType, long long durationMs)
{
	try
	{
		switch (property)
		{
		case XAPIProperties::Result:
			Result(durationMs, resultType);
			break;
		default:
			// SIM-T13 G6: surface unsupported XAPIProperties routes through the logger.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on statement update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::UpdateStatement(XAPIProperties property, ResultOptions resultType, ResultExtensionOptions resultExtensionType, int input)
{
	try
	{
		switch (property)
		{
		case XAPIProperties::Result:
			if (resultType == ResultOptions::Extensions)
			{
				Result(input, resultType, resultExtensionType);
			}
			break;
		default:
			// SIM-T13 G6: surface unsupported XAPIProperties routes through the logger.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on statement update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::UpdateStatement(XAPIProperties property, ResultOptions resultType, ResultExtensionOptions resultExtensionType, string input)
{
	try
	{
		switch (property)
		{
		case XAPIProperties::Result:
			if (resultType == ResultOptions::Extensions)
			{
				Result(input, resultType, resultExtensionType);
			}
			break;
		default:
			// SIM-T13 G6: surface unsupported XAPIProperties routes through the logger.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on statement update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}
// #endregion

// #region attachment handling interface

void StatementHandler::UpdateStatement(XAPIProperties property, string usageType, string display, string description, ContentType contentType, int length, string sha2, string fileUrl)
{
	try
	{
		switch (property)
		{
		case XAPIProperties::Attachments:
			AddAttachment(usageType, display, description, contentType, length, sha2, fileUrl);
			break;
		default:
			// SIM-T13 G6: surface unsupported XAPIProperties routes through the logger.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error updating statement: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::UpdateStatement(XAPIProperties property, string usageType, string display, string description, ContentType contentType, int length, string sha2)
{
	try
	{
		switch (property)
		{
		case XAPIProperties::Attachments:
			AddAttachment(usageType, display, description, contentType, length, sha2);
			break;
		default:
			// SIM-T13 G6: surface unsupported XAPIProperties routes through the logger.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error updating statement: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::UpdateStatement(XAPIProperties property, string usageType, string display, ContentType contentType, int length, string sha2, string fileUrl)
{
	try
	{
		switch (property)
		{
		case XAPIProperties::Attachments:
			AddAttachment(usageType, display, contentType, length, sha2, fileUrl);
			break;
		default:
			// SIM-T13 G6: surface unsupported XAPIProperties routes through the logger.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error updating statement: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::UpdateStatement(XAPIProperties property, string usageType, string display, ContentType contentType, int length, string sha2)
{
	try
	{
		switch (property)
		{
		case XAPIProperties::Attachments:
			AddAttachment(usageType, display, contentType, length, sha2);
			break;
		default:
			// SIM-T13 G6: surface unsupported XAPIProperties routes through the logger.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error updating statement: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}
// #endregion

// #region context interface

void StatementHandler::UpdateStatement(XAPIProperties property, ContextOptions contextOption, string input)
{
	try
	{
		switch (property)
		{
		case XAPIProperties::Context:
			Context(contextOption, input);
			break;
		default:
			// SIM-T13 G6: surface unsupported XAPIProperties routes through the logger.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on statement update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::UpdateStatement(XAPIProperties property, ContextOptions contextOption, ContextActivitesOptions contextActivitesOption, string input)
{
	try
	{
		switch (property)
		{
		case XAPIProperties::Context:
			Context(contextOption, contextActivitesOption, input);
			break;
		default:
			// SIM-T13 G6: surface unsupported XAPIProperties routes through the logger.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on statement update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::UpdateStatement(XAPIProperties property, ContextOptions contextOption, ContextStatementReferenceOptions contextStatementReferenceOption, string input)
{
	try
	{
		switch (property)
		{
		case XAPIProperties::Context:
			Context(contextOption, contextStatementReferenceOption, input);
			break;
		default:
			// SIM-T13 G6: surface unsupported XAPIProperties routes through the logger.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on statement update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::UpdateStatement(XAPIProperties property, ContextOptions contextOption, ContextExtensionOptions contextExtensionOption, string input)
{
	try
	{
		switch (property)
		{
		case XAPIProperties::Context:
			Context(contextOption, contextExtensionOption, input);
			break;
		default:
			// SIM-T13 G6: surface unsupported XAPIProperties routes through the logger.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on statement update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}
// #endregion

// #region Custom Verb Methods
///Use a custom verb that was previously setup in the portal. This is not selectable in the Febris Enum stock selections.
/// It is utilized through the URI generated on creation.
/// Please use that Uri string if you would like to use a custom Verb
void StatementHandler::CustomVerbUpdate(string uriInput)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		TokenHop(statement, "verb")["id"] = uriInput;
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance, no-op here
		//tally up score so it can set up success

		//
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on custom verb update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}
// #endregion
// #endregion
// #endregion


///Internal Methods that should not be able to be utilized directly
// #region Internal

bool StatementHandler::StatementCheck(const XApiJson::ojson& input)
{
	bool isCorrect = false;
	try
	{
		//Make sure required items exist
		bool containsActor = false;
		bool containsObject = false;
		bool containsVerb = false;
		if (input.contains("actor") || input.contains("Actor"))
		{
			containsActor = true;
		}
		if (input.contains("object") || input.contains("Object"))
		{
			containsObject = true;
		}
		if (input.contains("verb") || input.contains("Verb"))
		{
			containsVerb = true;
		}
		if (false == containsActor || false == containsObject || false == containsVerb)
		{
			return isCorrect;
		}

		//check to make sure they fit xAPI
		// (C# indexes with the lowercase keys only; an upper-cased-only key passed the
		// Contains gate but hands a null token to the validator -- LeafToken mirrors that.)
		bool actorIsCorrect = StatementHandler::ActorIsCorrect(LeafToken(input, "actor"));
		bool verbIsCorrect = StatementHandler::VerbIsCorrect(LeafToken(input, "verb"));
		bool objectIsCorrect = StatementHandler::ObjectIsCorrect(LeafToken(input, "object"));
		if (false == actorIsCorrect || false == verbIsCorrect || false == objectIsCorrect)
		{
			return isCorrect;
		}
		else
		{
			isCorrect = true;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on statement check: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
	return isCorrect;
}

// #region Statement id stamping (SDKV-19/20)
/// <summary>
/// FIX (SDKV-19/20): stamps the handoff ReferenceUUID into the statement's wire id
/// so every emitted statement carries an xAPI 1.0.3 section 4.1.1 statement id (a
/// UUID string). The ingest node dedupes retries on this wire id (it reads "id"
/// then "uuid" and ignores the "0" / empty-GUID placeholders the typed Statement
/// defaults used to emit), so host retries only become idempotent once the SDK
/// stamps a real one. Idempotent by design: an id that is already a valid,
/// non-empty GUID is left untouched, so re-serializing / re-emitting the same
/// statement (a retry, or every UpdatePost of the run) carries the SAME id.
/// Placeholders (missing, null, empty, the long-typed model default "0", the empty
/// GUID, or any non-GUID junk) are replaced with StaticDetails::GetReferenceUUID()
/// -- the same GUID used for the {uuid}.json handoff FILENAME on the PC path and
/// the ReferenceUUID intent extra on Android, so the handoff key and the wire id
/// always agree.
/// (C#'s null-input return-null branch is dropped: ojson has value semantics here,
/// callers always hold a real node; the C# instance-return-for-chaining becomes
/// mutate-in-place.)
/// </summary>
void StatementHandler::StampStatementId(XApiJson::ojson& statement)
{
	try
	{
		const XApiJson::ojson* idToken = statement.contains("id") ? &statement.at("id") : nullptr;
		bool currentIdIsNull = (idToken == nullptr || idToken->is_null());
		std::string currentId = currentIdIsNull ? std::string() : TokenText(*idToken);
		// Guid.TryParse mirror: boost string_generator inside try/catch; the nil
		// guid plays Guid.Empty.
		bool alreadyStamped = false;
		if (!currentIdIsNull)
		{
			try
			{
				GUID2 parsedId = GuidGen(currentId);
				alreadyStamped = !parsedId.is_nil();
			}
			catch (const std::exception&)
			{
				alreadyStamped = false;
			}
		}
		if (!alreadyStamped)
		{
			statement["id"] = StaticDetails::GetReferenceUUID();
		}
	}
	catch (const std::exception& e)
	{
		StaticDetails::Log(SimulationLogLevel::Error,
			"StampStatementId: failed stamping the statement id onto the outgoing statement.", e.what());
	}
}
// #endregion

// #region Actor
/// <summary>
/// Check if Actor exists
/// </summary>
bool StatementHandler::ActorIsCorrect(const XApiJson::ojson& input)
{
	bool actorContainsNeededItems = false;
	try
	{
		//check if token is null
		if (input.is_null())
		{
			return false;
		}
		if (!input.is_object())
		{
			// C#: indexing a value token below raises InvalidOperationException into the catch
			throw std::runtime_error("actor token is not an object");
		}
		//variables

		//test if id is already in system -- this SHOULD ALWAYS be the case.

		//Check identifiers

		//Actor has needed items
		// NOTE: the C# checks are `input["uuid"] != null` etc. -- Newtonsoft REFERENCE
		// comparisons, i.e. token-PRESENCE checks (a present JSON null still passes).
		// contains() is the exact mirror.
		if (input.contains("uuid"))
		{
			actorContainsNeededItems = true;
		}
		else if (input.contains("mbox") || input.contains("mbox_sha1sum") || input.contains("openid"))
		{
			actorContainsNeededItems = true;
		}
		else if (input.contains("account"))
		{
			actorContainsNeededItems = StatementHandler::AccountIsCorrect(input.at("account"));
		}
		else if (input.contains("member"))
		{
			// MDM-T2 / BUGS.md: copy-paste bug fixed -- was passing input["account"]
			// into MemberIsCorrect, which validated the wrong actor sub-field. Group
			// actors (xAPI's `member` array shape) silently passed validation against
			// the unrelated `account` field. See
			// docs/AUDIT_MDM_INTEGRATION_ERROR_HANDLING.md gap C-02 / M-10.
			actorContainsNeededItems = StatementHandler::MemberIsCorrect(input.at("member"));
		}
	}
	catch (const std::exception& e)
	{
		StaticDetails::Log(SimulationLogLevel::Error, "ActorIsCorrect: validation threw.", e.what());
	}

	return actorContainsNeededItems;
}

/// <summary>
/// MDM-T2 / SIM-T13 G2-7: replaced `throw new NotImplementedException()` with a
/// real structural check. xAPI's Group actor shape uses a `member` array of
/// inverse-functional-identifier objects (each containing mbox / mbox_sha1sum
/// / openid / account / uuid). A minimally-valid Group has a non-empty member
/// array; per-member validation is left to ActorIsCorrect recursion (Group
/// members may themselves be agents that fail validation, but that's a deeper
/// check we don't enforce today).
/// </summary>
bool StatementHandler::MemberIsCorrect(const XApiJson::ojson& input)
{
	if (input.is_null()) { return false; }
	// Expected shape: array of agent-like objects. Some emitters use an object
	// with a `member` array inside; tolerate both rather than reject.
	const XApiJson::ojson* members = nullptr;
	if (input.is_array())
	{
		members = &input;
	}
	else if (input.is_object() && input.contains("member") && input.at("member").is_array())
	{
		members = &input.at("member");
	}
	if (members == nullptr || members->empty())
	{
		StaticDetails::Log(SimulationLogLevel::Warn,
			"MemberIsCorrect: actor declared a `member` shape but the array is empty or wrong type. Treating actor as invalid.");
		return false;
	}
	return true;
}

/// <summary>
/// MDM-T2 / SIM-T13 G2-7: replaced `throw new NotImplementedException()` with a
/// real structural check. xAPI's Account inverse-functional identifier is an
/// object containing `homePage` (IRI) + `name` (string). Both are required;
/// neither can be empty per the spec.
/// </summary>
bool StatementHandler::AccountIsCorrect(const XApiJson::ojson& input)
{
	if (input.is_null()) { return false; }
	if (!input.is_object())
	{
		StaticDetails::Log(SimulationLogLevel::Warn,
			"AccountIsCorrect: actor declared an `account` shape but value isn't an object. Treating actor as invalid.");
		return false;
	}
	// C#: (string)input["homePage"] ?? (string)input["homepage"] -- the fallback key
	// only engages when the primary token is missing/null (an empty string does not).
	const XApiJson::ojson& homePagePrimary = LeafToken(input, "homePage");
	const XApiJson::ojson& homePageToken = homePagePrimary.is_null() ? LeafToken(input, "homepage") : homePagePrimary;
	const XApiJson::ojson& nameToken = LeafToken(input, "name");
	bool homePageIsNull = homePageToken.is_null();
	bool nameIsNull = nameToken.is_null();
	string homePage = homePageIsNull ? string() : CastStringOrEmpty(homePageToken);
	string name = nameIsNull ? string() : CastStringOrEmpty(nameToken);
	bool ok = !homePage.empty() && !name.empty();
	if (!ok)
	{
		StaticDetails::Log(SimulationLogLevel::Warn,
			"AccountIsCorrect: account requires both homePage + name; treating actor as invalid. homePage=" +
			(homePageIsNull ? string("<null>") : homePage) + " name=" + (nameIsNull ? string("<null>") : name));
	}
	return ok;
}
// #endregion

// #region Verb
/// <summary>
/// id is the only thing required for verb but it needs to be a verb set up
/// automatically from the febris launcher
/// </summary>
bool StatementHandler::VerbIsCorrect(const XApiJson::ojson& input)
{
	bool verbContainsNeededItems = false;
	try
	{
		//check if token is null
		if (input.is_null())
		{
			return false;
		}
		if (!input.is_object())
		{
			// C#: indexing a value token raises InvalidOperationException into the catch
			throw std::runtime_error("verb token is not an object");
		}
		//variables
		if (!CastStringOrEmpty(LeafToken(input, "id")).empty())
		// [Historical] commented-out check restricting the id to the
		// Initialized/Attempted launcher IRIs
		{
			verbContainsNeededItems = true;
		}
	}
	catch (const std::exception&)
	{
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}

	return verbContainsNeededItems;
}

// [Historical] two commented-out VerbUpdate(VerbStage) overloads elided (dead in C#)

/// <summary>
/// Update your verb manually using the VerbEnums.
/// Removed the auto calculation feature for more transparancy in Verb setting.
/// FIX (SDKV-3): the completion read goes through TokenToBool since
/// result.completion is now a genuine JSON Boolean.
/// (C# parity: `updated` is never set true, so this always returns false.)
/// </summary>
bool StatementHandler::VerbUpdate(VerbEnums verbEnum)
{
	bool updated = false;
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		XApiJson::ojson& verbNode = TokenHop(statement, "verb");
		verbNode["key"] = nullptr;
		verbNode["uuid"] = nullptr;
		verbNode["display"] = nullptr;
		const XApiJson::ojson& cstatement = statement;
		if (TokenToBool(LeafToken(TokenHop(cstatement, "result"), "completion")))
		{
			switch (verbEnum)
			{
			case VerbEnums::Attempted:
				verbNode["id"] = VerbIRIResolver::ResolveVerbIRI(VerbEnums::Attempted);
				break;
			case VerbEnums::Completed:
				verbNode["id"] = VerbIRIResolver::ResolveVerbIRI(VerbEnums::Completed);
				break;
			case VerbEnums::Initialized:
				verbNode["id"] = VerbIRIResolver::ResolveVerbIRI(VerbEnums::Initialized);
				break;
			// [Historical] commented-out calculated pass/fail block elided
			case VerbEnums::Terminated:
				verbNode["id"] = VerbIRIResolver::ResolveVerbIRI(VerbEnums::Terminated);
				break;
			case VerbEnums::Pass:
				verbNode["id"] = VerbIRIResolver::ResolveVerbIRI(VerbEnums::Pass);
				break;
			case VerbEnums::Not_Pass:
				verbNode["id"] = VerbIRIResolver::ResolveVerbIRI(VerbEnums::Not_Pass);
				break;
			default:
				// Handle bad URL, possibly throw
				throw std::runtime_error("unsupported VerbEnums value");
			}
		}
		else //if it is not complete
		{
			switch (verbEnum)
			{
			case VerbEnums::Attempted:
				verbNode["id"] = VerbIRIResolver::ResolveVerbIRI(VerbEnums::Attempted);
				break;
			case VerbEnums::Completed:
				verbNode["id"] = VerbIRIResolver::ResolveVerbIRI(VerbEnums::Completed);
				break;
			case VerbEnums::Initialized:
				verbNode["id"] = VerbIRIResolver::ResolveVerbIRI(VerbEnums::Initialized);
				// [Historical] commented-out calculated pass/fail block elided
				break;
			case VerbEnums::Terminated:
				verbNode["id"] = VerbIRIResolver::ResolveVerbIRI(VerbEnums::Terminated);
				break;
			case VerbEnums::Pass:
				verbNode["id"] = VerbIRIResolver::ResolveVerbIRI(VerbEnums::Pass);
				break;
			case VerbEnums::Not_Pass:
				verbNode["id"] = VerbIRIResolver::ResolveVerbIRI(VerbEnums::Not_Pass);
				break;
			default:
				// Handle bad URL, possibly throw
				throw std::runtime_error("unsupported VerbEnums value");
			}
			// [Historical] older not-complete remap switch elided (dead in C#)
		}
	}
	catch (const std::exception&)
	{
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
	return updated;
}

/// <summary>
/// Auto-calculating verb update used by EndSimulation.
/// FIX (SDKV-3): completion/success reads go through TokenToBool since both are
/// genuine JSON Booleans now (legacy string "true"/"false" values still parse).
/// (Takes and returns the SAME instance by reference -- C# JObject semantics.)
/// </summary>
XApiJson::ojson& StatementHandler::VerbUpdate(XApiJson::ojson& statement, VerbEnums verbEnum)
{
	try
	{
		XApiJson::ojson& verbNode = TokenHop(statement, "verb");
		verbNode["key"] = nullptr;
		verbNode["uuid"] = nullptr;
		verbNode["display"] = nullptr;

		const XApiJson::ojson& cstatement = statement;
		const XApiJson::ojson& resultNode = TokenHop(cstatement, "result");
		if (TokenToBool(LeafToken(resultNode, "completion")))
		{
			switch (verbEnum)
			{
			case VerbEnums::Attempted:
				verbNode["id"] = VerbIRIResolver::ResolveVerbIRI(VerbEnums::Completed);
				break;
			case VerbEnums::Completed:
				//Already marked as completed
				break;
			case VerbEnums::Initialized:
				//calculated pass and fail
				if (TokenToBool(LeafToken(resultNode, "success")))
				{
					verbNode["id"] = VerbIRIResolver::ResolveVerbIRI(VerbEnums::Pass);
				}
				else
				{
					verbNode["id"] = VerbIRIResolver::ResolveVerbIRI(VerbEnums::Not_Pass);
				}
				break;
			case VerbEnums::Terminated:
				//nothing needed
				break;
			case VerbEnums::Pass:
				//nothing needed
				break;
			case VerbEnums::Not_Pass:
				//nothing needed
				break;
			default:
				// Handle bad URL, possibly throw
				throw std::runtime_error("unsupported VerbEnums value");
			}
		}
		else //if it is not complete
		{
			switch (verbEnum)
			{
			case VerbEnums::Attempted:
				//nothing needed
				break;
			case VerbEnums::Completed:
				//nothing needed
				break;
			case VerbEnums::Initialized:
				verbNode["id"] = VerbIRIResolver::ResolveVerbIRI(VerbEnums::Terminated);
				break;
			case VerbEnums::Terminated:
				//nothing needed
				break;
			case VerbEnums::Pass:
				//this should not be an options
				break;
			case VerbEnums::Not_Pass:
				//should not be an option
				break;
			default:
				// Handle bad URL, possibly throw
				throw std::runtime_error("unsupported VerbEnums value");
			}
		}
	}
	catch (const std::exception&)
	{
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}

	return statement;
}

/// <summary>
/// FIX (SDKV-3): tolerant boolean read for statement tokens.
/// result.success/completion are genuine JSON Booleans now, but statements
/// written by older SDK builds may still carry the lowercase strings
/// "true"/"false" -- both parse; anything else (null, missing, junk) reads as false.
/// </summary>
bool StatementHandler::TokenToBool(const XApiJson::ojson& token)
{
	if (token.is_null())
	{
		return false;
	}
	if (token.is_boolean())
	{
		return token.get<bool>();
	}
	// C# bool.TryParse(token.ToString()) is case-insensitive; the working statement
	// only ever carries the lowercase legacy strings, so the common casings suffice.
	std::string raw = TokenText(token);
	return raw == "true" || raw == "True" || raw == "TRUE";
}
// #endregion

// #region Object
/// <summary>
/// For the object only an Id is required
/// </summary>
bool StatementHandler::ObjectIsCorrect(const XApiJson::ojson& input)
{
	if (input.is_null())
	{
		return false;
	}
	bool exists = false;
	try
	{
		if (!input.is_object())
		{
			// C#: indexing a value token raises InvalidOperationException into the catch
			throw std::runtime_error("object token is not an object");
		}
		// C#: input["id"] != null -- token-PRESENCE check (reference comparison)
		if (input.contains("id")/*&& input["objecttype"] != null && input["definition"] != null*/)
		{
			//bool objectDefinitionIsCorrect = ObjectDefinitionIsCorrect(input["definition"]);
			//if (objectDefinitionIsCorrect)
			//{
			exists = true;
			//}
		}
	}
	catch (const std::exception&)
	{
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
	return exists;
}

bool StatementHandler::ObjectDefinitionIsCorrect(const XApiJson::ojson& input)
{
	(void)input;
	bool exists = false;
	try { }
	catch (const std::exception&)
	{
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
	//if ()
	//{

	//}
	return exists;
}
// #endregion

// #region Result
// #region Generic Result

void StatementHandler::Result(string input, ResultOptions resultType)
{
	try
	{
		//could use an enum for routing
		switch (resultType)
		{
		case ResultOptions::Response:
			UpdateResponseStatus(input);
			break;
		default:
			// SIM-T13 G6: surface unsupported routes through the logger (same text as
			// the UpdateStatement default branches -- the C# reuses it verbatim).
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on result update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}

	//xapi result > Score
}

void StatementHandler::Result(float input, ResultOptions resultType)
{
	try
	{
		//could use an enum for routing
		switch (resultType)
		{
		case ResultOptions::ScoreMin:
			UpdateMinScore(input);
			break;
		case ResultOptions::ScoreMax:
			UpdateMaxScore(input);
			break;
		case ResultOptions::ScoreScale:
			UpdateScaleScore(input);
			break;
		case ResultOptions::ScoreRaw:
			UpdateRawScore(input);
			break;
		default:
			// SIM-T13 G6: surface unsupported routes through the logger.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on result update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
	//xapi result > Score
}

void StatementHandler::Result(bool input, ResultOptions resultType)
{
	try
	{
		//could use an enum for routing
		switch (resultType)
		{
		case ResultOptions::Success:
			UpdateSuccessStatus(input);
			break;
		case ResultOptions::Completion:
			UpdateCompletionStatus(input);
			break;
		default:
			// SIM-T13 G6: surface unsupported routes through the logger.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
		//xapi result > Score
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on result update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

// C# Result(TimeSpan, ResultOptions)
void StatementHandler::Result(long long durationMs, ResultOptions resultType)
{
	try
	{
		//could use an enum for routing
		switch (resultType)
		{
		case ResultOptions::Duration:
			UpdateDuration(durationMs);
			break;
		default:
			// SIM-T13 G6: surface unsupported routes through the logger.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
		//xapi result > Score
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on result update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::Result(int input, ResultOptions resultType, ResultExtensionOptions resultExtensionOptions)
{
	try
	{
		//could use an enum for routing
		switch (resultType)
		{
		case ResultOptions::Extensions:
			if (resultExtensionOptions == ResultExtensionOptions::RestartCounter)
			{
				string counterIRI = ExtensionIRIResolver::ResolveExtensionIRI(ExtensionIRIOptions::RestartCounterIRI);
				UpdateResultExtensions(counterIRI, std::to_string(input));
			}
			break;
		default:
			// SIM-T13 G6: surface unsupported routes through the logger.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on result update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
	//xapi result > Score
}

void StatementHandler::Result(string input, ResultOptions resultType, ResultExtensionOptions resultExtensionOptions)
{
	try
	{
		//could use an enum for routing
		switch (resultType)
		{
		case ResultOptions::Extensions:
			switch (resultExtensionOptions)
			{
			case ResultExtensionOptions::Notes:
			{
				string notesIRI = ExtensionIRIResolver::ResolveExtensionIRI(ExtensionIRIOptions::NotesIRI);
				UpdateResultExtensions(notesIRI, input);
				break;
			}
			case ResultExtensionOptions::RestartCounter:
			{
				string counterIRI = ExtensionIRIResolver::ResolveExtensionIRI(ExtensionIRIOptions::RestartCounterIRI);
				UpdateResultExtensions(counterIRI, input);
				break;
			}
			}
			break;
		default:
			// SIM-T13 G6: surface unsupported routes through the logger.
			StaticDetails::Log(SimulationLogLevel::Warn,
				"UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
			break;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error on result update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}

	//xapi result > Score
}
// #endregion

// #region update result
/// <summary>
/// Writes result.duration from a running Stopwatch.
/// FIX (SDKV-5): serialize the elapsed time as ISO 8601 via the XmlConvert mirror
/// (XMLConvert::millisecondsToIso8601) -- the old elapsedTime.ToString()
/// stringified the Stopwatch OBJECT (type name, not a duration at all), and even
/// Elapsed.ToString() would emit the .NET clock format that xAPI 1.0.3
/// section 4.1.5 rejects.
/// </summary>
void StatementHandler::UpdateDuration(Stopwatch elapsedTime)
{
	try
	{
		//xapi result > duration
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		// [Historical] CheckProperty/CreateProperty disk-era scaffolding elided

		TokenHop(statement, "result")["duration"] = XMLConvert::millisecondsToIso8601((long long)elapsedTime.ElapsedMilliseconds());
		//statement["result"]["duration"] = elapsedTime.ToString(); // [Historical - SDKV-5] wrote the Stopwatch type name, not a duration
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

// C# UpdateDuration(TimeSpan)
void StatementHandler::UpdateDuration(long long elapsedTimeMs)
{
	try
	{
		//xapi result > duration
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		// [Historical] CheckProperty/CreateProperty disk-era scaffolding elided

		TokenHop(statement, "result")["duration"] = XMLConvert::millisecondsToIso8601(elapsedTimeMs);
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

// C# UpdateDuration(JObject, TimeSpan) -- same instance in and out (reference).
XApiJson::ojson& StatementHandler::UpdateDuration(XApiJson::ojson& statement, long long elapsedTimeMs)
{
	try
	{
		TokenHop(statement, "result")["duration"] = XMLConvert::millisecondsToIso8601(elapsedTimeMs);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
	return statement;
}

void StatementHandler::UpdateRawScore(float score)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		AutoUpdateScaledScore(statement, score);  // C#: statement = AutoUpdateScaledScore(statement, score) -- same instance back
		// Score floats are stored as JSON NUMBERS through XApiJson::FloatJson so the
		// emitted digits match Newtonsoft float emission (parity harness pin).
		TokenHop(TokenHop(statement, "result"), "score")["raw"] = XApiJson::FloatJson(score);
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

XApiJson::ojson& StatementHandler::UpdateRawScore(XApiJson::ojson& statement, float score)
{
	try
	{
		AutoUpdateScaledScore(statement, score);  // C#: statement = AutoUpdateScaledScore(...) -- same instance back
		TokenHop(TokenHop(statement, "result"), "score")["raw"] = XApiJson::FloatJson(score);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
	return statement;
}

void StatementHandler::UpdateScaleScore(float score)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		// [Historical] CheckProperty/CreateProperty disk-era scaffolding elided
		//xApi result > Success
		TokenHop(TokenHop(statement, "result"), "score")["scaled"] = XApiJson::FloatJson(score);
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

XApiJson::ojson& StatementHandler::UpdateScaleScore(XApiJson::ojson& statement, float score)
{
	try
	{
		TokenHop(TokenHop(statement, "result"), "score")["scaled"] = XApiJson::FloatJson(score);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
	return statement;
}

/// <summary>
/// MDM-T2 G2-8 / BUGS.md: previously divided rawScore by statement.result.score.max
/// with no guard. When max was missing (default 0), the division produced
/// float.PositiveInfinity and the library wrote that into result.score.scaled,
/// corrupting downstream LMS reporting. xAPI requires scaled to be in [-1.0, 1.0].
/// Also no NaN guard.
///
/// Now: validates that max is positive and finite; logs a Warn if not and skips
/// the scaled update (leaving the existing value in place). Clamps the final
/// result to [-1.0, 1.0] per spec.
/// </summary>
XApiJson::ojson& StatementHandler::AutoUpdateScaledScore(XApiJson::ojson& statement, float rawScore)
{
	try
	{
		// The max field can be missing entirely (token is null) or present but unset
		// (default 0). Guard both. (float)null throws an InvalidCastException so the
		// null-check has to precede the cast.
		const XApiJson::ojson& cstatement = statement;
		const XApiJson::ojson* maxToken =
			ConditionalIndex(ConditionalIndex(ConditionalIndex(&cstatement, "result"), "score"), "max");
		if (maxToken == nullptr || maxToken->is_null())
		{
			StaticDetails::Log(SimulationLogLevel::Warn,
				"AutoUpdateScaledScore: result.score.max is not set; skipping scaled-score update. "
				"Game should call UpdateMaxScore before UpdateRawScore.");
			return statement;
		}

		float maxScore = 0.0f;
		if (maxToken->is_number())
		{
			maxScore = maxToken->get<float>();
		}
		else if (maxToken->is_string())
		{
			// C#'s (float) cast parses numeric strings; junk throws into the catch.
			const std::string maxText = maxToken->get<std::string>();
			size_t consumed = 0;
			maxScore = std::stof(maxText, &consumed);
			if (consumed != maxText.size())
			{
				throw std::runtime_error("result.score.max is not float-convertible: " + maxText);
			}
		}
		else
		{
			throw std::runtime_error("result.score.max is not float-convertible");
		}
		// FIX (SIM-B7): std::isnan/std::isinf guards on the division inputs.
		if (maxScore <= 0.0f || std::isnan(maxScore) || std::isinf(maxScore))
		{
			StaticDetails::Log(SimulationLogLevel::Warn,
				"AutoUpdateScaledScore: result.score.max=" + FloatText(maxScore) +
				" is non-positive / NaN / Infinity; skipping scaled-score update.");
			return statement;
		}

		if (std::isnan(rawScore) || std::isinf(rawScore))
		{
			StaticDetails::Log(SimulationLogLevel::Warn,
				"AutoUpdateScaledScore: rawScore=" + FloatText(rawScore) +
				" is NaN / Infinity; skipping scaled-score update.");
			return statement;
		}

		float scaledScore = rawScore / maxScore;

		// Clamp to xAPI's [-1.0, 1.0] range. Out-of-range usually means raw exceeds
		// max (over-100% scores are common in some scoring schemes); clamp + Warn so
		// the data still lands somewhere sane.
		if (scaledScore < -1.0f || scaledScore > 1.0f)
		{
			StaticDetails::Log(SimulationLogLevel::Warn,
				"AutoUpdateScaledScore: computed scaled=" + FloatText(scaledScore) +
				" is outside the xAPI [-1.0, 1.0] range; clamping. "
				"rawScore=" + FloatText(rawScore) + " maxScore=" + FloatText(maxScore));
			scaledScore = scaledScore < -1.0f ? -1.0f : 1.0f;
		}

		TokenHop(TokenHop(statement, "result"), "score")["scaled"] = XApiJson::FloatJson(scaledScore);
	}
	catch (const std::exception& e)
	{
		StaticDetails::Log(SimulationLogLevel::Error,
			"AutoUpdateScaledScore: unexpected exception updating scaled score.", e.what());
	}
	return statement;
}

void StatementHandler::UpdateMinScore(float score)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		// [Historical] CheckProperty/CreateProperty disk-era scaffolding elided
		//xAPi result > completion
		TokenHop(TokenHop(statement, "result"), "score")["min"] = XApiJson::FloatJson(score);
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

XApiJson::ojson& StatementHandler::UpdateMinScore(XApiJson::ojson& statement, float score)
{
	try
	{
		TokenHop(TokenHop(statement, "result"), "score")["min"] = XApiJson::FloatJson(score);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
	return statement;
}

void StatementHandler::UpdateMaxScore(float score)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		// [Historical] CheckProperty/CreateProperty disk-era scaffolding elided
		//xapi result > response
		TokenHop(TokenHop(statement, "result"), "score")["max"] = XApiJson::FloatJson(score);
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

XApiJson::ojson& StatementHandler::UpdateMaxScore(XApiJson::ojson& statement, float score)
{
	try
	{
		TokenHop(TokenHop(statement, "result"), "score")["max"] = XApiJson::FloatJson(score);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
	return statement;
}

/// <summary>
/// Writes result.completion on the current statement.
/// FIX (SDKV-3/11): JSON Boolean, not the lowercase string.
/// </summary>
void StatementHandler::UpdateCompletionStatus(bool status)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		// [Historical] CheckProperty/CreateProperty disk-era scaffolding elided
		//xapi result > extensions
		TokenHop(statement, "result")["completion"] = status;
		//statement["result"]["completion"] = status.ToString().ToLower(); // [Historical - SDKV-3] string-typed boolean
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

/// <summary>
/// Writes result.completion on the supplied statement.
/// FIX (SDKV-3/11): JSON Boolean, not the lowercase string.
/// </summary>
XApiJson::ojson& StatementHandler::UpdateCompletionStatus(XApiJson::ojson& statement, bool status)
{
	try
	{
		TokenHop(statement, "result")["completion"] = status;
		//statement["result"]["completion"] = status.ToString().ToLower(); // [Historical - SDKV-3] string-typed boolean
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
	return statement;
}

/// <summary>
/// Writes result.success on the current statement.
/// FIX (SDKV-3/11): JSON Boolean, not the lowercase string.
/// </summary>
void StatementHandler::UpdateSuccessStatus(bool status)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		TokenHop(statement, "result")["success"] = status;
		//statement["result"]["success"] = status.ToString().ToLower(); // [Historical - SDKV-3] string-typed boolean
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

/// <summary>
/// Writes result.success on the supplied statement.
/// FIX (SDKV-3/11): JSON Boolean, not the lowercase string.
/// </summary>
XApiJson::ojson& StatementHandler::UpdateSuccessStatus(XApiJson::ojson& statement, bool status)
{
	try
	{
		TokenHop(statement, "result")["success"] = status;
		//statement["result"]["success"] = status.ToString().ToLower(); // [Historical - SDKV-3] string-typed boolean
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
	return statement;
}

void StatementHandler::UpdateResponseStatus(string input)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		// [Historical] CheckProperty/CreateProperty disk-era scaffolding elided
		//xapi result > extensions
		TokenHop(statement, "result")["response"] = input;  // C#: input.ToString() on a string
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::UpdateResultExtensions(string key, string input)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;

		// The extensionmap is a serialized-TEXT convention: ONE string of
		// "scheme:rest-of-IRI:value" entries joined by commas (each key carries one
		// colon of its own), not a JSON object.
		const XApiJson::ojson& cstatement = statement;
		string extensionMap = CastStringOrEmpty(
			LeafToken(TokenHop(TokenHop(cstatement, "result"), "extensions"), "extensionmap"));
		vector<string> extensionMapArray = String_Helpers::Split(extensionMap, ',');
		string newExtensionMap = "";
		bool keyCurrentlyExists = false;
		if (extensionMap == "")
		{
			newExtensionMap = key + ":" + input;
			keyCurrentlyExists = true;
		}
		else
		{
			for (size_t i = 0; i < extensionMapArray.size(); i++)
			{
				vector<string> extensionSingle = String_Helpers::Split(extensionMapArray[i], ':');

				// MDM-T2 C-03: bounds-guard. Skip malformed entries (must have key:value:count shape).
				if (extensionSingle.size() < 3)
				{
					StaticDetails::Log(SimulationLogLevel::Warn,
						"UpdateResultExtensions: skipping malformed extensionmap entry (need 3 colon-separated parts, got " +
						std::to_string(extensionSingle.size()) + "): " + extensionMapArray[i]);
					continue;
				}

				if (extensionSingle[0] + ":" + extensionSingle[1] == key)
				{
					keyCurrentlyExists = true;
					ExtensionIRIOptions extensionEnum = ExtensionIRIResolver::ResolveExtensionIRI(key);
					switch (extensionEnum)
					{
					case ExtensionIRIOptions::NotesIRI:
						extensionSingle[2] += "|" + input;
						break;
					case ExtensionIRIOptions::RestartCounterIRI:
						extensionSingle[2] = input;
						break;
					}

					newExtensionMap += extensionSingle[0] + ":" + extensionSingle[1] + ":" + extensionSingle[2];
				}
				else
				{
					newExtensionMap += extensionSingle[0] + ":" + extensionSingle[1] + ":" + extensionSingle[2];
				}

				if (i + 1 < extensionMapArray.size())
				{
					newExtensionMap += ",";
				}
			}
		}
		if (!keyCurrentlyExists)
		{
			newExtensionMap += "," + key + ":" + input;
		}

		std::cout << newExtensionMap << std::endl;
		TokenHop(TokenHop(statement, "result"), "extensions")["extensionmap"] = newExtensionMap;
		//write statement
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}
// #endregion
// #endregion

// #region Context
// #region General Context

void StatementHandler::Context(ContextOptions contextOption, string input)
{
	//get the statement jobject
	switch (contextOption)
	{
	case ContextOptions::Registration:
		UpdatecontextRegistration(input);
		break;
	case ContextOptions::Revision:
		UpdateContextRevision(input);
		break;
	case ContextOptions::Platform:
		UpdateContextPlatform(input);
		break;
	case ContextOptions::Language:
		UpdateContextLanguage(input);
		break;
	default:
		std::cout << "This method has done nothing" << std::endl;
		break;
	}
}

void StatementHandler::Context(ContextOptions contextOption, ContextActivitesOptions contextActivitesOption, string input)
{
	switch (contextOption)
	{
	case ContextOptions::ContextActivites:
		ContextActivity(contextActivitesOption, input);
		break;
	default:
		std::cout << "This method has done nothing" << std::endl;
		break;
	}
}

void StatementHandler::ContextActivity(ContextActivitesOptions option, string input)
{
	switch (option)
	{
	case ContextActivitesOptions::Parent:
		UpdateContextActivityParent(input);
		break;
	case ContextActivitesOptions::Grouping:
		UpdateContextActivityGrouping(input);
		break;
	case ContextActivitesOptions::Category:
		UpdateContextActivityCategory(input);
		break;
	case ContextActivitesOptions::Other:
		UpdateContextActivityOther(input);
		break;
	default:
		std::cout << "This method has done nothing" << std::endl;
		break;
	}
}

void StatementHandler::Context(ContextOptions contextOption, ContextStatementReferenceOptions contextStatementReferenceOption, string input)
{
	switch (contextOption)
	{
	case ContextOptions::StatementReference:
		ContextStatementReferenceOption(contextStatementReferenceOption, input);
		break;
	default:
		std::cout << "This method has done nothing" << std::endl;
		break;
	}
}

void StatementHandler::ContextStatementReferenceOption(ContextStatementReferenceOptions option, string input)
{
	switch (option)
	{
	case ContextStatementReferenceOptions::ObjectType:
		UpdateContextStatementReferenceObjectType(input);
		break;
	case ContextStatementReferenceOptions::Id:
		UpdateContextStatementReferenceId(input);
		break;
	default:
		std::cout << "This method has done nothing" << std::endl;
		break;
	}
}

void StatementHandler::Context(ContextOptions contextOption, ContextExtensionOptions contextExtensionOption, string input)
{
	switch (contextOption)
	{
	case ContextOptions::Extensions:
		ContextExtensionOption(contextExtensionOption, input);
		break;
	default:
		std::cout << "This method has done nothing" << std::endl;
		break;
	}
}

void StatementHandler::ContextExtensionOption(ContextExtensionOptions option, string input)
{
	string key = ContextOptionResolver::ContextExtensionOptionResolver(option);
	switch (option)
	{
	case ContextExtensionOptions::Option1:
		UpdateContextExtensions(key, input);
		break;
	case ContextExtensionOptions::Option2:
		UpdateContextExtensions(key, input);
		break;
	default:
		std::cout << "This method has done nothing" << std::endl;
		break;
	}
}
// #endregion

// #region Update Context

void StatementHandler::UpdatecontextRegistration(string input)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		TokenHop(statement, "context")["registration"] = input;
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

/// <summary>
/// Writes context.contextActivities.parent on the current statement.
/// FIX (SDKV-4): the old key "contextactivites" (missing 'i') never exists in the
/// factored statement, so the write NRE'd into the catch and every
/// context-activity update was a silent no-op. The factored statement carries
/// "contextActivities" (spec casing via the Context model's [JsonProperty]).
/// </summary>
void StatementHandler::UpdateContextActivityParent(string input)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		TokenHop(TokenHop(statement, "context"), "contextActivities")["parent"] = input;
		//statement["context"]["contextactivites"]["parent"] = input; // [Historical - SDKV-4] typo key, never present
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

/// <summary>
/// Writes context.contextActivities.grouping on the current statement.
/// FIX (SDKV-4): see UpdateContextActivityParent.
/// </summary>
void StatementHandler::UpdateContextActivityGrouping(string input)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		TokenHop(TokenHop(statement, "context"), "contextActivities")["grouping"] = input;
		//statement["context"]["contextactivites"]["grouping"] = input; // [Historical - SDKV-4] typo key, never present
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

/// <summary>
/// Writes context.contextActivities.category on the current statement.
/// FIX (SDKV-4): see UpdateContextActivityParent.
/// </summary>
void StatementHandler::UpdateContextActivityCategory(string input)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		TokenHop(TokenHop(statement, "context"), "contextActivities")["category"] = input;
		//statement["context"]["contextactivites"]["category"] = input; // [Historical - SDKV-4] typo key, never present
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

/// <summary>
/// Writes context.contextActivities.other on the current statement.
/// FIX (SDKV-4): see UpdateContextActivityParent.
/// </summary>
void StatementHandler::UpdateContextActivityOther(string input)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		TokenHop(TokenHop(statement, "context"), "contextActivities")["other"] = input;
		//statement["context"]["contextactivites"]["other"] = input; // [Historical - SDKV-4] typo key, never present
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::UpdateContextInstructor(string input)
{
	try
	{
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		(void)statement;  // C# takes the alias and stops -- stub kept for parity
		(void)input;
		//this will need to be an actor
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::UpdateContextGroup(string input)
{
	try
	{
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		(void)statement;  // C# takes the alias and stops -- stub kept for parity
		(void)input;
		//this is a group of actors
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::UpdateContextRevision(string input)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		TokenHop(statement, "context")["revision"] = input;
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::UpdateContextPlatform(string input)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		TokenHop(statement, "context")["platform"] = input;
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::UpdateContextLanguage(string input)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		TokenHop(statement, "context")["language"] = input;
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::UpdateContextStatementReferenceId(string input)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		TokenHop(TokenHop(statement, "context"), "statementreference")["id"] = input;
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

/// <summary>
/// Writes the StatementRef discriminator. FIX (SDKV-12): the factored statement
/// now carries spec-cased "objectType" inside the dialect "statementreference" node.
/// </summary>
void StatementHandler::UpdateContextStatementReferenceObjectType(string input)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		TokenHop(TokenHop(statement, "context"), "statementreference")["objectType"] = input;
		//statement["context"]["statementreference"]["objecttype"] = input; // [Historical - SDKV-12] pre-spec-casing key
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing update: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

void StatementHandler::UpdateContextExtensions(string key, string input)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		// Same serialized-text extensionmap convention as UpdateResultExtensions,
		// but the value on a key match is REPLACED (no Notes-style append branch).
		const XApiJson::ojson& cstatement = statement;
		string extensionMap = CastStringOrEmpty(
			LeafToken(TokenHop(TokenHop(cstatement, "context"), "extensions"), "extensionmap"));
		vector<string> extensionMapArray = String_Helpers::Split(extensionMap, ',');
		string newExtensionMap = "";
		bool keyCurrentlyExists = false;
		if (extensionMap == "")
		{
			newExtensionMap = key + ":" + input;
			keyCurrentlyExists = true;
		}
		else
		{
			for (size_t i = 0; i < extensionMapArray.size(); i++)
			{
				vector<string> extensionSingle = String_Helpers::Split(extensionMapArray[i], ':');

				// MDM-T2 C-03: bounds-guard. Skip malformed entries.
				if (extensionSingle.size() < 3)
				{
					StaticDetails::Log(SimulationLogLevel::Warn,
						"UpdateContextExtensions: skipping malformed extensionmap entry (need 3 colon-separated parts, got " +
						std::to_string(extensionSingle.size()) + "): " + extensionMapArray[i]);
					continue;
				}

				if (extensionSingle[0] + ":" + extensionSingle[1] == key)
				{
					keyCurrentlyExists = true;
					extensionSingle[2] = input;
					newExtensionMap += extensionSingle[0] + ":" + extensionSingle[1] + ":" + extensionSingle[2];
				}
				else
				{
					newExtensionMap += extensionSingle[0] + ":" + extensionSingle[1] + ":" + extensionSingle[2];
				}

				if (i + 1 < extensionMapArray.size())
				{
					newExtensionMap += ",";
				}
			}
		}
		if (!keyCurrentlyExists)
		{
			newExtensionMap += "," + key + ":" + input;
		}
		std::cout << newExtensionMap << std::endl;
		TokenHop(TokenHop(statement, "context"), "extensions")["extensionmap"] = newExtensionMap;
		//write statement
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writting content extension: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}
// #endregion
// #endregion

// #region Attachments
/// Required: UsageType, Display, contenttype, length, sha2
/// optional: fileUrl, description

/// <summary>
/// Adds an attachment (full shape). FIX (SDKV-2): the caller's plain
/// display/description strings are wrapped into Language Map objects
/// ({"en": value}) via StatementFactoring::StringToLanguageMap so the wire
/// carries xAPI 1.0.3 section 4.1.11-conformant objects.
/// (C# validates usageType/fileUrl via new Uri(...) -- throws into the catch on
/// empty/invalid input; the C++ model stores the string, so only the empty-input
/// throw is mirrored and full URI validation/canonicalization is not ported.)
/// </summary>
void StatementHandler::AddAttachment(string usageType, string display, string description, ContentType contentType, int length, string sha2, string fileUrl)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		Attachment attachment;
		if (usageType.empty()) { throw std::runtime_error("Invalid URI: The URI is empty."); }  // C#: new Uri(usageType)
		attachment.UsageType = usageType;
		attachment.Display = StatementFactoring::StringToLanguageMap(display);
		attachment.Description = StatementFactoring::StringToLanguageMap(description);
		attachment.ContentType = ContentTypeResolver::ResolveExtensionIRI(contentType);
		attachment.Length = length;
		attachment.Sha2 = sha2;
		if (fileUrl.empty()) { throw std::runtime_error("Invalid URI: The URI is empty."); }  // C#: new Uri(fileUrl)
		attachment.FileURL = fileUrl;
		ProcessAttachmentJArray(statement, attachment);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing attachment: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

/// <summary>
/// Adds an attachment (no fileUrl). FIX (SDKV-2): display/description wrapped
/// into Language Map objects.
/// </summary>
void StatementHandler::AddAttachment(string usageType, string display, string description, ContentType contentType, int length, string sha2)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		Attachment attachment;
		if (usageType.empty()) { throw std::runtime_error("Invalid URI: The URI is empty."); }  // C#: new Uri(usageType)
		attachment.UsageType = usageType;
		attachment.Display = StatementFactoring::StringToLanguageMap(display);
		attachment.Description = StatementFactoring::StringToLanguageMap(description);
		attachment.ContentType = ContentTypeResolver::ResolveExtensionIRI(contentType);
		attachment.Length = length;
		attachment.Sha2 = sha2;
		ProcessAttachmentJArray(statement, attachment);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing attachment: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

/// <summary>
/// Adds an attachment (no description). FIX (SDKV-2): display wrapped into a
/// Language Map object.
/// </summary>
void StatementHandler::AddAttachment(string usageType, string display, ContentType contentType, int length, string sha2, string fileUrl)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		Attachment attachment;
		if (usageType.empty()) { throw std::runtime_error("Invalid URI: The URI is empty."); }  // C#: new Uri(usageType)
		attachment.UsageType = usageType;
		attachment.Display = StatementFactoring::StringToLanguageMap(display);
		attachment.ContentType = ContentTypeResolver::ResolveExtensionIRI(contentType);
		attachment.Length = length;
		attachment.Sha2 = sha2;
		if (fileUrl.empty()) { throw std::runtime_error("Invalid URI: The URI is empty."); }  // C#: new Uri(fileUrl)
		attachment.FileURL = fileUrl;
		ProcessAttachmentJArray(statement, attachment);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing attachment: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

/// <summary>
/// Adds an attachment (minimal shape). FIX (SDKV-2): display wrapped into a
/// Language Map object.
/// </summary>
void StatementHandler::AddAttachment(string usageType, string display, ContentType contentType, int length, string sha2)
{
	try
	{
		// REFERENCE SEMANTICS: shared statement, bound by reference.
		XApiJson::ojson& statement = StaticDetails::CurrentStatement;
		Attachment attachment;
		if (usageType.empty()) { throw std::runtime_error("Invalid URI: The URI is empty."); }  // C#: new Uri(usageType)
		attachment.UsageType = usageType;
		attachment.Display = StatementFactoring::StringToLanguageMap(display);
		attachment.ContentType = ContentTypeResolver::ResolveExtensionIRI(contentType);
		attachment.Length = length;
		attachment.Sha2 = sha2;
		ProcessAttachmentJArray(statement, attachment);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing attachment: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}

// C# JObject.FromObject(attachment) + rebuild-and-replace of the attachments
// array. The typed Attachment goes through the ordered parity emitter
// (to_json in XApiJson.h), so the emitted node shape matches the C# wire.
void StatementHandler::ProcessAttachmentJArray(XApiJson::ojson& statement, const Attachment& attachment)
{
	try
	{
		XApiJson::ojson attachmentObject;
		to_json(attachmentObject, attachment);
		XApiJson::ojson attachmentArray = XApiJson::ojson::array();
		const XApiJson::ojson& existing = statement.at("attachments");  // missing: throws like the C# foreach-over-null NRE
		if (!existing.is_array())
		{
			// C#: foreach over a null/value token throws into this catch
			throw std::runtime_error("statement 'attachments' is not an array");
		}
		for (const auto& attach : existing)
		{
			if (!attach.is_object())
			{
				// C#: the foreach's (JObject) element cast throws InvalidCastException
				throw std::runtime_error("attachments element is not an object");
			}
			attachmentArray.push_back(attach);
		}
		attachmentArray.push_back(attachmentObject);
		statement["attachments"] = attachmentArray;
		// C#: StaticDetails.CurrentStatement = statement -- same shared instance //SaveStatement(statement);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error writing attachment array: " << e.what() << std::endl;
		//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
	}
}
// #endregion
// #endregion

// [Historical] C# region ***************Old/Obsolete************* (lines ~2760-3150,
// commented-out result/verb-handling interfaces and the check-and-create-property
// scaffolding) is dead in the C# source and intentionally not ported.
