// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef XAPIPROPERTIES_H
#define XAPIPROPERTIES_H
//#include "pch.h";
#endif 

enum class XAPIPROPERTIES_H XAPIProperties
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

};