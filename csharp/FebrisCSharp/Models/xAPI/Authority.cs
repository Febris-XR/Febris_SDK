// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Text;

namespace Febris.CsharpSimulationLibraryNetStandard.Models.xAPI
{
    //################################################################
    //  1) these are sql references
    //  2) Need an actor class
    //  
    //################################################################
    
    /// <summary>
    /// Febris-dialect authority wrapper (spec authority is a bare Agent).
    /// Wire names declared explicitly (FIX SDKV-12); dialect keys keep
    /// their historical lowercase form.
    /// </summary>
    internal class Authority
    {
        //1
        [JsonProperty("id")] internal long Id { get; set; }
        [JsonProperty("uuid")] internal Guid UUID { get; set; }
        //2
        [JsonProperty("actor")] internal Actor Actor { get; set; }
    }
}
