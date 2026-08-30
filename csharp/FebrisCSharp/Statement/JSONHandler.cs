// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Febris.CsharpSimulationLibraryNetStandard.Models.xAPI;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
//using Serilog;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Threading.Tasks;

namespace Febris.CsharpSimulationLibraryNetStandard.Statement
{
    internal class JSONHandler
    {
        //private static readonly ILogger //_log = Log.Logger;
        #region CRUD
        

        #region Read


        #endregion

        #region Update

        #endregion

        #region Delete

        #endregion

        #endregion

        #region Helpers
        /// <summary>
        /// Deals with processed command line arguments coming from PC launcher
        /// </summary>
        /// <param name="arguments"></param>
        /// <returns></returns>
        internal static string ArgumentHandler(string[] arguments)
        {
            string jsonString = string.Empty;
            //JObject deserializedStatement = new JObject();// = string.Empty;
            foreach (string arg in arguments)
            {
                if (arg.StartsWith(SharedDetails.SharedDetails.StatementPreface))
                {
                    jsonString = arg.Substring(SharedDetails.SharedDetails.StatementPrefaceLength);
                    if (jsonString != null && jsonString.Length > 2)
                    {
                        try
                        {
                            return jsonString;
                        }
                        catch (Exception ex)
                        {
                            //_log.Error("ArgumentHandler Error: "+ex.Message);
                            //throw;
                        }
                    }
                }
            }
            return jsonString;
        }

        internal static string SerializeString(string input)
        {
            string outputString = string.Empty;
            try
            {
                outputString = JValue.Parse(input).ToString(Formatting.Indented);
            }
            catch (Exception ex)
            {
                //_log.Error("SerializeString Error: " + ex.Message);
            }
            return outputString;
        }

        internal static JObject ChangeToObject(string inputString)
        {
            JObject jObject = new JObject();
            try
            {
                //jObject = (JObject)JsonConvert.DeserializeObject(inputString);
                jObject = Newtonsoft.Json.Linq.JObject.Parse(inputString);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                Console.WriteLine(ex.StackTrace);
                //_log.Error("ChangeToObject Error: " + ex.Message);
            }
            return jObject;
        }
        internal static JObject CreateObjectFromDataModel(JObject inputJObject)
        {
            //variable
            Models.xAPI.Statement statement = new Models.xAPI.Statement();
            JObject jObject = new JObject();
            try
            {
                //statement builder
                StatementFactoring statementBuilder = new StatementFactoring();
                statement = statementBuilder.FactorStatement(inputJObject);
                //jobject to send back
                jObject = JObject.FromObject(statement);
            }
            catch (Exception ex)
            {
                //_log.Error("CreateObjectFromDataModel Error: " + ex.Message);
            }
            return jObject;
        }


        #endregion

        /// <summary>
        /// These are retired since the Android 11 update/refactor
        /// </summary>        
        #region unused
        #region Create
        //internal static bool CreateInitialDataFile(JObject statement)
        //{
        //    FileNameHandler.CleanMMF();
        //    string statementFileName = Guid.NewGuid().ToString();
        //    statementFileName += ".json";
        //    FileNameHandler.StoreFileName(statementFileName);
        //    bool dataWritten = false;
        //    using (StreamWriter file = File.CreateText(Path.Combine(FileSystem.FileSystem.StatementPath, statementFileName)))
        //    {
        //        try
        //        {
        //            string statementString = SerializeString(statement);
        //            file.Write(statementString);
        //            //file.Write(SerializeString(statement));
        //            //SerializeString(statement);
        //            dataWritten = true;
        //        }
        //        catch (Exception ex)
        //        {
        //            //_log.Error("CreateInitalDataFile Error: " + ex.Message);
        //        }
        //    }
        //    return dataWritten;
        //}

        //internal static bool WriteToDataFile(JObject statement)
        //{
        //    string statementFileName = FileNameHandler.GetFileName();
        //    bool dataWritten = false;
        //    using (StreamWriter file = new StreamWriter(Path.Combine(FileSystem.FileSystem.StatementPath, statementFileName)))
        //    {
        //        try
        //        {
        //            string statementString = SerializeString(statement);
        //            //file.Write(SerializeString(statementString));
        //            file.Write(statementString);
        //            //SerializeString(statement);
        //            dataWritten = true;
        //        }
        //        catch (Exception ex)
        //        {
        //            //_log.Error("WriteToDataFile Error: " + ex.Message);
        //            throw;
        //        }
        //    }
        //    return dataWritten;
        //}

        #endregion
        //internal static string ArgumentHandler(string argument)
        //{
        //    string jsonString = string.Empty;

        //    if (argument.StartsWith(SharedDetails.SharedDetails.StatementPreface))
        //    {
        //        jsonString = argument.Substring(SharedDetails.SharedDetails.StatementPrefaceLength);
        //        if (jsonString != null && jsonString.Length > 2)
        //        {
        //            try
        //            {
        //                return jsonString;
        //            }
        //            catch (Exception ex)
        //            {
        //                //_log.Error("ArgumentHandler Error: "+ex.Message);
        //                //throw;
        //            }
        //        }
        //    }

        //    return jsonString;
        //}
        //internal static bool CreateInitalDataFile(string statement)
        //{
        //    FileNameHandler.CleanMMF();
        //    string statementFileName = Guid.NewGuid().ToString();
        //    statementFileName += ".json";
        //    FileNameHandler.StoreFileName(statementFileName);
        //    bool dataWritten = false;
        //    using (StreamWriter file = File.CreateText(Path.Combine(FileSystem.FileSystem.StatementPath, statementFileName)))
        //    {
        //        try
        //        {
        //            file.Write(statement);
        //            dataWritten = true;
        //        }
        //        catch (Exception ex)
        //        {
        //            //_log.Error("CreateInitalDataFile Error: " + ex.Message);
        //        }
        //    }
        //    return dataWritten;
        //}
        //internal static JObject GetJObject()
        //{
        //    JObject jObject = new JObject();
        //    try
        //    {
        //        string statementFileName = FileNameHandler.GetFileName();
        //        using (StreamReader file = new StreamReader(Path.Combine(FileSystem.FileSystem.StatementPath, statementFileName)))
        //        {
        //            string json = file.ReadToEnd();
        //            jObject = ChangeToObject(json);
        //        }
        //    }
        //    catch (Exception ex)
        //    {
        //        //_log.Error("GetJObject Error: " + ex.Message);
        //        throw;
        //    }
        //    return jObject;
        //}
        #endregion




    }
}
