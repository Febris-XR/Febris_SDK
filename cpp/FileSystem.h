// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#define EXTERNALFILESYSTEMMETHODS_H
//#include "pch.h";
#endif 

class FILESYSTEM_H FileSystem
{	
public:
	static string BasePath; //= temp_path.string();
	static string ExternalBasePath;// = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "Febris");
	static string MediaPath;// = (fs::path(ExternalBasePath) / "Media").string();	
	static string VideoPath;// = Path.Combine(MediaPath, "Videos");
	static string SplitFilePath;// = Path.Combine(VideoPath, "SplitVideos");	
	static string RecordingsFilePath;// = Path.Combine(VideoPath, "Recordings");	
	static string TempRecordingsFilePath;// = Path.Combine(VideoPath, "TempRecording");	
	static string zipFolderPath;// = Path.Combine(VideoPath, "ZippedRecordings");	
	static string BaseModulePath;// = Path.Combine(ExternalBasePath, "Modules");	
	static string ModuleLinkPath;// = Path.Combine(BaseModulePath, "ModuleLinks");
	static string ModulePath;// = Path.Combine(BaseModulePath, "Modules");
	static string ZippedModulePath;// = Path.Combine(BaseModulePath, "ZippedModuleFiles");	
	static string BaseStatementPath;// = Path.Combine(ExternalBasePath, "statements");	
	static string StatementPath;// = Path.Combine(BaseStatementPath, "statements");
	static string WorkingStatementPath;// = Path.Combine(BaseStatementPath, "workingstatement");
	static string OldStatementPath;// = Path.Combine(BaseStatementPath, "oldstatements");	
	static string BaseLogPath;// = Path.Combine(BasePath, "Logs");	
	static string UploaderLogPath;// = Path.Combine(BaseLogPath, "UploaderLogs");
	static string LauncherLogPath;// = Path.Combine(BaseLogPath, "LauncherLogs");
	static string RecorderLogPath;// = Path.Combine(BaseLogPath, "RecorderLogs");
	static string ModuleManagerLogPath;// = Path.Combine(BaseLogPath, "ModuleManagerLogs");
	static string SimulationLogBasePath;// = Path.Combine(BaseLogPath, "SimulationLogs");	
	static string sLocation;// = Path.Combine(BasePath, "cred");
	static void SetFileSystemBasePath();

};

//#ifndef EXTERNALFILESYSTEMMETHODS_H
//#define EXTERNALFILESYSTEMMETHODS_H
//#include "pch.h";
//#endif 

class EXTERNALFILESYSTEMMETHODS_H ExternalFileSystemMethods
{
public:
	bool ExternalFileSystemRectifier(string externalFilePath);
};