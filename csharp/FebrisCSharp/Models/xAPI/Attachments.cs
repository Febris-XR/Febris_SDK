// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Text;

namespace Febris.CsharpSimulationLibraryNetStandard.Models.xAPI
{
    //################################################################
    //  1) sql ids
    //  2) usageType... not really sure but it is the correct type
    //  3) language mapped strings
    //  4) This is an internet media type. It is the content type of the
    //          -- is most likely "application/octet-stream"
    //  5) Length of the attachment data in octets
    //  6) hash of the attachment data
    //  7) an irl at which the attachment data can be retrieved, or from which it used to be retrievable
    //################################################################

    /// <summary>
    /// xAPI Attachment metadata ("attachments[]" on the wire). Per xAPI
    /// 1.0.3 section 4.1.11 <c>display</c>/<c>description</c> are Language Map
    /// objects (FIX SDKV-2 -- previously plain strings, which the node's
    /// typed DTO rejected wholesale) and the camelCase keys
    /// usageType / contentType / fileUrl are emitted spec-exact
    /// (FIX SDKV-12).
    /// </summary>
    internal class Attachment
    {
        //1
        [JsonProperty("id")] internal long Id { get; set; }
        [JsonProperty("uuid")] internal Guid UUID { get; set; }
        //2
        [JsonProperty("usageType")] internal Uri UsageType { get; set; }

        //3
        [JsonProperty("display")] internal Dictionary<string, string> Display { get; set; }
        [JsonProperty("description")] internal Dictionary<string, string> Description { get; set; }

        //4
        [JsonProperty("contentType")] internal string ContentType { get; set; } //ie "application/octet-stream"
        //5
        [JsonProperty("length")] internal int Length { get; set; }
        //6
        [JsonProperty("sha2")] internal string Sha2 { get; set; }
        //7
        [JsonProperty("fileUrl")] internal Uri FileURL { get; set; }//user UUID to name video file

        #region [Historical - SDKV-2] pre-language-map string members
        // Display / Description were plain strings; every emit path
        // serialized them as JSON strings, breaking xAPI 1.0.3 section 4.1.11
        // (Language Map required) and 400-ing at the node's typed /Submit
        // binder. Superseded by the Dictionary members above.
        //[JsonProperty]internal string Display { get; set; }
        //[JsonProperty]internal string Description { get; set; }
        #endregion
    }
}
