// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using System;
using System.IO;
using FluentAssertions;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Xunit;

namespace Febris.SimulationLibrary.Tests
{
    /// <summary>
    /// C# half of the cross-SDK byte-parity harness
    /// (<c>simulationintegration/parity/</c>). Drives an authored fixture
    /// through the same two calls the live emit path uses
    /// (<c>JSONHandler.CreateObjectFromDataModel</c> lines 104-107):
    /// <c>StatementFactoring.FactorStatement</c> then
    /// <c>JObject.FromObject</c>, and writes the compact wire string to
    /// <c>out/&lt;fixture&gt;-cs.json</c>. The C++ probe
    /// (<c>parity/probe/main.cpp</c>) does the same through the ported
    /// factoring + XApiJson emitter; <c>parity/compare.sh</c> runs both and
    /// byte-diffs the outputs.
    ///
    /// Workshop-only: the tests no-op unless FEBRIS_PARITY_DIR points at the
    /// fixture directory (compare.sh sets it), so the suite stays green in
    /// the OSS cut, where the C++ side and the fixtures are absent.
    /// </summary>
    public class CrossSdkEmitParityTests
    {
        [Theory]
        [InlineData("statement-full")]
        [InlineData("statement-edges")]
        public void Emit_WritesWireStringForParityDiff(string fixture)
        {
            string parityDir = Environment.GetEnvironmentVariable("FEBRIS_PARITY_DIR");
            if (string.IsNullOrEmpty(parityDir))
            {
                return; // cross-SDK harness not in play (OSS cut, plain CI)
            }

            string inputPath = Path.Combine(parityDir, fixture + ".json");
            File.Exists(inputPath).Should().BeTrue($"fixture {inputPath} must exist when FEBRIS_PARITY_DIR is set");

            JObject input = JObject.Parse(File.ReadAllText(inputPath));

            // The live emit pipeline, minus CreateObjectFromDataModel's
            // catch-all (a factoring failure should fail the test loudly,
            // not produce an empty diff side).
            var statement = new CsharpSimulationLibraryNetStandard.Statement.StatementFactoring()
                .FactorStatement(input);
            statement.Should().NotBeNull("FactorStatement must survive the fixture");

            string wire = JObject.FromObject(statement).ToString(Formatting.None);

            Directory.CreateDirectory(Path.Combine(parityDir, "out"));
            File.WriteAllText(Path.Combine(parityDir, "out", fixture + "-cs.json"), wire);
        }
    }
}
