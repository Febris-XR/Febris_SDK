// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
// EXPORT-SURFACE PORT (2026-08-29): SharedDetails is header-only (C++17 inline
// statics / constexpr carry the storage). This TU previously held a dead
// namespaced duplicate (namespace FebrisCppSharedDetails) that gave the real
// global-scope class no storage at all.
