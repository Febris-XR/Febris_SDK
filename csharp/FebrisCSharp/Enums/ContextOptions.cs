// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using System;
using System.Collections.Generic;
using System.Text;

namespace Febris.CsharpSimulationLibraryNetStandard.Enums
{
    public enum ContextOptions
    {
        Registration,
        Instructor,
        Group,
        ContextActivites,
        Revision,
        Platform,
        Language,
        StatementReference,
        Extensions        
    }
    public enum ContextActivitesOptions
    {
        Parent,
        Grouping,
        Category,
        Other
    }
    public enum ContextStatementReferenceOptions
    {
        Id,
        ObjectType
    }
    public enum ContextExtensionOptions
    {
        Option1,
        Option2
    }
    class ContextOptionResolver
    {
        internal static string ResolveExtensionIRI(ContextOptions option)
        {
            switch (option)
            {
                case ContextOptions.Registration:
                    return "Registration";
                case ContextOptions.Instructor:
                    return "Instructor";
                case ContextOptions.Group:
                    return "Group";
                case ContextOptions.ContextActivites:
                    return "ContextActivites";
                case ContextOptions.Revision:
                    return "Revision";
                case ContextOptions.Platform:
                    return "Platform";
                case ContextOptions.Language:
                    return "Language";
                case ContextOptions.StatementReference:
                    return "Statement Reference";
                case ContextOptions.Extensions:
                    return "Extensions";
                //case ContextOptions.ContextActivites_Parent:
                //    return "ContextActivites Parent";
                //case ContextOptions.ContextActivites_Grouping:
                //    return "ContextActivites Grouping";
                //case ContextOptions.ContextActivites_Category:
                //    return "ContextActivites Category";
                //case ContextOptions.ContextActivites_Other:
                //    return "ContextActivites Other";
                default:
                    // Handle bad URL, possibly throw
                    throw new Exception();
            }
        }
        internal static string ContextExtensionOptionResolver(ContextExtensionOptions option)
        {
            switch (option)
            {
                case ContextExtensionOptions.Option1:
                    return "https://Option1";
                case ContextExtensionOptions.Option2:
                    return "https://Option2";                
                default:
                    // Handle bad URL, possibly throw
                    throw new Exception();
            }
        }

    }
}
