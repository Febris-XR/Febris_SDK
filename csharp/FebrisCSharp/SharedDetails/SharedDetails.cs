// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using System;
using System.Collections.Generic;
using System.Text;

namespace Febris.CsharpSimulationLibraryNetStandard.SharedDetails
{
    public class SharedDetails
    {           
        //public static string videoName;
        public static bool SimulationIsRunning = false;

        //argument constants
        public const string StatementPreface = "-febrisData=";
        public const int StatementPrefaceLength = 12;
        public const string VideoDataPreface = "-videoData=";
        public const int VideoDataPrefaceLength = 11;
        public const string SimulationProcessIdPreface = "-simulationProcessId=";
        public const int SimulationProcessIdPrefaceLength = 21;
        public const string SimulationProcessName = "-simulationProcessName=";
        public const int SimulationProcessNameLength = 23;
    }    
}
