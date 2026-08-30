// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Text;

namespace Febris.CsharpSimulationLibraryNetStandard.Models.xAPI
{
    //################################################################
    // FIX (SDKV-12): every wire name is now declared explicitly on the
    // [JsonProperty] attribute. Spec-defined keys use exact xAPI 1.0.3
    // casing (objectType, moreInfo, interactionType,
    // correctResponsesPattern); Febris-dialect additions (key, uuid,
    // interactioncomponents) keep their historical all-lowercase wire
    // form. The Initializer's lowercase-everything pass is retired, so
    // these attributes ARE the wire contract now.
    //################################################################

    /// <summary>
    /// xAPI Activity object node ("object" on the wire). Febris dialect
    /// adds the DB-lookup hints <c>key</c> / <c>uuid</c> next to the
    /// spec fields.
    /// </summary>
    internal class Object
    {
        [JsonProperty("key")] internal long Key { get; set; }
        [JsonProperty("uuid")] internal Guid UUID { get; set; }
        [JsonProperty("id")] internal Uri Id { get; set; }
        [JsonProperty("objectType")] internal string ObjectType { get; set; }
        [JsonProperty("definition")] internal Definition Definition { get; set; }

    }

    //################################################################
    //Type URI - cannot tell if it should be a adlnet standard or my own
    //  1) sql ids
    //  2) dictionaries - Language Maps (RFC 5646 tag -> string)
    //  3) Uris
    //  4) Extensions  -- I am not sure if this is handled correctly
    //  5) Interactions
    //  6) Interaction Components this is broken into a two part array so the correct component can be written in. pg27 of spec
    //      -- should kinda be [,[,]]
    //################################################################

    /// <summary>
    /// xAPI Activity Definition ("object.definition"). Per xAPI 1.0.3
    /// section 4.1.4: <c>name</c>/<c>description</c> are Language Map objects
    /// (FIX SDKV-13) and <c>correctResponsesPattern</c> is an array of
    /// strings (FIX SDKV-1) -- previously all three were stored (and
    /// therefore emitted) as serialized strings.
    /// </summary>
    internal class Definition
    {
        //1
        [JsonProperty("id")] internal long Id { get; set; }
        [JsonProperty("uuid")] internal Guid UUID { get; set; } // lets use this to link? otherwise it is not stated as needed

        //2
        [JsonProperty("name")] internal Dictionary<string, string> Name { get; set; }
        [JsonProperty("description")] internal Dictionary<string, string> Description { get; set; }

        //3
        [JsonProperty("type")] internal Uri Type { get; set; }
        [JsonProperty("moreInfo")] internal Uri MoreInfo { get; set; }

        //4
        [JsonProperty("extensions")] internal Extensions Extensions { get; set; }

        //5
        [JsonProperty("interactionType")] internal string InteractionType { get; set; }
        [JsonProperty("correctResponsesPattern")] internal List<string> CorrectResponsesPattern { get; set; }

        //6
        [JsonProperty("interactioncomponents")] internal string InteractionComponents { get; set; }

        #region [Historical - SDKV-1/SDKV-13] pre-typed string members
        // Name / Description / CorrectResponsesPattern were plain strings,
        // which forced the Initializer's ChangeStringToObject re-parse hack
        // and still emitted correctResponsesPattern as a JSON string on the
        // wire (rejected by the node DTO's List<string> and by any
        // conformant LRS per xAPI 1.0.3 section 4.1.4.1). Superseded by the typed
        // members above.
        //[JsonProperty] internal string Name { get; set; }
        //[JsonProperty] internal string Description { get; set; }
        //[JsonProperty] internal string CorrectResponsesPattern { get; set; }
        #endregion
    }
}
