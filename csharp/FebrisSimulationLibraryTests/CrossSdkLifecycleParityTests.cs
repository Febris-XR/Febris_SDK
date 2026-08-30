// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using System;
using System.IO;
using System.Linq;
using FluentAssertions;
using Xunit;
using Febris.CsharpSimulationLibraryNetStandard.Enums;
using Febris.CsharpSimulationLibraryNetStandard.Statement;
using SimFileSystem = Febris.CsharpSimulationLibraryNetStandard.FileSystem;

namespace Febris.SimulationLibrary.Tests
{
    /// <summary>
    /// C# half of the cross-SDK LIFECYCLE parity harness
    /// (<c>simulationintegration/parity/compare-lifecycle.sh</c>). Where
    /// <see cref="CrossSdkEmitParityTests"/> pins the factoring+emit pipeline
    /// on a fixed input, this drives the full authoring lifecycle through the
    /// PUBLIC API -- Initialize, the UpdateStatement family, StageRestart,
    /// notes, attachment, verb transition, SimulationComplete -- against a
    /// scratch statement tree, producing the {uuid}.json handoff file. The
    /// C++ side (<c>parity/probe-abi/main.cpp</c>) drives the exported C ABI
    /// of Febris.CppSimulationLibrary.dll through the byte-identical
    /// sequence; the runner normalizes the random reference UUID on both
    /// sides and byte-diffs the two files.
    ///
    /// Workshop-only: no-ops unless FEBRIS_PARITY_DIR is set.
    /// </summary>
    [Collection("SimulationStatics")] // shares the SDK's process-wide statics; see StatementEmissionTests
    public class CrossSdkLifecycleParityTests
    {
        [Fact]
        public void Lifecycle_WritesHandoffFileForParityDiff()
        {
            string parityDir = Environment.GetEnvironmentVariable("FEBRIS_PARITY_DIR");
            if (string.IsNullOrEmpty(parityDir))
            {
                return; // cross-SDK harness not in play (OSS cut, plain CI)
            }

            // --- redirect the statement tree to scratch -------------------
            // The C# FileSystem has no recompute helper; every field is a
            // public mutable static initialized from Documents\Febris, so the
            // test re-derives them all from the scratch root the same way the
            // field initializers do (FileInitalizer() then creates each one).
            string scratch = Path.Combine(parityDir, "out", "lifecycle-cs");
            if (Directory.Exists(scratch))
            {
                Directory.Delete(scratch, recursive: true);
            }
            RedirectFileSystem(scratch);

            // --- the canonical deterministic sequence ---------------------
            // Mirrors StatementBuilder's shape with fixed values; the C++ ABI
            // probe replays exactly this list. Any change here must land in
            // parity/probe-abi/main.cpp in the same commit.
            string seed = File.ReadAllText(Path.Combine(parityDir, "lifecycle-seed.txt")).Trim();
            string[] args = { Febris.CsharpSimulationLibraryNetStandard.SharedDetails.SharedDetails.StatementPreface + seed };

            (bool initialized, _) = Initializer.Initialize(args, ExpectedOperatingSystem.WindowsPC).Result;
            initialized.Should().BeTrue("Initialize must accept the lifecycle seed");

            StatementHandler.UpdateStatement(XAPIProperties.Result, ResultOptions.ScoreMax, 100f);
            StatementHandler.UpdateStatement(XAPIProperties.Result, ResultOptions.ScoreMin, 0f);
            (bool ready1, _) = StatementHandler.GetSendableUpdate().Result;
            ready1.Should().BeTrue();

            StatementHandler.StageRestart();
            StatementHandler.StageRestart();
            StatementHandler.StageRestart();
            (bool ready2, _) = StatementHandler.GetSendableUpdate().Result;
            ready2.Should().BeTrue();

            StatementHandler.AddResultNote("Checklist deviation noted.");
            StatementHandler.AddResultNote("Torque sequence repeated.");

            StatementHandler.UpdateStatement(XAPIProperties.Context, ContextOptions.Registration, "5e6f7a8b-9c0d-4e5f-b1c2-3d4e5f6a7b8c");
            StatementHandler.UpdateStatement(XAPIProperties.Context, ContextOptions.Platform, "Febris Simulation Runtime");
            StatementHandler.UpdateStatement(XAPIProperties.Context, ContextOptions.Language, "en-US");
            StatementHandler.UpdateStatement(XAPIProperties.Context, ContextOptions.ContextActivites, ContextActivitesOptions.Parent, "https://febr.is/Curricula/parentActivity");

            StatementHandler.UpdateStatement(XAPIProperties.Attachments,
                "https://febr.is/Attachment/VideoReview", "Video Review", "Session capture",
                ContentType.video_mpeg, 2048, "sha2placeholder", "https://example.com/clip.mp4");
            (bool ready3, _) = StatementHandler.GetSendableUpdate().Result;
            ready3.Should().BeTrue();

            StatementHandler.UpdateStatement(XAPIProperties.Result, ResultOptions.ScoreRaw, 87f);
            StatementHandler.DurationUpdate(TimeSpan.FromMilliseconds(754000));
            StatementHandler.SimulationPassed(true);
            // Upstream C# quirk, mirrored by the port: VerbUpdate's 'updated'
            // local is declared false and never assigned, so the public method
            // always returns false even when the verb transition happened.
            StatementHandler.VerbUpdate(VerbEnums.Pass).Should().BeFalse();
            StatementHandler.SimulationComplete();

            // --- the artifact ---------------------------------------------
            string statementDir = SimFileSystem.FileSystem.StatementPath;
            string[] produced = Directory.GetFiles(statementDir, "*.json");
            produced.Should().HaveCount(1, "the WinPC flow rewrites one {uuid}.json handoff file");

            // ============ phase 2: Android extras path =====================
            // Same seed, Android handler: no disk, extras carry ReferenceUUID
            // + RawStatementJson. The driver writes the flattened extras
            // object PRE-NORMALIZED (its own reference uuid replaced) so the
            // runner can byte-compare directly.
            (bool aInit, string[,] aExtras) = Initializer.Initialize(args, ExpectedOperatingSystem.Android).Result;
            aInit.Should().BeTrue("Android Initialize must accept the seed");
            aExtras.Should().NotBeNull();
            var extrasObj = new Newtonsoft.Json.Linq.JObject();
            for (int i = 0; i < aExtras.GetLength(0); i++)
            {
                extrasObj[aExtras[i, 0]] = aExtras[i, 1];
            }
            string refUuid = Febris.CsharpSimulationLibraryNetStandard.StaticDetails.StaticDetails.ReferenceUUID;
            string extrasText = extrasObj.ToString(Newtonsoft.Json.Formatting.None).Replace(refUuid, "@REFERENCE@");
            File.WriteAllText(Path.Combine(parityDir, "out", "lifecycle-cs-android-extras.json"), extrasText);

            // ============ phase 3: authored-result alt scenario ============
            // Exercises the overloads phase 1 misses: bool/string result
            // updates, statement-reference + context-extension writes,
            // CustomVerbUpdate, EndSimulation(bool,bool,float,TimeSpan) with
            // the scaled-score auto-update, and the authored-result no-op
            // mirrors (extensions is JSON null, so StageRestart/AddResultNote
            // must no-op identically on both sides).
            string scratchAlt = Path.Combine(parityDir, "out", "lifecycle-cs-alt");
            if (Directory.Exists(scratchAlt))
            {
                Directory.Delete(scratchAlt, recursive: true);
            }
            RedirectFileSystem(scratchAlt);

            string seedAlt = File.ReadAllText(Path.Combine(parityDir, "lifecycle-seed-alt.txt")).Trim();
            string[] argsAlt = { Febris.CsharpSimulationLibraryNetStandard.SharedDetails.SharedDetails.StatementPreface + seedAlt };
            (bool altInit, _) = Initializer.Initialize(argsAlt, ExpectedOperatingSystem.WindowsPC).Result;
            altInit.Should().BeTrue("alt Initialize must accept the authored-result seed");

            StatementHandler.UpdateStatement(XAPIProperties.Result, ResultOptions.Success, true);
            StatementHandler.UpdateStatement(XAPIProperties.Result, ResultOptions.Response, "Corrected response");
            StatementHandler.StageRestart();                    // no-op: extensions is JSON null
            StatementHandler.AddResultNote("Should not land."); // no-op: same
            StatementHandler.UpdateStatement(XAPIProperties.Context, ContextOptions.StatementReference, ContextStatementReferenceOptions.Id, "9c0d1e2f-3a4b-4c9d-b5e6-7b8c9d0e1f2a");
            StatementHandler.UpdateStatement(XAPIProperties.Context, ContextOptions.StatementReference, ContextStatementReferenceOptions.ObjectType, "StatementRef");
            StatementHandler.UpdateStatement(XAPIProperties.Context, ContextOptions.Extensions, ContextExtensionOptions.Option1, "ctx-extension-value");
            StatementHandler.CustomVerbUpdate("https://febr.is/Verb/Details/Inspected");
            // Emit while the custom verb is live (EndSimulation cannot run on a
            // custom IRI: GetVerbEnum throws on anything outside the enum map,
            // an upstream C# gap both SDKs mirror).
            (bool altReady, _) = StatementHandler.GetSendableUpdate().Result;
            altReady.Should().BeTrue();
            // Restore a resolvable verb, then the 4-arg EndSimulation finale
            // (completion/success/duration/raw score + scaled-score auto-update).
            StatementHandler.VerbUpdate(VerbEnums.Not_Pass);
            (bool endReady, _) = StatementHandler.EndSimulation(true, true, 41.7f, TimeSpan.FromMilliseconds(90500)).Result;
            endReady.Should().BeTrue();

            Directory.GetFiles(SimFileSystem.FileSystem.StatementPath, "*.json")
                .Should().HaveCount(1, "the alt WinPC flow rewrites one handoff file");
        }

        private static void RedirectFileSystem(string basePath)
        {
            SimFileSystem.FileSystem.BasePath = basePath;
            SimFileSystem.FileSystem.ExternalBasePath = basePath;
            SimFileSystem.FileSystem.MediaPath = Path.Combine(basePath, "Media");
            SimFileSystem.FileSystem.VideoPath = Path.Combine(SimFileSystem.FileSystem.MediaPath, "Videos");
            SimFileSystem.FileSystem.SplitFilePath = Path.Combine(SimFileSystem.FileSystem.VideoPath, "SplitVideos");
            SimFileSystem.FileSystem.RecordingsFilePath = Path.Combine(SimFileSystem.FileSystem.VideoPath, "Recordings");
            SimFileSystem.FileSystem.TempRecordingsFilePath = Path.Combine(SimFileSystem.FileSystem.VideoPath, "TempRecording");
            SimFileSystem.FileSystem.zipFolderPath = Path.Combine(SimFileSystem.FileSystem.VideoPath, "ZippedRecordings");
            SimFileSystem.FileSystem.BaseModulePath = Path.Combine(basePath, "Modules");
            SimFileSystem.FileSystem.ModuleLinkPath = Path.Combine(SimFileSystem.FileSystem.BaseModulePath, "ModuleLinks");
            SimFileSystem.FileSystem.ModulePath = Path.Combine(SimFileSystem.FileSystem.BaseModulePath, "Modules");
            SimFileSystem.FileSystem.ZippedModulePath = Path.Combine(SimFileSystem.FileSystem.BaseModulePath, "ZippedModuleFiles");
            SimFileSystem.FileSystem.BaseStatementPath = Path.Combine(basePath, "statements");
            SimFileSystem.FileSystem.StatementPath = Path.Combine(SimFileSystem.FileSystem.BaseStatementPath, "statements");
            SimFileSystem.FileSystem.WorkingStatementPath = Path.Combine(SimFileSystem.FileSystem.BaseStatementPath, "workingstatement");
            SimFileSystem.FileSystem.OldStatementPath = Path.Combine(SimFileSystem.FileSystem.BaseStatementPath, "oldstatements");
            SimFileSystem.FileSystem.BaseLogPath = Path.Combine(basePath, "Logs");
            SimFileSystem.FileSystem.UploaderLogPath = Path.Combine(SimFileSystem.FileSystem.BaseLogPath, "UploaderLogs");
            SimFileSystem.FileSystem.LauncherLogPath = Path.Combine(SimFileSystem.FileSystem.BaseLogPath, "LauncherLogs");
            SimFileSystem.FileSystem.RecorderLogPath = Path.Combine(SimFileSystem.FileSystem.BaseLogPath, "RecorderLogs");
            SimFileSystem.FileSystem.ModuleManagerLogPath = Path.Combine(SimFileSystem.FileSystem.BaseLogPath, "ModuleManagerLogs");
            SimFileSystem.FileSystem.SimulationLogBasePath = Path.Combine(SimFileSystem.FileSystem.BaseLogPath, "SimulationLogs");
            SimFileSystem.FileSystem.sLocation = Path.Combine(basePath, "cred");
        }
    }
}
