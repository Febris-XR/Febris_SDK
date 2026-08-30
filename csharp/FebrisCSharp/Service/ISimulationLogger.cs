// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using System;

namespace Febris.CsharpSimulationLibraryNetStandard.Service
{
    /// <summary>
    /// Severity levels for <see cref="ISimulationLogger"/>. Intentionally mirrors
    /// <c>Febris.SharedMobileLibrary.P2pNetworking.FebrisP2pLogLevel</c> so a host
    /// that bridges both surfaces (e.g., a Companion routing both P2P transport
    /// logs + game-side simulation logs) can use a single severity vocabulary.
    /// </summary>
    public enum SimulationLogLevel
    {
        /// <summary>Verbose / per-statement trace. Off in production builds.</summary>
        Debug,

        /// <summary>Normal lifecycle events: simulation initialized, statement
        /// emitted, simulation ended. Always on.</summary>
        Info,

        /// <summary>Recoverable problem: handler returned non-ready, no-op
        /// XAPIProperties branch hit, statement validation soft-failure.
        /// Operator should investigate if frequent.</summary>
        Warn,

        /// <summary>Unrecoverable problem inside the library: handler threw,
        /// initialization failed, JSON parse error, write-to-disk failure.
        /// Host game needs to know.</summary>
        Error
    }

    /// <summary>
    /// Diagnostic sink for the Febris simulation library. Before SIM-Tier-13 the
    /// library scattered <c>Console.WriteLine</c> calls across every catch block;
    /// on Android those logs land in <c>logcat</c> the host game doesn't see, in
    /// Unity player builds they're invisible, and even in editor builds the host
    /// can't route them into its own diagnostics surface (Sentry, customer
    /// support telemetry, etc.).
    ///
    /// <para>
    /// <b>Host integration.</b> The game registers a logger before the first
    /// <c>Initialize()</c> call:
    /// <code>
    /// StaticDetails.StaticDetails.Logger = new MyHostLogger();
    /// </code>
    /// or via the overload:
    /// <code>
    /// await Initializer.Initialize(args, OS, logger: new MyHostLogger());
    /// </code>
    /// If no logger is registered, the library defaults to
    /// <see cref="ConsoleSimulationLogger.Instance"/> which preserves the
    /// pre-Tier-13 behavior.
    /// </para>
    ///
    /// <para>
    /// <b>Non-throwing contract.</b> Implementations MUST swallow their own
    /// failures. A logger crash must not bring down the host game thread -- the
    /// caller invokes <c>Log(...)</c> in catch blocks where re-throwing would
    /// destroy the user's session. <see cref="ConsoleSimulationLogger"/> wraps
    /// its Console writes in <c>try / catch</c> for this reason.
    /// </para>
    /// </summary>
    public interface ISimulationLogger
    {
        /// <summary>
        /// Emit a single event. Should be non-throwing (see interface remarks).
        /// </summary>
        /// <param name="level">Severity. Implementations MAY drop events below
        /// a configured threshold but MUST not throw.</param>
        /// <param name="message">Human-readable description. Active voice
        /// ("initialized simulation", "rejected unsupported update").</param>
        /// <param name="exception">Optional exception associated with the
        /// event. Always passed for <see cref="SimulationLogLevel.Error"/>;
        /// usually null for Info/Debug.</param>
        void Log(SimulationLogLevel level, string message, Exception exception = null);
    }

    /// <summary>
    /// Default fallback logger -- writes every event to <see cref="System.Console"/>.
    /// Preserves the pre-Tier-13 behavior: every <c>Console.WriteLine("Error...")</c>
    /// the library used to emit now flows through <c>Log(Error, ..., ex)</c>
    /// which calls <c>Console.WriteLine</c> internally. Net behavior unchanged
    /// for hosts that don't register a custom logger.
    ///
    /// <para>
    /// Format: <c>[FebrisSim][Level] message</c>, with the exception's type +
    /// message + stack trace appended on a new line when present.
    /// </para>
    /// </summary>
    public sealed class ConsoleSimulationLogger : ISimulationLogger
    {
        /// <summary>Shared singleton -- the library defaults to this when no host
        /// logger is registered.</summary>
        public static readonly ConsoleSimulationLogger Instance = new ConsoleSimulationLogger();

        /// <inheritdoc/>
        public void Log(SimulationLogLevel level, string message, Exception exception = null)
        {
            // Per the contract: a logger crash must never propagate. Wrap the
            // Console writes so a redirected stdout / closed handle doesn't
            // take the game down.
            try
            {
                string line = "[FebrisSim][" + level + "] " + (message ?? string.Empty);
                Console.WriteLine(line);
                if (exception != null)
                {
                    Console.WriteLine("[FebrisSim][" + level + "] " + exception.GetType().Name + ": " + exception.Message);
                    if (!string.IsNullOrEmpty(exception.StackTrace))
                    {
                        Console.WriteLine(exception.StackTrace);
                    }
                }
            }
            catch
            {
                // Swallow -- logger failures must never crash the caller.
            }
        }
    }

    /// <summary>
    /// Test double: captures every event in an in-memory list, thread-safe via
    /// lock. Used by parity tests + future integration tests to assert that the
    /// expected log lines fire on the expected code paths.
    ///
    /// <para>
    /// <b>Not for production use.</b> The capture list grows unbounded; no
    /// trimming, no rolling window.
    /// </para>
    /// </summary>
    public sealed class CapturingSimulationLogger : ISimulationLogger
    {
        private readonly System.Collections.Generic.List<SimulationLogEntry> _entries
            = new System.Collections.Generic.List<SimulationLogEntry>();
        private readonly object _lock = new object();

        /// <summary>Immutable snapshot of every event captured so far. Safe to
        /// enumerate while writers append from other threads.</summary>
        public System.Collections.Generic.IReadOnlyList<SimulationLogEntry> Entries
        {
            get
            {
                lock (_lock)
                {
                    return _entries.ToArray();
                }
            }
        }

        /// <inheritdoc/>
        public void Log(SimulationLogLevel level, string message, Exception exception = null)
        {
            lock (_lock)
            {
                _entries.Add(new SimulationLogEntry(level, message ?? string.Empty, exception));
            }
        }
    }

    /// <summary>A single captured log event. Immutable; safe to share across
    /// threads.</summary>
    public sealed class SimulationLogEntry
    {
        public SimulationLogLevel Level { get; }
        public string Message { get; }
        public Exception Exception { get; }

        public SimulationLogEntry(SimulationLogLevel level, string message, Exception exception)
        {
            Level = level;
            Message = message;
            Exception = exception;
        }
    }
}
