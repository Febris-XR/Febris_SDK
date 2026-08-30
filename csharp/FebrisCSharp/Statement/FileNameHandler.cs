// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Newtonsoft.Json.Linq;
//using Serilog;
using System;
using System.Collections.Generic;
using System.IO;
using System.IO.MemoryMappedFiles;
using System.Linq;
using System.Text;
using System.Threading;

namespace Febris.CsharpSimulationLibraryNetStandard.Statement
{
    ///No longer used after Android 11 refactor
    #region

    ///// <summary>
    ///// This may not be needed. I can't figure out how it would work better than just writting the file and reading it.
    ///// </summary>
    //internal class FileNameHandler
    //{
    //    //private static readonly ILogger //_log = Log.Logger;

    //    internal static void StoreFileName(string fileName)
    //    {
    //        try
    //        {
    //            byte[] Buffer = ASCIIEncoding.ASCII.GetBytes(fileName);
    //            MemoryMappedFile mmf = MemoryMappedFile.CreateOrOpen("statementFileName", 1000);
    //            MemoryMappedViewAccessor accessor = mmf.CreateViewAccessor();
    //            accessor.Write(54, (ushort)Buffer.Length);
    //            accessor.WriteArray(54 + 2, Buffer, 0, Buffer.Length);
    //        }
    //        catch (Exception ex)
    //        {
    //            //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);

    //        }
    //    }

    //    internal static string GetFileName()
    //    {
    //        try
    //        {
    //            MemoryMappedFile mmf = MemoryMappedFile.OpenExisting("statementFileName");
    //            MemoryMappedViewAccessor accessor = mmf.CreateViewAccessor();
    //            ushort Size = accessor.ReadUInt16(54);
    //            byte[] Buffer = new byte[Size];
    //            accessor.ReadArray(54 + 2, Buffer, 0, Buffer.Length);
    //            return ASCIIEncoding.ASCII.GetString(Buffer);
    //        }
    //        catch
    //        {
    //            DirectoryInfo directory = new DirectoryInfo(FileSystem.FileSystem.StatementPath);
    //            FileInfo fileInfo = directory.GetFiles().OrderByDescending(f => f.LastWriteTime).First();
    //            StoreFileName(fileInfo.Name);
    //            return fileInfo.Name;
    //        }
    //    }

    //    internal static void CleanMMF()
    //    {
    //        try
    //        {
    //            MemoryMappedFile mmf = MemoryMappedFile.OpenExisting("statementFileName");
    //            mmf.Dispose();
    //        }
    //        catch (Exception ex)
    //        {
    //            //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);

    //        }
    //    }
    //}
    #endregion
}
