// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Text;

namespace Febris.CsharpSimulationLibraryNetStandard.Models.xAPI
{
    //################################################################
    //object type bust me StatementRef
    //################################################################
    
    /// <summary>
    /// xAPI StatementRef shape (carried under the dialect key
    /// <c>context.statementreference</c>). FIX (SDKV-12): the
    /// <c>objectType</c> discriminator emits with exact spec casing;
    /// dialect DB hints stay lowercase.
    /// </summary>
    internal  class StatementReference
    {
        [JsonProperty("key")] internal  long Key { get; set; }
        [JsonProperty("uuid")] internal  Guid UUID { get; set; }
        [JsonProperty("id")] internal  Guid Id { get; set; }
        [JsonProperty("objectType")] internal  string ObjectType { get; set; }
    }
}
