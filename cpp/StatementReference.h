// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "pch.h"
#ifndef STATEMENTREFERENCE_H
#define STATEMENTREFERENCE_H
//#include ".h";


#endif 


//
//#ifdef FEBRISCPPINT_EXPORTS
//#define FEBRISCPPINT_API __declspec(dllexport)
//#else
//#define FEBRISCPPINT_API __declspec(dllimport)
//#endif


class STATEMENTREFERENCE_H StatementReference {
public:
    StatementReference(long Key, GUID2 UUID, GUID2 Id, string ObjectType);
    StatementReference() : Key(0), UUID{}, Id{} {}  // zero scalars: C# field-default parity
    long Key;
    GUID2 UUID;
    GUID2 Id;
    string ObjectType;
};