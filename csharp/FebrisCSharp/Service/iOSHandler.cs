// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;

namespace Febris.CsharpSimulationLibraryNetStandard.Service
{
    // NOTE (SIM-B1): iOS handler is non-functional. CreateInitialPost never sets
    // isInitialized/outputArray so Initialize() on iOS returns (false, default),
    // and UpdatePost/ErrorPost throw NotImplementedException. iOS was explicitly
    // deferred (Tier 13 G10). Wrapping the class in a dead region (mirroring
    // WinMobileHandler) to flag the known limitation. Logic is unchanged: making
    // iOS emit real data is a functionality change, deferred per
    // do-not-change-functionality. See docs/MODERNIZATION/SIM_MODERNIZATION.md
    // (the SYSTEM_SLICES.md "supported" claim still needs correcting in a separate change).
    #region [Dead - SIM-B1] iOSHandler - non-functional, iOS deferred
    internal class iOSHandler : IEnvironmentHandler
    {

        public async Task<(bool, string[,])> CreateInitialPost(JObject statementFromDataModel)
        {
            bool isInitialized = false;
            string[,] outputArray = default;
            try
            {
                StaticDetails.StaticDetails.CurrentStatement = statementFromDataModel;
                string referenceKey = StaticDetails.StaticDetails.ReferenceUUID;


                #region testing
                Console.WriteLine(outputArray);
                #endregion


            }
            catch (Exception ex)
            {
                Console.WriteLine("Error creating initial Statement: " + ex.Message);
                throw;
            }
            return (isInitialized, outputArray);
        }

        public async Task<(bool, string[,])> ErrorPost(JObject statementFromDataModel)
        {
            throw new NotImplementedException();
        }

        public async Task<(bool, string[,])> UpdatePost(JObject statementFromDataModel)
        {
            throw new NotImplementedException();
        }
    }
    #endregion // [Dead - SIM-B1] iOSHandler
}
