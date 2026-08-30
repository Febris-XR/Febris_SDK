// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
// ExtensionIRIOptions resolver port (2026-08-29): real global-scope definitions for
// the class declared in ExtensionIRIOptions.h, ported from the CURRENT C#
// Enums/ExtensionIRIOptions.cs (both ResolveExtensionIRI overloads). The old
// same-named duplicate inside the dead namespace FebrisCppEnums is deleted.

string ExtensionIRIResolver::ResolveExtensionIRI(ExtensionIRIOptions iri)
{
    switch (iri)
    {
    case ExtensionIRIOptions::RestartCounterIRI:
        return "http://febr.is/extensions/resultextensions/restartcounter";
    case ExtensionIRIOptions::NotesIRI:
        return "http://febr.is/extensions/resultextensions/notes";
    default:
        // Handle bad URL, possibly throw
        // FIX (SIM-B3): C# throws new Exception() here; a bare C++ 'throw;' outside a catch calls std::terminate. Throw a real exception instead. See docs/MODERNIZATION/SIM_MODERNIZATION.md.
        throw std::runtime_error("unknown extension IRI option in ResolveExtensionIRI");
    }
}

// C# switches on the extension IRI string; C++ mirrors the same case order as an if/else chain.
ExtensionIRIOptions ExtensionIRIResolver::ResolveExtensionIRI(string currentExtension)
{
    if (currentExtension == "http://febr.is/extensions/resultextensions/restartcounter") {
        return ExtensionIRIOptions::RestartCounterIRI;
    }
    else if (currentExtension == "http://febr.is/extensions/resultextensions/notes") {
        return ExtensionIRIOptions::NotesIRI;
    }
    else {
        // Handle bad URL, possibly throw
        // FIX (SIM-B3): bare throw outside a catch calls std::terminate. Throw a real exception instead. See docs/MODERNIZATION/SIM_MODERNIZATION.md.
        throw std::runtime_error("unknown extension IRI: " + currentExtension);
    }
}
