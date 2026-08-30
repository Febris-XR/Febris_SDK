// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
// ContextOptions resolver port (2026-08-29): real global-scope definitions for the
// class declared in ContextOptions.h, ported from the CURRENT C# Enums/ContextOptions.cs
// (ContextOptionResolver, ~lines 36-95). The old same-named duplicate inside the dead
// namespace FebrisCppEnums is deleted. Note: the 'ContextActivites' spelling is the
// canonical wire token, do not correct it.

string ContextOptionResolver::ResolveExtensionIRI(ContextOptions option)
{
	switch (option)
	{
	case ContextOptions::Registration:
		return "Registration";
	case ContextOptions::Instructor:
		return "Instructor";
	case ContextOptions::Group:
		return "Group";
	case ContextOptions::ContextActivites:
		return "ContextActivites";
	case ContextOptions::Revision:
		return "Revision";
	case ContextOptions::Platform:
		return "Platform";
	case ContextOptions::Language:
		return "Language";
	case ContextOptions::StatementReference:
		return "Statement Reference";
	case ContextOptions::Extensions:
		return "Extensions";
		// [Historical - the C# switch keeps commented-out ContextActivites_Parent/Grouping/Category/Other cases]
	default:
		// Handle bad URL, possibly throw
		// FIX (SIM-B3): C# throws new Exception() here; a bare C++ 'throw;' outside a catch calls std::terminate. Throw a real exception instead. See docs/MODERNIZATION/SIM_MODERNIZATION.md.
		throw std::runtime_error("unknown context option in ResolveExtensionIRI");
	}
}

string ContextOptionResolver::ContextExtensionOptionResolver(ContextExtensionOptions option)
{
	switch (option)
	{
	case ContextExtensionOptions::Option1:
		return "https://Option1";
	case ContextExtensionOptions::Option2:
		return "https://Option2";
	default:
		// Handle bad URL, possibly throw
		// FIX (SIM-B3): C# throws new Exception() here; a bare C++ 'throw;' outside a catch calls std::terminate. Throw a real exception instead. See docs/MODERNIZATION/SIM_MODERNIZATION.md.
		throw std::runtime_error("unknown context extension option in ContextExtensionOptionResolver");
	}
}
