// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Text;

namespace Febris.CsharpSimulationLibraryNetStandard.Models.xAPI
{
    //[Serializable]
    /// <summary>
    /// Root xAPI Statement model -- the shape the SDK serializes onto the
    /// wire. FIX (SDKV-12): wire names are declared explicitly with exact
    /// xAPI 1.0.3 casing (all root keys happen to be lowercase); the
    /// Initializer's lowercase-everything pass is retired.
    /// </summary>
    internal class Statement
    {
        //################################################################
        //this needs to be uuid (or guid) can be set up automatically with postgres
        //################################################################
        [JsonProperty("id")] internal long Id { get; set; }
        [JsonProperty("uuid")] internal Guid UUID { get; set; }

        //################################################################
        //if not provided needs to set by api
        //################################################################
        [JsonProperty("timestamp")] internal DateTime Timestamp { get; set; }

        //################################################################
        //Set this inside Db for when the record is stored
        //################################################################
        [JsonProperty("stored")] internal DateTime Stored { get; set; }

        //################################################################
        //xApi required fields
        //################################################################

        [JsonProperty("actor")] internal Actor Actor { get; set; }

        [JsonProperty("verb")] internal Verb Verb { get; set; }

        [JsonProperty("object")] internal Object Object { get; set; }

        //################################################################
        //Optional Fields
        //attachments needs to be an ordered array of objects
        //################################################################
        [JsonProperty("result")] internal Result Result { get; set; }
        [JsonProperty("context")] internal Context Context { get; set; }
        [JsonProperty("authority")] internal Authority Authority { get; set; }
        [JsonProperty("version")] internal Version Version { get; set; }
        [JsonProperty("attachments")] internal List<Attachment> Attachments { get; set; }
    }
}
