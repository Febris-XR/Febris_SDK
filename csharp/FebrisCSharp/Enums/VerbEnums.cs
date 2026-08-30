// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using System;
using System.Collections.Generic;
using System.Text;

namespace Febris.CsharpSimulationLibraryNetStandard.Enums
{
    public enum VerbEnums
    {
        Attempted,
        Completed,
        Initialized,
        Terminated,
        Pass,
        Not_Pass
    }
    //public enum VerbStage
    //{             
    //    Completed,
    //    Terminated,
    //}

    class VerbIRIResolver
    {
        internal static string ResolveVerbIRI(VerbEnums iri)
        {
            switch (iri)
            {
                case VerbEnums.Attempted:
                    return "https://febr.is/Verb/Details/Attempted";
                case VerbEnums.Completed:
                    return "https://febr.is/Verb/Details/Completed";
                case VerbEnums.Initialized:
                    return "https://febr.is/Verb/Details/Initialized";
                case VerbEnums.Terminated:
                    return "https://febr.is/Verb/Details/Terminated";
                case VerbEnums.Pass:
                    return "https://febr.is/Verb/Details/Pass";
                case VerbEnums.Not_Pass:
                    return "https://febr.is/Verb/Details/Not_Pass";
                default:
                    // Handle bad URL, possibly throw
                    throw new Exception();
            }
        }
        
        internal static VerbEnums GetVerbEnum(string currentVerb)
        {
            switch (currentVerb)
            {
                //case "https://febr.is/Verb/Details/Terminated_Early":
                //    return VerbEnums.Attempted;
                // FIX (SIM-B4): Attempted IRI was unmapped here so it fell through to throw. See docs/MODERNIZATION/SIM_MODERNIZATION.md.
                case "https://febr.is/Verb/Details/Attempted":
                    return VerbEnums.Attempted;
                case "https://febr.is/Verb/Details/Completed":
                    return VerbEnums.Completed;
                case "https://febr.is/Verb/Details/Initialized":
                    return VerbEnums.Initialized;
                case "https://febr.is/Verb/Details/Terminated":
                    return VerbEnums.Terminated;
                case "https://febr.is/Verb/Details/Pass":
                    return VerbEnums.Pass;
                case "https://febr.is/Verb/Details/Not_Pass":
                    return VerbEnums.Not_Pass;
                default:
                    // Handle bad URL, possibly throw
                    throw new Exception();
            }
        }
    }
    
}
