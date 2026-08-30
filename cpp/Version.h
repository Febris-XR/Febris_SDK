// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"

#ifndef VERSION_H
#define VERSION_H
//#include ".h";


#endif 



class VERSION_H Version {
public:
    Version(long Id, GUID2 UUID, string VersionNumber);
    Version() : Id(0), UUID{} {}  // zero scalars: C# field-default parity
    long Id;
    GUID2 UUID; // lets use this to link? otherwise it is not stated as needed
    string VersionNumber;

};