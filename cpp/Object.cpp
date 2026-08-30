// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
//#include <string>
//#include <list>
//#include <filesystem>
//#include <iostream>
//#include <guiddef.h>
//#include "Extensions.cpp"
#include "pch.h"
//#include "Object.h"
//using namespace std;

namespace FebrisCppModelsxAPI
{
    //################################################################
    //################################################################

    class Object
    {
    public:
        Object(long Key,
        GUID2 UUID,
        //Uri Id,
        string Id,
        string ObjectType,
        Definition Definition)
            : Key(Key), UUID(UUID), Id(Id), ObjectType(ObjectType), Definition(Definition)
        {}
        Object() = default;

        long Key;
        GUID2 UUID;
        //Uri Id;
        string Id;
        string ObjectType;
        Definition Definition;

    };

    //################################################################
    //Type URI - cannot tell if it should be a adlnet standard or my own
    //  1) sql ids
    //  2) dictionaries - Had to be changed to jagged arrays
    //  3) Uris
    //  4) Extensions  -- I am not sure if this is handled correctly
    //  5) Interactions
    //  6) Interaction Components this is broken into a two part array so the correct component can be written in. pg27 of spec
    //      -- should kinda be [,[,]]
    //################################################################

    class Definition
    {
    public:
        Definition(long Id,
        GUID2 UUID,  
        string Name,
        string Description,
        string Type,
        string MoreInfo,
        Extensions Extensions,
        string InteractionType,
        string CorrectResponsesPattern,
        string InteractionComponents)
            : Id(Id), UUID(UUID), Name(Name), Description(Description), Type(Type), MoreInfo(MoreInfo), Extensions(Extensions), InteractionType(InteractionType), CorrectResponsesPattern(CorrectResponsesPattern), InteractionComponents(InteractionComponents)
        {}

        //1
        long Id;
        GUID2 UUID; // lets use this to link? otherwise it is not stated as needed

        //2        
        string Name;
        string Description;

        //3
       /* Uri Type;
        Uri MoreInfo;*/
        string Type;
        string MoreInfo;

        //4
        Extensions Extensions;

        //5
        string InteractionType;
        string CorrectResponsesPattern;

        //6
        string InteractionComponents;

    };
};
