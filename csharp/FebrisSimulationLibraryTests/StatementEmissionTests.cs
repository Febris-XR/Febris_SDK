// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using System;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Febris.CsharpSimulationLibraryNetStandard.Enums;
using Febris.CsharpSimulationLibraryNetStandard.Statement;
using FluentAssertions;
using Newtonsoft.Json.Linq;
using Xunit;
using SimFileSystem = Febris.CsharpSimulationLibraryNetStandard.FileSystem;
using SimStatics = Febris.CsharpSimulationLibraryNetStandard.StaticDetails;

namespace Febris.SimulationLibrary.Tests
{
    /// <summary>
    /// SDKV emission tests pinning the fixes from
    /// <c>docs/AUDIT/SDK_E2E_VERIFICATION_2026-07-17.md</c> -- these assert
    /// the EMITTED JSON shapes, not just internal state:
    ///
    /// <list type="bullet">
    ///   <item>SDKV-3/11: result.success / result.completion are JSON
    ///         Booleans end-to-end (factoring + every update writer).</item>
    ///   <item>SDKV-1: definition.correctResponsesPattern is an array of
    ///         strings (array input preserved, lone string wrapped).</item>
    ///   <item>SDKV-2: attachments[].display / [].description are Language
    ///         Map objects (typed factoring + AddAttachment wrapping).</item>
    ///   <item>SDKV-5: result.duration is ISO 8601 (init default PT0S,
    ///         updates via XmlConvert).</item>
    ///   <item>SDKV-12: case-sensitive xAPI keys emit spec-exact
    ///         (objectType, homePage, moreInfo, interactionType,
    ///         correctResponsesPattern, usageType, contentType,
    ///         contextActivities); the lowercase-everything output pass is
    ///         retired.</item>
    ///   <item>SDKV-13: verb.display + definition.name/description are
    ///         Language Map objects on the wire.</item>
    ///   <item>SDKV-4: context-activity updates land on the
    ///         contextActivities node the statement actually carries.</item>
    ///   <item>SDKV-19/20 (SDK side): every emitted statement carries a
    ///         stamped wire <c>id</c> -- the handoff ReferenceUUID -- that is a
    ///         valid GUID, matches the {uuid}.json handoff filename, and is
    ///         STABLE across re-serialization (retry = same id), which is
    ///         what makes the node-side dedupe idempotent for host
    ///         retries.</item>
    ///   <item>SDKV-23: the INPUT lowercasing pass survives non-object array
    ///         elements (spec-shaped string-array correctresponsespattern no
    ///         longer fails the whole Initialize) and preserves RFC 5646
    ///         language-tag key casing ("en-US" stays "en-US" on the
    ///         wire).</item>
    /// </list>
    ///
    /// Statics note: the library state is process-wide statics
    /// (<c>StaticDetails.CurrentStatement</c>, the FileSystem path table), so
    /// every test class that touches them shares the "SimulationStatics"
    /// xunit collection -- collections serialize across classes, where the
    /// default would run classes in parallel and race the statics.
    /// </summary>
    [Collection("SimulationStatics")]
    public class StatementEmissionTests
    {
        // -----------------------------------------------------------------------------------
        // Helpers
        // -----------------------------------------------------------------------------------

        /// <summary>
        /// Builds a representative authored statement the way the Initializer
        /// hands it to factoring: all-lowercase keys (the INPUT normalization
        /// pass), Febris-dialect hint fields included.
        /// </summary>
        private static JObject MakeAuthoredInput()
        {
            return JObject.Parse(@"{
                ""timestamp"": ""2026-07-17T12:00:00Z"",
                ""actor"": {
                    ""id"": 3,
                    ""uuid"": ""672896d1-d9f7-48d8-ac22-d4efa4e94902"",
                    ""objecttype"": ""Agent"",
                    ""name"": ""Kiera"",
                    ""mbox"": null,
                    ""mbox_sha1sum"": ""4a544776e93b80615f77a462b0126c7976865fc6"",
                    ""openid"": null,
                    ""account"": { ""id"": 0, ""uuid"": ""00000000-0000-0000-0000-000000000000"", ""homepage"": ""https://example.com"", ""name"": ""kiera"" },
                    ""member"": null
                },
                ""verb"": {
                    ""key"": 0,
                    ""uuid"": ""00000000-0000-0000-0000-000000000000"",
                    ""id"": ""https://febr.is/Verb/Details/Initialized"",
                    ""display"": { ""en-US"": ""initialized"" }
                },
                ""object"": {
                    ""key"": 1,
                    ""uuid"": ""99d9db56-48e8-4736-b92f-29e4d3361403"",
                    ""id"": ""https://febr.is/Module/460a5ddf-02c3-4fb8-9d7c-0ef0da64925d"",
                    ""objecttype"": ""Activity"",
                    ""definition"": {
                        ""id"": 1,
                        ""uuid"": ""be503c6c-f44b-4784-9077-88410f93b071"",
                        ""name"": { ""en-US"": ""PC Demo"" },
                        ""description"": { ""en-US"": ""Sterile field demo"" },
                        ""type"": ""https://febr.is/Module/1"",
                        ""moreinfo"": ""https://febr.is/Module/1"",
                        ""extensions"": null,
                        ""interactiontype"": ""performance"",
                        ""correctresponsespattern"": [""step1[,]step2""],
                        ""interactioncomponents"": ""Step1:Clean Table""
                    }
                },
                ""attachments"": [{
                    ""usagetype"": ""https://febr.is/Attachment/VideoReview"",
                    ""display"": { ""en-US"": ""Video Review"" },
                    ""description"": { ""en-US"": ""Session capture"" },
                    ""contenttype"": ""video/mpeg"",
                    ""length"": 12345,
                    ""sha2"": ""abc123"",
                    ""fileurl"": ""https://example.com/video.mp4""
                }]
            }");
        }

        private static JObject Factor(JObject authored)
        {
            JObject factored = JSONHandler.CreateObjectFromDataModel(authored);
            factored.Should().NotBeNull();
            factored.HasValues.Should().BeTrue("factoring the authored statement must not fail");
            return factored;
        }

        // -----------------------------------------------------------------------------------
        // SDKV-3/11: booleans on the wire
        // -----------------------------------------------------------------------------------

        [Fact]
        public void FactoredStatement_SuccessAndCompletion_AreJsonBooleans()
        {
            var factored = Factor(MakeAuthoredInput());
            factored["result"]["success"].Type.Should().Be(JTokenType.Boolean,
                "xAPI 1.0.3 section 4.1.5 requires result.success to be a JSON Boolean");
            factored["result"]["completion"].Type.Should().Be(JTokenType.Boolean);
        }

        [Fact]
        public void UpdateSuccessStatus_WritesJsonBoolean()
        {
            var statement = Factor(MakeAuthoredInput());
            statement = StatementHandler.UpdateSuccessStatus(statement, true);
            statement["result"]["success"].Type.Should().Be(JTokenType.Boolean);
            ((bool)statement["result"]["success"]).Should().BeTrue();
            // serialized bytes carry an unquoted boolean
            statement["result"]["success"].ToString(Newtonsoft.Json.Formatting.None).Should().Be("true");
        }

        [Fact]
        public void UpdateCompletionStatus_WritesJsonBoolean()
        {
            var statement = Factor(MakeAuthoredInput());
            statement = StatementHandler.UpdateCompletionStatus(statement, false);
            statement["result"]["completion"].Type.Should().Be(JTokenType.Boolean);
            ((bool)statement["result"]["completion"]).Should().BeFalse();
        }

        [Fact]
        public void SimulationCompleteAndPassed_WriteJsonBooleans()
        {
            var previous = SimStatics.StaticDetails.CurrentStatement;
            try
            {
                SimStatics.StaticDetails.CurrentStatement = Factor(MakeAuthoredInput());
                StatementHandler.SimulationComplete();
                StatementHandler.SimulationPassed(true);

                var statement = SimStatics.StaticDetails.CurrentStatement;
                statement["result"]["completion"].Type.Should().Be(JTokenType.Boolean);
                ((bool)statement["result"]["completion"]).Should().BeTrue();
                statement["result"]["success"].Type.Should().Be(JTokenType.Boolean);
                ((bool)statement["result"]["success"]).Should().BeTrue();
            }
            finally
            {
                SimStatics.StaticDetails.CurrentStatement = previous;
            }
        }

        [Fact]
        public void TokenToBool_AcceptsBooleansAndLegacyStrings()
        {
            StatementHandler.TokenToBool(new JValue(true)).Should().BeTrue();
            StatementHandler.TokenToBool(new JValue(false)).Should().BeFalse();
            // statements written by older SDK builds carry lowercase strings
            StatementHandler.TokenToBool(new JValue("true")).Should().BeTrue();
            StatementHandler.TokenToBool(new JValue("false")).Should().BeFalse();
            StatementHandler.TokenToBool(null).Should().BeFalse();
            StatementHandler.TokenToBool(JValue.CreateNull()).Should().BeFalse();
            StatementHandler.TokenToBool(new JValue("junk")).Should().BeFalse();
        }

        [Fact]
        public void VerbUpdate_ReadsBooleanCompletion_AutoCalcsCompletedVerb()
        {
            var statement = Factor(MakeAuthoredInput());
            statement = StatementHandler.UpdateCompletionStatus(statement, true);
            statement = StatementHandler.VerbUpdate(statement, VerbEnums.Attempted);
            statement["verb"]["id"].ToString().Should().Be("https://febr.is/Verb/Details/Completed",
                "with completion=true (a JSON Boolean) the Attempted verb auto-calcs to Completed");
        }

        [Fact]
        public void VerbUpdate_ReadsLegacyStringCompletion_StillAutoCalcs()
        {
            var statement = Factor(MakeAuthoredInput());
            statement["result"]["completion"] = "true"; // legacy pre-SDKV-3 shape
            statement = StatementHandler.VerbUpdate(statement, VerbEnums.Attempted);
            statement["verb"]["id"].ToString().Should().Be("https://febr.is/Verb/Details/Completed");
        }

        // -----------------------------------------------------------------------------------
        // SDKV-1: correctResponsesPattern is an array of strings
        // -----------------------------------------------------------------------------------

        [Fact]
        public void FactoredStatement_CorrectResponsesPattern_ArrayInput_EmitsArray()
        {
            var factored = Factor(MakeAuthoredInput());
            var crp = factored["object"]["definition"]["correctResponsesPattern"];
            crp.Type.Should().Be(JTokenType.Array,
                "xAPI 1.0.3 section 4.1.4.1: correctResponsesPattern MUST be an array of strings");
            crp[0].Type.Should().Be(JTokenType.String);
            ((string)crp[0]).Should().Be("step1[,]step2");
        }

        [Fact]
        public void FactoredStatement_CorrectResponsesPattern_LoneString_WrapsToSingleElementArray()
        {
            var authored = MakeAuthoredInput();
            // the legacy test-module value: a lone string that is NOT valid JSON
            authored["object"]["definition"]["correctresponsespattern"] = "[,]";
            var factored = Factor(authored);
            var crp = factored["object"]["definition"]["correctResponsesPattern"];
            crp.Type.Should().Be(JTokenType.Array);
            crp.Should().HaveCount(1);
            ((string)crp[0]).Should().Be("[,]");
        }

        [Fact]
        public void FactoredStatement_CorrectResponsesPattern_SerializedArrayText_Parses()
        {
            var authored = MakeAuthoredInput();
            authored["object"]["definition"]["correctresponsespattern"] = @"[""golf"", ""tetris""]";
            var factored = Factor(authored);
            var crp = factored["object"]["definition"]["correctResponsesPattern"];
            crp.Type.Should().Be(JTokenType.Array);
            crp.Should().HaveCount(2);
            ((string)crp[0]).Should().Be("golf");
        }

        [Fact]
        public void ChangeStringToObject_ParsesArrayShapedText_AsJArray()
        {
            var holder = new JObject(new JProperty("x", @"[""a"",""b""]"));
            Initializer.ChangeStringToObject(holder["x"]);
            holder["x"].Type.Should().Be(JTokenType.Array, "SDKV-1: JArray must be attempted for array-shaped text");
            ((string)holder["x"][1]).Should().Be("b");
        }

        [Fact]
        public void ChangeStringToObject_ObjectText_UnparseableText_NonString_HandledSafely()
        {
            var holder = new JObject(
                new JProperty("obj", @"{""en"":""hi""}"),
                new JProperty("junk", "[,]"),
                new JProperty("already", new JObject(new JProperty("en", "hi"))));
            Initializer.ChangeStringToObject(holder["obj"]);
            Initializer.ChangeStringToObject(holder["junk"]);
            Initializer.ChangeStringToObject(holder["already"]);
            holder["obj"].Type.Should().Be(JTokenType.Object);
            holder["junk"].Type.Should().Be(JTokenType.String, "unparseable text is left untouched");
            holder["already"].Type.Should().Be(JTokenType.Object, "non-string tokens are a no-op");
        }

        // -----------------------------------------------------------------------------------
        // SDKV-23: INPUT normalization -- non-object array elements pass through
        // (previously blind-cast to JObject -> throw -> Initialize returned false)
        // and RFC 5646 language-tag keys keep their authored casing
        // -----------------------------------------------------------------------------------

        [Fact]
        public void ChangePropertiesToLowerCase_ScalarAndNestedArrayElements_DoNotThrow_AndPassThrough()
        {
            var authored = JObject.Parse(@"{
                ""Object"": {
                    ""Definition"": {
                        ""CorrectResponsesPattern"": [""step1"", ""step2""],
                        ""Choices"": [[""nested"", 7], [true, null]],
                        ""InteractionComponents"": [{ ""Id"": ""a"" }]
                    }
                }
            }");

            Action act = () => Initializer.ChangePropertiesToLowerCase(authored);
            act.Should().NotThrow(
                "SDKV-23: string/number array elements carry no property names and must pass through");

            var definition = (JObject)authored["object"]["definition"];
            // keys really lowercased (ordinal check -- the JObject indexer
            // above falls back to case-insensitive matching, so assert on
            // the actual property names)
            definition.Properties().Select(p => p.Name).Should().Contain("correctresponsespattern");
            var crp = definition["correctresponsespattern"];
            crp.Type.Should().Be(JTokenType.Array);
            crp.Select(t => (string)t).Should().Equal("step1", "step2");
            // nested arrays recurse; their scalar leaves are untouched
            var nested = definition["choices"];
            ((string)nested[0][0]).Should().Be("nested");
            ((int)nested[0][1]).Should().Be(7);
            // object elements inside arrays still get key-lowercased
            ((JObject)definition["interactioncomponents"][0]).Properties()
                .Select(p => p.Name).Should().Equal("id");
        }

        [Fact]
        public void ChangePropertiesToLowerCase_LanguageMapTagKeys_KeepAuthoredRfc5646Casing()
        {
            var authored = JObject.Parse(@"{
                ""Verb"": { ""Display"": { ""en-US"": ""initialized"" } },
                ""Object"": { ""Definition"": {
                    ""Name"": { ""en-US"": ""PC Demo"", ""fr-FR"": ""Demo PC"" },
                    ""Description"": { ""en-US"": ""desc"" } } },
                ""Attachments"": [{ ""Display"": { ""en-US"": ""Video"" } }]
            }");

            Initializer.ChangePropertiesToLowerCase(authored);

            // the HOLDER keys are still lowercased (factoring reads lowercase)...
            ((JObject)authored["verb"]).Properties().Select(p => p.Name).Should().Equal("display");
            // ...but the tag keys inside keep their canonical RFC 5646 casing
            ((JObject)authored["verb"]["display"]).Properties()
                .Select(p => p.Name).Should().Equal("en-US");
            ((JObject)authored["object"]["definition"]["name"]).Properties()
                .Select(p => p.Name).Should().Equal("en-US", "fr-FR");
            ((JObject)authored["object"]["definition"]["description"]).Properties()
                .Select(p => p.Name).Should().Equal("en-US");
            ((JObject)authored["attachments"][0]["display"]).Properties()
                .Select(p => p.Name).Should().Equal("en-US");
        }

        // -----------------------------------------------------------------------------------
        // SDKV-2: attachment display/description are Language Maps
        // -----------------------------------------------------------------------------------

        [Fact]
        public void FactoredStatement_AttachmentDisplayAndDescription_AreLanguageMapObjects()
        {
            var factored = Factor(MakeAuthoredInput());
            var attachment = factored["attachments"][0];
            attachment["display"].Type.Should().Be(JTokenType.Object,
                "xAPI 1.0.3 section 4.1.11: attachment display is a Language Map");
            ((string)attachment["display"]["en-US"]).Should().Be("Video Review");
            attachment["description"].Type.Should().Be(JTokenType.Object);
            ((string)attachment["description"]["en-US"]).Should().Be("Session capture");
        }

        [Fact]
        public void AddAttachment_PlainStrings_WrapIntoEnLanguageMaps()
        {
            var previous = SimStatics.StaticDetails.CurrentStatement;
            try
            {
                SimStatics.StaticDetails.CurrentStatement = Factor(MakeAuthoredInput());
                StatementHandler.AddAttachment(
                    "https://febr.is/Attachment/VideoReview",
                    "Video Review",
                    "A session capture",
                    ContentType.video_mpeg,
                    2048,
                    "sha2value",
                    "https://example.com/clip.mp4");

                var attachments = (JArray)SimStatics.StaticDetails.CurrentStatement["attachments"];
                var added = attachments[attachments.Count - 1];
                added["display"].Type.Should().Be(JTokenType.Object);
                ((string)added["display"]["en"]).Should().Be("Video Review");
                added["description"].Type.Should().Be(JTokenType.Object);
                ((string)added["description"]["en"]).Should().Be("A session capture");
                // SDKV-12: attachment keys carry spec casing
                ((JObject)added).ContainsKey("usageType").Should().BeTrue();
                ((JObject)added).ContainsKey("contentType").Should().BeTrue();
                ((JObject)added).ContainsKey("fileUrl").Should().BeTrue();
            }
            finally
            {
                SimStatics.StaticDetails.CurrentStatement = previous;
            }
        }

        // -----------------------------------------------------------------------------------
        // SDKV-5: ISO 8601 durations
        // -----------------------------------------------------------------------------------

        [Fact]
        public void FactoredStatement_NoResultInput_EmitsIsoZeroDuration()
        {
            var factored = Factor(MakeAuthoredInput());
            factored["result"]["duration"].ToString().Should().Be("PT0S",
                "the init default must be a valid ISO 8601 duration, not the .NET \"00:00:00\" clock format");
        }

        [Fact]
        public void FactoredStatement_LegacyDotNetDuration_ReemitsAsIso8601()
        {
            var authored = MakeAuthoredInput();
            authored["result"] = new JObject(
                new JProperty("success", true),
                new JProperty("completion", false),
                new JProperty("duration", "00:01:30"));
            var factored = Factor(authored);
            factored["result"]["duration"].ToString().Should().Be("PT1M30S");
        }

        [Fact]
        public void UpdateDuration_EmitsIso8601WithFractionalSeconds()
        {
            var statement = Factor(MakeAuthoredInput());
            statement = StatementHandler.UpdateDuration(statement, TimeSpan.FromSeconds(90.5));
            statement["result"]["duration"].ToString().Should().Be("PT1M30.5S");
        }

        // -----------------------------------------------------------------------------------
        // SDKV-12: spec-exact key casing on the wire
        // -----------------------------------------------------------------------------------

        [Fact]
        public void FactoredStatement_EmitsSpecCasedKeys()
        {
            var factored = Factor(MakeAuthoredInput());

            var actor = (JObject)factored["actor"];
            actor.ContainsKey("objectType").Should().BeTrue("xAPI keys are case-sensitive");
            actor.ContainsKey("objecttype").Should().BeFalse("the lowercased corruption must be gone");

            var account = (JObject)actor["account"];
            account.ContainsKey("homePage").Should().BeTrue(
                "a lowercased homepage invalidates the Account IFI on a conformant LRS (section 4.1.2.3)");
            account.ContainsKey("homepage").Should().BeFalse();

            var definition = (JObject)factored["object"]["definition"];
            definition.ContainsKey("moreInfo").Should().BeTrue();
            definition.ContainsKey("interactionType").Should().BeTrue();
            definition.ContainsKey("correctResponsesPattern").Should().BeTrue();

            ((JObject)factored["object"]).ContainsKey("objectType").Should().BeTrue();

            var attachment = (JObject)factored["attachments"][0];
            attachment.ContainsKey("usageType").Should().BeTrue();
            attachment.ContainsKey("contentType").Should().BeTrue();
            attachment.ContainsKey("fileUrl").Should().BeTrue();

            var context = (JObject)factored["context"];
            context.ContainsKey("contextActivities").Should().BeTrue();
            context.ContainsKey("contextactivities").Should().BeFalse();
        }

        // -----------------------------------------------------------------------------------
        // SDKV-13: verb.display + definition name/description as objects
        // -----------------------------------------------------------------------------------

        [Fact]
        public void FactoredStatement_VerbDisplay_IsLanguageMapObject()
        {
            var factored = Factor(MakeAuthoredInput());
            factored["verb"]["display"].Type.Should().Be(JTokenType.Object,
                "xAPI 1.0.3 section 4.1.3: verb.display is a Language Map, not a serialized string");
            ((string)factored["verb"]["display"]["en-US"]).Should().Be("initialized");
        }

        [Fact]
        public void FactoredStatement_DefinitionNameAndDescription_AreLanguageMapObjects()
        {
            var factored = Factor(MakeAuthoredInput());
            var definition = factored["object"]["definition"];
            definition["name"].Type.Should().Be(JTokenType.Object);
            ((string)definition["name"]["en-US"]).Should().Be("PC Demo");
            definition["description"].Type.Should().Be(JTokenType.Object);
            ((string)definition["description"]["en-US"]).Should().Be("Sterile field demo");
        }

        [Fact]
        public void FactoredStatement_PlainStringDisplay_WrapsIntoEnMap()
        {
            var authored = MakeAuthoredInput();
            authored["verb"]["display"] = "initialized"; // plain string author shortcut
            var factored = Factor(authored);
            factored["verb"]["display"].Type.Should().Be(JTokenType.Object);
            ((string)factored["verb"]["display"]["en"]).Should().Be("initialized");
        }

        // -----------------------------------------------------------------------------------
        // SDKV-4: context-activity updates land on the real node
        // -----------------------------------------------------------------------------------

        [Fact]
        public void UpdateStatement_ContextActivityParent_IsWrittenToContextActivities()
        {
            var previous = SimStatics.StaticDetails.CurrentStatement;
            try
            {
                SimStatics.StaticDetails.CurrentStatement = Factor(MakeAuthoredInput());
                StatementHandler.UpdateStatement(
                    XAPIProperties.Context,
                    ContextOptions.ContextActivites,
                    ContextActivitesOptions.Parent,
                    "https://febr.is/Curricula/parentActivity");

                var statement = SimStatics.StaticDetails.CurrentStatement;
                statement["context"]["contextActivities"]["parent"].ToString()
                    .Should().Be("https://febr.is/Curricula/parentActivity",
                        "SDKV-4: the update used to write the nonexistent 'contextactivites' typo key and silently no-op");
            }
            finally
            {
                SimStatics.StaticDetails.CurrentStatement = previous;
            }
        }

        [Fact]
        public void UpdateStatement_ContextActivityGroupingCategoryOther_AreWritten()
        {
            var previous = SimStatics.StaticDetails.CurrentStatement;
            try
            {
                SimStatics.StaticDetails.CurrentStatement = Factor(MakeAuthoredInput());
                StatementHandler.UpdateStatement(XAPIProperties.Context, ContextOptions.ContextActivites, ContextActivitesOptions.Grouping, "https://febr.is/g");
                StatementHandler.UpdateStatement(XAPIProperties.Context, ContextOptions.ContextActivites, ContextActivitesOptions.Category, "https://febr.is/c");
                StatementHandler.UpdateStatement(XAPIProperties.Context, ContextOptions.ContextActivites, ContextActivitesOptions.Other, "https://febr.is/o");

                var activities = SimStatics.StaticDetails.CurrentStatement["context"]["contextActivities"];
                activities["grouping"].ToString().Should().Be("https://febr.is/g");
                activities["category"].ToString().Should().Be("https://febr.is/c");
                activities["other"].ToString().Should().Be("https://febr.is/o");
            }
            finally
            {
                SimStatics.StaticDetails.CurrentStatement = previous;
            }
        }

        // -----------------------------------------------------------------------------------
        // SDKV-19/20 (SDK side): the wire id is stamped on emission
        // -----------------------------------------------------------------------------------

        [Fact]
        public void StampStatementId_FactoredPlaceholderId_IsReplacedWithReferenceUuid()
        {
            var previousReference = SimStatics.StaticDetails.ReferenceUUID;
            try
            {
                string expected = Guid.NewGuid().ToString();
                SimStatics.StaticDetails.ReferenceUUID = expected;

                var factored = Factor(MakeAuthoredInput());
                // the typed model's long default is the "0" placeholder the node ignores
                factored["id"].ToString().Should().Be("0",
                    "pre-stamp, the factored statement carries the long-typed model default");

                StatementHandler.StampStatementId(factored);

                factored["id"].Type.Should().Be(JTokenType.String,
                    "xAPI 1.0.3 section 4.1.1: the statement id is a UUID string");
                ((string)factored["id"]).Should().Be(expected,
                    "the stamped id must be the handoff ReferenceUUID");
                Guid parsed;
                Guid.TryParse((string)factored["id"], out parsed).Should().BeTrue();
                parsed.Should().NotBe(Guid.Empty);
            }
            finally
            {
                SimStatics.StaticDetails.ReferenceUUID = previousReference;
            }
        }

        [Theory]
        [InlineData("0")]                                        // long-typed model default
        [InlineData("")]                                         // empty string
        [InlineData("00000000-0000-0000-0000-000000000000")]     // empty-GUID placeholder
        [InlineData("not-a-guid")]                               // junk
        public void StampStatementId_Placeholders_AreReplaced(string placeholder)
        {
            var statement = new JObject(new JProperty("id", placeholder));
            StatementHandler.StampStatementId(statement);
            Guid parsed;
            Guid.TryParse((string)statement["id"], out parsed).Should().BeTrue(
                "every placeholder shape must be replaced with a real UUID");
            parsed.Should().NotBe(Guid.Empty);
        }

        [Fact]
        public void StampStatementId_MissingAndNullId_AreStamped()
        {
            var missing = new JObject();
            StatementHandler.StampStatementId(missing);
            Guid parsedMissing;
            Guid.TryParse((string)missing["id"], out parsedMissing).Should().BeTrue();
            parsedMissing.Should().NotBe(Guid.Empty);

            var nullId = new JObject(new JProperty("id", JValue.CreateNull()));
            StatementHandler.StampStatementId(nullId);
            Guid parsedNull;
            Guid.TryParse((string)nullId["id"], out parsedNull).Should().BeTrue();
            parsedNull.Should().NotBe(Guid.Empty);
        }

        [Fact]
        public void StampStatementId_AlreadyValidGuid_IsPreserved()
        {
            string authored = "672896d1-d9f7-48d8-ac22-d4efa4e94902";
            var statement = new JObject(new JProperty("id", authored));
            StatementHandler.StampStatementId(statement);
            ((string)statement["id"]).Should().Be(authored,
                "a valid non-empty GUID id must never be re-stamped -- that stability IS the retry idempotency");
        }

        [Fact]
        public void StampStatementId_NullStatement_IsNullSafe()
        {
            StatementHandler.StampStatementId(null).Should().BeNull();
        }

        [Fact]
        public void StampStatementId_StableAcrossReserialization()
        {
            var previousReference = SimStatics.StaticDetails.ReferenceUUID;
            try
            {
                var factored = Factor(MakeAuthoredInput());
                StatementHandler.StampStatementId(factored);
                string firstId = (string)factored["id"];

                // the retry path re-serializes + re-parses the same statement
                var reparsed = JObject.Parse(factored.ToString(Newtonsoft.Json.Formatting.None));
                StatementHandler.StampStatementId(reparsed);
                ((string)reparsed["id"]).Should().Be(firstId,
                    "re-serializing the same statement must carry the SAME id (retry = same id)");

                // even after the process-level ReferenceUUID rotates (a NEW
                // simulation run), an already-stamped statement keeps its id
                SimStatics.StaticDetails.ReferenceUUID = Guid.NewGuid().ToString();
                StatementHandler.StampStatementId(reparsed);
                ((string)reparsed["id"]).Should().Be(firstId);
            }
            finally
            {
                SimStatics.StaticDetails.ReferenceUUID = previousReference;
            }
        }

        /// <summary>
        /// End-to-end emission proof on the real WinPC pipeline:
        /// Initialize -> the handler writes {ReferenceUUID}.json -> the emitted
        /// JSON's <c>id</c> is present, a valid GUID, and EQUALS the handoff
        /// filename GUID; EndSimulation re-writes the same file and the id is
        /// unchanged (a host retry of the handoff re-submits the same id).
        /// The library's statement paths are redirected into a temp dir via
        /// the public <see cref="SimFileSystem.ExternalFileSystemMethods"/>
        /// so no real Febris folders are touched, and every static the run
        /// mutates is restored in the finally.
        /// </summary>
        [Fact]
        public async Task Initialize_WinPC_EmittedFile_CarriesIdMatchingFilename_AndIdSurvivesRewrite()
        {
            var previousStatement = SimStatics.StaticDetails.CurrentStatement;
            var previousHandler = SimStatics.StaticDetails.Handler;
            var previousReference = SimStatics.StaticDetails.ReferenceUUID;
            string previousExternalBase = SimFileSystem.FileSystem.ExternalBasePath;
            string tempRoot = Path.Combine(Path.GetTempPath(), "febris-sdkv19-" + Guid.NewGuid().ToString("N"));
            try
            {
                await new SimFileSystem.ExternalFileSystemMethods().ExternalFileSystemRectifier(tempRoot);

                var authored = MakeAuthoredInput();
                // The launcher authors correctResponsesPattern as serialized
                // text -- this pins that legacy authoring shape. (Real string
                // arrays also survive the entry path since SDKV-23; the
                // spec-shaped e2e test below covers that.)
                authored["object"]["definition"]["correctresponsespattern"] = @"[""step1"", ""step2""]";
                string json = authored.ToString(Newtonsoft.Json.Formatting.None);
                string[] args = { "-febrisData=" + json };
                var (initialized, _) = await Initializer.Initialize(args, ExpectedOperatingSystem.WindowsPC);
                initialized.Should().BeTrue("the WinPC pipeline must emit the initial statement file");

                string[] files = Directory.GetFiles(SimFileSystem.FileSystem.StatementPath, "*.json");
                files.Should().HaveCount(1);
                Guid filenameGuid = Guid.Parse(Path.GetFileNameWithoutExtension(files[0]));

                var emitted = JObject.Parse(File.ReadAllText(files[0]));
                emitted["id"].Should().NotBeNull("SDKV-19/20: the emitted statement must carry a wire id");
                emitted["id"].Type.Should().Be(JTokenType.String);
                Guid wireId = Guid.Parse((string)emitted["id"]);
                wireId.Should().NotBe(Guid.Empty);
                wireId.Should().Be(filenameGuid,
                    "the wire id and the {uuid}.json handoff filename must be the same GUID");

                // EndSimulation re-emits (re-writes the same handoff file):
                // the id must be STABLE so a host retry stays idempotent.
                var (reEmitted, _) = await StatementHandler.EndSimulation(true, true, 95f, TimeSpan.FromSeconds(90));
                reEmitted.Should().BeTrue();
                var final = JObject.Parse(File.ReadAllText(files[0]));
                Guid finalId = Guid.Parse((string)final["id"]);
                finalId.Should().Be(wireId, "re-emission of the same run must carry the SAME id");
            }
            finally
            {
                SimStatics.StaticDetails.CurrentStatement = previousStatement;
                SimStatics.StaticDetails.Handler = previousHandler;
                SimStatics.StaticDetails.ReferenceUUID = previousReference;
                await new SimFileSystem.ExternalFileSystemMethods().ExternalFileSystemRectifier(previousExternalBase);
                try { Directory.Delete(tempRoot, recursive: true); } catch { /* best-effort cleanup */ }
            }
        }

        /// <summary>
        /// SDKV-23 end-to-end regression: an authored statement carrying a
        /// spec-shaped correctresponsespattern -- a REAL JSON string array,
        /// not serialized text -- must survive the full Initialize entry path.
        /// Before the fix the INPUT lowercasing pass blind-cast every array
        /// element to JObject, the InvalidCastException was swallowed, and
        /// Initialize returned false with nothing emitted. Also asserts the
        /// part-2 fix: RFC 5646 language-tag keys ("en-US") reach the wire
        /// in their authored casing instead of being lowercased to "en-us".
        /// Same statics/temp-dir hygiene as the SDKV-19/20 e2e test above.
        /// </summary>
        [Fact]
        public async Task Initialize_WinPC_SpecShapedStringArrayCrp_Succeeds_EmitsArray_AndPreservesLanguageTags()
        {
            var previousStatement = SimStatics.StaticDetails.CurrentStatement;
            var previousHandler = SimStatics.StaticDetails.Handler;
            var previousReference = SimStatics.StaticDetails.ReferenceUUID;
            string previousExternalBase = SimFileSystem.FileSystem.ExternalBasePath;
            string tempRoot = Path.Combine(Path.GetTempPath(), "febris-sdkv23-" + Guid.NewGuid().ToString("N"));
            try
            {
                await new SimFileSystem.ExternalFileSystemMethods().ExternalFileSystemRectifier(tempRoot);

                var authored = MakeAuthoredInput();
                // spec-shaped authoring: a real JSON array of strings
                authored["object"]["definition"]["correctresponsespattern"] = new JArray("step1", "step2");
                string json = authored.ToString(Newtonsoft.Json.Formatting.None);
                string[] args = { "-febrisData=" + json };
                var (initialized, _) = await Initializer.Initialize(args, ExpectedOperatingSystem.WindowsPC);
                initialized.Should().BeTrue(
                    "SDKV-23: a spec-shaped string-array correctresponsespattern must initialize " +
                    "(previously the input pass threw and the swallow-catch returned false)");

                string[] files = Directory.GetFiles(SimFileSystem.FileSystem.StatementPath, "*.json");
                files.Should().HaveCount(1);
                var emitted = JObject.Parse(File.ReadAllText(files[0]));

                var crp = emitted["object"]["definition"]["correctResponsesPattern"];
                crp.Type.Should().Be(JTokenType.Array,
                    "the authored string array must be emitted as an array of strings");
                crp.Select(t => (string)t).Should().Equal("step1", "step2");

                // part 2: authored "en-US" survives to the wire (ordinal
                // property-name check -- the indexer is case-insensitive)
                ((JObject)emitted["verb"]["display"]).Properties()
                    .Select(p => p.Name).Should().Equal("en-US");
                ((JObject)emitted["object"]["definition"]["name"]).Properties()
                    .Select(p => p.Name).Should().Equal("en-US");
            }
            finally
            {
                SimStatics.StaticDetails.CurrentStatement = previousStatement;
                SimStatics.StaticDetails.Handler = previousHandler;
                SimStatics.StaticDetails.ReferenceUUID = previousReference;
                await new SimFileSystem.ExternalFileSystemMethods().ExternalFileSystemRectifier(previousExternalBase);
                try { Directory.Delete(tempRoot, recursive: true); } catch { /* best-effort cleanup */ }
            }
        }
    }
}
