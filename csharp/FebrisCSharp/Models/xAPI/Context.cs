// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Text;

namespace Febris.CsharpSimulationLibraryNetStandard.Models.xAPI
{
    //################################################################
    //  1)sql ids
    //  2) Needs to be a UUID 
    //  3) If there is an instructor and they are also an Actor/Group it can be linked
    //  4) This is an array of actors for a group if not included in the Actor of the statement
    //  5) Valid context types : 
    //            "parent", "grouping", "category", "other"
    //  6) Reision of the learning activitiy (Testbase.version)
    //  7) Platform used (febris)
    //  8) Language based on Language map RFC 5646
    //  9) statement
    //  10) extensions
    //################################################################
    
    /// <summary>
    /// xAPI Context node. FIX (SDKV-12): wire names declared explicitly --
    /// <c>contextActivities</c> uses exact spec casing; the dialect-only
    /// <c>statementreference</c> wrapper keeps its historical lowercase
    /// wire form.
    /// </summary>
    internal class Context
    {
        //1
        [JsonProperty("id")] internal long Id { get; set; }
        [JsonProperty("uuid")] internal Guid UUID { get; set; }
        //2
        [JsonProperty("registration")] internal Guid Registration { get; set; }
        //3
        [JsonProperty("instructor")] internal Actor Instructor { get; set; }
        //4
        [JsonProperty("group")] internal List<Actor> Group { get; set; }
        //5
        [JsonProperty("contextActivities")] internal ContextActivities ContextActivities { get; set; }
        //6
        [JsonProperty("revision")] internal string Revision { get; set; }
        //7
        [JsonProperty("platform")] internal string Platform { get; set; }
        //8
        [JsonProperty("language")] internal string Language { get; set; }
        //9
        [JsonProperty("statementreference")] internal StatementReference StatementReference { get; set; }
        //10
        [JsonProperty("extensions")] internal Extensions Extensions { get; set; }
    }
    //################################################################
    // There is more information to be found in spec page 36
    //################################################################
    
    /// <summary>
    /// xAPI contextActivities node (parent / grouping / category / other);
    /// spec keys are lowercase, <c>id</c>/<c>uuid</c> are dialect DB hints.
    /// </summary>
    internal class ContextActivities
    {
        [JsonProperty("id")] internal long Id { get; set; }
        [JsonProperty("uuid")] internal Guid UUID { get; set; }

        [JsonProperty("parent")] internal string Parent { get; set; }
        [JsonProperty("grouping")] internal string Grouping { get; set; }
        [JsonProperty("category")] internal string Category { get; set; }
        [JsonProperty("other")] internal string Other { get; set; }

    }
}
