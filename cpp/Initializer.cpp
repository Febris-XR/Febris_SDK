// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"

// EXPORT-SURFACE PORT (2026-08-29): global-scope definitions for Initializer.h,
// ported from the CURRENT C# Statement/Initializer.cs (the post-Android-11
// pipeline, C# lines 39-228, plus the SDKV/SIM-T13 helper fixes, lines 261-425).
// The same-named class that lived here inside namespace FebrisCppStatement carried
// six pre-refactor Initialize overloads -- all drift from before the C# refactor,
// never linkable from the header declarations -- and is deleted.

/// <summary>
/// SIM-T13 G4 overload -- initialize with a host-provided logger so library
/// failures route into the game's diagnostics surface instead of being swallowed
/// by console output. (C# Initializer.cs:39-43)
/// </summary>
HandlerResult Initializer::Initialize(std::vector<std::string> input, ExpectedOperatingSystem OS, SimulationLogFn logger)
{
	// C#: StaticDetails.StaticDetails.Logger = logger; SetLogger keeps the C#
	// setter's null-reset-to-console-fallback semantics.
	StaticDetails::SetLogger(logger);
	return Initialize(input, OS);
}

// C# Initializer.cs:45-228.
HandlerResult Initializer::Initialize(std::vector<std::string> input, ExpectedOperatingSystem OS)
{
	//Console.WriteLine("Inside Initializer");
	bool isInitialized = false;
	// C#: string[,] outputArray = default. HandlerResult flattens the C# tuple;
	// the default (Ready=false, HasExtras=false) plays the (false, null) role.
	HandlerResult outputResult;
	try
	{
		// [Historical] "file system initalizer -- need this moved to platform
		// specific areas" region: fully commented out on the C# side, retired
		// post-Android-11 along with FileSystem/FileSystemInitalizer.

		//#region command line args
		//get command line args
		std::vector<std::string> arguments = input;
		std::string statement = JSONHandler::ArgumentHandler(arguments);
		//#endregion

		//#region replace removed spaces from statement
		statement = String_Helpers::Replace(statement, "_-_", " ");
		//#endregion

		//#region Change to initialJobject
		json statementJObject = JSONHandler::ChangeToObject(statement);
		// INPUT normalization only (kept on purpose): authored statements arrive
		// with arbitrary key casing (the launcher/test harness author PascalCase)
		// and StatementFactoring reads all-lowercase keys. Lowercasing the INPUT
		// is a tolerant read. The matching OUTPUT lowercasing below was the
		// SDKV-12 bug and is retired.
		ChangePropertiesToLowerCase(statementJObject);
		//#endregion

		//#region Set up mapped logging
		// [Historical] log-directory creation (objectId + InitializeLogging):
		// commented out on the C# side, retired with the file system. The C#
		// InitializeLogging helper itself is dead (private, no caller) and is not
		// ported.
		std::cout << "Finished creating log directory" << std::endl;
		//#endregion

		//#region convert to jobject following data model
		XApiJson::ojson statementFromDataModel = JSONHandler::CreateObjectFromDataModel(statementJObject);
		// [Historical - SDKV-12] output lowercasing pass: re-lowercasing the
		// factored statement corrupted the case-sensitive xAPI 1.0.3 keys
		// (objectType, homePage, moreInfo, interactionType,
		// correctResponsesPattern, usageType, contentType, fileUrl) on the wire --
		// a conformant LRS rejects the Account IFI and loses objectType
		// discriminators. Superseded by the explicit wire names the XApiJson
		// to_json overloads emit (the C# side's [JsonProperty] declarations).
		//ChangePropertiesToLowerCase(statementFromDataModel);
		//#endregion

		//#region Check needed parts of JObject
		bool statmentIsProperlySetUp = StatementHandler::StatementCheck(statementFromDataModel);
		if (!statmentIsProperlySetUp)
		{
			// C#: return (statmentIsProperlySetUp, outputArray);
			outputResult.Ready = statmentIsProperlySetUp;
			return outputResult;
		}
		//#endregion

		//#region set the new strings to jobjects
		// interactioncomponents is still stored as a string on the typed model
		// (its authored shape is free-form object/array text), so it is the one
		// remaining field that needs re-objectifying here.
		// C# chained-index semantics: a missing "object" or "definition" throws
		// (NullReferenceException) into the outer catch -- at() mirrors that with
		// json::out_of_range. A missing LEAF reaches the C# helper as a null
		// token and is left untouched, so the leaf is guarded instead of thrown.
		{
			XApiJson::ojson& definition = statementFromDataModel.at("object").at("definition");
			if (definition.contains("interactioncomponents"))
			{
				ChangeStringToObject(definition.at("interactioncomponents"));
			}
		}
		// [Historical - SDKV-1/2/13] re-objectification of typed fields:
		// verb.display, definition.name/description and
		// definition.correctresponsespattern are now typed on the models, so the
		// factored output already carries real objects/arrays and the
		// string-repair pass is superseded (the C# side keeps those calls
		// commented out; the attachment display/description calls were already
		// commented out historically -- the typed Attachment model supersedes
		// them too).
		//#endregion

		std::cout << "Finished jobject factoring" << std::endl;

		switch (OS)
		{
			case ExpectedOperatingSystem::WindowsPC:
			{
				StaticDetails::Handler = std::make_shared<WinPCHandler>();
				//isInitialized = JSONHandler.CreateInitialDataFile(statementFromDataModel);
				break;
			}
			case ExpectedOperatingSystem::Android:
			{
				StaticDetails::Handler = std::make_shared<AndroidHandler>();
				break;
			}
			case ExpectedOperatingSystem::iOSvariant:
			{
				StaticDetails::Handler = std::make_shared<iOSHandler>();
				break;
			}
			case ExpectedOperatingSystem::WinMobile:
			{
				StaticDetails::Handler = std::make_shared<WinMobileHandler>();
				break;
			}
		}

		// C#: a Handler left null here surfaces as a NullReferenceException in
		// the outer catch; dereferencing a null shared_ptr is UB in C++, so the
		// equivalent throw is explicit. (Unreachable for the four enum values,
		// which the switch covers exhaustively.)
		if (!StaticDetails::Handler)
		{
			throw std::runtime_error("no environment handler set for the requested OS");
		}

		// C#: (isInitialized, outputArray) = await StaticDetails.Handler.CreateInitialPost(statementFromDataModel);
		// The working statement goes by reference on purpose (C# JObject is a
		// reference type): handler-side mutations must flow back into this tree.
		outputResult = StaticDetails::Handler->CreateInitialPost(statementFromDataModel);
		isInitialized = outputResult.Ready;

		std::cout << "Finished writing data to file" << std::endl;

		// [Historical] the trailing switch (OS) in the C# file (lines 188-211)
		// has an empty arm in every live case (the WindowsPC arm is commented
		// out) -- carried as this marker rather than as dead control flow.
	}
	catch (const std::exception& e)
	{
		// FIX (SIM-B11): route initialization failure through the host logger
		// instead of Console.WriteLine so it reaches the game diagnostics
		// surface. See docs/MODERNIZATION/SIM_MODERNIZATION.md.
		//Console.WriteLine(ex.Message);
		StaticDetails::Log(SimulationLogLevel::Error, "Initializer: initialization failed.", e.what());
		//Log.Logger.Fatal(ex.Message);
		//Log.Logger.Fatal(ex.Source);
		// C#: return (isInitialized, outputArray);
		outputResult.Ready = isInitialized;
		return outputResult;
		// FIX (SIM-B11): removed unreachable throw after the return above (dead
		// code). See docs/MODERNIZATION/SIM_MODERNIZATION.md.
		//throw;
	}
	// C#: return (isInitialized, outputArray);
	outputResult.Ready = isInitialized;
	return outputResult;
}

//#region Helpers
//#region break strings into objects
// C# Initializer.cs:267-317. (The C# body carries an older commented-out
// attachments-array branch above the live code; not ported.)
void Initializer::ChangeStringToObject(XApiJson::ojson& holder)
{
	try
	{
		// C#: input == null || input.Type != JTokenType.String. The absent-token
		// case is handled at the call site (a reference must bind to a real
		// node); an explicit JSON null lands in is_null() here.
		if (holder.is_null() || !holder.is_string())
		{
			// already structured (typed models emit real objects/arrays)
			// or nothing to repair
			return;
		}
		std::string inputValue = holder.get<std::string>();
		// C#: inputValue.Trim()
		std::string trimmed = inputValue;
		trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
		trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);
		XApiJson::ojson output;
		if (!trimmed.empty() && trimmed[0] == '[')
		{
			// FIX (SDKV-1): attempt the array parse for array-shaped text. The
			// C# side guards Newtonsoft's lenient parse of degenerate text like
			// "[,]" (to [undefined]) with StatementFactoring.HasUndefinedToken;
			// nlohmann's parse is STRICT and throws on that text, which lands in
			// the same leave-the-string-untouched outcome, so no guard is needed
			// here.
			output = XApiJson::ojson::parse(trimmed);
		}
		else
		{
			// C#: JsonConvert.DeserializeObject<JObject>(inputValue) -- object
			// text only. Scalar text throws there (and so stays a string), so
			// mirror that by leaving non-object parses untouched.
			output = XApiJson::ojson::parse(inputValue);
			if (!output.is_object())
			{
				return;
			}
		}
		// C#: input.Replace(output) swaps the token inside the tree; the
		// reference parameter makes this the same in-place assignment.
		holder = output;
	}
	catch (const std::exception&)
	{
		//Log.Logger.Error("Error converting string to Object :" + ex.Message);
	}
}
//#endregion

//#region change properties to lowercase
// C# Initializer.cs:352-390.
void Initializer::ChangePropertiesToLowerCase(json& jsonObject)
{
	// The C# parameter type (JObject) enforces object-ness statically; mirror
	// that tolerance for the untyped json node.
	if (!jsonObject.is_object())
	{
		return;
	}
	// C# iterates a snapshot (Properties().ToList()) because properties are
	// replaced mid-walk; the key snapshot below plays the same role.
	std::vector<std::string> names;
	names.reserve(jsonObject.size());
	for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it)
	{
		names.push_back(it.key());
	}
	for (const std::string& name : names)
	{
		json& value = jsonObject[name];
		if (value.is_object())// replace property names in child object
		{
			// FIX (SDKV-23, part 2): don't descend into language maps -- their
			// keys are RFC 5646 tags ("en-US" stays "en-US"), not dialect
			// property names.
			if (!IsLanguageMapHolder(name))
			{
				ChangePropertiesToLowerCase(value);
			}
		}

		if (value.is_array())
		{
			// [Historical - SDKV-23] blind JObject cast per array element: the
			// original array walk round-tripped the array through text and cast
			// EVERY element to JObject. A real JSON string array (spec-shaped
			// correctresponsespattern: ["a","b"]) made the cast throw, the
			// Initialize-wide catch swallowed it, and the whole initialization
			// returned false -- nothing emitted. Replaced by the type-checked
			// per-element walk below.
			ChangePropertiesToLowerCaseInArray(value);
		}

		// C#: property.Replace(new JProperty(property.Name.ToLower(),
		// property.Value)) -- Newtonsoft properties are read-only, so C# has to
		// replace them; here the value simply moves to the lowercased key when
		// the spelling differs.
		std::string lower = name;
		for (char& c : lower)
		{
			c = (char)tolower((unsigned char)c);
		}
		if (lower != name)
		{
			json moved = std::move(value);
			jsonObject.erase(name);
			jsonObject[lower] = std::move(moved);
		}
	}
}

// C# Initializer.cs:401-414.
void Initializer::ChangePropertiesToLowerCaseInArray(json& arrayNode)
{
	for (json& element : arrayNode)
	{
		if (element.is_object())
		{
			ChangePropertiesToLowerCase(element);
		}
		else if (element.is_array())
		{
			ChangePropertiesToLowerCaseInArray(element);
		}
	}
}

// C# Initializer.cs:421-425.
bool Initializer::IsLanguageMapHolder(const std::string& propertyName)
{
	// C#: string.Equals(..., StringComparison.OrdinalIgnoreCase)
	for (const char* holder : LanguageMapHolderKeys)
	{
		if (_stricmp(propertyName.c_str(), holder) == 0)
		{
			return true;
		}
	}
	return false;
}
//#endregion
//#endregion
