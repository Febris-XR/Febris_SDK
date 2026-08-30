// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
// Windows shell known-folder API: TU-local platform includes live below pch.h in
// the TU that needs them (pch.h stays platform-lean by rule).
// pch.h's global 'using namespace std' makes std::byte ambiguous with the SDK's
// global 'byte' typedef (C2872) once shlobj.h drags in the RPC headers. Renaming
// the SDK's 'byte' for the duration of the include sidesteps it: the SDK uses
// the typedef consistently under the new name, and std::byte is untouched after.
#pragma push_macro("byte")
#define byte win_sdk_byte
#include <shlobj.h>
#pragma pop_macro("byte")
#include <cstdlib>

// EXPORT-SURFACE PORT (2026-08-29): real out-of-class storage definitions for
// the FileSystem statics declared in FileSystem.h. The previous TU held a dead
// namespaced duplicate (namespace FebrisFileSystem) that shadowed the global
// class and left the real one with no storage at all, so nothing referencing
// FileSystem:: could ever have linked. Semantics mirror the CURRENT C#
// FileSystem\FileSystem.cs.

//File structure
//########################################################################
//Base febrisfile
//-Media
//--Video
//---splitvideo
//---recordings
//---ZippedRecordings
//-Modules
//  - links
//  - module files
//-Statements
//Logs
//  - Uploader
//  - Launcher
//########################################################################

// C# Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments) mirror.
// SHGetKnownFolderPath(FOLDERID_Documents) is the same known folder through the
// modern shell API; if the shell call fails, fall back to USERPROFILE\Documents
// (what the pre-refactor C++ used unconditionally).
static std::string GetMyDocumentsPath()
{
	PWSTR widePath = nullptr;
	HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &widePath);
	if (SUCCEEDED(hr) && widePath != nullptr)
	{
		std::wstring wide(widePath);
		CoTaskMemFree(widePath);
		return fs::path(wide).string();
	}
	// Shell docs: the out pointer must be freed whether the call succeeded or not.
	if (widePath != nullptr)
	{
		CoTaskMemFree(widePath);
	}
	char* userProfile = nullptr;
	size_t requiredSize = 0;
	if (_dupenv_s(&userProfile, &requiredSize, "USERPROFILE") == 0 && userProfile != nullptr)
	{
		std::string result = (fs::path(userProfile) / "Documents").string();
		free(userProfile);
		return result;
	}
	return std::string();
}

// Same-TU ordered initialization: C++ guarantees namespace-scope statics inside
// ONE translation unit initialize top-to-bottom, so BasePath/ExternalBasePath
// are fully constructed before every derived path below reads them. That
// ordering IS the correctness guarantee here (it mirrors the C# static field
// initializer order, which C# also runs top-to-bottom) -- keep these in
// dependency order and never split them across TUs.
//local folder paths
std::string FileSystem::BasePath = (fs::path(GetMyDocumentsPath()) / "Febris").string();
std::string FileSystem::ExternalBasePath = (fs::path(GetMyDocumentsPath()) / "Febris").string();
// [Historical - Android build once pointed ExternalBasePath at external storage "com.febris.files".]
//media
std::string FileSystem::MediaPath = (fs::path(FileSystem::ExternalBasePath) / "Media").string();
//inside Media directory: video path
std::string FileSystem::VideoPath = (fs::path(FileSystem::MediaPath) / "Videos").string();
//inside video directory: split file path
std::string FileSystem::SplitFilePath = (fs::path(FileSystem::VideoPath) / "SplitVideos").string();
//recording path
std::string FileSystem::RecordingsFilePath = (fs::path(FileSystem::VideoPath) / "Recordings").string();
//recording path
std::string FileSystem::TempRecordingsFilePath = (fs::path(FileSystem::VideoPath) / "TempRecording").string();
//zipped video path
std::string FileSystem::zipFolderPath = (fs::path(FileSystem::VideoPath) / "ZippedRecordings").string();
//Modules
std::string FileSystem::BaseModulePath = (fs::path(FileSystem::ExternalBasePath) / "Modules").string();
//inside module directory
std::string FileSystem::ModuleLinkPath = (fs::path(FileSystem::BaseModulePath) / "ModuleLinks").string();
std::string FileSystem::ModulePath = (fs::path(FileSystem::BaseModulePath) / "Modules").string();
std::string FileSystem::ZippedModulePath = (fs::path(FileSystem::BaseModulePath) / "ZippedModuleFiles").string();
// [Historical - internal simulationName / ModuleLinkLaunch statics stayed commented out in C#.]
//Statements directory
std::string FileSystem::BaseStatementPath = (fs::path(FileSystem::ExternalBasePath) / "statements").string();
//inside statement directory
std::string FileSystem::StatementPath = (fs::path(FileSystem::BaseStatementPath) / "statements").string();
std::string FileSystem::WorkingStatementPath = (fs::path(FileSystem::BaseStatementPath) / "workingstatement").string();
std::string FileSystem::OldStatementPath = (fs::path(FileSystem::BaseStatementPath) / "oldstatements").string();
//Logs
std::string FileSystem::BaseLogPath = (fs::path(FileSystem::BasePath) / "Logs").string();
//inside Log directory
std::string FileSystem::UploaderLogPath = (fs::path(FileSystem::BaseLogPath) / "UploaderLogs").string();
std::string FileSystem::LauncherLogPath = (fs::path(FileSystem::BaseLogPath) / "LauncherLogs").string();
std::string FileSystem::RecorderLogPath = (fs::path(FileSystem::BaseLogPath) / "RecorderLogs").string();
std::string FileSystem::ModuleManagerLogPath = (fs::path(FileSystem::BaseLogPath) / "ModuleManagerLogs").string();
std::string FileSystem::SimulationLogBasePath = (fs::path(FileSystem::BaseLogPath) / "SimulationLogs").string();
//Credentials
std::string FileSystem::sLocation = (fs::path(FileSystem::BasePath) / "cred").string();
// [Historical - credential file names, dll-assembly notes, and .exe path statics
//  stayed commented out in the C# source; not ported.]

// C++-only host-boot hook: the CURRENT C# SDK has NO SetFileSystemBasePath (its
// paths are computed once by the field initializers; grep verified 2026-08-29).
// The host (Initializer) calls this to recompute the whole tree at a known point
// after process start; the body replays the field-initializer chain above in
// exactly the same dependency order.
void FileSystem::SetFileSystemBasePath()
{
	// Recompute every DERIVED path from the CURRENT BasePath/ExternalBasePath.
	// This function has no C# counterpart (the C# fields are only ever set by
	// their initializers or assigned individually by the host); its C++ role
	// is the redirection hook behind FebrisSimSetBasePath, so it must respect
	// a caller-assigned BasePath rather than resetting to Documents\Febris --
	// the Documents default is the field initializers' job. (First version of
	// this port reset BasePath here, which silently un-did the redirect and
	// sent probe statements into the real Documents tree.)
	//Media
	FileSystem::MediaPath = (fs::path(FileSystem::ExternalBasePath) / "Media").string();
	FileSystem::VideoPath = (fs::path(FileSystem::MediaPath) / "Videos").string();
	FileSystem::SplitFilePath = (fs::path(FileSystem::VideoPath) / "SplitVideos").string();
	FileSystem::RecordingsFilePath = (fs::path(FileSystem::VideoPath) / "Recordings").string();
	FileSystem::TempRecordingsFilePath = (fs::path(FileSystem::VideoPath) / "TempRecording").string();
	FileSystem::zipFolderPath = (fs::path(FileSystem::VideoPath) / "ZippedRecordings").string();
	//Module
	FileSystem::BaseModulePath = (fs::path(FileSystem::ExternalBasePath) / "Modules").string();
	FileSystem::ModuleLinkPath = (fs::path(FileSystem::BaseModulePath) / "ModuleLinks").string();
	FileSystem::ModulePath = (fs::path(FileSystem::BaseModulePath) / "Modules").string();
	FileSystem::ZippedModulePath = (fs::path(FileSystem::BaseModulePath) / "ZippedModuleFiles").string();
	//Statements
	FileSystem::BaseStatementPath = (fs::path(FileSystem::ExternalBasePath) / "statements").string();
	FileSystem::StatementPath = (fs::path(FileSystem::BaseStatementPath) / "statements").string();
	FileSystem::WorkingStatementPath = (fs::path(FileSystem::BaseStatementPath) / "workingstatement").string();
	FileSystem::OldStatementPath = (fs::path(FileSystem::BaseStatementPath) / "oldstatements").string();
	//Logs
	FileSystem::BaseLogPath = (fs::path(FileSystem::BasePath) / "Logs").string();
	FileSystem::UploaderLogPath = (fs::path(FileSystem::BaseLogPath) / "UploaderLogs").string();
	FileSystem::LauncherLogPath = (fs::path(FileSystem::BaseLogPath) / "LauncherLogs").string();
	FileSystem::RecorderLogPath = (fs::path(FileSystem::BaseLogPath) / "RecorderLogs").string();
	FileSystem::ModuleManagerLogPath = (fs::path(FileSystem::BaseLogPath) / "ModuleManagerLogs").string();
	FileSystem::SimulationLogBasePath = (fs::path(FileSystem::BaseLogPath) / "SimulationLogs").string();
	//Credentials
	FileSystem::sLocation = (fs::path(FileSystem::BasePath) / "cred").string();
}

// Mirror of C# ExternalFileSystemMethods.ExternalFileSystemRectifier
// (FileSystem.cs). C# 'async Task<bool>' flattens to a synchronous bool (the
// native SDK has no task machinery by design), and the C#
// IExternalFileSystemMethods interface indirection collapses with it -- callers
// construct the class directly.
// PORT AUDIT: the CURRENT C# deliberately does NOT recompute ModuleLinkPath or
// ZippedModulePath here (the pre-refactor C++ drift did), and it never touches
// the BasePath-derived log/cred paths. The C# body is the contract.
bool ExternalFileSystemMethods::ExternalFileSystemRectifier(std::string externalFilePath)
{
	bool output = false;
	try
	{
		FileSystem::ExternalBasePath = externalFilePath;
		FileSystem::MediaPath = (fs::path(FileSystem::ExternalBasePath) / "Media").string();
		FileSystem::VideoPath = (fs::path(FileSystem::MediaPath) / "Videos").string();
		FileSystem::SplitFilePath = (fs::path(FileSystem::VideoPath) / "SplitVideos").string();
		FileSystem::RecordingsFilePath = (fs::path(FileSystem::VideoPath) / "Recordings").string();
		FileSystem::TempRecordingsFilePath = (fs::path(FileSystem::VideoPath) / "TempRecording").string();
		FileSystem::zipFolderPath = (fs::path(FileSystem::VideoPath) / "ZippedRecordings").string();
		FileSystem::BaseModulePath = (fs::path(FileSystem::ExternalBasePath) / "Modules").string();
		FileSystem::ModulePath = (fs::path(FileSystem::BaseModulePath) / "Modules").string();

		FileSystem::BaseStatementPath = (fs::path(FileSystem::ExternalBasePath) / "statements").string();
		FileSystem::StatementPath = (fs::path(FileSystem::BaseStatementPath) / "statements").string();
		FileSystem::WorkingStatementPath = (fs::path(FileSystem::BaseStatementPath) / "workingstatement").string();
		FileSystem::OldStatementPath = (fs::path(FileSystem::BaseStatementPath) / "oldstatements").string();
		output = true;
	}
	catch (const std::exception&)
	{
		// C# 'catch (Exception) { throw; }' -- rethrow unchanged.
		throw;
	}

	return output;
}
