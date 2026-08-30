// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Text;

namespace Febris.CsharpSimulationLibraryNetStandard.Models.xAPI
{
    //################################################################
    //################################################################
    
    /// <summary>
    /// Febris-dialect version wrapper (spec version is a bare string).
    /// Wire names declared explicitly (FIX SDKV-12), historical lowercase.
    /// </summary>
    internal class Version
    {
        [JsonProperty("id")] internal long Id { get; set; }
        [JsonProperty("uuid")] internal Guid UUID { get; set; } // lets use this to link? otherwise it is not stated as needed
        [JsonProperty("versionnumber")] internal string VersionNumber { get; set; }
    }
}
