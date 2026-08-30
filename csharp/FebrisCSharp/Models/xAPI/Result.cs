// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace Febris.CsharpSimulationLibraryNetStandard.Models.xAPI
{
    //################################################################
    //################################################################

    /// <summary>
    /// xAPI Result node ("result" on the wire). <c>success</c> /
    /// <c>completion</c> are genuine booleans (serialized as JSON
    /// Booleans) and <c>duration</c> serializes in ISO 8601 duration
    /// format via <see cref="Iso8601TimeSpanConverter"/> (FIX SDKV-5 --
    /// Newtonsoft's default TimeSpan format "00:00:00" is not a valid
    /// xAPI 1.0.3 section 4.1.5 duration).
    /// </summary>
    internal class Result
    {
        [JsonProperty("id")] internal long Id { get; set; }
        [JsonProperty("uuid")] internal Guid UUID { get; set; }
        [JsonProperty("score")] internal Score Score { get; set; }
        [JsonProperty("success")] internal bool Success { get; set; }
        [JsonProperty("completion")] internal bool Completion { get; set; }
        [JsonProperty("response")] internal string Response { get; set; }
        [JsonProperty("duration")]
        [JsonConverter(typeof(Iso8601TimeSpanConverter))]
        internal TimeSpan Duration { get; set; }
        [JsonProperty("extensions")] internal Extensions Extensions { get; set; }
    }
    //################################################################
    //################################################################

    /// <summary>
    /// xAPI Score node ("result.score"). Spec keys are lowercase;
    /// <c>id</c>/<c>uuid</c> are Febris-dialect DB hints.
    /// </summary>
    internal class Score
    {
        [JsonProperty("id")] internal long Id { get; set; }
        [JsonProperty("uuid")] internal Guid UUID { get; set; }
        [JsonProperty("scaled")] internal float Scaled { get; set; }
        [JsonProperty("raw")] internal float Raw { get; set; }
        [JsonProperty("min")] internal float Min { get; set; }
        [JsonProperty("max")] internal float Max { get; set; }
    }

    /// <summary>
    /// FIX (SDKV-5): serializes <see cref="TimeSpan"/> as an ISO 8601
    /// duration string (e.g. <c>PT0S</c>, <c>PT1M30.5S</c>) using
    /// <see cref="XmlConvert"/>, and reads either ISO 8601 or the legacy
    /// .NET "c" format back tolerantly. Without this converter Newtonsoft
    /// emits the invariant "00:00:00" clock format, which a conformant
    /// LRS rejects (xAPI 1.0.3 section 4.1.5 requires ISO 8601 durations).
    /// </summary>
    internal class Iso8601TimeSpanConverter : JsonConverter<TimeSpan>
    {
        /// <summary>Writes the TimeSpan as an ISO 8601 duration string.</summary>
        public override void WriteJson(JsonWriter writer, TimeSpan value, JsonSerializer serializer)
        {
            writer.WriteValue(XmlConvert.ToString(value));
        }

        /// <summary>
        /// Reads an ISO 8601 duration; falls back to .NET TimeSpan parsing
        /// for legacy "00:00:00"-style values, then to TimeSpan.Zero.
        /// </summary>
        public override TimeSpan ReadJson(JsonReader reader, Type objectType, TimeSpan existingValue, bool hasExistingValue, JsonSerializer serializer)
        {
            string raw = reader.Value?.ToString();
            if (string.IsNullOrEmpty(raw))
            {
                return TimeSpan.Zero;
            }
            try
            {
                return XmlConvert.ToTimeSpan(raw);
            }
            catch (FormatException)
            {
                TimeSpan fallback;
                return TimeSpan.TryParse(raw, out fallback) ? fallback : TimeSpan.Zero;
            }
        }
    }

    //this needs to be set up differently?
    //[JsonProperty] internal class Extensions
    //{

    //}
}
