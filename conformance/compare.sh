#!/usr/bin/env bash
# Cross-SDK byte-parity harness (C# vs C++). Drives the same authored fixtures
# through both factorings and byte-compares the emitted wire strings:
#
#   C#:  StatementFactoring.FactorStatement -> JObject.FromObject
#        (tests/FebrisSimulationLibraryTests/CrossSdkEmitParityTests.cs,
#        gated on FEBRIS_PARITY_DIR so the suite stays green in the OSS cut)
#   C++: StatementFactoring::FactorStatement -> XApiJson::ToJsonString
#        (probe/main.cpp, built by probe/build_probe.cmd with plain cl)
#
# A byte-identical diff on every fixture is the port's DONE condition
# (docs/SDK_CPP_PARITY_AUDIT.md). Run from anywhere:
#
#   bash simulationintegration/parity/compare.sh
#
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$HERE/.." && pwd)"
FIXTURES=(statement-full statement-edges)

mkdir -p "$HERE/out"

# --- C++ probe: build if missing, then emit ---------------------------------
if [ ! -x "$HERE/probe/build/parity_probe.exe" ]; then
    echo "[parity] building C++ probe"
    cmd //c "$(cygpath -w "$HERE/probe/build_probe.cmd")" > "$HERE/out/probe-build.log" 2>&1 \
        || { echo "[parity] probe build FAILED, see out/probe-build.log"; exit 1; }
fi
for f in "${FIXTURES[@]}"; do
    "$HERE/probe/build/parity_probe.exe" "$HERE/$f.json" "$HERE/out/$f-cpp.json"
done

# --- C# side: run the gated parity tests ------------------------------------
echo "[parity] running C# emit tests"
FEBRIS_PARITY_DIR="$(cygpath -w "$HERE")" dotnet test \
    "$REPO/csharp/FebrisSimulationLibraryTests/Febris.SimulationLibrary.Tests.csproj" \
    -c Release --filter "FullyQualifiedName~CrossSdkEmitParityTests" \
    -v q --nologo > "$HERE/out/cs-test.log" 2>&1 \
    || { echo "[parity] C# test run FAILED, see out/cs-test.log"; exit 1; }

# --- byte diff ---------------------------------------------------------------
fail=0
for f in "${FIXTURES[@]}"; do
    if cmp -s "$HERE/out/$f-cpp.json" "$HERE/out/$f-cs.json"; then
        echo "[parity] $f: BYTE-IDENTICAL"
    else
        echo "[parity] $f: DIFF"
        cmp "$HERE/out/$f-cpp.json" "$HERE/out/$f-cs.json" || true
        fail=1
    fi
done
exit $fail
