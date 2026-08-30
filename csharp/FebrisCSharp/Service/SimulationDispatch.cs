// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Febris.CsharpSimulationLibraryNetStandard.StaticDetails;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Febris.CsharpSimulationLibraryNetStandard.Service
{
    /// <summary>
    /// SIM-T13 G3: result shape for outbound simulation events. Replaces the
    /// opaque <c>(bool, string[,])</c> tuple that the legacy
    /// <see cref="IEnvironmentHandler"/> methods return.
    ///
    /// <para>
    /// <b>Why a typed result.</b> The legacy shape forced the Unity glue
    /// (FebrisScriptManager) to remember WHICH method it had just called so it
    /// could pass the correct intent action ("com.febris.STATEMENT_CREATE" vs
    /// "_UPDATE" vs "_ERROR") to Android_SendBroadcast. The action wasn't on the
    /// return value, just implicit in the method name. This invites mismatches
    /// -- call <c>CreateInitialPost</c> + accidentally pass StatementUpdate as
    /// the action and the Companion's StatementReceiver routes the broadcast
    /// to the wrong handler with no warning.
    /// </para>
    ///
    /// <para>
    /// The new shape moves the intent action into the result, so the Unity glue
    /// can mechanically forward whatever the library hands back.
    /// </para>
    ///
    /// <para>
    /// <b>Backward compatibility.</b> The legacy <c>(bool, string[,])</c>
    /// methods remain on <see cref="IEnvironmentHandler"/> + are wired exactly
    /// as before. <see cref="SimulationDispatch"/> is additive -- the
    /// <see cref="SimulationDispatchExtensions"/> below provides the new shape
    /// as extension methods over the existing interface, so no handler change
    /// is required and the 21 existing call sites keep working.
    /// </para>
    /// </summary>
    public sealed class SimulationDispatch
    {
        /// <summary>True if the dispatch is ready to fire (i.e., the underlying
        /// handler returned `bool ready = true`). False = library refused or
        /// the handler hit an error path.</summary>
        public bool Ready { get; }

        /// <summary>The platform-specific action string the host should use
        /// when firing the outbound transport. For Android this is one of
        /// "com.febris.STATEMENT_CREATE" / "_UPDATE" / "_ERROR" (the values
        /// from <see cref="StatementPassingStaticDetails"/>). For non-Android
        /// platforms this is <see cref="string.Empty"/>.</summary>
        public string IntentAction { get; }

        /// <summary>Key/value extras to attach to the outbound transport
        /// payload. For Android: Intent extras. For PC: ignored (the WinPCHandler
        /// writes to a file; there's no extras surface).</summary>
        public IReadOnlyDictionary<string, string> Extras { get; }

        public SimulationDispatch(
            bool ready,
            string intentAction,
            IReadOnlyDictionary<string, string> extras)
        {
            Ready = ready;
            IntentAction = intentAction ?? string.Empty;
            Extras = extras ?? new Dictionary<string, string>();
        }

        /// <summary>Convert back to the legacy <c>(bool, string[,])</c> shape
        /// so the Unity glue can adopt <see cref="SimulationDispatch"/>
        /// incrementally without rewriting <c>Android_SendBroadcast</c>.</summary>
        public (bool, string[,]) ToLegacyTuple()
        {
            int count = Extras.Count;
            string[,] arr;
            if (count == 0)
            {
                arr = new string[0, 2];
            }
            else
            {
                arr = new string[count, 2];
                int i = 0;
                foreach (var kvp in Extras)
                {
                    arr[i, 0] = kvp.Key;
                    arr[i, 1] = kvp.Value;
                    i++;
                }
            }
            return (Ready, arr);
        }

        /// <summary>Build a <see cref="SimulationDispatch"/> from the legacy
        /// <c>(bool, string[,])</c> shape + an explicit intent action. Used by
        /// the extension methods that wrap the existing handler API.</summary>
        public static SimulationDispatch FromLegacy(bool ready, string[,] legacy, string intentAction)
        {
            var dict = new Dictionary<string, string>();
            if (legacy != null)
            {
                int rows = legacy.GetLength(0);
                int cols = legacy.GetLength(1);
                if (cols >= 2)
                {
                    for (int r = 0; r < rows; r++)
                    {
                        string key = legacy[r, 0];
                        if (!string.IsNullOrEmpty(key))
                        {
                            dict[key] = legacy[r, 1];
                        }
                    }
                }
            }
            return new SimulationDispatch(ready, intentAction, dict);
        }
    }

    /// <summary>
    /// SIM-T13 G3: extension methods that surface the new
    /// <see cref="SimulationDispatch"/> shape on top of the existing
    /// <see cref="IEnvironmentHandler"/> interface. Hosts can adopt the new
    /// shape without any handler-implementation change -- the extensions call
    /// the legacy methods + bake in the implicit intent action.
    /// </summary>
    internal static class SimulationDispatchExtensions
    {
        /// <summary>Surface a "create" dispatch from a handler's legacy
        /// <see cref="IEnvironmentHandler.CreateInitialPost(JObject)"/>.
        /// Returned <see cref="SimulationDispatch.IntentAction"/> is the
        /// canonical "STATEMENT_CREATE" action shared with the Companion's
        /// StatementReceiver. Internal: <see cref="IEnvironmentHandler"/> is
        /// internal so external consumers shouldn't reach it. Public callers
        /// use the dispatch overloads on
        /// <see cref="Febris.CsharpSimulationLibraryNetStandard.Statement.Initializer"/>
        /// and
        /// <see cref="Febris.CsharpSimulationLibraryNetStandard.Statement.StatementHandler"/>.</summary>
        internal static async Task<SimulationDispatch> CreateInitialDispatchAsync(
            this IEnvironmentHandler handler,
            JObject statement)
        {
            if (handler == null) throw new ArgumentNullException(nameof(handler));
            var (ready, arr) = await handler.CreateInitialPost(statement).ConfigureAwait(false);
            return SimulationDispatch.FromLegacy(
                ready,
                arr,
                AndroidStatementPassingStaticDetails.StatementCreation);
        }

        /// <summary>Surface an "update" dispatch from a handler's legacy
        /// <see cref="IEnvironmentHandler.UpdatePost(JObject)"/>.</summary>
        internal static async Task<SimulationDispatch> UpdateDispatchAsync(
            this IEnvironmentHandler handler,
            JObject statement)
        {
            if (handler == null) throw new ArgumentNullException(nameof(handler));
            var (ready, arr) = await handler.UpdatePost(statement).ConfigureAwait(false);
            return SimulationDispatch.FromLegacy(
                ready,
                arr,
                AndroidStatementPassingStaticDetails.StatementUpdate);
        }

        /// <summary>Surface an "error" dispatch from a handler's legacy
        /// <see cref="IEnvironmentHandler.ErrorPost(JObject)"/>. Note: not every
        /// handler implements ErrorPost -- WinMobileHandler currently throws
        /// (its class is wrapped in a Dead-region pending revival; SIM-T13 G1).
        /// Callers should handle the exception case explicitly.</summary>
        internal static async Task<SimulationDispatch> ErrorDispatchAsync(
            this IEnvironmentHandler handler,
            JObject statement)
        {
            if (handler == null) throw new ArgumentNullException(nameof(handler));
            var (ready, arr) = await handler.ErrorPost(statement).ConfigureAwait(false);
            return SimulationDispatch.FromLegacy(
                ready,
                arr,
                AndroidStatementPassingStaticDetails.StatementError);
        }
    }
}
