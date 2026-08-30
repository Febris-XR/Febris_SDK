// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using System;
using System.Collections.Generic;
using System.Text;

namespace Febris.CsharpSimulationLibraryNetStandard.Enums
{
    public enum ResultOptions
    {
        Success,
        Completion,
        Response,
        Duration,
        Extensions,
        ScoreMin,
        ScoreMax,
        ScoreScale,
        ScoreRaw
    }
    public enum ResultExtensionOptions
    {
        Notes,
        RestartCounter,
        //[Display(Name = "Restart Counter")] RestartCounter
    }
}
