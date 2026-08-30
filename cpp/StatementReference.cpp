// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
//#include <string>
//#include <list>
//#include <filesystem>
//#include <iostream>
//#include <guiddef.h>
#include "pch.h"
//#include "StatementReference.h"
//using namespace std;

namespace FebrisCppModelsxAPI
{
    //################################################################
    //object type bust me StatementRef
    //################################################################

    class StatementReference
    {
    public:
        StatementReference(long Key, GUID2 UUID, GUID2 Id, string ObjectType)
            :Key(Key), UUID(UUID), Id(Id), ObjectType(ObjectType)
        {}
        StatementReference() = default;
        long Key;
        GUID2 UUID;
        GUID2 Id;
        string ObjectType;
    };
};
