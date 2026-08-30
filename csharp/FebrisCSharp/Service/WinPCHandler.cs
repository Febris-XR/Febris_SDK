// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Febris.CsharpSimulationLibraryNetStandard.Statement;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Threading.Tasks;

namespace Febris.CsharpSimulationLibraryNetStandard.Service
{
    internal class WinPCHandler : IEnvironmentHandler
    {
        #region CRUD
        #region Create
        public async Task<(bool, string[,])> CreateInitialPost(JObject statementFromDataModel)
        {
            bool isInitialized = false;
            string[,] outputArray = default;
            try
            {
                StaticDetails.StaticDetails.ReferenceUUID = Guid.NewGuid().ToString();
                StaticDetails.StaticDetails.CurrentStatement = statementFromDataModel;
                #region file system initalizer
                FileSystem.FileSystemInitalizer.FileInitalizer();
                #endregion

                #region write to data file
                isInitialized = await WriteFile(statementFromDataModel);
                #endregion              
            }
            catch (Exception ex)
            {
                // SIM-T13 G4: surface through the configured logger so the host
                // game can see the failure. Re-throw preserved -- callers may be
                // depending on it.
                StaticDetails.StaticDetails.Logger.Log(
                    SimulationLogLevel.Error,
                    "WinPCHandler.CreateInitialPost: failed.",
                    ex);
                throw;
            }
            return (isInitialized, outputArray);
        }


        #endregion

        #region Read


        #endregion

        #region Update
        public async Task<(bool, string[,])> UpdatePost(JObject statementFromDataModel)
        {
            bool complete = false;
            string[,] outputArray = default;
            try
            {
                complete = await WriteFile(statementFromDataModel);
            }
            catch (Exception ex)
            {
                // SIM-T13 G4: surface through logger instead of Console swallow.
                StaticDetails.StaticDetails.Logger.Log(
                    SimulationLogLevel.Error,
                    "WinPCHandler.UpdatePost: failed.",
                    ex);
                throw;
            }
            return (complete, outputArray);
        }

        /// <summary>
        /// SIM-T13 G2: error-emission for the PC path. Mirrors
        /// <see cref="UpdatePost(JObject)"/> but writes to a sibling file with
        /// the <c>.error.json</c> suffix so the PC Statement Manager (or test
        /// fixtures) can distinguish error-statement files from normal updates
        /// without parsing the JSON content. Was previously an unguarded
        /// <see cref="NotImplementedException"/> -- calling this from a Unity
        /// game running on Windows PC crashed the host.
        /// </summary>
        public async Task<(bool, string[,])> ErrorPost(JObject statementFromDataModel)
        {
            bool complete = false;
            string[,] outputArray = default;
            try
            {
                complete = await WriteErrorFile(statementFromDataModel);
            }
            catch (Exception ex)
            {
                // SIM-T13 G4: route through the configured logger instead of
                // Console.WriteLine so the host can surface failures into its
                // own diagnostics path.
                StaticDetails.StaticDetails.Logger.Log(
                    SimulationLogLevel.Error,
                    "WinPCHandler.ErrorPost: failed writing error-statement file.",
                    ex);
            }
            return (complete, outputArray);
        }

        private async static Task<bool> WriteErrorFile(JObject statementFromDataModel)
        {
            // Same shape as WriteFile(...) below but with a .error.json suffix
            // so the consumer side can pick it out without inspecting content.
            // FIX (SDKV-19/20): stamp the statement id before serializing so
            // the wire id matches the handoff filename GUID.
            StatementHandler.StampStatementId(statementFromDataModel);
            string statementFileName = StaticDetails.StaticDetails.ReferenceUUID + ".error.json";
            bool dataWritten = false;
            using (StreamWriter file = File.CreateText(Path.Combine(FileSystem.FileSystem.StatementPath, statementFileName)))
            {
                try
                {
                    string statementString = SerializeString(statementFromDataModel);
                    file.Write(statementString);
                    dataWritten = true;
                }
                catch (Exception ex)
                {
                    StaticDetails.StaticDetails.Logger.Log(
                        SimulationLogLevel.Error,
                        "WinPCHandler.WriteErrorFile: serialization or disk write failed.",
                        ex);
                }
            }
            return dataWritten;
        }
        #endregion

        #region Delete

        #endregion

        #endregion

        #region Helpers
        private async static Task<bool> WriteFile(JObject statementFromDataModel)
        {
            // FIX (SDKV-19/20): stamp the statement id before serializing so
            // every emitted {uuid}.json carries a wire id equal to the handoff
            // filename GUID; re-writes of the same run keep the same id, which
            // is what makes host retries idempotent on the ingest node.
            StatementHandler.StampStatementId(statementFromDataModel);
            string statementFileName = StaticDetails.StaticDetails.ReferenceUUID + ".json";
            bool dataWritten = false;
            using (StreamWriter file = File.CreateText(Path.Combine(FileSystem.FileSystem.StatementPath, statementFileName)))
            {
                try
                {
                    string statementString = SerializeString(statementFromDataModel);
                    file.Write(statementString);
                    //file.Write(SerializeString(statement));
                    //SerializeString(statement);
                    dataWritten = true;
                }
                catch (Exception ex)
                {
                    // SIM-T13 G4: surface disk-write failures through the logger.
                    StaticDetails.StaticDetails.Logger.Log(
                        SimulationLogLevel.Error,
                        "WinPCHandler.WriteFile: disk write failed for " + statementFileName + ".",
                        ex);
                }
            }
            return dataWritten;
        }

        internal static string SerializeString(JObject jObject)
        {
            string outputString = string.Empty;
            try
            {
                outputString = JsonConvert.SerializeObject(jObject);
            }
            catch (Exception ex)
            {
                // SIM-T13 G4: serialization failures bubble through the logger.
                StaticDetails.StaticDetails.Logger.Log(
                    SimulationLogLevel.Error,
                    "WinPCHandler.SerializeString: JsonConvert.SerializeObject failed.",
                    ex);
            }
            return outputString;
        }
        #endregion
    }
}
