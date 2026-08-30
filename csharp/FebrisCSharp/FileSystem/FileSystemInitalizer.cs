// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Febris.CsharpSimulationLibraryNetStandard.FileSystem
{
    public class FileSystemInitalizer
    {
        public static void FileInitalizer(string externalPath)
        {
            try
            {
                IExternalFileSystemMethods externalFileSystemRectifier = new ExternalFileSystemMethods();
                bool complete = externalFileSystemRectifier.ExternalFileSystemRectifier(externalPath).Result;
                if (!complete)
                {
                    Console.WriteLine("files not set up correctly");
                }
                //FileSystem.ExternalBasePath = externalPath;

                List<string> FileList = new List<string>
                {
                    FileSystem.ExternalBasePath,
                    FileSystem.BasePath,
                    FileSystem.MediaPath,
                    FileSystem.VideoPath,
                    FileSystem.SplitFilePath,
                    FileSystem.RecordingsFilePath,
                    FileSystem.TempRecordingsFilePath,
                    FileSystem.zipFolderPath,
                    FileSystem.BaseModulePath,
                    FileSystem.ModuleLinkPath,
                    FileSystem.ModulePath,
                    FileSystem.ZippedModulePath,
                    FileSystem.BaseStatementPath,
                    FileSystem.StatementPath,
                    FileSystem.WorkingStatementPath,
                    FileSystem.OldStatementPath,
                    FileSystem.BaseLogPath,
                    FileSystem.UploaderLogPath,
                    FileSystem.LauncherLogPath,
                    FileSystem.RecorderLogPath,
                    FileSystem.SimulationLogBasePath,
                    FileSystem.ModuleManagerLogPath,
                    FileSystem.sLocation,                    
                    //FileSystem.SharedDataPath
                };



                foreach (var file in FileList)
                {
                    try
                    {
                        CreateFileDirectory(file, string.Empty);
                    }
                    catch (Exception ex)
                    {
                        //_log.Error(ex.Message);
                    }
                }
            }
            catch (Exception ex)
            {
                //_log.Error(ex.Message);                
            }
        }
        public static void FileInitalizer()
        {
            try
            {
                List<string> FileList = new List<string>
                {
                    FileSystem.ExternalBasePath,
                    FileSystem.BasePath,
                    FileSystem.MediaPath,
                    FileSystem.VideoPath,
                    FileSystem.SplitFilePath,
                    FileSystem.RecordingsFilePath,
                    FileSystem.TempRecordingsFilePath,
                    FileSystem.zipFolderPath,
                    FileSystem.BaseModulePath,
                    FileSystem.ModuleLinkPath,
                    FileSystem.ModulePath,
                    FileSystem.ZippedModulePath,
                    FileSystem.BaseStatementPath,
                    FileSystem.StatementPath,
                    FileSystem.WorkingStatementPath,
                    FileSystem.OldStatementPath,
                    FileSystem.BaseLogPath,
                    FileSystem.UploaderLogPath,
                    FileSystem.LauncherLogPath,
                    FileSystem.RecorderLogPath,
                    FileSystem.SimulationLogBasePath,
                    FileSystem.ModuleManagerLogPath,
                    FileSystem.sLocation//,
                    //FileSystem.BasePath,
                    //FileSystem.BaseModulePath,
                    //FileSystem.ModuleLinkPath,
                    //FileSystem.ModulePath,
                    //FileSystem.ZippedModulePath,
                    //FileSystem.BaseStatementPath,
                    //FileSystem.StatementPath,
                    //FileSystem.WorkingStatementPath,
                    //FileSystem.OldStatementPath,
                    //FileSystem.BaseLogPath,
                    //FileSystem.SimulationLogBasePath
                };

                foreach (var file in FileList)
                {
                    try
                    {
                        CreateFileDirectory(file, string.Empty);
                    }
                    catch { }
                }
            }
            catch (Exception ex)
            {

                throw;
            }
        }
        //#############################################################################
        // connecting to the file storage retriever
        //#############################################################################
        public static void CreateFileDirectory(string path, string name)
        {
            try
            {                
                Directory.CreateDirectory(Path.Combine(path, name));
            }
            catch (Exception)
            {

                throw;
            }
        }



    }
}
