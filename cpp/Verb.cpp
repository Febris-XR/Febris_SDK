// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
//#include <string>
//#include <list>
//#include <filesystem>
//#include <iostream>
//#include <guiddef.h>
#include "pch.h"
//#include "Verb.h"
//using namespace std;

namespace FebrisCppModelsxAPI
{
    //################################################################
    //################################################################

    class Verb
    {
    public:
        Verb(long Key, GUID2 UUID, string Id, string Display)
            :Key(Key), UUID(UUID), Id(Id), Display(Display)
        {}
        Verb() = default;
        long Key;
        GUID2 UUID;
        //Uri Id;//needs to be an IRI - https://febr.is/TestBase/verbs/attempted or started or trained or completed        
        string Id;

        string Display;
    };
};
