// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Febris.CsharpSimulationLibraryNetStandard.Service;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using System.Threading;

namespace Febris.CsharpSimulationLibraryNetStandard.StaticDetails
{
    internal class StaticDetails
    {
        //running the statement writing loop
        internal const int writeFrequency = 30000;//ms

        //timer
        internal static Stopwatch timer;

        // SIM-T13 G4: process-wide ISimulationLogger sink. Host games register a
        // real logger before Initialize() to route library failures into their
        // own diagnostics surface. Defaults to ConsoleSimulationLogger.Instance
        // so behavior matches the pre-Tier-13 Console.WriteLine pattern.
        private static ISimulationLogger _logger = ConsoleSimulationLogger.Instance;
        public static ISimulationLogger Logger
        {
            get { return _logger; }
            set
            {
                // Null reset = revert to the Console fallback rather than
                // leaving a null deref hazard for every catch in the library.
                _logger = value ?? ConsoleSimulationLogger.Instance;
            }
        }


        private static JObject currentStatement;
        internal static JObject CurrentStatement 
        {
            get
            {
                return currentStatement;
            }
            set
            {
                currentStatement = value;
            }
        
        }

        


        private static string _referenceUUID;
        internal static string ReferenceUUID
        {
            get
            {
                if(_referenceUUID ==default || _referenceUUID == Guid.Empty.ToString())
                {
                    _referenceUUID = Guid.NewGuid().ToString();
                }
                return _referenceUUID;
            }
            set
            {
                if(value != _referenceUUID)
                {
                    _referenceUUID = value;
                }
            }
        }


        private static IEnvironmentHandler _handler;
        internal static IEnvironmentHandler Handler
        {
            get
            {
                return _handler;
            }
            set
            {
                _handler = value;
            }
        }

    }
    public class AndroidStatementPassingStaticDetails
    {
        /// <summary>
        /// Gathering arguments
        /// </summary>
        //public const string ArgumentExtraTag = "arguments";
        //public const string HasExtrasTag = "hasExtras";
        //public const string GetExtrasTag = "getExtras";
        //public const string GetStringTag = "getString";
        //public const string GetIntentTag = "getIntent";
        //public const string GetCurrentActivityTag = "currentActivity";
        //public const string UnityPlayerTag = "com.unity3d.player.UnityPlayer";

        ////credentials        
        //public const string IntentTag = "Intent";
        //public const string ReferenceUUIDIntentExtraTag = "ReferenceUUID";
        //public const string StatementJsonIntentExtraTag = "RawStatementJson";

        ///Intents for companion interactions
        public const string StatementCreation = "com.febris.STATEMENT_CREATE";
        public const string StatementUpdate = "com.febris.STATEMENT_UPDATE";
        public const string StatementError = "com.febris.STATEMENT_ERROR";

        public const string StatementBraodcastIntentUri = "com.febris.";
    }

    public class AndroidIntentConst
    {
        public const string IntentClass = "com.unity3d.player.UnityPlayer";
        public const string IntentObject = "android.content.Intent";
        public const string IntentSetAction = "setAction";
        public const string IntentSetData = "setData";
        public const string IntentStartActivity = "startActivity";
        public const string SendBroadcast = "sendBroadcast";

        public const string AndroidUriClass = "android.net.Uri";
        public const string AndroidParseTag = "parse";
        public const string PutExtraTag = "putExtra";


        /// <summary>
        /// Gathering arguments
        /// </summary>
        public const string ArgumentExtraTag = "arguments";
        public const string HasExtrasTag = "hasExtras";
        public const string GetExtrasTag = "getExtras";
        public const string GetStringTag = "getString";
        public const string GetIntentTag = "getIntent";
        public const string GetCurrentActivityTag = "currentActivity";
        public const string UnityPlayerTag = "com.unity3d.player.UnityPlayer";



        //credentials        
        public const string IntentTag = "Intent";
        public const string ReferenceUUIDIntentExtraTag = "ReferenceUUID";
        public const string StatementJsonIntentExtraTag = "RawStatementJson";

        ///Intents for companion interactions
        public const string StatementCreation = "com.febris.STATEMENT_CREATE";
        public const string StatementUpdate = "com.febris.STATEMENT_UPDATE";
        public const string StatementError = "com.febris.STATEMENT_ERROR";

        public const string StatementBroadcastIntentUri = "com.febris.";
    }
}
