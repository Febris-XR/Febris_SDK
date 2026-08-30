# Contributing

Thanks for looking. This library runs inside other people's simulations, on four platforms, and the
JSON it emits is consumed by learning record stores that were not written with it in mind. That
makes the emitted statement the real public interface, and it is the thing to be careful with.

## Prerequisites

- **.NET 8 SDK** for the C# side. A `global.json` pins the 8.0 feature band, so a machine carrying
  only a newer SDK is told so by name instead of quietly building against a different toolchain.
- **Windows with VS2022 MSBuild and `nuget.exe`** for the C++ side and the conformance harness.
  The C# side alone builds on Linux, macOS and Windows with no other requirement.

There is no database and no Docker anywhere in this repository.

## Build and test

```bash
# C# -- from the repo root, any OS
dotnet test csharp/febris-xapi-sdk.sln -c Release

# C++ -- Windows, from a VS2022 Developer prompt (or any shell with MSBuild on PATH)
nuget restore cpp/packages.config -PackagesDirectory cpp/packages
msbuild cpp/Febris.CppSimulationLibrary.vcxproj -p:Configuration=Release -p:Platform=x64
```

**61 C# tests**, all passing, across four files:

- `StatementEmissionTests.cs` asserts what comes out on the wire. Spec-cased keys, language maps for
  display and description fields, ISO-8601 durations, `correctResponsesPattern` arriving as an
  array, statement ids being stamped and surviving reserialization, and booleans emitting as JSON
  booleans rather than as the legacy quoted strings.
- `StatementHandlerBehaviorTests.cs` asserts the internal guards. Actor and account validation, and
  the scaled-score calculation including every way `ScoreMax` can be unusable.
- `CrossSdkEmitParityTests.cs` and `CrossSdkLifecycleParityTests.cs` are the C# halves of the
  cross-SDK conformance harness. They no-op unless `FEBRIS_PARITY_DIR` is set, which the
  `conformance/` scripts do for you.

If a change alters the emitted JSON, the emission tests are where it has to be pinned, and the
conformance harness is what proves the other SDK moved with it.

## Cross-SDK conformance (Windows)

The two SDKs must emit byte-identical statement JSON. After building the C++ DLL:

```bash
bash conformance/compare.sh            # factoring + emission on fixed fixtures
bash conformance/compare-lifecycle.sh  # full authoring lifecycles through the C ABI
```

Both must report BYTE-IDENTICAL on every scenario. **A wire change lands in both SDKs plus the
conformance expectations in ONE pull request**, never split across two, because either half alone
leaves the harness red. The probe build scripts assume the VS2022 Community `VsDevCmd.bat` path;
adjust the `call` line if your edition differs.

## About `InternalsVisibleTo`

The library grants internals to `Febris.SimulationLibrary.Tests`. That grant is intentional and it
stays.

The public entry points need an operating system, a handler and a parsed JSON object before they do
anything, which makes them awkward to drive from a unit test. The grant lets the tests reach the
validation and scoring helpers directly, so a rule like "a `ScoreMax` of zero must not produce a
scaled score" can be asserted in three lines instead of by constructing a whole simulation run.

Both projects ship in this repository under the same licence, so the grant does not cross a
licensing or repository boundary. Do not widen it to any other assembly.

## The emitted statement is a contract

Two rules follow from that:

- **An IRI is an identity, not a URL.** Changing a verb or extension IRI changes what a statement
  means to every consumer that has already stored one. Treat existing IRIs as frozen. The verb and
  extension IRIs are currently hardcoded to the `febr.is` namespace, and parameterising that base is
  open work rather than an invitation to edit the constants in place.
- **A wire change needs a test in the same pull request.** Not a follow-up. The emission tests exist
  so that a JSON change is visible in review as a diff in expected output.

## Keep it loadable in Unity

`netstandard2.0` is not a default that nobody revisited. It is the target Unity managed scripts use,
and it is why the library carries exactly one dependency and defines its own enums rather than
pulling in a shared package. Please do not raise the target framework or add dependencies without
saying in the pull request how you confirmed the result still loads in an engine.

## Style

- One logical change per pull request.
- Match the surrounding file. The codebase is not uniform and there is no formatter gate, so
  consistency with what you are editing beats consistency with your own preferences.
- New first-party source files, C# and C++ alike, carry the two-line SPDX header that the
  existing files carry.
- Say how you verified the change. "Tests pass" is enough when a test covers it. When the change
  touches platform handoff, which the suite cannot reach, say what you actually ran and on what.

## Reporting a security issue

Do not open a public issue. See [SECURITY.md](SECURITY.md) for the private reporting channel.

## Licence

By contributing you agree that your contributions are licensed under Apache-2.0, the same licence as
the project. See [LICENSE](LICENSE) and [NOTICE](NOTICE).
