// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
// EXPORT-SURFACE PORT (2026-08-29): StaticDetails is header-only (C++17 inline
// statics carry the storage). This TU previously held a dead namespaced
// duplicate (namespace FebrisCppStaticDetails) whose members were all private
// instance fields -- storage for the real class existed nowhere.
