// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Newtonsoft.Json.Linq;
using System.Threading.Tasks;

namespace Febris.CsharpSimulationLibraryNetStandard.Service
{
    internal interface IEnvironmentHandler
    {
        Task<(bool, string[,])> CreateInitialPost(JObject statementFromDataModel);

        Task<(bool, string[,])> UpdatePost(JObject statementFromDataModel);
        //Task(bool)> UpdatePost(JObject statementFromDataModel);

        Task<(bool, string[,])> ErrorPost(JObject statementFromDataModel);

    }
}