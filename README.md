This is the first part of the Febris OSS release. It is safe to call this version 4 of the Febris platform. Many aspects of Version 3 had to be stripped out (The central hub, marketplace, developer system, accreditation system, micro-credentialing, CRM, LMS components that added centralized truth, and there may be a few parts that are now gone that previously existed that I cannot recall right this second) and I used Claude to create and cut that seam. If there are lingering parts, I apologize and I will fix it as soon as I can. I feel like I stretched Claude's capabilities while working on this project. AI was not used on any of the other version of Febris so some of these cuts may seem a little ragged but the entire system was built by one person so, please cut me a little slack.

Claude is far better at documenting code than I have ever been and I suspect between my naming conventions and Claude's documentation, this release will be easy to follow.

# Febris xAPI Simulation SDK

**An on-device xAPI statement builder for simulations and games, in two lock-step
implementations: a C# library (`netstandard2.0`, one dependency) and a native C++ DLL with a flat
`extern "C"` ABI. No HTTP client and no database. Both build and update an xAPI 1.0.3 statement
in-process and hand the finished statement to a host application, either as a file on disk or as
Android Intent extras. The two SDKs emit byte-identical statement JSON, and the conformance
harness in this repository is what proves it.**

The SDK never talks to a Learning Record Store. It produces the statement and stops. Getting that
statement to an LRS is the host application's job, which is what lets the same library run inside a
Unity build, a Windows executable and an Android app without any of them agreeing on a transport.

---

## Installing

Both SDKs are published. You do not need to build either one.

**C#**, from nuget.org:

```
dotnet add package Febris.Simulation.XApiSdk --version 0.1.0
```

**C++**, through the Febris vcpkg registry. Add
`https://github.com/Febris-XR/Febris_VcpkgRegistry` to the `registries` array of your
`vcpkg-configuration.json`, then:

```
vcpkg install febris-simulation-sdk
```

Or take the Windows x64 bundle straight from the
[latest release](https://github.com/Febris-XR/Febris_SDK/releases): it carries `FebrisSimApi.h`,
the DLL and the import library, with a `SHA256SUMS` file to verify against.

From v0.1.1 onward the bundle also carries build provenance, so you can prove where it came from
and not only that it arrived intact:

```sh
gh attestation verify febris-simulation-sdk-cpp-<version>-win-x64.zip --repo Febris-XR/Febris_SDK
```

There is no key to fetch and no keyring to trust. The attestation binds the bytes to the release
workflow in this repository at that tag.

Adding the project to your own solution still works and is documented below, but it is no longer
the expected route.

---

## What is in here

Each language directory is fully self-contained with its own build entry point. The root holds
only what both sides must read: the conformance contract and the legal files.

| Path | What |
|---|---|
| `csharp/FebrisCSharp` | the C# library, `netstandard2.0` |
| `csharp/StatementBuilder` | a console harness that drives the C# library in a loop |
| `csharp/FebrisSimulationLibraryTests` | **61 xUnit tests**, all passing |
| `csharp/febris-xapi-sdk.sln` | the C# solution, spans `csharp/` only |
| `cpp/` | the C++ SDK: sources, `Febris.CppSimulationLibrary.vcxproj`, and `FebrisSimApi.h`, the public C ABI |
| `conformance/` | the cross-SDK byte-parity harness: authored seeds, both probes, and the compare scripts |

The C# library carries one NuGet dependency, **Newtonsoft.Json 13.0.1**, and zero project
references. It is a leaf on purpose, and the cut script that produces this repository refuses to
run if that stops being true. The C++ SDK restores three native packages at build time (boost,
nlohmann.json, rapidxml) and exports a versioned flat C ABI, so consumers bind to
`FebrisSimApi.h` plus the DLL and never to the C++ classes.

## Getting the SDKs

You are not expected to build these yourself. Each version tag makes CI produce both artifacts
from the same source:

- **C#**: a NuGet package, built and published to nuget.org by `pack.yml` through Trusted
  Publishing, so no API key is stored anywhere. The published id is
  `Febris.Simulation.XApiSdk`, and the pack step asserts that the id and the version match the
  tag before it will push.
- **C++**: a prebuilt Windows x64 bundle attached to the GitHub Release for the tag by
  `release-cpp.yml` -- `FebrisSimApi.h`, the DLL and the import library, with a `SHA256SUMS` to
  verify the download and, from v0.1.1, a build-provenance attestation to verify its origin.
  Binaries are never committed to git. They exist only as Release assets built from the tagged
  source, and the release step runs only after the conformance harness proves the DLL byte-matches
  the C# SDK.

## Building from source (optional)

```bash
# C# (any OS with the .NET 8 SDK)
dotnet test csharp/febris-xapi-sdk.sln -c Release
dotnet pack csharp/FebrisCSharp/Febris.CsharpSimulationLibraryNetStandard.csproj -c Release -o artifacts

# C++ (Windows, VS2022 MSBuild)
nuget restore cpp/packages.config -PackagesDirectory cpp/packages
msbuild cpp/Febris.CppSimulationLibrary.vcxproj -p:Configuration=Release -p:Platform=x64
# output: cpp/x64/Release/Febris.CppSimulationLibrary.dll + .lib
```

## The two SDKs are one product

The C# and C++ implementations are kept in lock-step: at the same version they emit
byte-identical statement JSON for the same authored inputs. `conformance/compare.sh` proves the
factoring and emission pipeline on fixed fixtures, and `conformance/compare-lifecycle.sh` drives
full authoring lifecycles through the C# public API and through the exported C ABI of the built
DLL (via `LoadLibrary`/`GetProcAddress`, exactly as a customer binds) and byte-compares the
handoff files. A wire change that lands in one SDK without the other fails the harness, which is
why a wire change belongs in one pull request touching both.

## Why `netstandard2.0`

Because Unity managed scripts target it. That is the whole reason, and it is worth stating plainly
before someone helpfully modernises the target framework. A newer TFM would compile fine here and
then fail to load in the engine that most consumers are using.

## The API is static, and that is deliberate

Simulation code calls this from wherever the interesting thing just happened, usually deep inside a
frame update or an event handler that has no dependency-injection container in reach. So the entry
points are static and the statement being built is ambient.

```csharp
using Febris.CsharpSimulationLibraryNetStandard.Enums;
using Febris.CsharpSimulationLibraryNetStandard.Statement;

// 1. Hand in the launch payload. The host application passes it on the command line.
var (initialized, handoff) = await Initializer.Initialize(args, ExpectedOperatingSystem.WindowsPC);
if (!initialized) return;

// 2. Update the statement as the run progresses. Every call is static.
StatementHandler.UpdateStatement(XAPIProperties.Result, ResultOptions.ScoreMax, 100f);
StatementHandler.UpdateStatement(XAPIProperties.Result, ResultOptions.ScoreRaw, 87f);
StatementHandler.AddResultNote("Cleaned the field before opening the pack.");
StatementHandler.StageRestart();
StatementHandler.VerbUpdate(VerbEnums.Completed);

// 3. Emit.
var dispatch = await StatementHandler.GetSendableDispatch();
```

`ScoreScale` is derived rather than set. Write `ScoreRaw` and `ScoreMax` and the scaled score is
computed and clamped to the xAPI range of -1 to 1. A `ScoreMax` of zero, negative, `NaN` or infinity
skips the update and logs a warning instead of emitting a nonsense value, and there are ten tests
pinning exactly that.

## Getting the statement out

`GetSendableDispatch()` returns a `SimulationDispatch` with three members:

- `Ready` is false when the library refused or the handler hit an error path.
- `IntentAction` is the action string the host should fire the outbound transport with. On Android
  it is one of `com.febris.STATEMENT_CREATE`, `_UPDATE` or `_ERROR`. On every other platform it is
  empty.
- `Extras` is the key/value payload. On Android these become Intent extras. On Windows it is
  ignored, because that handler writes the statement to a file and there is no extras surface.

The older `GetSendableUpdate()` returns the same information as an opaque `(bool, string[,])` tuple.
It still works and is still tested. `SimulationDispatch` exists because the tuple did not carry the
intent action, so a host had to remember which method it had just called in order to fire the right
broadcast, and getting that wrong routed the statement to the wrong receiver silently.

Four platforms are handled: `WindowsPC`, `Android`, `iOSvariant` and `WinMobile`.

## The xAPI models here are internal, on purpose

`Statement`, `Actor`, `Verb`, `Object`, `Result` and `Context` all exist in this library and all of
them are `internal`. This is not an oversight and it is not a package you should reach into for xAPI
shapes.

The reason is that the statement is built through `Initializer` and `StatementHandler`, which own
the ordering and validation rules. Handing out mutable models would let a caller construct a
statement that the emitter would then have to defend against. If you want plain xAPI types to pass
around, that is a different package and a different contract.

## Known limits before 1.0

Stated here rather than left to be discovered:

- **The verb and extension IRIs are hardcoded to the `febr.is` namespace.** A fork emits Febris
  IRIs. Parameterising the base is required before first publish and has not been done. Note also
  that the verb IRIs use `https` and the extension IRIs use `http`, which is inconsistent and
  becomes permanent once statements are in the wild, because an IRI is an identity and changing it
  changes meaning.
- **`StatementBuilder` is a development harness**, not a shipped artifact. It targets `net8.0`
  (retargeted 2026-08-29 from the end-of-life `netcoreapp3.1`).
- **Unity and Unreal glue are not in this repository yet.** The C++ SDK is here and
  conformance-verified against the C# SDK, but the engine-side glue packages are still to come.
  Unreal and native hosts can consume the C ABI (`cpp/FebrisSimApi.h`) directly today.
- **The C++ SDK ships through two channels.** `release-cpp.yml` attaches the prebuilt bundle to
  each version tag, and the vcpkg port is live in
  [Febris_VcpkgRegistry](https://github.com/Febris-XR/Febris_VcpkgRegistry) at 0.1.0. The install
  route is documented above. This bullet used to say the port was still to come, which stopped
  being true when the registry published and contradicted this file's own install section.
- **One name is historical.** `FileSystemInitalizer` is a misspelled public type. It is
  load-bearing now, so it is not being renamed casually. This bullet also used to name a
  `FebrisCShapTesting` directory. No such directory exists here. The test project is
  `csharp/FebrisSimulationLibraryTests`, and the misspelled name survives only in the workshop.

## Versioning

Pre-1.0. The public surface may change between minor versions. The emitted statement JSON is the
part to treat as the real contract, and the test suite is written against the emitted JSON rather
than against the C# API for that reason.

## Licence

Apache-2.0. See [LICENSE](LICENSE) and [NOTICE](NOTICE).

Apache-2.0 is deliberate for this tier. The SDK is meant to be embedded in proprietary simulations
and games, so it carries no copyleft obligation on the work that links it. The Febris platform
itself is AGPL-3.0, and that licence stops at the server.

## Security

Report vulnerabilities privately through this repository's Security tab. See
[SECURITY.md](SECURITY.md). Please do not open a public issue for a security bug.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md). The emitted JSON is a contract, so a change to what a
statement looks like on the wire is reviewed more carefully than a change to how it is built.
