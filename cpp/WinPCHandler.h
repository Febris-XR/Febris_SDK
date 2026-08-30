// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef WINPCHANDLER_H
#define WINPCHANDLER_H
//#include "pch.h";
#endif 


class WINPCHANDLER_H WinPCHandler : public IEnvironmentHandler
{
public:


	virtual HandlerResult CreateInitialPost(XApiJson::ojson& statementFromDataModel) override;

	virtual HandlerResult UpdatePost(XApiJson::ojson& statementFromDataModel) override;

	virtual HandlerResult ErrorPost(XApiJson::ojson& statementFromDataModel) override;

	// C# `internal static` serializer helper. Compact JsonConvert.SerializeObject
	// mirror over ordered keys (SDKV emit port).
	static std::string SerializeString(const XApiJson::ojson& jObject);

private:
	// Mutating by design: both stamp the wire id (SDKV-19/20) onto the shared
	// working statement before serializing, so the parameter is a non-const
	// reference (C# JObject reference semantics).
	static bool WriteFile(XApiJson::ojson& statementFromDataModel);
	static bool WriteErrorFile(XApiJson::ojson& statementFromDataModel);

};
#pragma once
