// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"

#ifndef STATEMENT_H
#define STATEMENT_H

#endif 


//
//#ifdef FEBRISCPPINT_EXPORTS
//#define FEBRISCPPINT_API __declspec(dllexport)
//#else
//#define FEBRISCPPINT_API __declspec(dllimport)
//#endif


class STATEMENT_H Statement {
public:   
    Statement(long Id, GUID2 UUID, chrono::system_clock::time_point Timestamp, chrono::system_clock::time_point Stored,
        Actor Actor, Verb Verb, Object Object, Result Result, Context Context, Authority Authority, Version Version, vector<Attachment> Attachments);
    // Inline definition, zeroing scalars to the C# field defaults (0, Guid.Empty,
    // false). The ctors were declared but never defined anywhere; the DLL "linked"
    // only because it exports nothing, so the linker dead-stripped the whole model
    // layer. The parity probe is the first binary that actually links these.
    Statement() : Id(0), UUID{} {}
    long Id;
    GUID2 UUID;
    chrono::system_clock::time_point Timestamp;
    chrono::system_clock::time_point Stored;
    Actor Actor;
    Verb Verb;
    Object Object;
    Result Result;
    Context Context;
    Authority Authority;
    // PARITY (lifecycle harness): optional mirrors the C# nullable reference; see Member.Actors.
    std::optional<::Version> Version;
    vector<Attachment> Attachments;

};