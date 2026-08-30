#!/usr/bin/env bash
# Cross-SDK LIFECYCLE byte-parity harness -- the export surface's conformance
# gate. Both SDKs replay the same deterministic authoring sequence:
#
#   C#:  the PUBLIC API (Initializer + StatementHandler), via the gated test
#        CrossSdkLifecycleParityTests, into out/lifecycle-cs/...
#   C++: the EXPORTED C ABI of Febris.CppSimulationLibrary.dll, via
#        probe-abi (LoadLibrary + GetProcAddress -- customer-grade binding),
#        into out/lifecycle-cpp/...
#
# Each side produces one {uuid}.json handoff file. The reference UUID is
# random by design (WinPCHandler regenerates it at Initialize), so both files
# are normalized by replacing their own filename stem with @REFERENCE@ before
# the byte-diff. Everything else must be byte-identical.
#
#   bash simulationintegration/parity/compare-lifecycle.sh
#
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$HERE/.." && pwd)"
DLL="$REPO/cpp/x64/Release/Febris.CppSimulationLibrary.dll"

mkdir -p "$HERE/out"

# --- C++ side: DLL must exist (build_cpp / msbuild), probe built on demand ---
if [ ! -f "$DLL" ]; then
    echo "[lifecycle] DLL missing: $DLL (build the Release x64 DLL first)"; exit 1
fi
if [ ! -x "$HERE/probe-abi/build/parity_lifecycle.exe" ]; then
    echo "[lifecycle] building ABI probe"
    cmd //c "$(cygpath -w "$HERE/probe-abi/build_probe_abi.cmd")" > "$HERE/out/probe-abi-build.log" 2>&1 \
        || { echo "[lifecycle] probe build FAILED, see out/probe-abi-build.log"; exit 1; }
fi

CPP_SCRATCH="$HERE/out/lifecycle-cpp"
CPP_SCRATCH_ALT="$HERE/out/lifecycle-cpp-alt"
rm -rf "$CPP_SCRATCH" "$CPP_SCRATCH_ALT"
mkdir -p "$CPP_SCRATCH" "$CPP_SCRATCH_ALT"
"$HERE/probe-abi/build/parity_lifecycle.exe" \
    "$(cygpath -w "$DLL")" "$(cygpath -w "$CPP_SCRATCH")" "$(cygpath -w "$HERE/lifecycle-seed.txt")" \
    "$(cygpath -w "$CPP_SCRATCH_ALT")" "$(cygpath -w "$HERE/lifecycle-seed-alt.txt")" \
    "$(cygpath -w "$HERE/out/lifecycle-cpp-android-extras.json")" \
    > "$HERE/out/lifecycle-cpp-stdout.json" \
    || { echo "[lifecycle] C++ ABI probe FAILED"; exit 1; }

# --- C# side: the gated public-API test --------------------------------------
echo "[lifecycle] running C# lifecycle test"
FEBRIS_PARITY_DIR="$(cygpath -w "$HERE")" dotnet test \
    "$REPO/csharp/FebrisSimulationLibraryTests/Febris.SimulationLibrary.Tests.csproj" \
    -c Release --filter "FullyQualifiedName~CrossSdkLifecycleParityTests" \
    -v q --nologo > "$HERE/out/lifecycle-cs-test.log" 2>&1 \
    || { echo "[lifecycle] C# test run FAILED, see out/lifecycle-cs-test.log"; exit 1; }

# --- locate + normalize + diff -----------------------------------------------
normalize() {
    local dir="$1" out="$2"
    local files=("$dir"/statements/statements/*.json)
    if [ ${#files[@]} -ne 1 ] || [ ! -f "${files[0]}" ]; then
        echo "[lifecycle] expected exactly one handoff json under $dir/statements/statements"; exit 1
    fi
    local stem
    stem="$(basename "${files[0]}" .json)"
    sed "s/${stem}/@REFERENCE@/g" "${files[0]}" > "$out"
}

fail=0
diffpair() {
    local label="$1" cppf="$2" csf="$3"
    if cmp -s "$cppf" "$csf"; then
        echo "[lifecycle] $label: BYTE-IDENTICAL"
    else
        echo "[lifecycle] $label: DIFF"
        cmp "$cppf" "$csf" || true
        fail=1
    fi
}

normalize "$HERE/out/lifecycle-cs"     "$HERE/out/lifecycle-cs-normalized.json"
normalize "$CPP_SCRATCH"               "$HERE/out/lifecycle-cpp-normalized.json"
normalize "$HERE/out/lifecycle-cs-alt" "$HERE/out/lifecycle-cs-alt-normalized.json"
normalize "$CPP_SCRATCH_ALT"           "$HERE/out/lifecycle-cpp-alt-normalized.json"

diffpair "handoff statement"      "$HERE/out/lifecycle-cpp-normalized.json"       "$HERE/out/lifecycle-cs-normalized.json"
diffpair "android extras"         "$HERE/out/lifecycle-cpp-android-extras.json"   "$HERE/out/lifecycle-cs-android-extras.json"
diffpair "alt handoff statement"  "$HERE/out/lifecycle-cpp-alt-normalized.json"   "$HERE/out/lifecycle-cs-alt-normalized.json"
exit $fail
