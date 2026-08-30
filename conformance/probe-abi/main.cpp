// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
// C++ half of the cross-SDK LIFECYCLE parity harness -- and the proof of the
// export surface itself. Unlike the factoring probe (which compiles the SDK
// sources), this program consumes Febris.CppSimulationLibrary.dll EXACTLY the
// way a customer would: LoadLibrary + GetProcAddress against the flat C ABI
// declared in FebrisSimApi.h. Every GetProcAddress here asserts an unmangled
// extern "C" export by name; a missing export fails loudly.
//
// It replays, call for call, the deterministic sequence in
// tests/FebrisSimulationLibraryTests/CrossSdkLifecycleParityTests.cs. Any
// change to that sequence must land here in the same commit.
//
// usage: parity_lifecycle <dll-path> <scratch-base-path> <seed-file>
//                          <scratch-alt-path> <seed-alt-file> <android-extras-out>
#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

// ABI signatures (kept standalone on purpose: a real consumer only has the
// header and the DLL; this probe goes one further and does not even include
// the header, pinning the raw signatures the ABI doc promises).
typedef int32_t(*FnAbiVersion)(void);
typedef int32_t(*FnSetBasePath)(const char*);
typedef int32_t(*FnInitialize)(const char*, int32_t, int32_t*, char*, int32_t);
typedef int32_t(*FnGetSendableUpdate)(int32_t*, char*, int32_t);
typedef void(*FnVoidNoArg)(void);
typedef void(*FnVoidI32)(int32_t);
typedef void(*FnVoidI64)(int64_t);
typedef void(*FnVoidStr)(const char*);
typedef void(*FnUpdateResultFloat)(int32_t, float);
typedef void(*FnUpdateContext)(int32_t, const char*);
typedef void(*FnAddAttachment)(const char*, const char*, const char*, int32_t, int32_t, const char*, const char*);
typedef int32_t(*FnVerbUpdate)(int32_t);
typedef int32_t(*FnGetJson)(char*, int32_t);

static FARPROC Need(HMODULE mod, const char* name)
{
    FARPROC p = GetProcAddress(mod, name);
    if (p == nullptr)
    {
        std::fprintf(stderr, "missing export: %s\n", name);
        std::exit(3);
    }
    return p;
}

int main(int argc, char** argv)
{
    if (argc != 7)
    {
        std::fprintf(stderr, "usage: parity_lifecycle <dll-path> <scratch-base-path> <seed-file> <scratch-alt-path> <seed-alt-file> <android-extras-out>\n");
        return 2;
    }

    HMODULE mod = LoadLibraryA(argv[1]);
    if (mod == nullptr)
    {
        std::fprintf(stderr, "LoadLibrary failed for %s (error %lu)\n", argv[1], GetLastError());
        return 2;
    }

    auto abiVersion = (FnAbiVersion)Need(mod, "FebrisSimAbiVersion");
    auto setBasePath = (FnSetBasePath)Need(mod, "FebrisSimSetBasePath");
    auto initialize = (FnInitialize)Need(mod, "FebrisSimInitialize");
    auto getSendableUpdate = (FnGetSendableUpdate)Need(mod, "FebrisSimGetSendableUpdate");
    auto stageRestart = (FnVoidNoArg)Need(mod, "FebrisSimStageRestart");
    auto addResultNote = (FnVoidStr)Need(mod, "FebrisSimAddResultNote");
    auto updateResultFloat = (FnUpdateResultFloat)Need(mod, "FebrisSimUpdateResultFloat");
    auto updateContext = (FnUpdateContext)Need(mod, "FebrisSimUpdateContext");
    auto updateContextActivity = (FnUpdateContext)Need(mod, "FebrisSimUpdateContextActivity");
    auto addAttachment = (FnAddAttachment)Need(mod, "FebrisSimAddAttachment");
    auto durationUpdateMs = (FnVoidI64)Need(mod, "FebrisSimDurationUpdateMs");
    auto simulationPassed = (FnVoidI32)Need(mod, "FebrisSimSimulationPassed");
    auto verbUpdate = (FnVerbUpdate)Need(mod, "FebrisSimVerbUpdate");
    auto simulationComplete = (FnVoidNoArg)Need(mod, "FebrisSimSimulationComplete");
    auto getCurrentStatementJson = (FnGetJson)Need(mod, "FebrisSimGetCurrentStatementJson");
    typedef void(*FnUpdateResultBool)(int32_t, int32_t);
    typedef void(*FnUpdateResultString)(int32_t, const char*);
    typedef int32_t(*FnEndSimulationWith)(int32_t, int32_t, float, int64_t, int32_t*, char*, int32_t);
    auto getReferenceUuid = (FnGetJson)Need(mod, "FebrisSimGetReferenceUuid");
    auto updResultBool = (FnUpdateResultBool)Need(mod, "FebrisSimUpdateResultBool");
    auto updResultString = (FnUpdateResultString)Need(mod, "FebrisSimUpdateResultString");
    auto updContextSref = (FnUpdateContext)Need(mod, "FebrisSimUpdateContextStatementReference");
    auto updContextExt = (FnUpdateContext)Need(mod, "FebrisSimUpdateContextExtension");
    auto customVerbUpdate = (FnVoidStr)Need(mod, "FebrisSimCustomVerbUpdate");
    auto endSimulationWith = (FnEndSimulationWith)Need(mod, "FebrisSimEndSimulationWith");

    int32_t ver = abiVersion();
    if (ver != 1)
    {
        std::fprintf(stderr, "unexpected ABI version %d\n", ver);
        return 3;
    }

    if (setBasePath(argv[2]) != 0)
    {
        std::fprintf(stderr, "SetBasePath failed\n");
        return 4;
    }

    std::ifstream seedIn(argv[3], std::ios::binary);
    if (!seedIn)
    {
        std::fprintf(stderr, "cannot open seed: %s\n", argv[3]);
        return 2;
    }
    std::ostringstream seedBuf;
    seedBuf << seedIn.rdbuf();
    std::string seed = seedBuf.str();
    while (!seed.empty() && (seed.back() == '\n' || seed.back() == '\r'))
    {
        seed.pop_back();
    }

    // --- the canonical deterministic sequence (mirror of the C# test) ---
    int32_t ready = 0;
    if (initialize(seed.c_str(), /*WindowsPC*/ 0, &ready, nullptr, 0) < 0 || ready != 1)
    {
        std::fprintf(stderr, "Initialize not ready (ready=%d)\n", ready);
        return 5;
    }

    updateResultFloat(/*ScoreMax*/ 6, 100.0f);
    updateResultFloat(/*ScoreMin*/ 5, 0.0f);
    if (getSendableUpdate(&ready, nullptr, 0) < 0 || ready != 1) { std::fprintf(stderr, "update 1 not ready\n"); return 5; }

    stageRestart();
    stageRestart();
    stageRestart();
    if (getSendableUpdate(&ready, nullptr, 0) < 0 || ready != 1) { std::fprintf(stderr, "update 2 not ready\n"); return 5; }

    addResultNote("Checklist deviation noted.");
    addResultNote("Torque sequence repeated.");

    updateContext(/*Registration*/ 0, "5e6f7a8b-9c0d-4e5f-b1c2-3d4e5f6a7b8c");
    updateContext(/*Platform*/ 5, "Febris Simulation Runtime");
    updateContext(/*Language*/ 6, "en-US");
    updateContextActivity(/*Parent*/ 0, "https://febr.is/Curricula/parentActivity");

    addAttachment("https://febr.is/Attachment/VideoReview", "Video Review", "Session capture",
        /*video_mpeg*/ 12, 2048, "sha2placeholder", "https://example.com/clip.mp4");
    if (getSendableUpdate(&ready, nullptr, 0) < 0 || ready != 1) { std::fprintf(stderr, "update 3 not ready\n"); return 5; }

    updateResultFloat(/*ScoreRaw*/ 8, 87.0f);
    durationUpdateMs(754000);
    simulationPassed(1);
    // Upstream C# quirk, mirrored by the port: VerbUpdate's 'updated' local is
    // never assigned, so the call always answers 0 even on a real transition.
    if (verbUpdate(/*Pass*/ 4) != 0) { std::fprintf(stderr, "VerbUpdate(Pass) unexpected return\n"); return 5; }
    simulationComplete();

    // Debug aid: show the final working statement on stdout.
    int32_t needed = getCurrentStatementJson(nullptr, 0);
    if (needed > 0)
    {
        std::string out((size_t)needed, '\0');
        getCurrentStatementJson(&out[0], needed);
        out.resize((size_t)needed - 1);
        std::printf("%s\n", out.c_str());
    }

    // ============ phase 2: Android extras path ==========================
    // Mirror of the C# test's phase 2: same seed, Android handler, extras
    // carry ReferenceUUID + RawStatementJson. Written PRE-NORMALIZED (this
    // run's reference uuid replaced) for a direct byte-compare.
    {
        int32_t extrasNeeded = initialize(seed.c_str(), /*Android*/ 1, &ready, nullptr, 0);
        if (extrasNeeded < 0 || ready != 1)
        {
            std::fprintf(stderr, "Android Initialize not ready (ready=%d)\n", ready);
            return 6;
        }
        // second run with a sized buffer to capture the extras (idempotent on
        // the Android path: no disk, statics rewritten)
        std::string extras((size_t)extrasNeeded, '\0');
        if (initialize(seed.c_str(), 1, &ready, &extras[0], extrasNeeded) < 0 || ready != 1)
        {
            std::fprintf(stderr, "Android Initialize (sized) not ready\n");
            return 6;
        }
        std::string::size_type nul = extras.find('\0');
        if (nul != std::string::npos) { extras.resize(nul); }

        int32_t un = getReferenceUuid(nullptr, 0);
        std::string uuid((size_t)(un > 0 ? un : 1), '\0');
        getReferenceUuid(&uuid[0], un);
        nul = uuid.find('\0');
        if (nul != std::string::npos) { uuid.resize(nul); }

        std::string::size_type pos = 0;
        while (!uuid.empty() && (pos = extras.find(uuid, pos)) != std::string::npos)
        {
            extras.replace(pos, uuid.size(), "@REFERENCE@");
            pos += 11;
        }
        std::ofstream extrasOut(argv[6], std::ios::binary);
        extrasOut << extras;
    }

    // ============ phase 3: authored-result alt scenario =================
    // Mirror of the C# test's phase 3 (see CrossSdkLifecycleParityTests):
    // bool/string result updates, statement-reference + context-extension
    // writes, CustomVerbUpdate, EndSimulationWith with the scaled-score
    // auto-update, and the authored-result no-op mirrors (extensions is
    // JSON null, so StageRestart/AddResultNote must no-op on both sides).
    {
        if (setBasePath(argv[4]) != 0)
        {
            std::fprintf(stderr, "SetBasePath(alt) failed\n");
            return 4;
        }
        std::ifstream altIn(argv[5], std::ios::binary);
        if (!altIn)
        {
            std::fprintf(stderr, "cannot open alt seed: %s\n", argv[5]);
            return 2;
        }
        std::ostringstream altBuf;
        altBuf << altIn.rdbuf();
        std::string altSeed = altBuf.str();
        while (!altSeed.empty() && (altSeed.back() == '\n' || altSeed.back() == '\r'))
        {
            altSeed.pop_back();
        }

        if (initialize(altSeed.c_str(), /*WindowsPC*/ 0, &ready, nullptr, 0) < 0 || ready != 1)
        {
            std::fprintf(stderr, "alt Initialize not ready (ready=%d)\n", ready);
            return 7;
        }
        updResultBool(/*Success*/ 0, 1);
        updResultString(/*Response*/ 2, "Corrected response");
        stageRestart();                        // no-op: extensions is JSON null
        addResultNote("Should not land.");     // no-op: same
        updContextSref(/*Id*/ 0, "9c0d1e2f-3a4b-4c9d-b5e6-7b8c9d0e1f2a");
        updContextSref(/*ObjectType*/ 1, "StatementRef");
        updContextExt(/*Option1*/ 0, "ctx-extension-value");
        customVerbUpdate("https://febr.is/Verb/Details/Inspected");
        // Emit while the custom verb is live (EndSimulation cannot run on a
        // custom IRI: GetVerbEnum throws on anything outside the enum map, an
        // upstream C# gap both SDKs mirror).
        if (getSendableUpdate(&ready, nullptr, 0) < 0 || ready != 1)
        {
            std::fprintf(stderr, "alt update not ready\n");
            return 7;
        }
        // Restore a resolvable verb, then the 4-arg EndSimulation finale.
        verbUpdate(/*Not_Pass*/ 5);
        if (endSimulationWith(1, 1, 41.7f, 90500, &ready, nullptr, 0) < 0 || ready != 1)
        {
            std::fprintf(stderr, "EndSimulationWith not ready (ready=%d)\n", ready);
            return 7;
        }
    }

    FreeLibrary(mod);
    return 0;
}
