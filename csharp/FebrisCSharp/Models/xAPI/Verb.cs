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
    /// xAPI Verb node ("verb" on the wire). <c>display</c> is a Language
    /// Map object per xAPI 1.0.3 section 4.1.3 (FIX SDKV-13 -- previously a
    /// serialized string that the Initializer re-parsed back into an
    /// object after the fact). Febris dialect adds the <c>key</c> /
    /// <c>uuid</c> DB-lookup hints.
    /// </summary>
    internal  class Verb
    {
        [JsonProperty("key")] internal  long Key { get; set; }
        [JsonProperty("uuid")] internal  Guid UUID { get; set; }
        [JsonProperty("id")] internal  Uri Id { get; set; }//needs to be an IRI - https://febr.is/TestBase/verbs/attempted or started or trained or completed

        [JsonProperty("display")] internal Dictionary<string, string> Display { get; set; }

        #region [Historical - SDKV-13] pre-language-map string member
        // Display was a plain string holding a serialized language map;
        // the wire shape depended on the Initializer's ChangeStringToObject
        // hack firing. Superseded by the typed Dictionary above.
        //[JsonProperty] internal  string Display { get; set; }
        #endregion
    }
}
