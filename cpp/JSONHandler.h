// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "pch.h"
#ifndef JSONHANDLER_H
#define JSONHANDLER_H
//#include "pch.h";
#endif

// EXPORT-SURFACE PORT (2026-08-29): mirror of the CURRENT C# Statement/JSONHandler.cs.
// The live surface is the four helpers below; everything else is retired on BOTH
// sides since the Android 11 update/refactor (see the [retired post-Android-11]
// marker at the bottom). The never-defined to_ptree/StructToJson template
// declarations that lived here were pre-refactor drift, superseded by the XApiJson
// emit port, and are deleted.

class JSONHANDLER_H JSONHandler
{
public:
	//private static readonly ILogger //_log = Log.Logger;

	/// <summary>
	/// Deals with processed command line arguments coming from PC launcher
	/// </summary>
	static std::string ArgumentHandler(std::vector<std::string> arguments);

	// C#: JValue.Parse(input).ToString(Formatting.Indented); parse failure answers "".
	static std::string SerializeString(std::string input);

	// Pre-factoring INPUT handling, so plain (unordered) json on purpose. The
	// ordered wire object is built by CreateObjectFromDataModel below.
	static json ChangeToObject(std::string inputString);

	// StatementFactoring + XApiJson emit. Returns ordered_json because insertion
	// order IS the wire key order (SDKV emit port) -- this is the object the whole
	// working-statement pipeline carries from here on.
	static XApiJson::ojson CreateObjectFromDataModel(json inputJObject);

	// [retired post-Android-11] The C# file keeps these only as its commented
	// "unused" region; the C++ declarations are deleted outright:
	// CreateInitalDataFile / WriteToDataFile / GetJObject (the FileNameHandler +
	// MMF file hand-off, dead on both sides), the single-argument
	// ArgumentHandler(string), and SerializeString(json).
};
