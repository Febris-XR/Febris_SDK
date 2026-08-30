// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Febris.CsharpSimulationLibraryNetStandard.Enums;
using Febris.CsharpSimulationLibraryNetStandard.Models.xAPI;
using Febris.CsharpSimulationLibraryNetStandard.Service;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
//using Serilog;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.MemoryMappedFiles;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Xml;

namespace Febris.CsharpSimulationLibraryNetStandard.Statement
{
    /// <summary>
    /// In Android 11 refactor the JObject statement = JSONHandler.GetJObject(); was removed and was changed to a static JObject called JObject statement = StaticDetails.StaticDetails.CurrentStatement;
    /// </summary>
    public class StatementHandler
    {
        ///Methods Developers can use to update Statements and trigger different actions
        #region External
        #region Returns values for use

        /// <summary>
        /// SIM-T13 G3 dispatch overload -- same as <see cref="GetSendableUpdate"/>
        /// but returns a typed
        /// <see cref="Service.SimulationDispatch"/> so the host (Unity glue,
        /// Unreal glue, future engine integrations) does not have to remember
        /// the intent action by which library method was called. The dispatch
        /// carries <c>IntentAction = "com.febris.STATEMENT_UPDATE"</c> for
        /// Android; the host fires it mechanically.
        /// </summary>
        public static async Task<Service.SimulationDispatch> GetSendableDispatch()
        {
            try
            {
                return await StaticDetails.StaticDetails.Handler
                    .UpdateDispatchAsync(StaticDetails.StaticDetails.CurrentStatement)
                    .ConfigureAwait(false);
            }
            catch (Exception ex)
            {
                StaticDetails.StaticDetails.Logger.Log(
                    Service.SimulationLogLevel.Error,
                    "StatementHandler.GetSendableDispatch: handler.UpdateDispatchAsync failed.",
                    ex);
                return new Service.SimulationDispatch(
                    ready: false,
                    intentAction: StaticDetails.AndroidStatementPassingStaticDetails.StatementUpdate,
                    extras: null);
            }
        }

        /// <summary>
        /// This needs to be used on update loops. This will cause the system to either write to a shared file system or
        /// creates a way of posting to the controlling application
        /// </summary>
        /// <returns></returns>
        public static async Task<(bool, string[,])> GetSendableUpdate()
        {
            bool ready = false;
            string[,] outputArray = default;
            try
            {
                (ready, outputArray) = await StaticDetails.StaticDetails.Handler.UpdatePost(StaticDetails.StaticDetails.CurrentStatement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error getting sendable statement: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
            return (ready, outputArray);
        }

        /// <summary>
        /// 
        /// </summary>
        public async static Task<(bool, string[,])> EndSimulation()
        {
            bool ready = false;
            string[,] outputArray = default;
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                //calculate passing /complete?

                //update verb
                string currentVerb = statement["verb"]["id"].ToString();
                VerbEnums currentVerbEnum = VerbIRIResolver.GetVerbEnum(currentVerb);
                //bool verbUpdated = Task.Run(() => VerbUpdate(currentVerbEnum)).Result;
                statement = VerbUpdate(statement, currentVerbEnum);


                //update verb
                //string currentVerb = statement["verb"]["id"].ToString();
                //VerbEnums currentVerbEnum = VerbIRIResolver.GetVerbEnum(currentVerb);
                //bool verbUpdated = Task.Run(() => VerbUpdate(currentVerbEnum)).Result;
                //write statement
                //bool written = JSONHandler.WriteToDataFile(statement);
                StaticDetails.StaticDetails.CurrentStatement = statement;

                (ready, outputArray) = await GetSendableUpdate();
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing the final simulation statement: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
            return (ready, outputArray);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="success"></param>
        /// <param name="complete"></param>
        /// <param name="rawScore"></param>
        /// <param name="duration"></param>
        public async static Task<(bool, string[,])> EndSimulation(bool success, bool complete, float rawScore, TimeSpan duration)
        {
            bool ready = false;
            string[,] outputArray = default;
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;

                //update complete
                statement = UpdateCompletionStatus(statement, complete);
                //update success
                statement = UpdateSuccessStatus(statement, success);
                //update result
                statement = UpdateDuration(statement, duration);
                //update score
                statement = UpdateRawScore(statement, rawScore);
                //update verb
                string currentVerb = statement["verb"]["id"].ToString();
                VerbEnums currentVerbEnum = VerbIRIResolver.GetVerbEnum(currentVerb);
                //bool verbUpdated = Task.Run(() => VerbUpdate(currentVerbEnum)).Result;
                statement = VerbUpdate(statement, currentVerbEnum);
                //write statement
                StaticDetails.StaticDetails.CurrentStatement = statement;

                (ready, outputArray) = await GetSendableUpdate();
                //bool written = JSONHandler.WriteToDataFile(statement);

            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing the final simulation statement: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
            return (ready, outputArray);
        }




        #endregion

        #region One way calls
        #region Specific calls
        /// <summary>
        /// this method is used to finish the simulation and finalize the JSON Statement.
        /// FIX (SDKV-3/11): result.completion is written as a JSON Boolean --
        /// the old lowercase-string "true" violated xAPI 1.0.3 section 4.1.5.
        /// </summary>
        public static void SimulationComplete()
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                statement["result"]["completion"] = true;
                //statement["result"]["completion"] = "true"; // [Historical - SDKV-3] string-typed boolean
                StaticDetails.StaticDetails.CurrentStatement = statement;
                //tally up score so it can set up success

                //
                bool ready = false;
                string[,] outputArray = default;
                // NOTE (SIM-B9): sync-over-async blocking on the game thread (.Result) here, together with the caller rewiring in FebrisScriptManager.cs, needs an async refactor of SimulationComplete and pairs with SIM-M3 (non-blocking queue). Deferred per do-not-change-functionality. See docs/MODERNIZATION/SIM_MODERNIZATION.md.
                (ready, outputArray) = GetSendableUpdate().Result;

            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing passed simulation statement: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// Marks the simulation's result.success flag.
        /// FIX (SDKV-3/11): written as a JSON Boolean -- the old
        /// lowercase-string "true"/"false" violated xAPI 1.0.3 section 4.1.5.
        /// </summary>
        /// <param name="input"></param>
        public static void SimulationPassed(bool input)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                statement["result"]["success"] = input;
                //statement["result"]["success"] = input.ToString().ToLower(); // [Historical - SDKV-3] string-typed boolean
                StaticDetails.StaticDetails.CurrentStatement = statement;
                //tally up score so it can set up success

                //
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing passed simulation statement: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// Updates the duration of the simulation
        /// </summary>
        /// <param name="duration"></param>
        public static void DurationUpdate(TimeSpan input)
        {
            try
            {
                UpdateStatement(XAPIProperties.Result, ResultOptions.Duration, input);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error Duration Update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// Every time a stage is restarted this function should be called.
        /// </summary>
        public static void StageRestart()
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                int count = 0;
                try
                {
                    string extensionMap = (string)statement["result"]["extensions"]["extensionmap"] ?? string.Empty;
                    if (extensionMap != string.Empty)
                    {
                        string[] extensionMapArray = extensionMap.Split(',');
                        string key = ExtensionIRIResolver.ResolveExtensionIRI(ExtensionIRIOptions.RestartCounterIRI);
                        for (var i = 0; i < extensionMapArray.Length; i++)
                        {
                            string[] extensionSingle = extensionMapArray[i].Split(':');
                            // MDM-T2 C-03 / BUGS.md: bounds-guard the indexer access + int parse.
                            // Previously `extensionSingle[2]` and `Int32.Parse` would throw
                            // IndexOutOfRange / FormatException on any malformed entry,
                            // killing the whole stage-restart counter update.
                            if (extensionSingle.Length < 3)
                            {
                                StaticDetails.StaticDetails.Logger.Log(
                                    Service.SimulationLogLevel.Warn,
                                    "StageRestart: skipping malformed extensionmap entry (need 3 colon-separated parts, got " +
                                    extensionSingle.Length + "): " + extensionMapArray[i]);
                                continue;
                            }
                            if (extensionSingle[0] + ":" + extensionSingle[1] == key)
                            {
                                if (!Int32.TryParse(extensionSingle[2], out count))
                                {
                                    StaticDetails.StaticDetails.Logger.Log(
                                        Service.SimulationLogLevel.Warn,
                                        "StageRestart: extensionmap counter value isn't a valid int32: " + extensionSingle[2]);
                                    count = 0;
                                }
                            }
                        }
                    }

                }
                catch (Exception ex)
                {
                    StaticDetails.StaticDetails.Logger.Log(
                        Service.SimulationLogLevel.Error,
                        "StageRestart: unexpected exception parsing extensionmap.",
                        ex);
                }
                count++;
                UpdateStatement(XAPIProperties.Result, ResultOptions.Extensions, ResultExtensionOptions.RestartCounter, count);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on stage restart counter: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// This is the simple way to add notes.
        /// </summary>
        /// <param name="note"></param>
        public static void AddResultNote(string note)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                UpdateStatement(XAPIProperties.Result, ResultOptions.Extensions, ResultExtensionOptions.Notes, note);
                StaticDetails.StaticDetails.CurrentStatement = statement;
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error add result note: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        #endregion
        #region Direct Statement Updates
        /// <summary>
        /// 
        /// </summary>
        /// <param name="property"></param>
        /// <param name="resultType"></param>
        /// <param name="input"></param>
        public static void UpdateStatement(XAPIProperties property, ResultOptions resultType, bool input)
        {
            try
            {
                switch (property)
                {
                    //case XAPIProperties.Actor:
                    //    //this should not update
                    //    break;
                    //case XAPIProperties.Object:
                    //    //this should not be updated
                    //    break;
                    //case XAPIProperties.Verb:
                    //    //VerbUpdate(input);//may need more information
                    //    break;
                    //case XAPIProperties.Timestamp:
                    //    //should already be set, if not set up on initalization
                    //    break;
                    case XAPIProperties.Result:
                        //need more information for this one
                        Result(input, resultType);
                        break;
                    //case XAPIProperties.Context:
                    //    break;
                    //case XAPIProperties.Authority:
                    //    break;
                    //case XAPIProperties.Version:
                    //    break;
                    //case XAPIProperties.Attachments:
                    //    break;
                    //case XAPIProperties.Account:
                    //    break;
                    //case XAPIProperties.Member:
                    //    break;
                    //case XAPIProperties.Definition:
                    //    break;
                    case XAPIProperties.Score:
                        break;
                    //case XAPIProperties.ContextActivities:
                    //    break;
                    //case XAPIProperties.StatementReference:
                    //    break;
                    //case XAPIProperties.Extensions:
                    //    break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on statement update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="property"></param>
        /// <param name="resultType"></param>
        /// <param name="input"></param>
        public static void UpdateStatement(XAPIProperties property, ResultOptions resultType, string input)
        {
            try
            {
                switch (property)
                {
                    //case XAPIProperties.Actor:
                    //    //this should not update
                    //    break;
                    //case XAPIProperties.Object:
                    //    //this should not be updated
                    //    break;
                    //case XAPIProperties.Verb:
                    //    //VerbUpdate(input);//may need more information
                    //    break;
                    //case XAPIProperties.Timestamp:
                    //    //should already be set, if not set up on initalization
                    //    break;
                    case XAPIProperties.Result:
                        //need more information for this one
                        Result(input, resultType);
                        break;
                    //case XAPIProperties.Context:
                    //    break;
                    //case XAPIProperties.Authority:
                    //    break;
                    //case XAPIProperties.Version:
                    //    break;
                    //case XAPIProperties.Attachments:
                    //    break;
                    //case XAPIProperties.Account:
                    //    break;
                    //case XAPIProperties.Member:
                    //    break;
                    //case XAPIProperties.Definition:
                    //    break;
                    case XAPIProperties.Score:
                        break;
                    //case XAPIProperties.ContextActivities:
                    //    break;
                    //case XAPIProperties.StatementReference:
                    //    break;
                    //case XAPIProperties.Extensions:
                    //    break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on statement update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="property"></param>
        /// <param name="resultType"></param>
        /// <param name="input"></param>
        public static void UpdateStatement(XAPIProperties property, ResultOptions resultType, float input)
        {
            try
            {
                switch (property)
                {
                    //case XAPIProperties.Verb:
                    //    //VerbUpdate(input);//may need more information
                    //    break;                
                    case XAPIProperties.Result:
                        Result(input, resultType);
                        break;
                    //case XAPIProperties.Context:
                    //    break;
                    //case XAPIProperties.Authority:
                    //    break;
                    //case XAPIProperties.Version:
                    //    break;
                    //case XAPIProperties.Attachments:
                    //    break;
                    //case XAPIProperties.Account:
                    //    break;
                    //case XAPIProperties.Member:
                    //    break;
                    //case XAPIProperties.Definition:
                    //    break;
                    //case XAPIProperties.Score:
                    //    break;
                    //case XAPIProperties.ContextActivities:
                    //    break;
                    //case XAPIProperties.StatementReference:
                    //    break;                
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on statement update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="property"></param>
        /// <param name="resultType"></param>
        /// <param name="input"></param>
        public static void UpdateStatement(XAPIProperties property, ResultOptions resultType, TimeSpan input)
        {
            try
            {
                switch (property)
                {
                    case XAPIProperties.Result:
                        Result(input, resultType);
                        break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on statement update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="property"></param>
        /// <param name="resultType"></param>
        /// <param name="resultExtensionType"></param>
        /// <param name="input"></param>
        public static void UpdateStatement(XAPIProperties property, ResultOptions resultType, ResultExtensionOptions resultExtensionType, int input)
        {
            try
            {
                switch (property)
                {
                    case XAPIProperties.Result:
                        if (resultType == ResultOptions.Extensions)
                        {
                            Result(input, resultType, resultExtensionType);
                        }
                        break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on statement update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="property"></param>
        /// <param name="resultType"></param>
        /// <param name="resultExtensionType"></param>
        /// <param name="input"></param>
        public static void UpdateStatement(XAPIProperties property, ResultOptions resultType, ResultExtensionOptions resultExtensionType, string input)
        {
            try
            {
                switch (property)
                {
                    case XAPIProperties.Result:
                        if (resultType == ResultOptions.Extensions)
                        {
                            Result(input, resultType, resultExtensionType);
                        }
                        break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on statement update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        #endregion
        #region attachment handling interface
        /// <summary>
        /// 
        /// </summary>
        /// <param name="property"></param>
        /// <param name="usageType"></param>
        /// <param name="display"></param>
        /// <param name="description"></param>
        /// <param name="contentType"></param>
        /// <param name="length"></param>
        /// <param name="sha2"></param>
        /// <param name="fileUrl"></param>
        public static void UpdateStatement(XAPIProperties property, string usageType, string display, string description, ContentType contentType, int length, string sha2, string fileUrl)
        {
            try
            {
                switch (property)
                {

                    case XAPIProperties.Attachments:
                        AddAttachment(usageType, display, description, contentType, length, sha2, fileUrl);
                        break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error updating statement: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="property"></param>
        /// <param name="usageType"></param>
        /// <param name="display"></param>
        /// <param name="description"></param>
        /// <param name="contentType"></param>
        /// <param name="length"></param>
        /// <param name="sha2"></param>
        public static void UpdateStatement(XAPIProperties property, string usageType, string display, string description, ContentType contentType, int length, string sha2)
        {
            try
            {
                switch (property)
                {

                    case XAPIProperties.Attachments:
                        AddAttachment(usageType, display, description, contentType, length, sha2);
                        break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error updating statement: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="property"></param>
        /// <param name="usageType"></param>
        /// <param name="display"></param>
        /// <param name="contentType"></param>
        /// <param name="length"></param>
        /// <param name="sha2"></param>
        /// <param name="fileUrl"></param>
        public static void UpdateStatement(XAPIProperties property, string usageType, string display, ContentType contentType, int length, string sha2, string fileUrl)
        {
            try
            {
                switch (property)
                {
                    case XAPIProperties.Attachments:
                        AddAttachment(usageType, display, contentType, length, sha2, fileUrl);
                        break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error updating statement: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="property"></param>
        /// <param name="usageType"></param>
        /// <param name="display"></param>
        /// <param name="contentType"></param>
        /// <param name="length"></param>
        /// <param name="sha2"></param>
        public static void UpdateStatement(XAPIProperties property, string usageType, string display, ContentType contentType, int length, string sha2)
        {
            try
            {
                switch (property)
                {
                    case XAPIProperties.Attachments:
                        AddAttachment(usageType, display, contentType, length, sha2);
                        break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error updating statement: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        #endregion
        #region context interface

        public static void UpdateStatement(XAPIProperties property, ContextOptions contextOption, string input)
        {
            try
            {
                switch (property)
                {

                    case XAPIProperties.Context:
                        Context(contextOption, input);
                        break;
                    ////case XAPIProperties.Authority:
                    ////    break;
                    ////case XAPIProperties.Version:
                    ////    break;
                    ////case XAPIProperties.Attachments:
                    ////    break;
                    ////case XAPIProperties.Account:
                    ////    break;
                    ////case XAPIProperties.Member:
                    ////    break;
                    ////case XAPIProperties.Definition:
                    ////    break;
                    //case XAPIProperties.Score:
                    //    break;
                    //case XAPIProperties.ContextActivities:
                    //    break;
                    //case XAPIProperties.StatementReference:
                    //    break;
                    //case XAPIProperties.Extensions:
                    //    break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on statement update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }

        public static void UpdateStatement(XAPIProperties property, ContextOptions contextOption, ContextActivitesOptions contextActivitesOption, string input)
        {
            try
            {
                switch (property)
                {
                    //case XAPIProperties.Result:
                    //    if (resultType == ResultOptions.Extensions)
                    //    {
                    //        Result(input, resultType, resultExtensionType);
                    //    }
                    //    break;
                    case XAPIProperties.Context:
                        Context(contextOption, contextActivitesOption, input);
                        break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on statement update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }

        public static void UpdateStatement(XAPIProperties property, ContextOptions contextOption, ContextStatementReferenceOptions contextStatementReferenceOption, string input)
        {
            try
            {
                switch (property)
                {
                    //case XAPIProperties.Result:
                    //    if (resultType == ResultOptions.Extensions)
                    //    {
                    //        Result(input, resultType, resultExtensionType);
                    //    }
                    //    break;
                    case XAPIProperties.Context:
                        Context(contextOption, contextStatementReferenceOption, input);
                        break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on statement update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }

        public static void UpdateStatement(XAPIProperties property, ContextOptions contextOption, ContextExtensionOptions contextExtensionOption, string input)
        {
            try
            {
                switch (property)
                {
                    //case XAPIProperties.Result:
                    //    if (resultType == ResultOptions.Extensions)
                    //    {
                    //        Result(input, resultType, resultExtensionType);
                    //    }
                    //    break;
                    case XAPIProperties.Context:
                        Context(contextOption, contextExtensionOption, input);
                        break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on statement update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        #endregion
        #region Custom Verb Methods
        ///Use a custom verb that was previously setup in the portal. This is not selectable in the Febris Enum stock selections. 
        /// It is utilized through the URI generated on creation. 
        /// Please use that Uri string if you would like to use a custom Verb
        public static void CustomVerbUpdate(string uriInput)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                statement["verb"]["id"] = uriInput;
                StaticDetails.StaticDetails.CurrentStatement = statement;
                //tally up score so it can set up success

                //
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on custom verb update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        #endregion
        #endregion
        #endregion


        ///Internal Methods that should not be able to be utilized directly
        #region Internal 
        internal static bool StatementCheck(JObject input)
        {
            bool isCorrect = false;
            try
            {
                //Make sure required items exist
                bool containsActor = false;
                bool containsObject = false;
                bool containsVerb = false;
                if (input.ContainsKey("actor") || input.ContainsKey("Actor"))
                {
                    containsActor = true;
                }
                if (input.ContainsKey("object") || input.ContainsKey("Object"))
                {
                    containsObject = true;
                }
                if (input.ContainsKey("verb") || input.ContainsKey("Verb"))
                {
                    containsVerb = true;
                }
                if (false == containsActor || false == containsObject || false == containsVerb)
                {
                    return isCorrect;
                }

                //check to make sure they fit xAPI
                bool actorIsCorrect = StatementHandler.ActorIsCorrect(input["actor"]);
                bool verbIsCorrect = StatementHandler.VerbIsCorrect(input["verb"]);
                bool objectIsCorrect = StatementHandler.ObjectIsCorrect(input["object"]);
                if (false == actorIsCorrect || false == verbIsCorrect || false == objectIsCorrect)
                {
                    return isCorrect;
                }
                else
                {
                    isCorrect = true;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on statement check: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
            return isCorrect;
        }

        #region Statement id stamping (SDKV-19/20)
        /// <summary>
        /// FIX (SDKV-19/20): stamps the handoff <c>ReferenceUUID</c> into the
        /// statement's wire <c>id</c> so every emitted statement carries an
        /// xAPI 1.0.3 section 4.1.1 statement id (a UUID string). The ingest node
        /// dedupes retries on this wire id (it reads <c>"id"</c> then
        /// <c>"uuid"</c> and ignores the <c>"0"</c> / empty-GUID placeholders
        /// the typed <see cref="Models.xAPI.Statement"/> defaults used to
        /// emit), so host retries only become idempotent once the SDK stamps
        /// a real one. Idempotent by design: an <c>id</c> that is already a
        /// valid, non-empty GUID is left untouched, so re-serializing /
        /// re-emitting the same statement (a retry, or every UpdatePost of
        /// the run) carries the SAME id. Placeholders (missing, null, empty,
        /// the long-typed model default <c>0</c>, the empty GUID, or any
        /// non-GUID junk) are replaced with
        /// <see cref="StaticDetails.StaticDetails.ReferenceUUID"/> -- the same
        /// GUID used for the <c>{uuid}.json</c> handoff FILENAME on the PC
        /// path and the <c>ReferenceUUID</c> intent extra on Android, so the
        /// handoff key and the wire id always agree.
        /// </summary>
        /// <param name="statement">The factored statement JObject about to be emitted. Null-safe.</param>
        /// <returns>The same instance, with a stamped <c>id</c>, for call-site chaining.</returns>
        internal static JObject StampStatementId(JObject statement)
        {
            if (statement == null)
            {
                return null;
            }
            try
            {
                JToken idToken = statement["id"];
                string currentId = (idToken == null || idToken.Type == JTokenType.Null)
                    ? null
                    : idToken.ToString();
                Guid parsedId;
                bool alreadyStamped = Guid.TryParse(currentId, out parsedId) && parsedId != Guid.Empty;
                if (!alreadyStamped)
                {
                    statement["id"] = StaticDetails.StaticDetails.ReferenceUUID;
                }
            }
            catch (Exception ex)
            {
                StaticDetails.StaticDetails.Logger.Log(
                    Service.SimulationLogLevel.Error,
                    "StampStatementId: failed stamping the statement id onto the outgoing statement.",
                    ex);
            }
            return statement;
        }
        #endregion

        #region Authority interface



        #endregion
        #region version interface - should not be needed

        #endregion
        #region Actor
        /// <summary>
        /// Check if Actor exists
        /// </summary>
        internal static bool ActorIsCorrect(JToken input)
        {
            bool actorContainsNeededItems = false;
            try
            {
                //check if token is null
                if (input == null)
                {
                    return false;
                }
                //variables                               

                //test if id is already in system -- this SHOULD ALWAYS be the case.
                //JToken token = input["id"];

                //Check identifiers

                //Actor has needed items                
                if (input["uuid"] != null)
                {
                    actorContainsNeededItems = true;
                }
                else if (input["mbox"] != null || input["mbox_sha1sum"] != null || input["openid"] != null)
                {
                    actorContainsNeededItems = true;
                }
                else if (input["account"] != null)
                {
                    actorContainsNeededItems = StatementHandler.AccountIsCorrect(input["account"]);
                }
                else if (input["member"] != null)
                {
                    // MDM-T2 / BUGS.md: copy-paste bug fixed -- was passing input["account"]
                    // into MemberIsCorrect, which validated the wrong actor sub-field. Group
                    // actors (xAPI's `member` array shape) silently passed validation against
                    // the unrelated `account` field. See
                    // docs/AUDIT_MDM_INTEGRATION_ERROR_HANDLING.md gap C-02 / M-10.
                    actorContainsNeededItems = StatementHandler.MemberIsCorrect(input["member"]);
                }

            }
            catch (Exception ex)
            {
                StaticDetails.StaticDetails.Logger.Log(
                    Service.SimulationLogLevel.Error,
                    "ActorIsCorrect: validation threw.",
                    ex);
            }

            return actorContainsNeededItems;
        }

        /// <summary>
        /// MDM-T2 / SIM-T13 G2-7: replaced `throw new NotImplementedException()` with a
        /// real structural check. xAPI's Group actor shape uses a `member` array of
        /// inverse-functional-identifier objects (each containing mbox / mbox_sha1sum
        /// / openid / account / uuid). A minimally-valid Group has a non-empty member
        /// array; per-member validation is left to <see cref="ActorIsCorrect"/>
        /// recursion (Group members may themselves be agents that fail
        /// validation, but that's a deeper check we don't enforce today).
        /// </summary>
        internal static bool MemberIsCorrect(JToken input)
        {
            if (input == null) return false;
            // Expected shape: JArray of agent-like objects. Some emitters use a JObject
            // with a `member` array inside; tolerate both rather than reject.
            JArray members = null;
            if (input.Type == JTokenType.Array)
            {
                members = (JArray)input;
            }
            else if (input.Type == JTokenType.Object && input["member"]?.Type == JTokenType.Array)
            {
                members = (JArray)input["member"];
            }
            if (members == null || members.Count == 0)
            {
                StaticDetails.StaticDetails.Logger.Log(
                    Service.SimulationLogLevel.Warn,
                    "MemberIsCorrect: actor declared a `member` shape but the array is empty or wrong type. Treating actor as invalid.");
                return false;
            }
            return true;
        }

        /// <summary>
        /// MDM-T2 / SIM-T13 G2-7: replaced `throw new NotImplementedException()` with a
        /// real structural check. xAPI's Account inverse-functional identifier is an
        /// object containing `homePage` (IRI) + `name` (string). Both are required;
        /// neither can be empty per the spec.
        /// </summary>
        internal static bool AccountIsCorrect(JToken input)
        {
            if (input == null) return false;
            if (input.Type != JTokenType.Object)
            {
                StaticDetails.StaticDetails.Logger.Log(
                    Service.SimulationLogLevel.Warn,
                    "AccountIsCorrect: actor declared an `account` shape but value isn't an object. Treating actor as invalid.");
                return false;
            }
            string homePage = (string)input["homePage"] ?? (string)input["homepage"];
            string name = (string)input["name"];
            bool ok = !string.IsNullOrEmpty(homePage) && !string.IsNullOrEmpty(name);
            if (!ok)
            {
                StaticDetails.StaticDetails.Logger.Log(
                    Service.SimulationLogLevel.Warn,
                    "AccountIsCorrect: account requires both homePage + name; treating actor as invalid. homePage=" +
                    (homePage ?? "<null>") + " name=" + (name ?? "<null>"));
            }
            return ok;
        }
        #endregion
        #region Verb   
        /// <summary>
        /// id is the only thing required for verb but it needs to be a verb set up automatically from the febris launcher
        /// </summary>
        /// <param name="input"></param>
        /// <returns></returns>
        internal static bool VerbIsCorrect(JToken input)
        {
            bool verbContainsNeededItems = false;
            try
            {
                //check if token is null
                if (input == null)
                {
                    return false;
                }
                //variables
                if (!string.IsNullOrEmpty((string)input["id"]))
                //if ((string)input["id"] == VerbIRIResolver.ResolveVerbIRI(Enums.VerbEnums.Initialized)
                //    || (string)input["id"] == VerbIRIResolver.ResolveVerbIRI(Enums.VerbEnums.Attempted))
                {
                    verbContainsNeededItems = true;
                }
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }

            return verbContainsNeededItems;
        }
        //internal static bool VerbUpdate(JToken input, VerbStage verbStage)
        //{
        //    bool verbUpdated = false;            

        //    switch (verbStage)
        //    {
        //        case VerbStage.Completed:
        //            //route to a function that processes data
        //            break;
        //        case VerbStage.Terminated:
        //            //route to a function that processes data
        //            break;
        //        default:
        //            Console.WriteLine("This function did not modify anything");
        //            break;
        //    }

        //    return verbUpdated;
        //}

        //internal static bool VerbUpdate(VerbStage verbStage)
        //{
        //    bool verbUpdated = false;
        //    //get verb
        //    JObject statement = StaticDetails.StaticDetails.StatementJObject;
        //    JToken verbToken = statement["verb"]["id"];

        //    switch (verbStage)
        //    {
        //        case VerbStage.Completed:
        //            //route to a function that processes data
        //            break;
        //        case VerbStage.Terminated:
        //            //route to a function that processes data
        //            break;
        //        default:
        //            Console.WriteLine("This function did not modify anything");
        //            break;
        //    }
        //    return verbUpdated;
        //}
        /// <summary>
        /// Update your verb manually using the VerbEnums.
        /// Removed the auto calculation feature for more transparancy in Verb setting.
        /// FIX (SDKV-3): the completion read goes through <see cref="TokenToBool"/>
        /// since result.completion is now a genuine JSON Boolean.
        /// </summary>
        /// <param name="verbEnum"></param>
        /// <returns></returns>
        public static bool VerbUpdate(VerbEnums verbEnum)
        {
            bool updated = false;
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                statement["verb"]["key"] = null;
                statement["verb"]["uuid"] = null;
                statement["verb"]["display"] = null;
                if (TokenToBool(statement["result"]["completion"]))
                {
                    switch (verbEnum)
                    {
                        case VerbEnums.Attempted:
                            //VerbUpdate(VerbEnums.Attempted, VerbStage.Completed, false);
                            statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Attempted);
                            break;
                        case VerbEnums.Completed:
                            statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Completed);
                            break;
                        case VerbEnums.Initialized:
                            statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Initialized);
                            break;
                        //calculated pass and fail
                        //if (statement["result"]["success"].ToString() == "true")
                        //{
                        //    statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Pass);
                        //}
                        //else
                        //{
                        //    statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Not_Pass);
                        //}
                        //break;
                        case VerbEnums.Terminated:
                            statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Terminated);
                            break;
                        case VerbEnums.Pass:
                            statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Pass);
                            break;
                        case VerbEnums.Not_Pass:
                            statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Not_Pass);
                            break;
                        default:
                            // Handle bad URL, possibly throw
                            throw new Exception();
                    }
                }
                else //if it is not complete
                {
                    switch (verbEnum)
                    {
                        case VerbEnums.Attempted:
                            //VerbUpdate(VerbEnums.Attempted, VerbStage.Completed, false);
                            statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Attempted);
                            break;
                        case VerbEnums.Completed:
                            statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Completed);
                            break;
                        case VerbEnums.Initialized:
                            statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Initialized);
                            //calculated pass and fail
                            //if (statement["result"]["success"].ToString() == "true")
                            //{
                            //    statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Pass);
                            //}
                            //else
                            //{
                            //    statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Not_Pass);
                            //}
                            break;
                        case VerbEnums.Terminated:
                            statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Terminated);
                            break;
                        case VerbEnums.Pass:
                            statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Pass);
                            break;
                        case VerbEnums.Not_Pass:
                            statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Not_Pass);
                            break;
                        default:
                            // Handle bad URL, possibly throw
                            throw new Exception();
                    }
                    //switch (verbEnum)
                    //{
                    //    case VerbEnums.Attempted:
                    //        //nothing needed
                    //        break;
                    //    case VerbEnums.Completed:
                    //        //nothing needed
                    //        break;
                    //    case VerbEnums.Initialized:
                    //        statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Terminated);
                    //        break;
                    //    case VerbEnums.Terminated:
                    //        //nothing needed
                    //        break;
                    //    case VerbEnums.Pass:
                    //        //this should not be an options
                    //        break;
                    //    case VerbEnums.Not_Pass:
                    //        //should not be an option
                    //        break;
                    //    default:
                    //        // Handle bad URL, possibly throw
                    //        throw new Exception();
                    //}
                }
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
            return updated;
        }
        /// <summary>
        /// Auto-calculating verb update used by EndSimulation.
        /// FIX (SDKV-3): completion/success reads go through
        /// <see cref="TokenToBool"/> since both are genuine JSON Booleans now
        /// (legacy string "true"/"false" values still parse).
        /// </summary>
        internal static JObject VerbUpdate(JObject statement, VerbEnums verbEnum)
        {
            try
            {
                statement["verb"]["key"] = null;
                statement["verb"]["uuid"] = null;
                statement["verb"]["display"] = null;

                if (TokenToBool(statement["result"]["completion"]))
                {
                    switch (verbEnum)
                    {
                        case VerbEnums.Attempted:
                            //VerbUpdate(VerbEnums.Attempted, VerbStage.Completed, false);
                            statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Completed);
                            break;
                        case VerbEnums.Completed:
                            //Already marked as completed                        
                            break;
                        case VerbEnums.Initialized:
                            //calculated pass and fail
                            if (TokenToBool(statement["result"]["success"]))
                            {
                                statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Pass);
                            }
                            else
                            {
                                statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Not_Pass);
                            }
                            break;
                        case VerbEnums.Terminated:
                            //nothing needed                        
                            break;
                        case VerbEnums.Pass:
                            //nothing needed
                            break;
                        case VerbEnums.Not_Pass:
                            //nothing needed
                            break;
                        default:
                            // Handle bad URL, possibly throw
                            throw new Exception();
                    }
                }
                else //if it is not complete
                {
                    switch (verbEnum)
                    {
                        case VerbEnums.Attempted:
                            //nothing needed
                            break;
                        case VerbEnums.Completed:
                            //nothing needed
                            break;
                        case VerbEnums.Initialized:
                            statement["verb"]["id"] = VerbIRIResolver.ResolveVerbIRI(VerbEnums.Terminated);
                            break;
                        case VerbEnums.Terminated:
                            //nothing needed
                            break;
                        case VerbEnums.Pass:
                            //this should not be an options
                            break;
                        case VerbEnums.Not_Pass:
                            //should not be an option
                            break;
                        default:
                            // Handle bad URL, possibly throw
                            throw new Exception();
                    }
                }
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }

            return statement;
        }

        /// <summary>
        /// FIX (SDKV-3): tolerant boolean read for statement tokens.
        /// result.success/completion are genuine JSON Booleans now, but
        /// statements written by older SDK builds may still carry the
        /// lowercase strings "true"/"false" -- both parse; anything else
        /// (null, missing, junk) reads as false.
        /// </summary>
        internal static bool TokenToBool(JToken token)
        {
            if (token == null || token.Type == JTokenType.Null)
            {
                return false;
            }
            if (token.Type == JTokenType.Boolean)
            {
                return (bool)token;
            }
            bool parsed;
            return bool.TryParse(token.ToString(), out parsed) && parsed;
        }
        #endregion
        #region Object
        /// <summary>
        /// For the object only an Id is required
        /// </summary>
        /// <param name="input"></param>
        /// <returns></returns>
        internal static bool ObjectIsCorrect(JToken input)
        {
            if (input == null)
            {
                return false;
            }
            bool exists = false;
            try
            {
                if (input["id"] != null/*&& input["objecttype"] != null && input["definition"] != null*/)
                {
                    //bool objectDefinitionIsCorrect = ObjectDefinitionIsCorrect(input["definition"]);
                    //if (objectDefinitionIsCorrect)
                    //{
                    exists = true;
                    //}

                }
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
            return exists;
        }
        internal static bool ObjectDefinitionIsCorrect(JToken input)
        {
            bool exists = false;
            try { }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
            //if ()
            //{

            //}
            return exists;
        }


        #endregion
        #region Result
        #region Generic Result       
        internal static void Result(string input, ResultOptions resultType)
        {
            try
            {
                //could use an enum for routing
                switch (resultType)
                {
                    case ResultOptions.Response:
                        UpdateResponseStatus(input);
                        break;
                    //case ResultOptions.Extensions:
                    //    UpdateResultExtensions(input);
                    //    break;
                    //case ResultOptions.Duration:
                    //    break;
                    //case default:
                    //    break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on result update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }

            //xapi result > Score
        }
        internal static void Result(float input, ResultOptions resultType)
        {
            try
            {
                //could use an enum for routing
                switch (resultType)
                {
                    case ResultOptions.ScoreMin:
                        UpdateMinScore(input);
                        break;
                    case ResultOptions.ScoreMax:
                        UpdateMaxScore(input);
                        break;
                    case ResultOptions.ScoreScale:
                        UpdateScaleScore(input);
                        break;
                    case ResultOptions.ScoreRaw:
                        UpdateRawScore(input);
                        break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on result update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
            //xapi result > Score
        }
        internal static void Result(bool input, ResultOptions resultType)
        {
            try
            {
                //could use an enum for routing
                switch (resultType)
                {
                    case ResultOptions.Success:
                        UpdateSuccessStatus(input);
                        break;
                    case ResultOptions.Completion:
                        UpdateCompletionStatus(input);
                        break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                }
                //xapi result > Score
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on result update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        internal static void Result(TimeSpan input, ResultOptions resultType)
        {
            try
            {
                //could use an enum for routing
                switch (resultType)
                {
                    case ResultOptions.Duration:
                        UpdateDuration(input);
                        break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                }
                //xapi result > Score
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on result update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        internal static void Result(int input, ResultOptions resultType, ResultExtensionOptions resultExtensionOptions)
        {
            try
            {
                //could use an enum for routing
                switch (resultType)
                {
                    case ResultOptions.Extensions:
                        if (resultExtensionOptions == ResultExtensionOptions.RestartCounter)
                        {
                            string counterIRI = ExtensionIRIResolver.ResolveExtensionIRI(ExtensionIRIOptions.RestartCounterIRI);
                            UpdateResultExtensions(counterIRI, input.ToString());
                        }
                        break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;

                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on result update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
            //xapi result > Score
        }
        internal static void Result(string input, ResultOptions resultType, ResultExtensionOptions resultExtensionOptions)
        {
            try
            {
                //could use an enum for routing
                switch (resultType)
                {
                    case ResultOptions.Extensions:
                        switch (resultExtensionOptions)
                        {
                            case ResultExtensionOptions.Notes:
                                string notesIRI = ExtensionIRIResolver.ResolveExtensionIRI(ExtensionIRIOptions.NotesIRI);
                                UpdateResultExtensions(notesIRI, input.ToString());
                                break;
                            case ResultExtensionOptions.RestartCounter:
                                string counterIRI = ExtensionIRIResolver.ResolveExtensionIRI(ExtensionIRIOptions.RestartCounterIRI);
                                UpdateResultExtensions(counterIRI, input.ToString());
                                break;
                        }

                        break;
                    default:
                        // SIM-T13 G6: surface unsupported XAPIProperties routes through the logger
                        // so silent no-ops become visible. The Result/Context/Attachments cases
                        // above are the only branches that do anything; everything else hits
                        // here. Game-engine authors who pass e.g. XAPIProperties.Actor will see
                        // a Warn line instead of a black hole.
                        StaticDetails.StaticDetails.Logger.Log(
                            SimulationLogLevel.Warn,
                            "UpdateStatement: no-op default branch hit -- the supplied XAPIProperties value isn't supported by this overload. See SIM-T13 G6 in TIER_13_AUDIT_MDM_COMPAT.md.");
                        break;
                        //case ResultOptions.Duration:
                        //    break;
                        //case default:
                        //    break;

                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error on result update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }

            //xapi result > Score
        }
        #endregion
        #region update result   
        /// <summary>
        /// Writes result.duration from a running Stopwatch.
        /// FIX (SDKV-5): serialize the elapsed TimeSpan as ISO 8601 via
        /// <see cref="XmlConvert"/> -- the old <c>elapsedTime.ToString()</c>
        /// stringified the Stopwatch OBJECT (type name, not a duration at
        /// all), and even <c>Elapsed.ToString()</c> would emit the .NET
        /// clock format that xAPI 1.0.3 section 4.1.5 rejects.
        /// </summary>
        internal static void UpdateDuration(Stopwatch elapsedTime)
        {
            try
            {
                //xapi result > duration
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                //bool exists = CheckProperty(statement["result"], "result", "duration"); // this is wrong because it only checks in result exists
                //if (!exists)
                //{
                //    statement = CreateProperty(statement, "result", "duration");
                //}

                statement["result"]["duration"] = XmlConvert.ToString(elapsedTime.Elapsed);
                //statement["result"]["duration"] = elapsedTime.ToString(); // [Historical - SDKV-5] wrote the Stopwatch type name, not a duration
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        internal static void UpdateDuration(TimeSpan elapsedTime)
        {
            try
            {
                //xapi result > duration
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                //bool exists = CheckProperty(statement["result"], "result", "duration"); // this is wrong because it only checks in result exists
                //if (!exists)
                //{
                //    statement = CreateProperty(statement, "result", "duration");
                //}

                statement["result"]["duration"] = XmlConvert.ToString(elapsedTime);
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }

        }
        internal static JObject UpdateDuration(JObject statement, TimeSpan elapsedTime)
        {
            try
            {
                statement["result"]["duration"] = XmlConvert.ToString(elapsedTime);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
            return statement;
        }
        internal static void UpdateRawScore(float score)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                statement = AutoUpdateScaledScore(statement, score);
                statement["result"]["score"]["raw"] = score;
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        internal static JObject UpdateRawScore(JObject statement, float score)
        {
            try
            {
                statement = AutoUpdateScaledScore(statement, score);
                statement["result"]["score"]["raw"] = score;
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
            return statement;
        }
        internal static void UpdateScaleScore(float score)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                //bool exists = CheckProperty(statement["result"], "result", "score", "scalescore");
                //if (!exists)
                //{
                //    statement = CreateProperty(statement, "result", "score", "scalescore");
                //}
                //xApi result > Success
                statement["result"]["score"]["scaled"] = score;
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        internal static JObject UpdateScaleScore(JObject statement, float score)
        {
            try
            {
                statement["result"]["score"]["scaled"] = score;
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
            return statement;
        }
        /// <summary>
        /// MDM-T2 G2-8 / BUGS.md: previously divided <paramref name="rawScore"/>
        /// by <c>statement.result.score.max</c> with no guard. When max was
        /// missing (default 0), the division produced <see cref="float.PositiveInfinity"/>
        /// and the library wrote that into <c>result.score.scaled</c>, corrupting
        /// downstream LMS reporting. xAPI requires scaled to be in
        /// <c>[-1.0, 1.0]</c>. Also no NaN guard.
        ///
        /// Now: validates that max is positive and finite; logs a Warn if not
        /// and skips the scaled update (leaving the existing value in place).
        /// Clamps the final result to <c>[-1.0, 1.0]</c> per spec.
        /// </summary>
        internal static JObject AutoUpdateScaledScore(JObject statement, float rawScore)
        {
            try
            {
                // The max field can be missing entirely (token is null) or present
                // but unset (default 0). Guard both. (float)null throws an
                // InvalidCastException so the null-check has to precede the cast.
                JToken maxToken = statement?["result"]?["score"]?["max"];
                if (maxToken == null || maxToken.Type == JTokenType.Null)
                {
                    StaticDetails.StaticDetails.Logger.Log(
                        Service.SimulationLogLevel.Warn,
                        "AutoUpdateScaledScore: result.score.max is not set; skipping scaled-score update. " +
                        "Game should call UpdateMaxScore before UpdateRawScore.");
                    return statement;
                }

                float maxScore = (float)maxToken;
                if (maxScore <= 0f || float.IsNaN(maxScore) || float.IsInfinity(maxScore))
                {
                    StaticDetails.StaticDetails.Logger.Log(
                        Service.SimulationLogLevel.Warn,
                        "AutoUpdateScaledScore: result.score.max=" + maxScore +
                        " is non-positive / NaN / Infinity; skipping scaled-score update.");
                    return statement;
                }

                if (float.IsNaN(rawScore) || float.IsInfinity(rawScore))
                {
                    StaticDetails.StaticDetails.Logger.Log(
                        Service.SimulationLogLevel.Warn,
                        "AutoUpdateScaledScore: rawScore=" + rawScore +
                        " is NaN / Infinity; skipping scaled-score update.");
                    return statement;
                }

                float scaledScore = rawScore / maxScore;

                // Clamp to xAPI's [-1.0, 1.0] range. Out-of-range usually means
                // raw exceeds max (over-100% scores are common in some scoring
                // schemes); clamp + Warn so the data still lands somewhere sane.
                if (scaledScore < -1f || scaledScore > 1f)
                {
                    StaticDetails.StaticDetails.Logger.Log(
                        Service.SimulationLogLevel.Warn,
                        "AutoUpdateScaledScore: computed scaled=" + scaledScore +
                        " is outside the xAPI [-1.0, 1.0] range; clamping. " +
                        "rawScore=" + rawScore + " maxScore=" + maxScore);
                    scaledScore = scaledScore < -1f ? -1f : 1f;
                }

                statement["result"]["score"]["scaled"] = scaledScore;
            }
            catch (Exception ex)
            {
                StaticDetails.StaticDetails.Logger.Log(
                    Service.SimulationLogLevel.Error,
                    "AutoUpdateScaledScore: unexpected exception updating scaled score.",
                    ex);
            }
            return statement;
        }
        internal static void UpdateMinScore(float score)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                //bool exists = CheckProperty(statement["result"], "result", "score", "minscore");
                //if (!exists)
                //{
                //    statement = CreateProperty(statement, "result", "score", "minscore");
                //}
                //xAPi result > completion
                statement["result"]["score"]["min"] = score;
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        internal static JObject UpdateMinScore(JObject statement, float score)
        {
            try
            {
                statement["result"]["score"]["min"] = score;
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
            return statement;
        }
        internal static void UpdateMaxScore(float score)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                //bool exists = CheckProperty(statement["result"], "result", "score", "maxscore");
                //if (!exists)
                //{
                //    statement = CreateProperty(statement, "result", "score", "maxscore");
                //}
                //xapi result > response
                statement["result"]["score"]["max"] = score;
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }

        }
        internal static JObject UpdateMaxScore(JObject statement, float score)
        {
            try
            {
                statement["result"]["score"]["max"] = score;
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
            return statement;
        }
        /// <summary>
        /// Writes result.completion on the current statement.
        /// FIX (SDKV-3/11): JSON Boolean, not the lowercase string.
        /// </summary>
        internal static void UpdateCompletionStatus(bool status)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                //bool exists = CheckProperty(statement["result"], "result", "completion");
                //if (!exists)
                //{
                //    statement = CreateProperty(statement, "result", "completion");
                //}
                //xapi result > extensions
                statement["result"]["completion"] = status;
                //statement["result"]["completion"] = status.ToString().ToLower(); // [Historical - SDKV-3] string-typed boolean
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// Writes result.completion on the supplied statement.
        /// FIX (SDKV-3/11): JSON Boolean, not the lowercase string.
        /// </summary>
        internal static JObject UpdateCompletionStatus(JObject statement, bool status)
        {
            try
            {
                statement["result"]["completion"] = status;
                //statement["result"]["completion"] = status.ToString().ToLower(); // [Historical - SDKV-3] string-typed boolean
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
            return statement;
        }
        /// <summary>
        /// Writes result.success on the current statement.
        /// FIX (SDKV-3/11): JSON Boolean, not the lowercase string.
        /// </summary>
        internal static void UpdateSuccessStatus(bool status)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                statement["result"]["success"] = status;
                //statement["result"]["success"] = status.ToString().ToLower(); // [Historical - SDKV-3] string-typed boolean
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// Writes result.success on the supplied statement.
        /// FIX (SDKV-3/11): JSON Boolean, not the lowercase string.
        /// </summary>
        internal static JObject UpdateSuccessStatus(JObject statement, bool status)
        {
            try
            {
                statement["result"]["success"] = status;
                //statement["result"]["success"] = status.ToString().ToLower(); // [Historical - SDKV-3] string-typed boolean
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
            return statement;
        }
        internal static void UpdateResponseStatus(string input)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                //bool exists = CheckProperty(statement["result"], "result", "response");
                //if (!exists)
                //{
                //    statement = CreateProperty(statement, "result", "response");
                //}
                //xapi result > extensions
                statement["result"]["response"] = input.ToString();
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        internal static void UpdateResultExtensions(string key, string input)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;

                string extensionMap = (string)statement["result"]["extensions"]["extensionmap"] ?? string.Empty;
                string[] extensionMapArray = extensionMap.Split(',');
                string newExtensionMap = string.Empty;
                bool keyCurrentlyExists = false;
                if (extensionMap == string.Empty)
                {
                    newExtensionMap = key + ":" + input;
                    keyCurrentlyExists = true;
                }
                else
                {
                    for (var i = 0; i < extensionMapArray.Length; i++)
                    {

                        string[] extensionSingle = extensionMapArray[i].Split(':');

                        // MDM-T2 C-03: bounds-guard. Skip malformed entries (must have key:value:count shape).
                        if (extensionSingle.Length < 3)
                        {
                            StaticDetails.StaticDetails.Logger.Log(
                                Service.SimulationLogLevel.Warn,
                                "UpdateResultExtensions: skipping malformed extensionmap entry (need 3 colon-separated parts, got " +
                                extensionSingle.Length + "): " + extensionMapArray[i]);
                            continue;
                        }

                        if (extensionSingle[0] + ":" + extensionSingle[1] == key)
                        {
                            keyCurrentlyExists = true;
                            ExtensionIRIOptions extensionEnum = ExtensionIRIResolver.ResolveExtensionIRI(key);
                            switch (extensionEnum)
                            {
                                case ExtensionIRIOptions.NotesIRI:
                                    extensionSingle[2] += "|" + input;
                                    break;
                                case ExtensionIRIOptions.RestartCounterIRI:
                                    extensionSingle[2] = input;
                                    break;
                            }

                            newExtensionMap += extensionSingle[0] + ":" + extensionSingle[1] + ":" + extensionSingle[2];
                        }
                        else
                        {
                            newExtensionMap += extensionSingle[0] + ":" + extensionSingle[1] + ":" + extensionSingle[2];
                        }

                        if (i + 1 < extensionMapArray.Length)
                        {
                            newExtensionMap += ",";
                        }

                    }
                }
                if (!keyCurrentlyExists)
                {
                    newExtensionMap += "," + key + ":" + input;
                }

                Console.WriteLine(newExtensionMap);
                statement["result"]["extensions"]["extensionmap"] = newExtensionMap;
                //write statement
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        #endregion
        #endregion
        #region Context

        #region General Context

        internal static void Context(ContextOptions contextOption, string input)
        {
            //JObject statement = StaticDetails.StaticDetails.CurrentStatement;
            //get the statement jobject
            switch (contextOption)
            {
                case ContextOptions.Registration:
                    UpdatecontextRegistration(input);
                    break;
                case ContextOptions.Revision:
                    UpdateContextRevision(input);
                    break;
                case ContextOptions.Platform:
                    UpdateContextPlatform(input);
                    break;
                case ContextOptions.Language:
                    UpdateContextLanguage(input);
                    break;
                default:
                    Console.WriteLine("This method has done nothing");
                    break;

            }

        }

        internal static void Context(ContextOptions contextOption, ContextActivitesOptions contextActivitesOption, string input)
        {
            switch (contextOption)
            {
                case ContextOptions.ContextActivites:
                    ContextActivity(contextActivitesOption, input);
                    break;
                //case ContextOptions.Revision:
                //    break;
                //case ContextOptions.Platform:
                //    break;
                //case ContextOptions.Language:
                //    break;
                default:
                    Console.WriteLine("This method has done nothing");
                    break;

            }
        }
        internal static void ContextActivity(ContextActivitesOptions option, string input)
        {
            //JObject statement = StaticDetails.StaticDetails.CurrentStatement;
            switch (option)
            {
                case ContextActivitesOptions.Parent:
                    UpdateContextActivityParent(input);
                    break;
                case ContextActivitesOptions.Grouping:
                    UpdateContextActivityGrouping(input);
                    break;
                case ContextActivitesOptions.Category:
                    UpdateContextActivityCategory(input);
                    break;
                case ContextActivitesOptions.Other:
                    UpdateContextActivityOther(input);
                    break;
                default:
                    Console.WriteLine("This method has done nothing");
                    break;
            }
        }

        internal static void Context(ContextOptions contextOption, ContextStatementReferenceOptions contextStatementReferenceOption, string input)
        {
            switch (contextOption)
            {
                case ContextOptions.StatementReference:
                    ContextStatementReferenceOption(contextStatementReferenceOption, input);
                    break;
                //case ContextOptions.Revision:
                //    break;
                //case ContextOptions.Platform:
                //    break;
                //case ContextOptions.Language:
                //    break;
                default:
                    Console.WriteLine("This method has done nothing");
                    break;

            }
        }
        internal static void ContextStatementReferenceOption(ContextStatementReferenceOptions option, string input)
        {
            switch (option)
            {
                case ContextStatementReferenceOptions.ObjectType:
                    UpdateContextStatementReferenceObjectType(input);
                    break;
                case ContextStatementReferenceOptions.Id:
                    UpdateContextStatementReferenceId(input);
                    break;
                default:
                    Console.WriteLine("This method has done nothing");
                    break;
            }
        }

        internal static void Context(ContextOptions contextOption, ContextExtensionOptions contextExtensionOption, string input)
        {
            switch (contextOption)
            {
                case ContextOptions.Extensions:
                    ContextExtensionOption(contextExtensionOption, input);
                    break;
                //case ContextOptions.Revision:
                //    break;
                //case ContextOptions.Platform:
                //    break;
                //case ContextOptions.Language:
                //    break;
                default:
                    Console.WriteLine("This method has done nothing");
                    break;

            }
        }
        internal static void ContextExtensionOption(ContextExtensionOptions option, string input)
        {
            string key = ContextOptionResolver.ContextExtensionOptionResolver(option);
            switch (option)
            {
                case ContextExtensionOptions.Option1:
                    UpdateContextExtensions(key, input);
                    break;
                case ContextExtensionOptions.Option2:
                    UpdateContextExtensions(key, input);
                    break;
                default:
                    Console.WriteLine("This method has done nothing");
                    break;
            }
        }
        #endregion

        #region Update Context
        private static void UpdatecontextRegistration(string input)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                statement["context"]["registration"] = input;
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// Writes context.contextActivities.parent on the current statement.
        /// FIX (SDKV-4): the old key "contextactivites" (missing 'i') never
        /// exists in the factored statement, so the write NRE'd into the
        /// catch and every context-activity update was a silent no-op. The
        /// factored statement carries "contextActivities" (spec casing via
        /// the Context model's [JsonProperty]).
        /// </summary>
        private static void UpdateContextActivityParent(string input)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                statement["context"]["contextActivities"]["parent"] = input;
                //statement["context"]["contextactivites"]["parent"] = input; // [Historical - SDKV-4] typo key, never present
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// Writes context.contextActivities.grouping on the current statement.
        /// FIX (SDKV-4): see <see cref="UpdateContextActivityParent"/>.
        /// </summary>
        private static void UpdateContextActivityGrouping(string input)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                statement["context"]["contextActivities"]["grouping"] = input;
                //statement["context"]["contextactivites"]["grouping"] = input; // [Historical - SDKV-4] typo key, never present
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// Writes context.contextActivities.category on the current statement.
        /// FIX (SDKV-4): see <see cref="UpdateContextActivityParent"/>.
        /// </summary>
        private static void UpdateContextActivityCategory(string input)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                statement["context"]["contextActivities"]["category"] = input;
                //statement["context"]["contextactivites"]["category"] = input; // [Historical - SDKV-4] typo key, never present
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// Writes context.contextActivities.other on the current statement.
        /// FIX (SDKV-4): see <see cref="UpdateContextActivityParent"/>.
        /// </summary>
        private static void UpdateContextActivityOther(string input)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                statement["context"]["contextActivities"]["other"] = input;
                //statement["context"]["contextactivites"]["other"] = input; // [Historical - SDKV-4] typo key, never present
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        private static void UpdateContextInstructor(string input)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                //this will need to be an actor
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        private static void UpdateContextGroup(string input)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                //this is a group of actors
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        private static void UpdateContextRevision(string input)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                statement["context"]["revision"] = input;
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        private static void UpdateContextPlatform(string input)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                statement["context"]["platform"] = input;
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        private static void UpdateContextLanguage(string input)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                statement["context"]["language"] = input;
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        private static void UpdateContextStatementReferenceId(string input)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                statement["context"]["statementreference"]["id"] = input;
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// Writes the StatementRef discriminator. FIX (SDKV-12): the factored
        /// statement now carries spec-cased "objectType" inside the dialect
        /// "statementreference" node.
        /// </summary>
        private static void UpdateContextStatementReferenceObjectType(string input)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                statement["context"]["statementreference"]["objectType"] = input;
                //statement["context"]["statementreference"]["objecttype"] = input; // [Historical - SDKV-12] pre-spec-casing key
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing update: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        private static void UpdateContextExtensions(string key, string input)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                string extensionMap = (string)statement["context"]["extensions"]["extensionmap"] ?? string.Empty;
                string[] extensionMapArray = extensionMap.Split(',');
                string newExtensionMap = string.Empty;
                bool keyCurrentlyExists = false;
                if (extensionMap == string.Empty)
                {
                    newExtensionMap = key + ":" + input;
                    keyCurrentlyExists = true;
                }
                else
                {
                    for (var i = 0; i < extensionMapArray.Length; i++)
                    {

                        string[] extensionSingle = extensionMapArray[i].Split(':');

                        // MDM-T2 C-03: bounds-guard. Skip malformed entries.
                        if (extensionSingle.Length < 3)
                        {
                            StaticDetails.StaticDetails.Logger.Log(
                                Service.SimulationLogLevel.Warn,
                                "UpdateContextExtensions: skipping malformed extensionmap entry (need 3 colon-separated parts, got " +
                                extensionSingle.Length + "): " + extensionMapArray[i]);
                            continue;
                        }

                        if (extensionSingle[0] + ":" + extensionSingle[1] == key)
                        {
                            keyCurrentlyExists = true;
                            extensionSingle[2] = input;
                            newExtensionMap += extensionSingle[0] + ":" + extensionSingle[1] + ":" + extensionSingle[2];
                        }
                        else
                        {
                            //newExtensionMap += extensionMapArray[i] +",";
                            newExtensionMap += extensionSingle[0] + ":" + extensionSingle[1] + ":" + extensionSingle[2];
                        }

                        if (i + 1 < extensionMapArray.Length)
                        {
                            newExtensionMap += ",";
                        }

                    }
                }
                if (!keyCurrentlyExists)
                {
                    newExtensionMap += "," + key + ":" + input;
                }
                Console.WriteLine(newExtensionMap);
                statement["context"]["extensions"]["extensionmap"] = newExtensionMap;
                //write statement
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writting content extension: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }


        #endregion
        #endregion
        #region Authority
        #region General Authority
        #endregion
        #region Update Authority
        #endregion
        #endregion
        #region Version
        //I don't think this is really needed at this time
        #endregion
        #region Attachments
        /// Required: UsageType, Display, contenttype, length, sha2
        /// optional: fileUrl, description
        /// <summary>
        /// Adds an attachment (full shape). FIX (SDKV-2): the caller's plain
        /// display/description strings are wrapped into Language Map objects
        /// (<c>{"en": value}</c>) via
        /// <see cref="StatementFactoring.StringToLanguageMap"/> so the wire
        /// carries xAPI 1.0.3 section 4.1.11-conformant objects.
        /// </summary>
        internal static void AddAttachment(string usageType, string display, string description, ContentType contentType, int length, string sha2, string fileUrl)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                Models.xAPI.Attachment attachment = new Models.xAPI.Attachment()
                {
                    UsageType = new Uri(usageType),
                    Display = StatementFactoring.StringToLanguageMap(display),
                    Description = StatementFactoring.StringToLanguageMap(description),
                    ContentType = ContentTypeResolver.ResolveExtensionIRI(contentType),
                    Length = length,
                    Sha2 = sha2,
                    FileURL = new Uri(fileUrl)
                };
                ProcessAttachmentJArray(statement, attachment);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing attachment: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// Adds an attachment (no fileUrl). FIX (SDKV-2): display/description
        /// wrapped into Language Map objects.
        /// </summary>
        internal static void AddAttachment(string usageType, string display, string description, ContentType contentType, int length, string sha2)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                Models.xAPI.Attachment attachment = new Models.xAPI.Attachment()
                {
                    UsageType = new Uri(usageType),
                    Display = StatementFactoring.StringToLanguageMap(display),
                    Description = StatementFactoring.StringToLanguageMap(description),
                    ContentType = ContentTypeResolver.ResolveExtensionIRI(contentType),
                    Length = length,
                    Sha2 = sha2
                };
                ProcessAttachmentJArray(statement, attachment);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing attachment: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// Adds an attachment (no description). FIX (SDKV-2): display wrapped
        /// into a Language Map object.
        /// </summary>
        internal static void AddAttachment(string usageType, string display, ContentType contentType, int length, string sha2, string fileUrl)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                Models.xAPI.Attachment attachment = new Models.xAPI.Attachment()
                {
                    UsageType = new Uri(usageType),
                    Display = StatementFactoring.StringToLanguageMap(display),
                    ContentType = ContentTypeResolver.ResolveExtensionIRI(contentType),
                    Length = length,
                    Sha2 = sha2,
                    FileURL = new Uri(fileUrl)
                };
                ProcessAttachmentJArray(statement, attachment);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing attachment: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        /// <summary>
        /// Adds an attachment (minimal shape). FIX (SDKV-2): display wrapped
        /// into a Language Map object.
        /// </summary>
        internal static void AddAttachment(string usageType, string display, ContentType contentType, int length, string sha2)
        {
            try
            {
                JObject statement = StaticDetails.StaticDetails.CurrentStatement;
                Models.xAPI.Attachment attachment = new Models.xAPI.Attachment()
                {
                    UsageType = new Uri(usageType),
                    Display = StatementFactoring.StringToLanguageMap(display),
                    ContentType = ContentTypeResolver.ResolveExtensionIRI(contentType),
                    Length = length,
                    Sha2 = sha2
                };
                ProcessAttachmentJArray(statement, attachment);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing attachment: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        private static void ProcessAttachmentJArray(JObject statement, Attachment attachment)
        {
            try
            {
                JObject attachmentObject = JObject.FromObject(attachment);
                JArray attachmentArray = new JArray();//JsonConvert.DeserializeObject<JArray>(statement["attachments"].ToString()));
                foreach (JObject attach in statement["attachments"])
                {
                    attachmentArray.Add(attach);
                }
                attachmentArray.Add(attachmentObject);
                statement["attachments"] = attachmentArray;
                StaticDetails.StaticDetails.CurrentStatement = statement;//SaveStatement(statement);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error writing attachment array: " + ex.Message);
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
            }
        }
        #region General Attachments
        #endregion
        #region Update Attachments
        #endregion
        #endregion



        #endregion



        #region ***************Old/Obsolete*************
        #region result handling interface
        #region only usable if we change jtoken to jobject
        //public static void UpdateStatement(XAPIProperties property, JToken input, ResultOptions resultType, bool boolInput)
        //{
        //    switch (property)
        //    {
        //        //case XAPIProperties.Actor:
        //        //    //this should not update
        //        //    break;
        //        //case XAPIProperties.Object:
        //        //    //this should not be updated
        //        //    break;
        //        //case XAPIProperties.Verb:
        //        //    //VerbUpdate(input);//may need more information
        //        //    break;
        //        //case XAPIProperties.Timestamp:
        //        //    //should already be set, if not set up on initalization
        //        //    break;
        //        case XAPIProperties.Result:                    
        //            Result(boolInput, resultType);
        //            break;
        //        //case XAPIProperties.Context:
        //        //    break;
        //        //case XAPIProperties.Authority:
        //        //    break;
        //        //case XAPIProperties.Version:
        //        //    break;
        //        //case XAPIProperties.Attachments:
        //        //    break;
        //        //case XAPIProperties.Account:
        //        //    break;
        //        //case XAPIProperties.Member:
        //        //    break;
        //        //case XAPIProperties.Definition:
        //        //    break;
        //        case XAPIProperties.Score:
        //            break;
        //        //case XAPIProperties.ContextActivities:
        //        //    break;
        //        //case XAPIProperties.StatementReference:
        //        //    break;
        //        //case XAPIProperties.Extensions:
        //        //    break;
        //        default:
        //            Console.WriteLine("This function did not modify anything");
        //            break;
        //    }
        //}
        //public static void UpdateStatement(XAPIProperties property, JToken input, ResultOptions resultType, string stringInput)
        //{
        //    ResultSwitch(property, resultType);

        //    switch (property)
        //    {
        //        //case XAPIProperties.Actor:
        //        //    //this should not update
        //        //    break;
        //        //case XAPIProperties.Object:
        //        //    //this should not be updated
        //        //    break;
        //        //case XAPIProperties.Verb:
        //        //    //VerbUpdate(input);//may need more information
        //        //    break;
        //        //case XAPIProperties.Timestamp:
        //        //    //should already be set, if not set up on initalization
        //        //    break;
        //        case XAPIProperties.Result:
        //            //need more information for this one
        //            Result(stringInput, resultType);
        //            break;
        //        //case XAPIProperties.Context:
        //        //    break;
        //        //case XAPIProperties.Authority:
        //        //    break;
        //        //case XAPIProperties.Version:
        //        //    break;
        //        //case XAPIProperties.Attachments:
        //        //    break;
        //        //case XAPIProperties.Account:
        //        //    break;
        //        //case XAPIProperties.Member:
        //        //    break;
        //        //case XAPIProperties.Definition:
        //        //    break;
        //        case XAPIProperties.Score:
        //            break;
        //        //case XAPIProperties.ContextActivities:
        //        //    break;
        //        //case XAPIProperties.StatementReference:
        //        //    break;
        //        //case XAPIProperties.Extensions:
        //        //    break;
        //        default:
        //            Console.WriteLine("This function did not modify anything");
        //            break;
        //    }
        //}
        //public static void UpdateStatement(XAPIProperties property, JToken input, ResultOptions resultType, float floatInput)
        //{
        //    switch (property)
        //    {
        //        //case XAPIProperties.Actor:
        //        //    //this should not update
        //        //    break;
        //        //case XAPIProperties.Object:
        //        //    //this should not be updated
        //        //    break;
        //        //case XAPIProperties.Verb:
        //        //    //VerbUpdate(input);//may need more information
        //        //    break;
        //        //case XAPIProperties.Timestamp:
        //        //    //should already be set, if not set up on initalization
        //        //    break;
        //        case XAPIProperties.Result:
        //            //need more information for this one
        //            Result(floatInput, resultType);
        //            break;
        //        //case XAPIProperties.Context:
        //        //    break;
        //        //case XAPIProperties.Authority:
        //        //    break;
        //        //case XAPIProperties.Version:
        //        //    break;
        //        //case XAPIProperties.Attachments:
        //        //    break;
        //        //case XAPIProperties.Account:
        //        //    break;
        //        //case XAPIProperties.Member:
        //        //    break;
        //        //case XAPIProperties.Definition:
        //        //    break;
        //        case XAPIProperties.Score:
        //            break;
        //        //case XAPIProperties.ContextActivities:
        //        //    break;
        //        //case XAPIProperties.StatementReference:
        //        //    break;
        //        //case XAPIProperties.Extensions:
        //        //    break;
        //        default:
        //            Console.WriteLine("This function did not modify anything");
        //            break;
        //    }
        //}
        #endregion





        #endregion
        #region verb handling interface
        //public static void UpdateStatement(XAPIProperties property, JToken input, VerbStage stage)
        //{
        //    switch (property)
        //    {
        //        case XAPIProperties.Actor:
        //            //this should not update
        //            break;
        //        case XAPIProperties.Object:
        //            //this should not be updated
        //            break;
        //        case XAPIProperties.Verb:
        //            VerbUpdate(input, stage);
        //            break;
        //        case XAPIProperties.Timestamp:
        //            //should already be set, if not set up on initalization
        //            break;
        //        case XAPIProperties.Result:
        //            //need more information for this one
        //            //Result(floatInput, resultType);
        //            break;
        //        case XAPIProperties.Context:
        //            break;
        //        case XAPIProperties.Authority:
        //            break;
        //        case XAPIProperties.Version:
        //            break;
        //        case XAPIProperties.Attachments:
        //            break;
        //        case XAPIProperties.Account:
        //            break;
        //        case XAPIProperties.Member:
        //            break;
        //        case XAPIProperties.Definition:
        //            break;
        //        case XAPIProperties.Score:
        //            break;
        //        case XAPIProperties.ContextActivities:
        //            break;
        //        case XAPIProperties.StatementReference:
        //            break;
        //        case XAPIProperties.Extensions:
        //            break;
        //        default:
        //            Console.WriteLine("This function did not modify anything");
        //            break;
        //    }
        //}
        //public static void UpdateStatement(XAPIProperties property, VerbStage stage)
        //{
        //    switch (property)
        //    {
        //        case XAPIProperties.Actor:
        //            //this should not update
        //            break;
        //        case XAPIProperties.Object:
        //            //this should not be updated
        //            break;
        //        case XAPIProperties.Verb:
        //            VerbUpdate(stage);
        //            break;
        //        case XAPIProperties.Timestamp:
        //            //should already be set, if not set up on initalization
        //            break;
        //        case XAPIProperties.Result:
        //            //need more information for this one
        //            //Result(boolInput, resultType);
        //            break;
        //        case XAPIProperties.Context:
        //            break;
        //        case XAPIProperties.Authority:
        //            break;
        //        case XAPIProperties.Version:
        //            break;
        //        case XAPIProperties.Attachments:
        //            break;
        //        case XAPIProperties.Account:
        //            break;
        //        case XAPIProperties.Member:
        //            break;
        //        case XAPIProperties.Definition:
        //            break;
        //        case XAPIProperties.Score:
        //            break;
        //        case XAPIProperties.ContextActivities:
        //            break;
        //        case XAPIProperties.StatementReference:
        //            break;
        //        case XAPIProperties.Extensions:
        //            break;
        //        default:
        //            Console.WriteLine("This function did not modify anything");
        //            break;
        //    }
        //}
        //public static void UpdateStatement(JToken input, VerbStage stage)
        //{
        //    VerbUpdate(input, stage);
        //    //switch (property)
        //    //{
        //    //    case XAPIProperties.Actor:
        //    //        //this should not update
        //    //        break;
        //    //    case XAPIProperties.Object:
        //    //        //this should not be updated
        //    //        break;
        //    //    case XAPIProperties.Verb:
        //    //        VerbUpdate(input);//may need more information
        //    //        break;
        //    //    case XAPIProperties.Timestamp:
        //    //        //should already be set, if not set up on initalization
        //    //        break;
        //    //    case XAPIProperties.Result:
        //    //        //need more information for this one
        //    //        //Result(floatInput, resultType);
        //    //        break;
        //    //    case XAPIProperties.Context:
        //    //        break;
        //    //    case XAPIProperties.Authority:
        //    //        break;
        //    //    case XAPIProperties.Version:
        //    //        break;
        //    //    case XAPIProperties.Attachments:
        //    //        break;
        //    //    case XAPIProperties.Account:
        //    //        break;
        //    //    case XAPIProperties.Member:
        //    //        break;
        //    //    case XAPIProperties.Definition:
        //    //        break;
        //    //    case XAPIProperties.Score:
        //    //        break;
        //    //    case XAPIProperties.ContextActivities:
        //    //        break;
        //    //    case XAPIProperties.StatementReference:
        //    //        break;
        //    //    case XAPIProperties.Extensions:
        //    //        break;
        //    //}
        //}
        //public static void UpdateStatement(VerbStage stage)
        //{
        //    VerbUpdate(stage);
        //    //switch (property)
        //    //{
        //    //    case XAPIProperties.Actor:
        //    //        //this should not update
        //    //        break;
        //    //    case XAPIProperties.Object:
        //    //        //this should not be updated
        //    //        break;
        //    //    case XAPIProperties.Verb:
        //    //        //VerbUpdate(input);//may need more information
        //    //        break;
        //    //    case XAPIProperties.Timestamp:
        //    //        //should already be set, if not set up on initalization
        //    //        break;
        //    //    case XAPIProperties.Result:
        //    //        //need more information for this one
        //    //        //Result(boolInput, resultType);
        //    //        break;
        //    //    case XAPIProperties.Context:
        //    //        break;
        //    //    case XAPIProperties.Authority:
        //    //        break;
        //    //    case XAPIProperties.Version:
        //    //        break;
        //    //    case XAPIProperties.Attachments:
        //    //        break;
        //    //    case XAPIProperties.Account:
        //    //        break;
        //    //    case XAPIProperties.Member:
        //    //        break;
        //    //    case XAPIProperties.Definition:
        //    //        break;
        //    //    case XAPIProperties.Score:
        //    //        break;
        //    //    case XAPIProperties.ContextActivities:
        //    //        break;
        //    //    case XAPIProperties.StatementReference:
        //    //        break;
        //    //    case XAPIProperties.Extensions:
        //    //        break;
        //    //}
        //}
        #endregion
        #region obsolete
        //internal static JObject CreateJArray(JObject statement, string property)//this returns bool but needs to return the jobject or token. otherwise the jobject does not do anything
        //{
        //    JObject updatedStatement = new JObject();
        //    if (statement == null)
        //    {
        //        JObject pulledStatement = JSONHandler.GetJObject();
        //        pulledStatement[property] = new JArray();
        //        updatedStatement = pulledStatement;
        //    }
        //    else
        //    {
        //        statement[property] = new JArray();
        //        updatedStatement = statement;
        //    }
        //    return updatedStatement;
        //}
        #region check and create property - should not be used any more

        #endregion
        #endregion




        // private static readonly ILogger //_log = Log.Logger;

        //catch (Exception ex)
        //        {
        //            //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name+" Error: " + ex.Message);
        //
        //        }


        //this will be used for alternative approach
        //private static Models.xAPI.Statement _statement;        
        /// <summary>
        /// ***************This seems like it is redundant**************************
        /// This saves or creates savable data for the statements once they are updated. 
        /// Windows requries no additional action
        /// Mobile divices need to call their respective techniques
        /// </summary>
        /// <param name="statementObject"></param>
        /// <returns></returns>
        //internal static async Task<(bool, string[,])> SaveStatement(JObject statementObject)
        //{
        //    bool written = false;
        //    string[,] outputArray = default;
        //    try
        //    {
        //        StaticDetails.StaticDetails.CurrentStatement = statementObject;
        //        (written, outputArray) = await StaticDetails.StaticDetails.Handler.UpdatePost(statementObject);
        //        //written = JSONHandler.WriteToDataFile(statementObject);

        //    }
        //    catch (Exception ex)
        //    {
        //        Console.WriteLine("Error saving statement: " + ex.Message);
        //        //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
        //    }
        //    return (written, outputArray);
        //}
        #endregion
    }
}
