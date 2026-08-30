// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
//#include <string>
//#include <list>
//#include <filesystem>
//#include <iostream>
//#include <guiddef.h>
//#include <chrono>
//#include "Actor.cpp"
//#include "Verb.cpp"
//#include "Object.cpp"
//#include "Result.cpp"
//#include "Context.cpp"
//#include "Authority.cpp"
//#include "Attachment.cpp"
//#include "Version.cpp"
#include "pch.h"
//#include "Statement.h"
//#include "pch.h"
//using namespace std;

namespace FebrisCppModelsxAPI
{
    //[Serializable]
    class Statement
    {
    public:
        Statement(long Id, GUID2 UUID, chrono::system_clock::time_point Timestamp, chrono::system_clock::time_point Stored, 
            Actor Actor, Verb Verb, Object Object, Result Result, Context Context, Authority Authority, Version Version, vector<Attachment> Attachments)
            :Id(Id), UUID(UUID), Timestamp(Timestamp), Stored(Stored), Actor(Actor), Verb(Verb), Object(Object), Result(Result), Context(Context), Authority(Authority), Version(Version), Attachments(Attachments)
        {}
        Statement() = default;
        //################################################################
        //this needs to be uuid (or guid) can be set up automatically with postgres       
        //################################################################
        long Id;
        GUID2 UUID;

        //################################################################
        //if not provided needs to set by api
        //################################################################
        chrono::system_clock::time_point Timestamp;

        //################################################################
        //Set this inside Db for when the record is stored
        //################################################################
        chrono::system_clock::time_point Stored;

        //################################################################
        //xApi required fields
        //################################################################

        Actor Actor;

        Verb Verb;

        Object Object;

        //################################################################
        //Optional Fields
        //attachments needs to be an ordered array of objects
        //################################################################
        Result Result;
        Context Context;
        Authority Authority;
        Version Version;
        vector<Attachment> Attachments;
    };
};
