// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "pch.h"
#ifndef ATTACHMENT_H
#define ATTACHMENT_H

#endif 


class ATTACHMENT_H Attachment {
public:
    Attachment(long Id,
        GUID2 UUID,
        string UsageType,
        string Display,
        string Description,
        string ContentType,
        int Length,
        string Sha2,
        string FileURL);
    Attachment() : Id(0), UUID{}, Length(0) {}  // zero scalars: C# field-default parity
    long  Id;
    GUID2 UUID;
    //2        
    //Uri UsageType;
    string UsageType;

    //3               
    // SDKV-13: language maps, previously strings with an \r\n strip.
    map<string, string> Display;
    map<string, string> Description;

    //4        
    string ContentType; //ie "application/octet-stream"
    //5         
    int Length;
    //6        
    string Sha2;
    //7
    //Uri FileURL;//user UUID to name video file
    string FileURL;

};