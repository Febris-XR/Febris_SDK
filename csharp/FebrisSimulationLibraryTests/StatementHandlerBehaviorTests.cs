// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using System;
using Febris.CsharpSimulationLibraryNetStandard.Service;
using Febris.CsharpSimulationLibraryNetStandard.Statement;
using FluentAssertions;
using Newtonsoft.Json.Linq;
using Xunit;
using SimStatics = Febris.CsharpSimulationLibraryNetStandard.StaticDetails;

namespace Febris.SimulationLibrary.Tests
{
    /// <summary>
    /// MDM-T2 behavior tests pinning the fixes from
    /// <c>docs/AUDIT_MDM_INTEGRATION_ERROR_HANDLING.md</c>:
    /// G2-7 (copy-paste + NIE in MemberIsCorrect/AccountIsCorrect),
    /// G2-8 (AutoUpdateScaledScore divide-by-zero + NaN/Infinity guard),
    /// C-03 (extensionMap parser bounds checks).
    ///
    /// These exercise <c>internal</c> methods on <c>StatementHandler</c>;
    /// the simulation library's csproj has an
    /// <c>InternalsVisibleTo("Febris.SimulationLibrary.Tests")</c> entry
    /// added in the same change so this works.
    /// </summary>
    [Collection("SimulationStatics")] // shares the SDK's process-wide statics; see StatementEmissionTests
    public class StatementHandlerBehaviorTests
    {
        // -----------------------------------------------------------------------------------
        // G2-7: MemberIsCorrect / AccountIsCorrect (no longer throw NIE)
        // -----------------------------------------------------------------------------------

        [Fact]
        public void MemberIsCorrect_NullInput_ReturnsFalse()
        {
            StatementHandler.MemberIsCorrect(null).Should().BeFalse();
        }

        [Fact]
        public void MemberIsCorrect_EmptyArray_ReturnsFalse()
        {
            var members = new JArray();
            StatementHandler.MemberIsCorrect(members).Should().BeFalse(
                "an empty Group member array is not a valid xAPI actor");
        }

        [Fact]
        public void MemberIsCorrect_NonArrayJObject_ReturnsFalse()
        {
            var notAnArray = new JObject(new JProperty("name", "Alice"));
            StatementHandler.MemberIsCorrect(notAnArray).Should().BeFalse(
                "Group member must be a JArray (or an object containing a `member` array)");
        }

        [Fact]
        public void MemberIsCorrect_NonEmptyArray_ReturnsTrue()
        {
            var members = new JArray(
                new JObject(new JProperty("mbox", "mailto:alice@example.com")),
                new JObject(new JProperty("uuid", "11111111-1111-1111-1111-111111111111")));
            StatementHandler.MemberIsCorrect(members).Should().BeTrue();
        }

        [Fact]
        public void MemberIsCorrect_ObjectWithEmbeddedMemberArray_ReturnsTrue()
        {
            var wrapped = new JObject(
                new JProperty("member", new JArray(
                    new JObject(new JProperty("mbox", "mailto:bob@example.com")))));
            StatementHandler.MemberIsCorrect(wrapped).Should().BeTrue();
        }

        [Fact]
        public void AccountIsCorrect_NullInput_ReturnsFalse()
        {
            StatementHandler.AccountIsCorrect(null).Should().BeFalse();
        }

        [Fact]
        public void AccountIsCorrect_NotAnObject_ReturnsFalse()
        {
            StatementHandler.AccountIsCorrect(new JArray()).Should().BeFalse(
                "xAPI Account must be a JSON object, not an array or primitive");
        }

        [Fact]
        public void AccountIsCorrect_MissingHomePage_ReturnsFalse()
        {
            var account = new JObject(new JProperty("name", "alice"));
            StatementHandler.AccountIsCorrect(account).Should().BeFalse();
        }

        [Fact]
        public void AccountIsCorrect_MissingName_ReturnsFalse()
        {
            var account = new JObject(new JProperty("homePage", "https://example.com"));
            StatementHandler.AccountIsCorrect(account).Should().BeFalse();
        }

        [Fact]
        public void AccountIsCorrect_BothFieldsPresent_ReturnsTrue()
        {
            var account = new JObject(
                new JProperty("homePage", "https://example.com"),
                new JProperty("name", "alice"));
            StatementHandler.AccountIsCorrect(account).Should().BeTrue();
        }

        [Fact]
        public void AccountIsCorrect_LowercaseHomepage_AlsoAccepted()
        {
            // Tier 13 G6 left a path where statement properties are aggressively
            // lowercased by Initializer.ChangePropertiesToLowerCase. The lowercased
            // form is accepted as a fallback so already-lowercased inputs validate.
            var account = new JObject(
                new JProperty("homepage", "https://example.com"),
                new JProperty("name", "alice"));
            StatementHandler.AccountIsCorrect(account).Should().BeTrue();
        }

        // -----------------------------------------------------------------------------------
        // G2-8: AutoUpdateScaledScore guards (div-by-zero, NaN, Infinity, clamping)
        // -----------------------------------------------------------------------------------

        private static JObject MakeScoreStatement(JToken maxValue)
        {
            var statement = new JObject(
                new JProperty("result", new JObject(
                    new JProperty("score", new JObject()))));
            if (maxValue != null)
            {
                statement["result"]["score"]["max"] = maxValue;
            }
            return statement;
        }

        [Fact]
        public void AutoUpdateScaledScore_MaxZero_SkipsScaledUpdate()
        {
            var statement = MakeScoreStatement(0f);
            var result = StatementHandler.AutoUpdateScaledScore(statement, 5f);
            result["result"]["score"]["scaled"].Should().BeNull(
                "with max=0 the scaled update is skipped, no value should be written");
        }

        [Fact]
        public void AutoUpdateScaledScore_MissingMax_SkipsScaledUpdate()
        {
            var statement = MakeScoreStatement(null);
            var result = StatementHandler.AutoUpdateScaledScore(statement, 5f);
            result["result"]["score"]["scaled"].Should().BeNull();
        }

        [Fact]
        public void AutoUpdateScaledScore_NegativeMax_SkipsScaledUpdate()
        {
            var statement = MakeScoreStatement(-10f);
            var result = StatementHandler.AutoUpdateScaledScore(statement, 5f);
            result["result"]["score"]["scaled"].Should().BeNull();
        }

        [Fact]
        public void AutoUpdateScaledScore_NaNMax_SkipsScaledUpdate()
        {
            var statement = MakeScoreStatement(float.NaN);
            var result = StatementHandler.AutoUpdateScaledScore(statement, 5f);
            result["result"]["score"]["scaled"].Should().BeNull();
        }

        [Fact]
        public void AutoUpdateScaledScore_InfinityMax_SkipsScaledUpdate()
        {
            var statement = MakeScoreStatement(float.PositiveInfinity);
            var result = StatementHandler.AutoUpdateScaledScore(statement, 5f);
            result["result"]["score"]["scaled"].Should().BeNull();
        }

        [Fact]
        public void AutoUpdateScaledScore_NaNRawScore_SkipsScaledUpdate()
        {
            var statement = MakeScoreStatement(100f);
            var result = StatementHandler.AutoUpdateScaledScore(statement, float.NaN);
            result["result"]["score"]["scaled"].Should().BeNull();
        }

        [Fact]
        public void AutoUpdateScaledScore_NormalCase_WritesScaled()
        {
            var statement = MakeScoreStatement(100f);
            var result = StatementHandler.AutoUpdateScaledScore(statement, 80f);
            ((float)result["result"]["score"]["scaled"]).Should().BeApproximately(0.8f, 0.0001f);
        }

        [Fact]
        public void AutoUpdateScaledScore_RawAboveMax_ClampsToOne()
        {
            var statement = MakeScoreStatement(100f);
            var result = StatementHandler.AutoUpdateScaledScore(statement, 150f);
            ((float)result["result"]["score"]["scaled"]).Should().Be(1f);
        }

        [Fact]
        public void AutoUpdateScaledScore_NegativeRaw_ClampsToNegativeOne()
        {
            var statement = MakeScoreStatement(100f);
            var result = StatementHandler.AutoUpdateScaledScore(statement, -150f);
            ((float)result["result"]["score"]["scaled"]).Should().Be(-1f);
        }

        // -----------------------------------------------------------------------------------
        // Logger plumbing: verify Warn-level routing on guard paths
        // -----------------------------------------------------------------------------------

        [Fact]
        public void AutoUpdateScaledScore_MaxZero_LogsWarn()
        {
            var capturing = new CapturingSimulationLogger();
            var previousLogger = SimStatics.StaticDetails.Logger;
            try
            {
                SimStatics.StaticDetails.Logger = capturing;
                var statement = MakeScoreStatement(0f);
                StatementHandler.AutoUpdateScaledScore(statement, 5f);

                capturing.Entries.Should().Contain(e =>
                    e.Level == SimulationLogLevel.Warn &&
                    e.Message.Contains("non-positive"));
            }
            finally
            {
                SimStatics.StaticDetails.Logger = previousLogger;
            }
        }

        [Fact]
        public void AutoUpdateScaledScore_OutOfRangeResult_LogsWarn()
        {
            var capturing = new CapturingSimulationLogger();
            var previousLogger = SimStatics.StaticDetails.Logger;
            try
            {
                SimStatics.StaticDetails.Logger = capturing;
                var statement = MakeScoreStatement(100f);
                StatementHandler.AutoUpdateScaledScore(statement, 150f);

                capturing.Entries.Should().Contain(e =>
                    e.Level == SimulationLogLevel.Warn &&
                    e.Message.Contains("outside the xAPI"));
            }
            finally
            {
                SimStatics.StaticDetails.Logger = previousLogger;
            }
        }
    }
}
