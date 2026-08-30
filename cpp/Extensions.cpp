// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
//#include "Extensions.h"
//using namespace std;

namespace FebrisCppModelsxAPI
{
    //################################################################
    //keys must be IRIs
    //
    //################################################################

    class Extensions
    {
    public:
        Extensions(long Id,
            GUID2 UUID,
            string ExtensionMap)
            : Id(Id), UUID(UUID), ExtensionMap(ExtensionMap)
        {}
        Extensions() = default;

        long Id;
        //check page 50
        GUID2 UUID; // lets use this to link? otherwise it is not stated as needed

        string ExtensionMap;
    };
};
