// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using System;
using System.Collections.Generic;
using System.Text;

namespace Febris.CsharpSimulationLibraryNetStandard.Enums
{
    enum ExtensionIRIOptions
    {
        RestartCounterIRI,
        NotesIRI,
    }

    class ExtensionIRIResolver
    {
        internal static string ResolveExtensionIRI(ExtensionIRIOptions iri)
        {
            switch (iri)
            {
                case ExtensionIRIOptions.RestartCounterIRI:
                    return "http://febr.is/extensions/resultextensions/restartcounter";
                case ExtensionIRIOptions.NotesIRI:
                    return "http://febr.is/extensions/resultextensions/notes";                
                default:
                    // Handle bad URL, possibly throw
                    throw new Exception();
            }
        }
        internal static ExtensionIRIOptions ResolveExtensionIRI(string currentExtension)
        {
            switch (currentExtension)
            {
                case "http://febr.is/extensions/resultextensions/restartcounter":
                    return ExtensionIRIOptions.RestartCounterIRI;
                case "http://febr.is/extensions/resultextensions/notes":                
                    return ExtensionIRIOptions.NotesIRI;                
                default:
                    // Handle bad URL, possibly throw
                    throw new Exception();
            }
        }
    }    
}
