// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using System;
using System.Collections.Generic;
using System.Text;

namespace Febris.CsharpSimulationLibraryNetStandard.Enums
{
    public enum XAPIProperties
    {
        //overall
        Statement,
        //first layer
        Timestamp,
        Actor,
        Verb,
        Object,
        Result,
        Context,
        Authority,
        Version,
        Attachments,
        //Actor
        Account,
        Member,
        //Object
        Definition,        
        //Result
        Score,        
        //Context
        ContextActivities,
        StatementReference,
        //multi use
        Extensions,

    }
}
