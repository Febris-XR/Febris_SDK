// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;

namespace Febris.CsharpSimulationLibraryNetStandard.Service
{
    // SIM-T13 G1: [Dead] WinMobileHandler. Three of the four IEnvironmentHandler
    // entry points throw NotImplementedException (UpdatePost / ErrorPost / explicit
    // CreateInitialPost on line 46-49). The instance-method `internal
    // CreateInitialPost` at line 21 is unreachable through IEnvironmentHandler
    // because the explicit-interface impl below shadows it. Initializing the
    // simulation library with OS = ExpectedOperatingSystem.WinMobile therefore
    // crashes the host game on the first Initialize() call.
    //
    // The Windows-on-mobile platform is effectively dead in 2026, but the class
    // is kept on disk per the comment-out-dont-delete policy (CLAUDE.md rule #10)
    // so future maintainers can see what was attempted. If a customer ever needs
    // WinMobile, fill out UpdatePost / ErrorPost / the public CreateInitialPost
    // following the AndroidHandler pattern + delete this region wrapper.
    //
    // Audit doc: docs/SIMULATION_ROADMAP/TIER_13_AUDIT_MDM_COMPAT.md gap G1.
    #region [Dead - SIM-T13] WinMobileHandler - throws NIE on every live path
    internal class WinMobileHandler : IEnvironmentHandler
    {
        public Task<(bool, string[,])> ErrorPost(JObject statementFromDataModel)
        {
            throw new NotImplementedException();
        }

        public Task<(bool, string[,])> UpdatePost(JObject statementFromDataModel)
        {
            throw new NotImplementedException();
        }

        internal async Task<(bool, string[,])> CreateInitialPost(JObject statementFromDataModel)
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

        Task<(bool, string[,])> IEnvironmentHandler.CreateInitialPost(JObject statementFromDataModel)
        {
            throw new NotImplementedException();
        }
    }
    #endregion // [Dead - SIM-T13] WinMobileHandler
}
