// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef DEFINITION_H
#define DEFINITION_H
//#include ".h";

//#include "Extensions.h"
#endif

class DEFINITION_H Definition
{
public:
    Definition(long Id,
        GUID2 UUID,
        map<string, string> Name,
        map<string, string> Description,
        string Type,
        string MoreInfo,
        Extensions Extensions,
        string InteractionType,
        vector<string> CorrectResponsesPattern,
        string InteractionComponents);
    Definition() : Id(0), UUID{} {}  // zero scalars: C# field-default parity
    //1
    long Id;
    GUID2 UUID; // lets use this to link? otherwise it is not stated as needed

    //2        
    // SDKV-13: language maps, previously flattened strings.
    map<string, string> Name;
    map<string, string> Description;

    //3
   /* Uri Type;
    Uri MoreInfo;*/
    string Type;
    string MoreInfo;

    //4
    Extensions Extensions;

    //5
    string InteractionType;
    // SDKV-1/2: a list, previously a single string.
    vector<string> CorrectResponsesPattern;

    //6
    string InteractionComponents;

};