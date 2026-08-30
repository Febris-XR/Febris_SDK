// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"

// EXPORT-SURFACE PORT (2026-08-29): the dead namespaced duplicate that lived
// here (namespace FebrisCppHelpers) hand-rolled "PT<sec>.<ms>S", which is NOT
// what C# XmlConvert.ToString(TimeSpan) emits. The real XmlConvert.ToString
// (TimeSpan) semantics live in XApiJson::DurationIso8601 (byte-parity with the
// C# wire output verified 2026-08-29); delegate so the logic exists exactly once.
std::string XMLConvert::millisecondsToIso8601(long long durationMs)
{
	return XApiJson::DurationIso8601(durationMs);
}
