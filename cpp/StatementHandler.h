// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef STATEMENTHANDLER_H
#define STATEMENTHANDLER_H
//#include "pch.h";
#endif

// EXPORT-SURFACE PORT (2026-08-29): rewritten against the CURRENT C#
// Statement\StatementHandler.cs (the Android-11 refactor + SDKV/SIM-T13 fix
// series). The previous header declared methods that were never defined at
// global scope -- the .cpp defined a DIFFERENT same-named class inside the dead
// namespace FebrisCppStatement (pre-refactor disk-backed drift: GetJObject /
// SaveStatement / WriteToDataFile). That state model is gone on the C# side;
// the working statement lives in memory as StaticDetails::CurrentStatement.
//
// In Android 11 refactor the JObject statement = JSONHandler.GetJObject(); was
// removed and was changed to a static JObject called
// JObject statement = StaticDetails.StaticDetails.CurrentStatement;
//
// REFERENCE SEMANTICS: C# JObject is a reference type -- every
// "JObject statement = CurrentStatement; statement[...] = x;" site mutates the
// SHARED object. The C++ mirrors bind XApiJson::ojson& (never a copy), and every
// helper that mutates the working statement takes XApiJson::ojson&.

#ifndef SIMULATIONDISPATCH_H
#define SIMULATIONDISPATCH_H
//#include "pch.h";
#endif

/// <summary>
/// SIM-T13 G3 mirror of Service\SimulationDispatch.cs: typed result shape for
/// outbound simulation events, replacing the opaque (bool, string[,]) tuple so
/// the host (Unity glue, Unreal glue, future engine integrations) does not have
/// to remember the intent action by which library method was called. For Android
/// IntentAction is one of "com.febris.STATEMENT_CREATE" / "_UPDATE" / "_ERROR";
/// the host fires it mechanically.
/// NOTE: the C# ctor normalizes a null extras dictionary to an empty one, so a
/// C# dispatch never carries null extras. The C-ABI glue still needs the legacy
/// null-vs-empty distinction of the underlying (bool, string[,]) tuple (see
/// HandlerResult in IEnvironmentHandler.h), so the mirror keeps it in HasExtras.
/// </summary>
struct SIMULATIONDISPATCH_H SimulationDispatch
{
	bool Ready = false;
	std::string IntentAction;
	bool HasExtras = false;
	std::vector<std::pair<std::string, std::string>> Extras;
};

class STATEMENTHANDLER_H StatementHandler {
public:
	///Methods Developers can use to update Statements and trigger different actions
	// #region External
	// #region Returns values for use

	/// <summary>
	/// SIM-T13 G3 dispatch overload -- same as GetSendableUpdate but returns a
	/// typed SimulationDispatch carrying IntentAction = "com.febris.STATEMENT_UPDATE".
	/// </summary>
	static SimulationDispatch GetSendableDispatch();

	/// <summary>
	/// This needs to be used on update loops. This will cause the system to either
	/// write to a shared file system or creates a way of posting to the controlling
	/// application. (C# Task&lt;(bool, string[,])&gt; flattens to the synchronous
	/// HandlerResult.)
	/// </summary>
	static HandlerResult GetSendableUpdate();

	static HandlerResult EndSimulation();
	static HandlerResult EndSimulation(bool success, bool complete, float rawScore, long long durationMs);
	// #endregion

	// #region One way calls
	// #region Specific calls

	/// <summary>
	/// this method is used to finish the simulation and finalize the JSON Statement.
	/// FIX (SDKV-3/11): result.completion is written as a JSON Boolean --
	/// the old lowercase-string "true" violated xAPI 1.0.3 section 4.1.5.
	/// </summary>
	static void SimulationComplete();

	/// <summary>
	/// Marks the simulation's result.success flag.
	/// FIX (SDKV-3/11): written as a JSON Boolean.
	/// </summary>
	static void SimulationPassed(bool input);

	/// <summary>Updates the duration of the simulation (C# TimeSpan -> milliseconds).</summary>
	static void DurationUpdate(long long input);

	/// <summary>Every time a stage is restarted this function should be called.</summary>
	static void StageRestart();

	/// <summary>This is the simple way to add notes.</summary>
	static void AddResultNote(string note);
	// #endregion

	// #region Direct Statement Updates (the 14 public UpdateStatement overloads)
	static void UpdateStatement(XAPIProperties property, ResultOptions resultType, bool input);
	static void UpdateStatement(XAPIProperties property, ResultOptions resultType, string input);
	static void UpdateStatement(XAPIProperties property, ResultOptions resultType, float input);
	static void UpdateStatement(XAPIProperties property, ResultOptions resultType, long long durationMs);  // C# TimeSpan overload
	static void UpdateStatement(XAPIProperties property, ResultOptions resultType, ResultExtensionOptions resultExtensionType, int input);
	static void UpdateStatement(XAPIProperties property, ResultOptions resultType, ResultExtensionOptions resultExtensionType, string input);
	// #endregion
	// #region attachment handling interface
	/// Required: UsageType, Display, contenttype, length, sha2
	/// optional: fileUrl, description
	static void UpdateStatement(XAPIProperties property, string usageType, string display, string description, ContentType contentType, int length, string sha2, string fileUrl);
	static void UpdateStatement(XAPIProperties property, string usageType, string display, string description, ContentType contentType, int length, string sha2);
	static void UpdateStatement(XAPIProperties property, string usageType, string display, ContentType contentType, int length, string sha2, string fileUrl);
	static void UpdateStatement(XAPIProperties property, string usageType, string display, ContentType contentType, int length, string sha2);
	// #endregion
	// #region context interface
	static void UpdateStatement(XAPIProperties property, ContextOptions contextOption, string input);
	static void UpdateStatement(XAPIProperties property, ContextOptions contextOption, ContextActivitesOptions contextActivitesOption, string input);
	static void UpdateStatement(XAPIProperties property, ContextOptions contextOption, ContextStatementReferenceOptions contextStatementReferenceOption, string input);
	static void UpdateStatement(XAPIProperties property, ContextOptions contextOption, ContextExtensionOptions contextExtensionOption, string input);
	// #endregion

	// #region Custom Verb Methods
	///Use a custom verb that was previously setup in the portal. This is not selectable in the Febris Enum stock selections.
	/// It is utilized through the URI generated on creation.
	/// Please use that Uri string if you would like to use a custom Verb
	static void CustomVerbUpdate(string uriInput);
	// #endregion

	/// <summary>
	/// Update your verb manually using the VerbEnums.
	/// Removed the auto calculation feature for more transparancy in Verb setting.
	/// FIX (SDKV-3): the completion read goes through TokenToBool since
	/// result.completion is now a genuine JSON Boolean.
	/// (C# parity: always returns false -- the local is never set true.)
	/// </summary>
	static bool VerbUpdate(VerbEnums verbEnum);
	// #endregion

	///Internal Methods that should not be able to be utilized directly.
	///(C# marks these internal; C++ has no assembly boundary, so they stay public
	/// for the sibling TUs -- Initializer and the environment handlers -- that call them.)
	// #region Internal
	static bool StatementCheck(const XApiJson::ojson& input);

	/// <summary>
	/// FIX (SDKV-19/20): stamps the handoff ReferenceUUID into the statement's wire
	/// id so every emitted statement carries an xAPI 1.0.3 section 4.1.1 statement id.
	/// Idempotent: an id that is already a valid, non-empty GUID is left untouched;
	/// placeholders (missing, null, empty, the model default "0", the empty GUID, or
	/// any non-GUID junk) are replaced with StaticDetails::GetReferenceUUID() -- the
	/// same GUID used for the {uuid}.json handoff FILENAME on the PC path and the
	/// ReferenceUUID intent extra on Android, so the handoff key and the wire id
	/// always agree.
	/// (C# is null-tolerant and returns the instance for chaining; ojson is a value
	/// type here, so the C++ mirror mutates in place and the null branch is dropped.)
	/// </summary>
	static void StampStatementId(XApiJson::ojson& statement);

	// #region validation family (C# JToken params -> const ojson& subtrees)
	static bool ActorIsCorrect(const XApiJson::ojson& input);
	static bool MemberIsCorrect(const XApiJson::ojson& input);
	static bool AccountIsCorrect(const XApiJson::ojson& input);
	static bool VerbIsCorrect(const XApiJson::ojson& input);
	static bool ObjectIsCorrect(const XApiJson::ojson& input);
	static bool ObjectDefinitionIsCorrect(const XApiJson::ojson& input);

	/// <summary>
	/// FIX (SDKV-3): tolerant boolean read for statement tokens.
	/// result.success/completion are genuine JSON Booleans now, but statements
	/// written by older SDK builds may still carry the lowercase strings
	/// "true"/"false" -- both parse; anything else (null, missing, junk) reads false.
	/// </summary>
	static bool TokenToBool(const XApiJson::ojson& token);
	// #endregion

	/// <summary>
	/// Auto-calculating verb update used by EndSimulation.
	/// FIX (SDKV-3): completion/success reads go through TokenToBool.
	/// Takes and returns the SAME instance by reference (C# JObject semantics).
	/// </summary>
	static XApiJson::ojson& VerbUpdate(XApiJson::ojson& statement, VerbEnums verbEnum);

	// #region Result routing
	static void Result(string input, ResultOptions resultType);
	static void Result(float input, ResultOptions resultType);
	static void Result(bool input, ResultOptions resultType);
	static void Result(long long durationMs, ResultOptions resultType);  // C# TimeSpan overload
	static void Result(int input, ResultOptions resultType, ResultExtensionOptions resultExtensionOptions);
	static void Result(string input, ResultOptions resultType, ResultExtensionOptions resultExtensionOptions);
	// #endregion

	// #region update result
	static void UpdateDuration(Stopwatch elapsedTime);
	static void UpdateDuration(long long elapsedTimeMs);  // C# TimeSpan overload
	static XApiJson::ojson& UpdateDuration(XApiJson::ojson& statement, long long elapsedTimeMs);
	static void UpdateRawScore(float score);
	static XApiJson::ojson& UpdateRawScore(XApiJson::ojson& statement, float score);
	static void UpdateScaleScore(float score);
	static XApiJson::ojson& UpdateScaleScore(XApiJson::ojson& statement, float score);
	static XApiJson::ojson& AutoUpdateScaledScore(XApiJson::ojson& statement, float rawScore);
	static void UpdateMinScore(float score);
	static XApiJson::ojson& UpdateMinScore(XApiJson::ojson& statement, float score);
	static void UpdateMaxScore(float score);
	static XApiJson::ojson& UpdateMaxScore(XApiJson::ojson& statement, float score);
	static void UpdateCompletionStatus(bool status);
	static XApiJson::ojson& UpdateCompletionStatus(XApiJson::ojson& statement, bool status);
	static void UpdateSuccessStatus(bool status);
	static XApiJson::ojson& UpdateSuccessStatus(XApiJson::ojson& statement, bool status);
	static void UpdateResponseStatus(string input);
	static void UpdateResultExtensions(string key, string input);
	// #endregion

	// #region Context routing
	static void Context(ContextOptions contextOption, string input);
	static void Context(ContextOptions contextOption, ContextActivitesOptions contextActivitesOption, string input);
	static void ContextActivity(ContextActivitesOptions option, string input);
	static void Context(ContextOptions contextOption, ContextStatementReferenceOptions contextStatementReferenceOption, string input);
	static void ContextStatementReferenceOption(ContextStatementReferenceOptions option, string input);
	static void Context(ContextOptions contextOption, ContextExtensionOptions contextExtensionOption, string input);
	static void ContextExtensionOption(ContextExtensionOptions option, string input);
	// #endregion

	// #region Attachments
	static void AddAttachment(string usageType, string display, string description, ContentType contentType, int length, string sha2, string fileUrl);
	static void AddAttachment(string usageType, string display, string description, ContentType contentType, int length, string sha2);
	static void AddAttachment(string usageType, string display, ContentType contentType, int length, string sha2, string fileUrl);
	static void AddAttachment(string usageType, string display, ContentType contentType, int length, string sha2);
	// #endregion
	// #endregion

private:
	// C# private members
	// #region Update Context (private per-field updaters)
	static void UpdatecontextRegistration(string input);
	static void UpdateContextActivityParent(string input);
	static void UpdateContextActivityGrouping(string input);
	static void UpdateContextActivityCategory(string input);
	static void UpdateContextActivityOther(string input);
	static void UpdateContextInstructor(string input);
	static void UpdateContextGroup(string input);
	static void UpdateContextRevision(string input);
	static void UpdateContextPlatform(string input);
	static void UpdateContextLanguage(string input);
	static void UpdateContextStatementReferenceId(string input);
	static void UpdateContextStatementReferenceObjectType(string input);
	static void UpdateContextExtensions(string key, string input);
	// #endregion
	static void ProcessAttachmentJArray(XApiJson::ojson& statement, const Attachment& attachment);
};
