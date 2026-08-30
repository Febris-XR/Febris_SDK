// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
#include <stdexcept>

// EXPORT-SURFACE PORT (2026-08-29): real global-scope definitions for the
// iOSHandler declared in iOSHandler.h, ported from the CURRENT C#
// Service/iOSHandler.cs. This TU previously held a dead same-named duplicate
// class inside namespace Service (a "Post Created" console stub) -- pre-refactor
// drift, deleted, never a semantic source.

// NOTE (SIM-B1): iOS handler is non-functional. CreateInitialPost never sets
// Ready/Extras so Initialize() on iOS returns (false, default), and
// UpdatePost/ErrorPost throw the NotImplementedException mirror. iOS was
// explicitly deferred (Tier 13 G10). Wrapping the class in a dead region
// (mirroring WinMobileHandler) to flag the known limitation. Logic is
// unchanged: making iOS emit real data is a functionality change, deferred per
// do-not-change-functionality. See docs/MODERNIZATION/SIM_MODERNIZATION.md
// (the SYSTEM_SLICES.md "supported" claim still needs correcting in a separate change).
// #region [Dead - SIM-B1] iOSHandler - non-functional, iOS deferred

HandlerResult iOSHandler::CreateInitialPost(XApiJson::ojson& statementFromDataModel)
{
	HandlerResult result{};
	try
	{
		StaticDetails::CurrentStatement = statementFromDataModel;
		// C# reads the lazy ReferenceUUID property. The read itself generates
		// the GUID on first touch, so keep it for the side effect.
		std::string referenceKey = StaticDetails::GetReferenceUUID();
		(void)referenceKey;

		// [C# testing region: Console.WriteLine(outputArray) with a null array,
		// prints an empty line. Not mirrored as output.]
	}
	catch (const std::exception& e)
	{
		// Pre-G4 console write kept verbatim from the C# dead path (iOS never
		// received the SIM-T13 G4 logger routing).
		std::cout << "Error creating initial Statement: " << e.what() << std::endl;
		throw;
	}
	// (false, default): Ready=false, HasExtras=false by design (SIM-B1).
	return result;
}

HandlerResult iOSHandler::ErrorPost(XApiJson::ojson& statementFromDataModel)
{
	// C# NotImplementedException mirror.
	throw std::logic_error("The method or operation is not implemented.");
}

HandlerResult iOSHandler::UpdatePost(XApiJson::ojson& statementFromDataModel)
{
	// C# NotImplementedException mirror.
	throw std::logic_error("The method or operation is not implemented.");
}

// #endregion [Dead - SIM-B1] iOSHandler
