// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef INITIALIZER_H
#define INITIALIZER_H
//#include "pch.h";
#endif

// EXPORT-SURFACE PORT (2026-08-29): mirror of the CURRENT C# Statement/Initializer.cs
// (the post-Android-11 pipeline plus the SDKV/SIM-T13 fix series). The four retired
// pre-refactor Initialize overload declarations that lived here are deleted -- the
// C# side keeps their five earlier shapes only as the commented
// [Historical - SIM-T13] block (gap G11, docs/SIMULATION_ROADMAP/
// TIER_13_AUDIT_MDM_COMPAT.md); do NOT revive them.

class INITIALIZER_H Initializer
{
public:
	/// <summary>
	/// This is the new default way of Initializing this library. It Now takes into
	/// account the Operating system and will route files properly taking into
	/// account the changes made in Android 11+
	///
	/// This had to be update to work with intents and completely get rid of
	/// FileSystem
	/// </summary>
	// C#: async Task<(bool, string[,])>. The C++ SDK is synchronous by design, so
	// the tuple flattens into HandlerResult (see IEnvironmentHandler.h);
	// HasExtras=false plays the C# null-array (default) role.
	static HandlerResult Initialize(std::vector<std::string> input, ExpectedOperatingSystem OS);

	/// <summary>
	/// SIM-T13 G4 overload -- initialize with a host-provided logger so library
	/// failures route into the game's diagnostics surface instead of being
	/// swallowed by console output. Hosts that don't have a logger to supply can
	/// keep calling the original overload above; the library then uses the
	/// console fallback sink (StaticDetails::DefaultConsoleLog).
	/// </summary>
	static HandlerResult Initialize(std::vector<std::string> input, ExpectedOperatingSystem OS, SimulationLogFn logger);

	/// <summary>
	/// Replaces a string-typed node whose text is serialized JSON with the parsed
	/// structure. FIX (SDKV-1): array-shaped text now parses as an array as well
	/// as object-shaped text (previously only objects were attempted, so
	/// array-shaped values stayed strings on the wire). Non-string nodes (already
	/// objects/arrays from the typed models) and unparseable text are left
	/// untouched. Internal for test access on the C# side.
	/// C# takes a JToken and calls input.Replace(parsed) to swap the node inside
	/// the tree; nlohmann has no token replace, so the C++ shape operates on the
	/// referenced node in place -- pass the tree node itself, never a copy.
	/// </summary>
	static void ChangeStringToObject(XApiJson::ojson& holder);

	/// <summary>
	/// Recursively lowercases every property name. SDKV-12 note: this is now used
	/// ONLY to normalize the authored INPUT statement before factoring
	/// (StatementFactoring reads all-lowercase keys) -- hence plain json, the
	/// pre-factoring input type. It must never run on the factored output again;
	/// emission casing comes from the explicit wire names in the XApiJson to_json
	/// overloads (the C# side's [JsonProperty] names). FIX (SDKV-23): arrays are
	/// walked per element by actual token type (see
	/// ChangePropertiesToLowerCaseInArray) instead of blind-casting every element
	/// to an object, and language-map values keep their RFC 5646 tag keys
	/// untouched (see LanguageMapHolderKeys). Internal for test access on the C#
	/// side (same MDM-T2 rationale as ChangeStringToObject).
	/// </summary>
	static void ChangePropertiesToLowerCase(json& jsonObject);

private:
	/// <summary>
	/// Property names whose OBJECT values are xAPI Language Maps (RFC 5646
	/// language tag -> string): verb.display, definition.name/description,
	/// attachments[].display/description, interactioncomponents[].description.
	/// Their child keys are language TAGS ("en-US"), not dialect property names --
	/// the canonical RFC 5646 casing (lowercase language, UPPERCASE region) must
	/// survive the input-normalization pass because
	/// StatementFactoring::TokenToLanguageMap copies the keys verbatim onto the
	/// wire. The holder key itself IS still lowercased (factoring reads
	/// all-lowercase keys). In the authored dialect no non-language-map object
	/// ever sits under these names (actor.name / account.name are plain strings,
	/// which never recurse anyway).
	/// </summary>
	static constexpr const char* LanguageMapHolderKeys[3] = { "display", "name", "description" };

	/// <summary>
	/// FIX (SDKV-23): array companion to ChangePropertiesToLowerCase. Object
	/// elements recurse into the normal key-lowercasing walk, nested arrays
	/// recurse here, and scalar elements (strings, numbers, booleans, nulls)
	/// carry no property names so they pass through untouched -- previously they
	/// were blind-cast to an object, which threw and failed the whole Initialize.
	/// </summary>
	static void ChangePropertiesToLowerCaseInArray(json& arrayNode);

	/// <summary>
	/// FIX (SDKV-23, part 2): true when propertyName is one of the
	/// LanguageMapHolderKeys (authored casing is arbitrary, so the comparison is
	/// case-insensitive).
	/// </summary>
	static bool IsLanguageMapHolder(const std::string& propertyName);
};
