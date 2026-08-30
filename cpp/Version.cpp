// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
//#include "Version.h"

namespace FebrisCppModelsxAPI
{
    //################################################################
    //################################################################

    class Version
    {
    public:
        Version(long Id, GUID2 UUID, string VersionNumber)
            :Id(Id), UUID(UUID), VersionNumber(VersionNumber)
        {}
        Version() = default;

        long Id;
        GUID2 UUID; // lets use this to link? otherwise it is not stated as needed
        string VersionNumber;
    };
};
