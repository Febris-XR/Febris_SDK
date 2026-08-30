// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
#include "FebrisSimApi.h"
// EXPORT-SURFACE (2026-08-29): the C ABI implementation. Every function body
// is a thin adapter over the ported C# mirror classes (Initializer,
// StatementHandler, StaticDetails, FileSystem) -- no simulation logic lives
// here. Two invariants this file owns:
//   1. No C++ exception ever crosses the extern "C" boundary (every body is
//      wrapped; unexpected failures come back as FEBRISSIM_E_EXCEPTION and go
//      through the registered logger).
//   2. The buffer convention: string outputs return the required byte count
//      INCLUDING the NUL, write at most cap-1 bytes, and always NUL-terminate
//      when cap > 0.

namespace
{
	// Buffer-convention writer.
	int32_t WriteOut(const std::string& text, char* buf, int32_t cap)
	{
		const int32_t needed = (int32_t)text.size() + 1;
		if (buf != nullptr && cap > 0)
		{
			const size_t n = (size_t)((cap - 1 < (int32_t)text.size()) ? cap - 1 : (int32_t)text.size());
			std::memcpy(buf, text.data(), n);
			buf[n] = '\0';
		}
		return needed;
	}

	// Flatten the C# (bool, string[,]) extras half: a null array (HasExtras
	// false, the WinPC shape) becomes the JSON literal null; otherwise a JSON
	// object in pair order (ordered_json preserves insertion order).
	std::string ExtrasToJson(const HandlerResult& result)
	{
		if (!result.HasExtras)
		{
			return "null";
		}
		XApiJson::ojson obj = XApiJson::ojson::object();
		for (const auto& kv : result.Extras)
		{
			obj[kv.first] = kv.second;
		}
		return obj.dump();
	}

	// Shared tail for the lifecycle calls that return (ready, extras).
	int32_t FinishTuple(const HandlerResult& result, int32_t* ready, char* extrasJson, int32_t extrasCap)
	{
		if (ready != nullptr)
		{
			*ready = result.Ready ? 1 : 0;
		}
		return WriteOut(ExtrasToJson(result), extrasJson, extrasCap);
	}

	void LogBoundaryFailure(const char* function, const std::exception* e)
	{
		StaticDetails::Log(SimulationLogLevel::Error,
			std::string(function) + ": failed at the C ABI boundary.",
			e ? e->what() : "unknown exception");
	}
}

extern "C" {

/* ------------------------------------------------------------------ */
/* Introspection, logging, configuration                               */
/* ------------------------------------------------------------------ */

int32_t FebrisSimAbiVersion(void)
{
	return FEBRISSIM_ABI_VERSION;
}

void FebrisSimSetLogger(FebrisSimLogCallback callback)
{
	// The public callback takes the level as int32_t; the internal sink takes
	// the enum. Bridge through a stored pointer.
	static FebrisSimLogCallback s_hostCallback = nullptr;
	s_hostCallback = callback;
	if (callback == nullptr)
	{
		StaticDetails::SetLogger(nullptr); // null reset -> console fallback
		return;
	}
	StaticDetails::SetLogger([](SimulationLogLevel level, const char* message, const char* detail)
	{
		// s_hostCallback is written before SetLogger installs this shim and
		// the library is single-threaded by contract, so the read is safe.
		if (s_hostCallback != nullptr)
		{
			s_hostCallback((int32_t)level, message, detail);
		}
	});
}

int32_t FebrisSimSetBasePath(const char* basePath)
{
	if (basePath == nullptr || basePath[0] == '\0')
	{
		return FEBRISSIM_E_INVALID_ARG;
	}
	try
	{
		FileSystem::BasePath = basePath;
		FileSystem::ExternalBasePath = basePath;
		FileSystem::SetFileSystemBasePath();
		return FEBRISSIM_OK;
	}
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimSetBasePath", &e); return FEBRISSIM_E_EXCEPTION; }
	catch (...) { LogBoundaryFailure("FebrisSimSetBasePath", nullptr); return FEBRISSIM_E_EXCEPTION; }
}

int32_t FebrisSimSetReferenceUuid(const char* uuid)
{
	if (uuid == nullptr || uuid[0] == '\0')
	{
		return FEBRISSIM_E_INVALID_ARG;
	}
	try
	{
		StaticDetails::SetReferenceUUID(uuid);
		return FEBRISSIM_OK;
	}
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimSetReferenceUuid", &e); return FEBRISSIM_E_EXCEPTION; }
	catch (...) { LogBoundaryFailure("FebrisSimSetReferenceUuid", nullptr); return FEBRISSIM_E_EXCEPTION; }
}

int32_t FebrisSimGetReferenceUuid(char* buf, int32_t cap)
{
	try
	{
		return WriteOut(StaticDetails::GetReferenceUUID(), buf, cap);
	}
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimGetReferenceUuid", &e); return FEBRISSIM_E_EXCEPTION; }
	catch (...) { LogBoundaryFailure("FebrisSimGetReferenceUuid", nullptr); return FEBRISSIM_E_EXCEPTION; }
}

/* ------------------------------------------------------------------ */
/* Lifecycle                                                           */
/* ------------------------------------------------------------------ */

int32_t FebrisSimInitialize(
	const char* febrisData, int32_t expectedOs,
	int32_t* ready, char* extrasJson, int32_t extrasCap)
{
	if (febrisData == nullptr)
	{
		return FEBRISSIM_E_INVALID_ARG;
	}
	try
	{
		// Convenience: accept the raw authored JSON as well as the launcher
		// "-febrisData={...}" form the C# ArgumentHandler scans for.
		std::string arg = febrisData;
		if (arg.rfind(SharedDetails::StatementPreface, 0) != 0)
		{
			arg = std::string(SharedDetails::StatementPreface) + arg;
		}
		std::vector<std::string> args{ arg };
		HandlerResult result = Initializer::Initialize(args, (ExpectedOperatingSystem)expectedOs);
		return FinishTuple(result, ready, extrasJson, extrasCap);
	}
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimInitialize", &e); return FEBRISSIM_E_EXCEPTION; }
	catch (...) { LogBoundaryFailure("FebrisSimInitialize", nullptr); return FEBRISSIM_E_EXCEPTION; }
}

int32_t FebrisSimInitializeArgv(
	int32_t argc, const char* const* argv, int32_t expectedOs,
	int32_t* ready, char* extrasJson, int32_t extrasCap)
{
	if (argc < 0 || (argc > 0 && argv == nullptr))
	{
		return FEBRISSIM_E_INVALID_ARG;
	}
	try
	{
		std::vector<std::string> args;
		args.reserve((size_t)argc);
		for (int32_t i = 0; i < argc; ++i)
		{
			args.push_back(argv[i] ? argv[i] : "");
		}
		HandlerResult result = Initializer::Initialize(args, (ExpectedOperatingSystem)expectedOs);
		return FinishTuple(result, ready, extrasJson, extrasCap);
	}
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimInitializeArgv", &e); return FEBRISSIM_E_EXCEPTION; }
	catch (...) { LogBoundaryFailure("FebrisSimInitializeArgv", nullptr); return FEBRISSIM_E_EXCEPTION; }
}

int32_t FebrisSimGetSendableUpdate(int32_t* ready, char* extrasJson, int32_t extrasCap)
{
	if (StaticDetails::Handler == nullptr)
	{
		return FEBRISSIM_E_NOT_INITIALIZED;
	}
	try
	{
		HandlerResult result = StatementHandler::GetSendableUpdate();
		return FinishTuple(result, ready, extrasJson, extrasCap);
	}
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimGetSendableUpdate", &e); return FEBRISSIM_E_EXCEPTION; }
	catch (...) { LogBoundaryFailure("FebrisSimGetSendableUpdate", nullptr); return FEBRISSIM_E_EXCEPTION; }
}

int32_t FebrisSimGetSendableDispatchJson(char* buf, int32_t cap)
{
	if (StaticDetails::Handler == nullptr)
	{
		return FEBRISSIM_E_NOT_INITIALIZED;
	}
	try
	{
		SimulationDispatch dispatch = StatementHandler::GetSendableDispatch();
		XApiJson::ojson obj = XApiJson::ojson::object();
		obj["ready"] = dispatch.Ready;
		obj["intentAction"] = dispatch.IntentAction;
		if (!dispatch.HasExtras)
		{
			obj["extras"] = nullptr;
		}
		else
		{
			XApiJson::ojson extras = XApiJson::ojson::object();
			for (const auto& kv : dispatch.Extras)
			{
				extras[kv.first] = kv.second;
			}
			obj["extras"] = extras;
		}
		return WriteOut(obj.dump(), buf, cap);
	}
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimGetSendableDispatchJson", &e); return FEBRISSIM_E_EXCEPTION; }
	catch (...) { LogBoundaryFailure("FebrisSimGetSendableDispatchJson", nullptr); return FEBRISSIM_E_EXCEPTION; }
}

int32_t FebrisSimEndSimulation(int32_t* ready, char* extrasJson, int32_t extrasCap)
{
	if (StaticDetails::Handler == nullptr)
	{
		return FEBRISSIM_E_NOT_INITIALIZED;
	}
	try
	{
		HandlerResult result = StatementHandler::EndSimulation();
		return FinishTuple(result, ready, extrasJson, extrasCap);
	}
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimEndSimulation", &e); return FEBRISSIM_E_EXCEPTION; }
	catch (...) { LogBoundaryFailure("FebrisSimEndSimulation", nullptr); return FEBRISSIM_E_EXCEPTION; }
}

int32_t FebrisSimEndSimulationWith(
	int32_t success, int32_t complete, float rawScore, int64_t durationMs,
	int32_t* ready, char* extrasJson, int32_t extrasCap)
{
	if (StaticDetails::Handler == nullptr)
	{
		return FEBRISSIM_E_NOT_INITIALIZED;
	}
	try
	{
		HandlerResult result = StatementHandler::EndSimulation(
			success != 0, complete != 0, rawScore, (long long)durationMs);
		return FinishTuple(result, ready, extrasJson, extrasCap);
	}
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimEndSimulationWith", &e); return FEBRISSIM_E_EXCEPTION; }
	catch (...) { LogBoundaryFailure("FebrisSimEndSimulationWith", nullptr); return FEBRISSIM_E_EXCEPTION; }
}

void FebrisSimSimulationComplete(void)
{
	try { StatementHandler::SimulationComplete(); }
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimSimulationComplete", &e); }
	catch (...) { LogBoundaryFailure("FebrisSimSimulationComplete", nullptr); }
}

void FebrisSimSimulationPassed(int32_t passed)
{
	try { StatementHandler::SimulationPassed(passed != 0); }
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimSimulationPassed", &e); }
	catch (...) { LogBoundaryFailure("FebrisSimSimulationPassed", nullptr); }
}

void FebrisSimDurationUpdateMs(int64_t durationMs)
{
	try { StatementHandler::DurationUpdate((long long)durationMs); }
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimDurationUpdateMs", &e); }
	catch (...) { LogBoundaryFailure("FebrisSimDurationUpdateMs", nullptr); }
}

void FebrisSimStageRestart(void)
{
	try { StatementHandler::StageRestart(); }
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimStageRestart", &e); }
	catch (...) { LogBoundaryFailure("FebrisSimStageRestart", nullptr); }
}

void FebrisSimAddResultNote(const char* note)
{
	if (note == nullptr) { return; }
	try { StatementHandler::AddResultNote(note); }
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimAddResultNote", &e); }
	catch (...) { LogBoundaryFailure("FebrisSimAddResultNote", nullptr); }
}

/* ------------------------------------------------------------------ */
/* UpdateStatement family                                              */
/* ------------------------------------------------------------------ */

void FebrisSimUpdateResultBool(int32_t resultOption, int32_t value)
{
	try { StatementHandler::UpdateStatement(XAPIProperties::Result, (ResultOptions)resultOption, value != 0); }
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimUpdateResultBool", &e); }
	catch (...) { LogBoundaryFailure("FebrisSimUpdateResultBool", nullptr); }
}

void FebrisSimUpdateResultString(int32_t resultOption, const char* value)
{
	if (value == nullptr) { return; }
	try { StatementHandler::UpdateStatement(XAPIProperties::Result, (ResultOptions)resultOption, std::string(value)); }
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimUpdateResultString", &e); }
	catch (...) { LogBoundaryFailure("FebrisSimUpdateResultString", nullptr); }
}

void FebrisSimUpdateResultFloat(int32_t resultOption, float value)
{
	try { StatementHandler::UpdateStatement(XAPIProperties::Result, (ResultOptions)resultOption, value); }
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimUpdateResultFloat", &e); }
	catch (...) { LogBoundaryFailure("FebrisSimUpdateResultFloat", nullptr); }
}

void FebrisSimUpdateResultDurationMs(int64_t durationMs)
{
	// C# UpdateStatement(Result, Duration, TimeSpan) -- the TimeSpan overload.
	try { StatementHandler::UpdateStatement(XAPIProperties::Result, ResultOptions::Duration, (long long)durationMs); }
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimUpdateResultDurationMs", &e); }
	catch (...) { LogBoundaryFailure("FebrisSimUpdateResultDurationMs", nullptr); }
}

void FebrisSimUpdateResultExtensionInt(int32_t resultExtensionOption, int32_t value)
{
	try { StatementHandler::UpdateStatement(XAPIProperties::Result, ResultOptions::Extensions, (ResultExtensionOptions)resultExtensionOption, (int)value); }
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimUpdateResultExtensionInt", &e); }
	catch (...) { LogBoundaryFailure("FebrisSimUpdateResultExtensionInt", nullptr); }
}

void FebrisSimUpdateResultExtensionString(int32_t resultExtensionOption, const char* value)
{
	if (value == nullptr) { return; }
	try { StatementHandler::UpdateStatement(XAPIProperties::Result, ResultOptions::Extensions, (ResultExtensionOptions)resultExtensionOption, std::string(value)); }
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimUpdateResultExtensionString", &e); }
	catch (...) { LogBoundaryFailure("FebrisSimUpdateResultExtensionString", nullptr); }
}

void FebrisSimAddAttachment(
	const char* usageType, const char* display, const char* descriptionOrNull,
	int32_t contentType, int32_t length, const char* sha2, const char* fileUrlOrNull)
{
	if (usageType == nullptr || display == nullptr || sha2 == nullptr) { return; }
	try
	{
		// Route to the exact C# overload the null pattern selects, so the
		// emitted attachment shape matches the C# call sites one for one.
		const std::string usage = usageType;
		const std::string disp = display;
		const std::string sha = sha2;
		if (descriptionOrNull != nullptr && fileUrlOrNull != nullptr)
		{
			StatementHandler::UpdateStatement(XAPIProperties::Attachments, usage, disp, std::string(descriptionOrNull), (ContentType)contentType, (int)length, sha, std::string(fileUrlOrNull));
		}
		else if (descriptionOrNull != nullptr)
		{
			StatementHandler::UpdateStatement(XAPIProperties::Attachments, usage, disp, std::string(descriptionOrNull), (ContentType)contentType, (int)length, sha);
		}
		else if (fileUrlOrNull != nullptr)
		{
			StatementHandler::UpdateStatement(XAPIProperties::Attachments, usage, disp, (ContentType)contentType, (int)length, sha, std::string(fileUrlOrNull));
		}
		else
		{
			StatementHandler::UpdateStatement(XAPIProperties::Attachments, usage, disp, (ContentType)contentType, (int)length, sha);
		}
	}
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimAddAttachment", &e); }
	catch (...) { LogBoundaryFailure("FebrisSimAddAttachment", nullptr); }
}

void FebrisSimUpdateContext(int32_t contextOption, const char* value)
{
	if (value == nullptr) { return; }
	try { StatementHandler::UpdateStatement(XAPIProperties::Context, (ContextOptions)contextOption, std::string(value)); }
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimUpdateContext", &e); }
	catch (...) { LogBoundaryFailure("FebrisSimUpdateContext", nullptr); }
}

void FebrisSimUpdateContextActivity(int32_t contextActivityOption, const char* value)
{
	if (value == nullptr) { return; }
	try { StatementHandler::UpdateStatement(XAPIProperties::Context, ContextOptions::ContextActivites, (ContextActivitesOptions)contextActivityOption, std::string(value)); }
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimUpdateContextActivity", &e); }
	catch (...) { LogBoundaryFailure("FebrisSimUpdateContextActivity", nullptr); }
}

void FebrisSimUpdateContextStatementReference(int32_t referenceOption, const char* value)
{
	if (value == nullptr) { return; }
	try { StatementHandler::UpdateStatement(XAPIProperties::Context, ContextOptions::StatementReference, (ContextStatementReferenceOptions)referenceOption, std::string(value)); }
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimUpdateContextStatementReference", &e); }
	catch (...) { LogBoundaryFailure("FebrisSimUpdateContextStatementReference", nullptr); }
}

void FebrisSimUpdateContextExtension(int32_t contextExtensionOption, const char* value)
{
	if (value == nullptr) { return; }
	try { StatementHandler::UpdateStatement(XAPIProperties::Context, ContextOptions::Extensions, (ContextExtensionOptions)contextExtensionOption, std::string(value)); }
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimUpdateContextExtension", &e); }
	catch (...) { LogBoundaryFailure("FebrisSimUpdateContextExtension", nullptr); }
}

/* ------------------------------------------------------------------ */
/* Verbs                                                               */
/* ------------------------------------------------------------------ */

int32_t FebrisSimVerbUpdate(int32_t verbEnum)
{
	try { return StatementHandler::VerbUpdate((VerbEnums)verbEnum) ? 1 : 0; }
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimVerbUpdate", &e); return FEBRISSIM_E_EXCEPTION; }
	catch (...) { LogBoundaryFailure("FebrisSimVerbUpdate", nullptr); return FEBRISSIM_E_EXCEPTION; }
}

void FebrisSimCustomVerbUpdate(const char* verbIri)
{
	if (verbIri == nullptr) { return; }
	try { StatementHandler::CustomVerbUpdate(verbIri); }
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimCustomVerbUpdate", &e); }
	catch (...) { LogBoundaryFailure("FebrisSimCustomVerbUpdate", nullptr); }
}

/* ------------------------------------------------------------------ */
/* Introspection of the working statement                              */
/* ------------------------------------------------------------------ */

int32_t FebrisSimGetCurrentStatementJson(char* buf, int32_t cap)
{
	if (StaticDetails::Handler == nullptr)
	{
		return FEBRISSIM_E_NOT_INITIALIZED;
	}
	try
	{
		return WriteOut(StaticDetails::CurrentStatement.dump(), buf, cap);
	}
	catch (const std::exception& e) { LogBoundaryFailure("FebrisSimGetCurrentStatementJson", &e); return FEBRISSIM_E_EXCEPTION; }
	catch (...) { LogBoundaryFailure("FebrisSimGetCurrentStatementJson", nullptr); return FEBRISSIM_E_EXCEPTION; }
}

} /* extern "C" */
