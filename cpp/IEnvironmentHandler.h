// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef IENVIRONMENTHANDLER_H
#define IENVIRONMENTHANDLER_H
//#include "pch.h";
#endif

// EXPORT-SURFACE PORT (2026-08-29): mirror of the C# Service.IEnvironmentHandler.
// The C# methods are async Task<(bool, string[,])>; the C++ SDK is synchronous by
// design (no task machinery exists or is needed on the native path), so the tuple
// flattens into HandlerResult below and each method returns it directly.

// Mirror of the C# (bool, string[,]) handler tuple. The string[,] is an N x 2
// rectangular array of key/value intent-extra pairs (row [i,0]=key, [i,1]=value)
// and CAN be null -- WinPCHandler returns default -- which is distinct from an
// empty array (SimulationDispatch.ToLegacyTuple pins the mapping). HasExtras
// carries that null-vs-empty distinction.
struct HandlerResult
{
	bool Ready = false;
	bool HasExtras = false;
	std::vector<std::pair<std::string, std::string>> Extras;
};

class IENVIRONMENTHANDLER_H IEnvironmentHandler
{
public:

	virtual ~IEnvironmentHandler() = default;

	// The working statement is passed by reference on purpose: C# JObject is a
	// reference type, so handler-side mutations (StatementHandler::StampStatementId
	// inside WinPCHandler's WriteFile) flow back into StaticDetails::CurrentStatement.
	// A by-value ojson would silently drop that write-back.
	virtual HandlerResult CreateInitialPost(XApiJson::ojson& statementFromDataModel) = 0;

	virtual HandlerResult UpdatePost(XApiJson::ojson& statementFromDataModel) = 0;

	virtual HandlerResult ErrorPost(XApiJson::ojson& statementFromDataModel) = 0;

};
