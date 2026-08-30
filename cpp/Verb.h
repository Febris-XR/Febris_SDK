// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"

#ifndef VERB_H
#define VERB_H
//#include ".h";


#endif 


//
//#ifdef FEBRISCPPINT_EXPORTS
//#define FEBRISCPPINT_API __declspec(dllexport)
//#else
//#define FEBRISCPPINT_API __declspec(dllimport)
//#endif


class VERB_H Verb {
public:
    Verb(long Key, GUID2 UUID, string Id, map<string, string> Display);
    Verb() : Key(0), UUID{} {}  // zero scalars: C# field-default parity
    long Key;
    GUID2 UUID;
    //Uri Id;//needs to be an IRI - https://febr.is/TestBase/verbs/attempted or started or trained or completed        
    string Id;

    // SDKV-13: language map, previously a flattened string.
    map<string, string> Display;

}; 
