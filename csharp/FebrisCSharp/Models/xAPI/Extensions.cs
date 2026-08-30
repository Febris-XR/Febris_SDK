// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Text;

namespace Febris.CsharpSimulationLibraryNetStandard.Models.xAPI
{
    //################################################################
    //keys must be IRIs
    //
    //################################################################
    
    /// <summary>
    /// Febris-dialect extensions wrapper (spec extensions are an
    /// IRI-keyed map; the dialect flattens them into <c>extensionmap</c>).
    /// Wire names declared explicitly (FIX SDKV-12), all dialect keys in
    /// their historical lowercase form.
    /// </summary>
    internal  class Extensions
    {
        [JsonProperty("id")] internal  long Id { get; set; }
        //check page 50
        [JsonProperty("uuid")] internal  Guid UUID { get; set; } // lets use this to link? otherwise it is not stated as needed

        [JsonProperty("extensionmap")] internal  string ExtensionMap { get; set; }
    }
}
