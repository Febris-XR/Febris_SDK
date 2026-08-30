// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"

// EXPORT-SURFACE PORT (2026-08-29): global-scope definitions for JSONHandler.h,
// ported from the CURRENT C# Statement/JSONHandler.cs. The same-named class that
// lived here inside namespace FebrisCppStatement was pre-refactor drift -- every
// cross-file caller bound the header declaration, so nothing could ever have
// linked against it -- and it is deleted along with the retired file-hand-off
// methods it carried (CreateInitalDataFile / WriteToDataFile / GetJObject and the
// to_ptree/StructToJson experiments).

/// <summary>
/// Deals with processed command line arguments coming from PC launcher
/// (C# JSONHandler.cs:40-64).
/// </summary>
std::string JSONHandler::ArgumentHandler(std::vector<std::string> arguments)
{
	std::string jsonString;  // C#: string.Empty
	for (const std::string& arg : arguments)
	{
		if (String_Helpers::StartsWith(arg, SharedDetails::StatementPreface))
		{
			jsonString = arg.substr(SharedDetails::StatementPrefaceLength);
			// C#: jsonString != null && jsonString.Length > 2 (substr never
			// answers the C# null, so the length guard is the whole check)
			if (jsonString.length() > 2)
			{
				// C# wraps this bare return in a try/catch that can never fire;
				// carried shape-for-shape.
				try
				{
					return jsonString;
				}
				catch (const std::exception&)
				{
					//_log.Error("ArgumentHandler Error: "+ex.Message);
					//throw;
				}
			}
		}
	}
	return jsonString;
}

// C# JSONHandler.cs:66-78.
std::string JSONHandler::SerializeString(std::string input)
{
	std::string outputString;  // C#: string.Empty
	try
	{
		// C#: JValue.Parse(input).ToString(Formatting.Indented)
		outputString = json::parse(input).dump(4);
	}
	catch (const std::exception&)
	{
		//_log.Error("SerializeString Error: " + ex.Message);
	}
	return outputString;
}

// C# JSONHandler.cs:80-95. Pre-factoring INPUT handling: plain json on purpose.
json JSONHandler::ChangeToObject(std::string inputString)
{
	json jObject = json::object();  // C#: new JObject()
	try
	{
		// C#: JObject.Parse(inputString) -- object text only; array/scalar text
		// throws there, so it must land in the catch here too and answer the
		// empty object.
		json parsed = json::parse(inputString);
		if (!parsed.is_object())
		{
			throw std::runtime_error("input is not a JSON object");
		}
		jObject = parsed;
	}
	catch (const std::exception& e)
	{
		// C#: Console.WriteLine(ex.Message) + Console.WriteLine(ex.StackTrace).
		// Routed through the SIM-T13 logger on the native side (no stack trace
		// to print here).
		StaticDetails::Log(SimulationLogLevel::Error, "JSONHandler.ChangeToObject: parse failed.", e.what());
		//_log.Error("ChangeToObject Error: " + ex.Message);
	}
	return jObject;
}

// C# JSONHandler.cs:96-114.
XApiJson::ojson JSONHandler::CreateObjectFromDataModel(json inputJObject)
{
	//variable
	Statement statement;  // C#: new Models.xAPI.Statement()
	XApiJson::ojson jObject = XApiJson::ojson::object();  // C#: new JObject()
	try
	{
		//statement builder
		StatementFactoring statementBuilder;  // C#: new StatementFactoring()
		statement = statementBuilder.FactorStatement(inputJObject);
		//jobject to send back
		// C#: JObject.FromObject(statement). SDKV emit port: ordered-key
		// serialization via the XApiJson to_json overloads -- key order IS the
		// wire contract, so the emitted object is ordered_json end to end.
		to_json(jObject, statement);
	}
	catch (const std::exception&)
	{
		//_log.Error("CreateObjectFromDataModel Error: " + ex.Message);
	}
	return jObject;
}
