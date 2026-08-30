// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Febris.CsharpSimulationLibraryNetStandard.StaticDetails;
using Febris.CsharpSimulationLibraryNetStandard.Statement;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;

namespace Febris.CsharpSimulationLibraryNetStandard.Service
{
    internal class AndroidHandler : IEnvironmentHandler
    {
        public async Task<(bool, string[,])> CreateInitialPost(JObject statementFromDataModel)
        {
            bool isInitialized = false;
            string[,] outputArray = default;
            try
            {
                string referenceKey = StaticDetails.StaticDetails.ReferenceUUID;
                StaticDetails.StaticDetails.CurrentStatement = statementFromDataModel;
                // FIX (SDKV-19/20): stamp the wire id before the JSON goes into
                // the intent extra so it matches the ReferenceUUID extra key.
                StatementHandler.StampStatementId(statementFromDataModel);
                #region this is one way but I don't think it is the best way
                //KeyValuePair<string, string>[] keyValuePairs = new KeyValuePair<string, string>[3];
                //keyValuePairs[0] = new KeyValuePair<string, string>("intent", AndroidStatementPassingStaticDetails.StatementCreation );
                //keyValuePairs[1] = new KeyValuePair<string, string>(AndroidStatementPassingStaticDetails.ReferenceUUIDIntentExtraTag, referenceKey );
                //keyValuePairs[2] = new KeyValuePair<string, string>(AndroidStatementPassingStaticDetails.StatementJsonIntentExtraTag, statementFromDataModel.ToString());
                #endregion

                //{ key, value}
                outputArray = new string[,] {
                    //{ AndroidIntentConst.IntentTag, AndroidStatementPassingStaticDetails.StatementCreation},
                    { AndroidIntentConst.ReferenceUUIDIntentExtraTag, referenceKey},
                    { AndroidIntentConst.StatementJsonIntentExtraTag, statementFromDataModel.ToString()}
                };
                #region testing
                Console.WriteLine(outputArray);
                #endregion
                isInitialized = true;
            }
            catch (Exception ex)
            {
                // SIM-T13 G4: route through ISimulationLogger so the host game
                // can see the failure. Re-throw preserved -- callers depend on it.
                StaticDetails.StaticDetails.Logger.Log(
                    SimulationLogLevel.Error,
                    "AndroidHandler: failed building Intent extras for outgoing broadcast.",
                    ex);
                throw;
            }
            return (isInitialized, outputArray);
        }

        public async Task<(bool, string[,])> UpdatePost(JObject statementFromDataModel)
        {
            bool isInitialized = false;
            string[,] outputArray = default;
            try
            {
                string referenceKey = StaticDetails.StaticDetails.ReferenceUUID;
                // FIX (SDKV-19/20): stamp the wire id before the JSON goes into
                // the intent extra; idempotent, so every update re-emission of
                // the run carries the SAME id (retry-safe on the ingest node).
                StatementHandler.StampStatementId(statementFromDataModel);

                #region this is one way but I don't think it is the best way
                //KeyValuePair<string, string>[] keyValuePairs = new KeyValuePair<string, string>[3];
                //keyValuePairs[0] = new KeyValuePair<string, string>("intent", AndroidStatementPassingStaticDetails.StatementCreation );
                //keyValuePairs[1] = new KeyValuePair<string, string>(AndroidStatementPassingStaticDetails.ReferenceUUIDIntentExtraTag, referenceKey );
                //keyValuePairs[2] = new KeyValuePair<string, string>(AndroidStatementPassingStaticDetails.StatementJsonIntentExtraTag, statementFromDataModel.ToString());
                #endregion

                ///This is really just a key value pair{ key, value}
                outputArray = new string[,] {
                    //{ AndroidIntentConst.IntentTag, AndroidStatementPassingStaticDetails.StatementCreation},
                    { AndroidIntentConst.ReferenceUUIDIntentExtraTag, referenceKey},
                    { AndroidIntentConst.StatementJsonIntentExtraTag, statementFromDataModel.ToString()}
                };

                #region testing
                Console.WriteLine(outputArray);
                #endregion

                isInitialized = true;

            }
            catch (Exception ex)
            {
                // SIM-T13 G4: route through ISimulationLogger so the host game
                // can see the failure. Re-throw preserved -- callers depend on it.
                StaticDetails.StaticDetails.Logger.Log(
                    SimulationLogLevel.Error,
                    "AndroidHandler: failed building Intent extras for outgoing broadcast.",
                    ex);
                throw;
            }
            return (isInitialized, outputArray);
        }

        public async Task<(bool, string[,])> ErrorPost(JObject statementFromDataModel)
        {
            bool isInitialized = false;
            string[,] outputArray = default;
            try
            {
                string referenceKey = StaticDetails.StaticDetails.ReferenceUUID;
                // FIX (SDKV-19/20): error emissions carry the stamped id too.
                StatementHandler.StampStatementId(statementFromDataModel);
                ///Key and value
                outputArray = new string[,] {                  
                    { AndroidIntentConst.ReferenceUUIDIntentExtraTag, referenceKey},
                    { AndroidIntentConst.StatementJsonIntentExtraTag, statementFromDataModel.ToString()}
                };

                #region testing
                Console.WriteLine(outputArray);
                #endregion

                isInitialized = true;

            }
            catch (Exception ex)
            {
                // SIM-T13 G4: route error-emission failures through the logger.
                StaticDetails.StaticDetails.Logger.Log(
                    SimulationLogLevel.Error,
                    "AndroidHandler.ErrorPost: failed building error-intent extras.",
                    ex);
                throw;
            }
            return (isInitialized, outputArray);            
        }
    }
}
