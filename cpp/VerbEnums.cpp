// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
// VerbEnums resolver port (2026-08-29): real global-scope definitions for the class
// declared in VerbEnums.h, ported from the CURRENT C# Enums/VerbEnums.cs. The old
// same-named duplicate inside the dead namespace FebrisCppEnums (which no caller
// could ever have linked against) is deleted.
// [Historical - the C# file keeps a commented-out VerbStage enum (Completed, Terminated) next to VerbEnums]

string VerbIRIResolver::ResolveVerbIRI(VerbEnums iri)
{
	switch (iri)
	{
	case VerbEnums::Attempted:
		return "https://febr.is/Verb/Details/Attempted";
	case VerbEnums::Completed:
		return "https://febr.is/Verb/Details/Completed";
	case VerbEnums::Initialized:
		return "https://febr.is/Verb/Details/Initialized";
	case VerbEnums::Terminated:
		return "https://febr.is/Verb/Details/Terminated";
	case VerbEnums::Pass:
		return "https://febr.is/Verb/Details/Pass";
	case VerbEnums::Not_Pass:
		return "https://febr.is/Verb/Details/Not_Pass";
	default:
		// Handle bad URL, possibly throw
		// FIX (SIM-B3): C# throws new Exception() here; a bare C++ 'throw;' outside a catch calls std::terminate. Throw a real exception instead. See docs/MODERNIZATION/SIM_MODERNIZATION.md.
		throw std::runtime_error("unknown verb enum in ResolveVerbIRI");
	}
}

// C# switches on the verb IRI string; C++ mirrors the same case order as an if/else chain.
VerbEnums VerbIRIResolver::GetVerbEnum(string currentVerb)
{
	// [Historical - the C# switch keeps a commented-out Terminated_Early -> Attempted mapping]
	// FIX (SIM-B4): Attempted IRI was unmapped here so it fell through to throw. See docs/MODERNIZATION/SIM_MODERNIZATION.md.
	if (currentVerb == "https://febr.is/Verb/Details/Attempted") {
		return VerbEnums::Attempted;
	}
	else if (currentVerb == "https://febr.is/Verb/Details/Completed") {
		return VerbEnums::Completed;
	}
	else if (currentVerb == "https://febr.is/Verb/Details/Initialized") {
		return VerbEnums::Initialized;
	}
	else if (currentVerb == "https://febr.is/Verb/Details/Terminated") {
		return VerbEnums::Terminated;
	}
	else if (currentVerb == "https://febr.is/Verb/Details/Pass") {
		return VerbEnums::Pass;
	}
	else if (currentVerb == "https://febr.is/Verb/Details/Not_Pass") {
		return VerbEnums::Not_Pass;
	}
	else {
		// Handle bad URL, possibly throw
		// FIX (SIM-B3): bare throw outside a catch calls std::terminate. Throw a real exception instead. See docs/MODERNIZATION/SIM_MODERNIZATION.md.
		throw std::runtime_error("unknown verb IRI: " + currentVerb);
	}
}
