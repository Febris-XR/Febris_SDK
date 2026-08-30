// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"

// EXPORT-SURFACE PORT (2026-08-29): global-scope definitions for the
// FileSystemInitalizer declared in FileSystemInitalizer.h; the dead namespaced
// duplicate (namespace FebrisFileSystem) that previously lived here is gone.
// Semantics mirror the CURRENT C# FileSystem\FileSystemInitalizer.cs.
// PORT AUDIT: the pre-refactor C++ drift called FileSystem::SetFileSystemBasePath()
// at the top of both overloads; the CURRENT C# does not, so neither do we (the
// path statics are already initialized by FileSystem.cpp's ordered definitions).

void FileSystemInitalizer::FileInitalizer(std::string externalPath)
{
	try
	{
		// C# 'IExternalFileSystemMethods rectifier = new ExternalFileSystemMethods()':
		// the interface indirection collapses in the native SDK, and the C#
		// '.Result' on the Task is simply the synchronous return value here.
		ExternalFileSystemMethods externalFileSystemRectifier{};
		bool complete = externalFileSystemRectifier.ExternalFileSystemRectifier(externalPath);
		if (!complete)
		{
			std::cout << "files not set up correctly" << std::endl;
		}
		//FileSystem.ExternalBasePath = externalPath;

		std::vector<std::string> FileList =
		{
			FileSystem::ExternalBasePath,
			FileSystem::BasePath,
			FileSystem::MediaPath,
			FileSystem::VideoPath,
			FileSystem::SplitFilePath,
			FileSystem::RecordingsFilePath,
			FileSystem::TempRecordingsFilePath,
			FileSystem::zipFolderPath,
			FileSystem::BaseModulePath,
			FileSystem::ModuleLinkPath,
			FileSystem::ModulePath,
			FileSystem::ZippedModulePath,
			FileSystem::BaseStatementPath,
			FileSystem::StatementPath,
			FileSystem::WorkingStatementPath,
			FileSystem::OldStatementPath,
			FileSystem::BaseLogPath,
			FileSystem::UploaderLogPath,
			FileSystem::LauncherLogPath,
			FileSystem::RecorderLogPath,
			FileSystem::SimulationLogBasePath,
			FileSystem::ModuleManagerLogPath,
			FileSystem::sLocation,
			//FileSystem.SharedDataPath
		};

		for (const auto& file : FileList)
		{
			try
			{
				CreateFileDirectory(file, std::string());
			}
			catch (const std::exception&)
			{
				//_log.Error(ex.Message);
			}
		}
	}
	catch (const std::exception&)
	{
		//_log.Error(ex.Message);
	}
}

void FileSystemInitalizer::FileInitalizer()
{
	try
	{
		std::vector<std::string> FileList =
		{
			FileSystem::ExternalBasePath,
			FileSystem::BasePath,
			FileSystem::MediaPath,
			FileSystem::VideoPath,
			FileSystem::SplitFilePath,
			FileSystem::RecordingsFilePath,
			FileSystem::TempRecordingsFilePath,
			FileSystem::zipFolderPath,
			FileSystem::BaseModulePath,
			FileSystem::ModuleLinkPath,
			FileSystem::ModulePath,
			FileSystem::ZippedModulePath,
			FileSystem::BaseStatementPath,
			FileSystem::StatementPath,
			FileSystem::WorkingStatementPath,
			FileSystem::OldStatementPath,
			FileSystem::BaseLogPath,
			FileSystem::UploaderLogPath,
			FileSystem::LauncherLogPath,
			FileSystem::RecorderLogPath,
			FileSystem::SimulationLogBasePath,
			FileSystem::ModuleManagerLogPath,
			FileSystem::sLocation//,
			// [Historical - a commented duplicate tail of the same path list in C#.]
		};

		for (const auto& file : FileList)
		{
			try
			{
				CreateFileDirectory(file, std::string());
			}
			// C# bare 'catch { }' swallows everything, so catch-all here too.
			catch (...) { }
		}
	}
	catch (const std::exception&)
	{
		// C# 'catch (Exception ex) { throw; }' -- rethrow unchanged.
		throw;
	}
}
//#############################################################################
// connecting to the file storage retriever
//#############################################################################
void FileSystemInitalizer::CreateFileDirectory(std::string path, std::string name)
{
	try
	{
		// C# Directory.CreateDirectory creates the whole missing chain, so
		// create_directories (plural), not create_directory.
		fs::create_directories(fs::path(path) / name);
	}
	catch (const std::exception&)
	{
		// C# 'catch (Exception) { throw; }' -- rethrow unchanged.
		throw;
	}
}
