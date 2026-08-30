// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
#include <stdexcept>

// EXPORT-SURFACE PORT (2026-08-29): real global-scope definitions for the
// WinPCHandler declared in WinPCHandler.h, ported from the CURRENT C#
// Service/WinPCHandler.cs (SDKV-19/20 + SIM-T13 fix series). This TU previously
// held a dead same-named duplicate class inside namespace Service. That
// duplicate was PRE-REFACTOR drift (hand-rolled UUIDs, pretty-printed dump(4),
// no id stamping, unordered json) that nothing ever linked against. Deleted.
// Do not resurrect it as a semantic source.

// ---- CRUD / Create ----

HandlerResult WinPCHandler::CreateInitialPost(XApiJson::ojson& statementFromDataModel)
{
	HandlerResult result{};
	try
	{
		StaticDetails::SetReferenceUUID(StaticDetails::NewUuidString());
		// PORT NOTE (JObject reference semantics): in C# this assignment ALIASES.
		// CurrentStatement and the caller's object become one JObject, so the id
		// stamped inside WriteFile below lands on both. ojson assignment COPIES.
		// The stamp still reaches the caller's object (the ojson& parameter), and
		// StampStatementId is idempotent against the unchanged ReferenceUUID, so
		// the first UpdatePost (always called with CurrentStatement itself,
		// StatementHandler.cs line 69) re-stamps the SAME id onto CurrentStatement.
		// Residual divergence window: CurrentStatement carries no wire id between
		// this call and the first UpdatePost unless the caller passes
		// StaticDetails::CurrentStatement as the argument.
		StaticDetails::CurrentStatement = statementFromDataModel;
		// file system initalizer
		FileSystemInitalizer::FileInitalizer();
		// write to data file
		result.Ready = WriteFile(statementFromDataModel);
	}
	catch (const std::exception& e)
	{
		// SIM-T13 G4: surface through the configured logger so the host
		// game can see the failure. Re-throw preserved -- callers may be
		// depending on it.
		StaticDetails::Log(SimulationLogLevel::Error, "WinPCHandler.CreateInitialPost: failed.", e.what());
		throw;
	}
	// C# returns (isInitialized, default) -- no extras on the PC path, so
	// HasExtras stays false (the null-vs-empty distinction in HandlerResult).
	return result;
}

// ---- CRUD / Update ----

HandlerResult WinPCHandler::UpdatePost(XApiJson::ojson& statementFromDataModel)
{
	HandlerResult result{};
	try
	{
		result.Ready = WriteFile(statementFromDataModel);
	}
	catch (const std::exception& e)
	{
		// SIM-T13 G4: surface through logger instead of Console swallow.
		StaticDetails::Log(SimulationLogLevel::Error, "WinPCHandler.UpdatePost: failed.", e.what());
		throw;
	}
	return result;
}

// SIM-T13 G2: error-emission for the PC path. Mirrors UpdatePost but writes to
// a sibling file with the .error.json suffix so the PC Statement Manager (or
// test fixtures) can distinguish error-statement files from normal updates
// without parsing the JSON content. Was previously an unguarded
// NotImplementedException -- calling this from a Unity game running on Windows
// PC crashed the host.
HandlerResult WinPCHandler::ErrorPost(XApiJson::ojson& statementFromDataModel)
{
	HandlerResult result{};
	try
	{
		result.Ready = WriteErrorFile(statementFromDataModel);
	}
	catch (const std::exception& e)
	{
		// SIM-T13 G4: route through the configured logger instead of
		// Console.WriteLine so the host can surface failures into its
		// own diagnostics path. NOTE: no rethrow here -- the C# error
		// path swallows so error emission can never crash the host.
		StaticDetails::Log(SimulationLogLevel::Error, "WinPCHandler.ErrorPost: failed writing error-statement file.", e.what());
	}
	return result;
}

bool WinPCHandler::WriteErrorFile(XApiJson::ojson& statementFromDataModel)
{
	// Same shape as WriteFile(...) below but with a .error.json suffix
	// so the consumer side can pick it out without inspecting content.
	// FIX (SDKV-19/20): stamp the statement id before serializing so
	// the wire id matches the handoff filename GUID.
	StatementHandler::StampStatementId(statementFromDataModel);
	std::string statementFileName = StaticDetails::GetReferenceUUID() + ".error.json";
	bool dataWritten = false;
	// C# File.CreateText(Path.Combine(...)) creates-or-overwrites, and it sits
	// OUTSIDE the inner try. An open failure (missing directory and the like)
	// escapes to the caller, where ErrorPost logs and swallows it. Mirror that
	// with an explicit throw.
	fs::path fullPath = fs::path(FileSystem::StatementPath) / statementFileName;
	std::ofstream file(fullPath, std::ios::binary | std::ios::trunc);
	if (!file.is_open())
	{
		throw std::runtime_error("WinPCHandler.WriteErrorFile: could not open " + fullPath.string() + " for writing.");
	}
	try
	{
		std::string statementString = SerializeString(statementFromDataModel);
		file.write(statementString.data(), static_cast<std::streamsize>(statementString.size()));
		file.flush();
		if (!file)
		{
			// ofstream reports write failure via stream state, not exceptions.
			// Promote it so the catch below mirrors the C# IOException path.
			throw std::runtime_error("stream reported a write failure");
		}
		dataWritten = true;
	}
	catch (const std::exception& e)
	{
		StaticDetails::Log(SimulationLogLevel::Error, "WinPCHandler.WriteErrorFile: serialization or disk write failed.", e.what());
	}
	return dataWritten;
}

// ---- Helpers ----

bool WinPCHandler::WriteFile(XApiJson::ojson& statementFromDataModel)
{
	// FIX (SDKV-19/20): stamp the statement id before serializing so
	// every emitted {uuid}.json carries a wire id equal to the handoff
	// filename GUID; re-writes of the same run keep the same id, which
	// is what makes host retries idempotent on the ingest node.
	StatementHandler::StampStatementId(statementFromDataModel);
	std::string statementFileName = StaticDetails::GetReferenceUUID() + ".json";
	bool dataWritten = false;
	// C# File.CreateText(Path.Combine(...)) creates-or-overwrites, and it sits
	// OUTSIDE the inner try. An open failure escapes to the caller, where
	// CreateInitialPost / UpdatePost log and rethrow. Mirror that with an
	// explicit throw.
	fs::path fullPath = fs::path(FileSystem::StatementPath) / statementFileName;
	std::ofstream file(fullPath, std::ios::binary | std::ios::trunc);
	if (!file.is_open())
	{
		throw std::runtime_error("WinPCHandler.WriteFile: could not open " + fullPath.string() + " for writing.");
	}
	try
	{
		std::string statementString = SerializeString(statementFromDataModel);
		file.write(statementString.data(), static_cast<std::streamsize>(statementString.size()));
		//file.Write(SerializeString(statement));
		//SerializeString(statement);
		file.flush();
		if (!file)
		{
			// Promote stream-state failure so the catch mirrors the C# IOException path.
			throw std::runtime_error("stream reported a write failure");
		}
		dataWritten = true;
	}
	catch (const std::exception& e)
	{
		// SIM-T13 G4: surface disk-write failures through the logger.
		StaticDetails::Log(SimulationLogLevel::Error, "WinPCHandler.WriteFile: disk write failed for " + statementFileName + ".", e.what());
	}
	return dataWritten;
}

std::string WinPCHandler::SerializeString(const XApiJson::ojson& jObject)
{
	std::string outputString;
	try
	{
		// C# JsonConvert.SerializeObject default formatting is COMPACT.
		// ordered_json::dump() matches, and insertion order IS the wire key
		// order (SDKV emit port).
		outputString = jObject.dump();
	}
	catch (const std::exception& e)
	{
		// SIM-T13 G4: serialization failures bubble through the logger.
		StaticDetails::Log(SimulationLogLevel::Error, "WinPCHandler.SerializeString: JsonConvert.SerializeObject failed.", e.what());
	}
	return outputString;
}
