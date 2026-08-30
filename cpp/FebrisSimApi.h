// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#ifndef FEBRIS_SIM_API_H
#define FEBRIS_SIM_API_H

/*
 * Febris Simulation Library -- public C ABI (EXPORT-SURFACE, 2026-08-29).
 *
 * This header is the ONLY supported binary interface of
 * Febris.CppSimulationLibrary.dll. It is a flat extern "C" surface on purpose:
 *  - stable across compilers and CRTs (no MSVC name mangling, no std types),
 *  - P/Invoke-able from C# hosts ([DllImport("Febris.CppSimulationLibrary")]),
 *  - additive-friendly (docs/OSS_SDK_DISTRIBUTION.md: the C++ ABI evolves
 *    additively in MINOR versions; existing signatures never change shape).
 * The C++ classes behind it (Initializer, StatementHandler, ...) are NOT
 * exported; source-level consumers may still compile the SDK sources directly
 * (define FEBRISSIM_STATIC so this header stops importing).
 *
 * This surface mirrors the C# public API (Febris.CsharpSimulationLibraryNetStandard):
 * Initializer.Initialize + the StatementHandler entry points, with C# overloads
 * flattened into distinct names and enums passed as their C# integer values.
 * The C# async Task<(bool, string[,])> results are synchronous here; the
 * string[,] extras array flattens to a JSON object string ("null" when the C#
 * side would return a null array, as WinPC does).
 *
 * CONVENTIONS
 *  - Status: functions returning int32_t status yield FEBRISSIM_OK (0) or a
 *    negative FEBRISSIM_E_* code. The library never lets a C++ exception cross
 *    this boundary.
 *  - String outputs: functions taking (char* buf, int32_t cap) return the
 *    REQUIRED byte count INCLUDING the NUL terminator (>= 1), or a negative
 *    error. When cap > 0, up to cap-1 bytes are written and NUL-terminated.
 *    Call with (NULL, 0) to size a buffer. Text is UTF-8.
 *  - Threading: the library is single-threaded by design, exactly like the C#
 *    SDK (no internal synchronization). Drive it from one thread.
 *  - Update functions returning void mirror C# void methods, which swallow
 *    and log failures; register a logger via FebrisSimSetLogger to see them.
 */

#include <stdint.h>

#if defined(FEBRISSIM_STATIC)
#define FEBRISSIM_API
#elif defined(FEBRISCPPSIMULATIONLIBRARY_EXPORTS)
#define FEBRISSIM_API __declspec(dllexport)
#else
#define FEBRISSIM_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/* Version and status codes                                            */
/* ------------------------------------------------------------------ */

/* Bumped only when an EXISTING function changes shape or meaning
 * (a MAJOR event per the additive-ABI rule). New functions do not bump it. */
#define FEBRISSIM_ABI_VERSION 1

#define FEBRISSIM_OK                 0
#define FEBRISSIM_E_INVALID_ARG     (-1)   /* null/malformed required argument */
#define FEBRISSIM_E_EXCEPTION       (-2)   /* an internal failure was caught at the boundary */
#define FEBRISSIM_E_NOT_INITIALIZED (-3)   /* no platform handler: call FebrisSimInitialize first */

/* ------------------------------------------------------------------ */
/* Enum values (the C# enum integer values, verbatim)                  */
/* ------------------------------------------------------------------ */

/* Enums.ExpectedOperatingSystem */
enum FebrisSimOs
{
	FebrisSimOs_WindowsPC = 0,
	FebrisSimOs_Android = 1,
	FebrisSimOs_iOSvariant = 2,
	FebrisSimOs_WinMobile = 3
};

/* Service.SimulationLogLevel */
enum FebrisSimLogLevel
{
	FebrisSimLog_Debug = 0,
	FebrisSimLog_Info = 1,
	FebrisSimLog_Warn = 2,
	FebrisSimLog_Error = 3
};

/* Enums.ResultOptions */
enum FebrisSimResultOption
{
	FebrisSimResult_Success = 0,
	FebrisSimResult_Completion = 1,
	FebrisSimResult_Response = 2,
	FebrisSimResult_Duration = 3,
	FebrisSimResult_Extensions = 4,
	FebrisSimResult_ScoreMin = 5,
	FebrisSimResult_ScoreMax = 6,
	FebrisSimResult_ScoreScale = 7,
	FebrisSimResult_ScoreRaw = 8
};

/* Enums.ResultExtensionOptions */
enum FebrisSimResultExtensionOption
{
	FebrisSimResultExt_Notes = 0,
	FebrisSimResultExt_RestartCounter = 1
};

/* Enums.ContextOptions ("ContextActivites" spelling is canonical C# API) */
enum FebrisSimContextOption
{
	FebrisSimContext_Registration = 0,
	FebrisSimContext_Instructor = 1,
	FebrisSimContext_Group = 2,
	FebrisSimContext_ContextActivites = 3,
	FebrisSimContext_Revision = 4,
	FebrisSimContext_Platform = 5,
	FebrisSimContext_Language = 6,
	FebrisSimContext_StatementReference = 7,
	FebrisSimContext_Extensions = 8
};

/* Enums.ContextActivitesOptions */
enum FebrisSimContextActivityOption
{
	FebrisSimContextActivity_Parent = 0,
	FebrisSimContextActivity_Grouping = 1,
	FebrisSimContextActivity_Category = 2,
	FebrisSimContextActivity_Other = 3
};

/* Enums.ContextStatementReferenceOptions */
enum FebrisSimContextStatementReferenceOption
{
	FebrisSimContextStatementReference_Id = 0,
	FebrisSimContextStatementReference_ObjectType = 1
};

/* Enums.ContextExtensionOptions */
enum FebrisSimContextExtensionOption
{
	FebrisSimContextExtension_Option1 = 0,
	FebrisSimContextExtension_Option2 = 1
};

/* Enums.VerbEnums */
enum FebrisSimVerb
{
	FebrisSimVerb_Attempted = 0,
	FebrisSimVerb_Completed = 1,
	FebrisSimVerb_Initialized = 2,
	FebrisSimVerb_Terminated = 3,
	FebrisSimVerb_Pass = 4,
	FebrisSimVerb_Not_Pass = 5
};

/* Enums.ContentType (43 members, exact C# order and values) */
enum FebrisSimContentType
{
	FebrisSimContent_application_octet_stream = 0,
	FebrisSimContent_application_x_abiword = 1,
	FebrisSimContent_application_x_freearc = 2,
	FebrisSimContent_application_x_bzip = 3,
	FebrisSimContent_application_x_bzip2 = 4,
	FebrisSimContent_application_ogg = 5,
	FebrisSimContent_audio_aac = 6,
	FebrisSimContent_audio_mpeg = 7,
	FebrisSimContent_audio_ogg = 8,
	FebrisSimContent_audio_wav = 9,
	FebrisSimContent_audio_webm = 10,
	FebrisSimContent_video_x_msvideo = 11,
	FebrisSimContent_video_mpeg = 12,
	FebrisSimContent_video_ogg = 13,
	FebrisSimContent_video_mp2t = 14,
	FebrisSimContent_video_webm = 15,
	FebrisSimContent_application_epub_zip = 16,
	FebrisSimContent_application_gzip = 17,
	FebrisSimContent_application_vnd_rar = 18,
	FebrisSimContent_application_x_tar = 19,
	FebrisSimContent_application_zip = 20,
	FebrisSimContent_application_x_7z_compressed = 21,
	FebrisSimContent_image_bmp = 22,
	FebrisSimContent_image_gif = 23,
	FebrisSimContent_image_jpeg = 24,
	FebrisSimContent_image_png = 25,
	FebrisSimContent_image_svg_xml = 26,
	FebrisSimContent_image_tiff = 27,
	FebrisSimContent_image_webp = 28,
	FebrisSimContent_application_x_csh = 29,
	FebrisSimContent_application_msword = 30,
	FebrisSimContent_application_vnd_openxmlformats_officedocument_wordprocessingml_document = 31,
	FebrisSimContent_text_html = 32,
	FebrisSimContent_text_javascript = 33,
	FebrisSimContent_application_json = 34,
	FebrisSimContent_application_ld_json = 35,
	FebrisSimContent_application_vnd_ms_powerpoint = 36,
	FebrisSimContent_application_vnd_openxmlformats_officedocument_presentationml_presentation = 37,
	FebrisSimContent_application_xhtml_xml = 38,
	FebrisSimContent_application_vnd_openxmlformats_officedocument_spreadsheetml_sheet = 39,
	FebrisSimContent_text_csv = 40,
	FebrisSimContent_audio_basic = 41,
	FebrisSimContent_application_pdf = 42
};

/* ------------------------------------------------------------------ */
/* Introspection, logging, configuration                               */
/* ------------------------------------------------------------------ */

/* Returns FEBRISSIM_ABI_VERSION of the loaded library. Call first and check
 * compatibility before anything else. */
FEBRISSIM_API int32_t FebrisSimAbiVersion(void);

/* SIM-T13 G4 mirror: route library diagnostics into the host. level is a
 * FebrisSimLogLevel value; detail may be empty. The callback must not throw
 * and must not call back into the library. NULL resets to the built-in
 * console sink ("[FebrisSim][Level] message"). */
typedef void (*FebrisSimLogCallback)(int32_t level, const char* message, const char* detail);
FEBRISSIM_API void FebrisSimSetLogger(FebrisSimLogCallback callback);

/* Mirror of the C# public mutable FileSystem.BasePath: points the statement
 * handoff tree somewhere other than Documents\Febris. Recomputes every
 * dependent path. Call BEFORE FebrisSimInitialize. */
FEBRISSIM_API int32_t FebrisSimSetBasePath(const char* basePath);

/* Mirror of StaticDetails.ReferenceUUID (set/get). The reference UUID names
 * the handoff file ({uuid}.json) and is stamped into the outgoing statement
 * id (SDKV-19/20), so hosts can use it to correlate runs. Setting it is
 * optional; a random UUID is generated otherwise. Get uses the buffer
 * convention. */
FEBRISSIM_API int32_t FebrisSimSetReferenceUuid(const char* uuid);
FEBRISSIM_API int32_t FebrisSimGetReferenceUuid(char* buf, int32_t cap);

/* ------------------------------------------------------------------ */
/* Lifecycle                                                           */
/* ------------------------------------------------------------------ */

/* Initializer.Initialize(string[], ExpectedOperatingSystem) mirror.
 * febrisData is ONE argument string: either the raw authored statement JSON
 * or the launcher form "-febrisData={...}" (the prefix is added when absent).
 * expectedOs is a FebrisSimOs value. *ready receives 1 when the platform
 * handler accepted and persisted the initial statement, else 0 (mirrors the
 * C# isInitialized flag). extrasJson receives the flattened extras object
 * ("null" on WindowsPC); buffer convention applies to the return value. */
FEBRISSIM_API int32_t FebrisSimInitialize(
	const char* febrisData, int32_t expectedOs,
	int32_t* ready, char* extrasJson, int32_t extrasCap);

/* Faithful argv form of the same call (C# takes the full args array and
 * scans for the -febrisData= prefix itself). */
FEBRISSIM_API int32_t FebrisSimInitializeArgv(
	int32_t argc, const char* const* argv, int32_t expectedOs,
	int32_t* ready, char* extrasJson, int32_t extrasCap);

/* StatementHandler.GetSendableUpdate mirror: hands the current statement to
 * the platform handler (on WindowsPC: rewrites the {uuid}.json handoff file).
 * *ready receives the handler flag; extras flatten as in Initialize. */
FEBRISSIM_API int32_t FebrisSimGetSendableUpdate(
	int32_t* ready, char* extrasJson, int32_t extrasCap);

/* StatementHandler.GetSendableDispatch mirror, flattened to one JSON object:
 * {"ready":bool,"intentAction":"com.febris.STATEMENT_UPDATE","extras":{...}|null}
 * Buffer convention on the return value. */
FEBRISSIM_API int32_t FebrisSimGetSendableDispatchJson(char* buf, int32_t cap);

/* StatementHandler.EndSimulation() mirror: re-resolves the current verb,
 * reapplies it, then emits (GetSendableUpdate). */
FEBRISSIM_API int32_t FebrisSimEndSimulation(
	int32_t* ready, char* extrasJson, int32_t extrasCap);

/* StatementHandler.EndSimulation(bool, bool, float, TimeSpan) mirror.
 * durationMs carries the TimeSpan as milliseconds. */
FEBRISSIM_API int32_t FebrisSimEndSimulationWith(
	int32_t success, int32_t complete, float rawScore, int64_t durationMs,
	int32_t* ready, char* extrasJson, int32_t extrasCap);

/* StatementHandler.SimulationComplete / SimulationPassed mirrors. */
FEBRISSIM_API void FebrisSimSimulationComplete(void);
FEBRISSIM_API void FebrisSimSimulationPassed(int32_t passed);

/* StatementHandler.DurationUpdate(TimeSpan) mirror. */
FEBRISSIM_API void FebrisSimDurationUpdateMs(int64_t durationMs);

/* StatementHandler.StageRestart / AddResultNote mirrors. */
FEBRISSIM_API void FebrisSimStageRestart(void);
FEBRISSIM_API void FebrisSimAddResultNote(const char* note);

/* ------------------------------------------------------------------ */
/* UpdateStatement family (C# overloads flattened to distinct names;    */
/* the XAPIProperties routing argument is implied by each name)         */
/* ------------------------------------------------------------------ */

/* UpdateStatement(Result, ResultOptions, bool | string | float | TimeSpan) */
FEBRISSIM_API void FebrisSimUpdateResultBool(int32_t resultOption, int32_t value);
FEBRISSIM_API void FebrisSimUpdateResultString(int32_t resultOption, const char* value);
FEBRISSIM_API void FebrisSimUpdateResultFloat(int32_t resultOption, float value);
FEBRISSIM_API void FebrisSimUpdateResultDurationMs(int64_t durationMs);

/* UpdateStatement(Result, Extensions, ResultExtensionOptions, int | string) */
FEBRISSIM_API void FebrisSimUpdateResultExtensionInt(int32_t resultExtensionOption, int32_t value);
FEBRISSIM_API void FebrisSimUpdateResultExtensionString(int32_t resultExtensionOption, const char* value);

/* UpdateStatement(Attachments, ...) -- the four C# attachment overloads in
 * one function: pass NULL for description and/or fileUrl to select the
 * corresponding shorter overload's behavior. contentType is a
 * FebrisSimContentType value; length is the attachment byte length. */
FEBRISSIM_API void FebrisSimAddAttachment(
	const char* usageType, const char* display, const char* descriptionOrNull,
	int32_t contentType, int32_t length, const char* sha2, const char* fileUrlOrNull);

/* UpdateStatement(Context, ...) family. */
FEBRISSIM_API void FebrisSimUpdateContext(int32_t contextOption, const char* value);
FEBRISSIM_API void FebrisSimUpdateContextActivity(int32_t contextActivityOption, const char* value);
FEBRISSIM_API void FebrisSimUpdateContextStatementReference(int32_t referenceOption, const char* value);
FEBRISSIM_API void FebrisSimUpdateContextExtension(int32_t contextExtensionOption, const char* value);

/* ------------------------------------------------------------------ */
/* Verbs                                                               */
/* ------------------------------------------------------------------ */

/* StatementHandler.VerbUpdate(VerbEnums) mirror. Returns 1 on success,
 * 0 on the C# false path, negative on boundary errors. */
FEBRISSIM_API int32_t FebrisSimVerbUpdate(int32_t verbEnum);

/* StatementHandler.CustomVerbUpdate mirror: caller-supplied verb IRI. */
FEBRISSIM_API void FebrisSimCustomVerbUpdate(const char* verbIri);

/* ------------------------------------------------------------------ */
/* Introspection of the working statement                              */
/* ------------------------------------------------------------------ */

/* Returns the current working statement as compact JSON (the same text the
 * Android path exposes through the RawStatementJson intent extra). Buffer
 * convention. FEBRISSIM_E_NOT_INITIALIZED before Initialize. */
FEBRISSIM_API int32_t FebrisSimGetCurrentStatementJson(char* buf, int32_t cap);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FEBRIS_SIM_API_H */
