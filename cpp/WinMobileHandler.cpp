// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
#include <stdexcept>

// EXPORT-SURFACE PORT (2026-08-29): real global-scope definitions for the
// WinMobileHandler declared in WinMobileHandler.h, ported from the CURRENT C#
// Service/WinMobileHandler.cs. This TU previously held a dead same-named
// duplicate class inside namespace Service (a "Post Created" console stub) --
// pre-refactor drift, deleted, never a semantic source.

// SIM-T13 G1: [Dead] WinMobileHandler. In the C# source three of the four
// IEnvironmentHandler entry points throw NotImplementedException (UpdatePost /
// ErrorPost / the explicit-interface CreateInitialPost). The `internal`
// CreateInitialPost with a real body is unreachable through IEnvironmentHandler
// because the explicit-interface impl shadows it. Initializing the simulation
// library with OS = ExpectedOperatingSystem::WinMobile therefore crashes the
// host game on the first Initialize() call.
//
// The Windows-on-mobile platform is effectively dead in 2026, but the class
// is kept on disk per the comment-out-dont-delete policy (CLAUDE.md rule #10)
// so future maintainers can see what was attempted. If a customer ever needs
// WinMobile, fill out UpdatePost / ErrorPost / CreateInitialPost following the
// AndroidHandler pattern + delete this region wrapper.
//
// Audit doc: docs/SIMULATION_ROADMAP/TIER_13_AUDIT_MDM_COMPAT.md gap G1.
// #region [Dead - SIM-T13] WinMobileHandler - throws NIE on every live path

HandlerResult WinMobileHandler::ErrorPost(XApiJson::ojson& statementFromDataModel)
{
	// C# NotImplementedException mirror.
	throw std::logic_error("The method or operation is not implemented.");
}

HandlerResult WinMobileHandler::UpdatePost(XApiJson::ojson& statementFromDataModel)
{
	// C# NotImplementedException mirror.
	throw std::logic_error("The method or operation is not implemented.");
}

HandlerResult WinMobileHandler::CreateInitialPost(XApiJson::ojson& statementFromDataModel)
{
	// C++ has no explicit-interface-implementation split, so this single
	// override mirrors the REACHABLE C# behavior: the explicit
	// IEnvironmentHandler.CreateInitialPost, which throws.
	// [Historical - unreachable C# internal CreateInitialPost body: set
	// CurrentStatement = statementFromDataModel, read the lazy ReferenceUUID,
	// testing Console.WriteLine, return (false, default).]
	throw std::logic_error("The method or operation is not implemented.");
}

// #endregion [Dead - SIM-T13] WinMobileHandler
