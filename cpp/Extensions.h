// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "pch.h"
#ifndef EXTENSIONS_H
#define EXTENSIONS_H
//#include ".h";


#endif 


//
//#ifdef FEBRISCPPINT_EXPORTS
//#define FEBRISCPPINT_API __declspec(dllexport)
//#else
//#define FEBRISCPPINT_API __declspec(dllimport)
//#endif


class EXTENSIONS_H Extensions {
public:
    Extensions(long Id,
        GUID2 UUID,
        string ExtensionMap);
    Extensions() : Id(0), UUID{} {}  // zero scalars: C# field-default parity
    long Id;
    //check page 50
    GUID2 UUID; // lets use this to link? otherwise it is not stated as needed
    string ExtensionMap;
};