// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "pch.h"
//#include "Definition.h"
#ifndef OBJECT_H
#define OBJECT_H
//#include ".h";

#endif 

//#ifndef DEFINITION_H
//#define DEFINITION_H
////#include ".h";
//#include "pch.h";
//#endif

//
//#ifdef FEBRISCPPINT_EXPORTS
//#define FEBRISCPPINT_API __declspec(dllexport)
//#else
//#define FEBRISCPPINT_API __declspec(dllimport)
//#endif


class OBJECT_H Object {
public:
    Object(long Key,
        GUID2 UUID,
        //Uri Id,
        string Id,
        string ObjectType,
        Definition Definition);
    Object() : Key(0), UUID{} {}  // zero scalars: C# field-default parity
    long Key;
    GUID2 UUID;
    //Uri Id;
    string Id;
    string ObjectType;
    Definition Definition;

};

//class DEFINITION_H Definition
//{
//public:
//    Definition(long Id,
//        GUID UUID,
//        string Name,
//        string Description,
//        string Type,
//        string MoreInfo,
//        Extensions Extensions,
//        string InteractionType,
//        string CorrectResponsesPattern,
//        string InteractionComponents);
//    //1
//    long Id;
//    GUID UUID; // lets use this to link? otherwise it is not stated as needed
//
//    //2        
//    string Name;
//    string Description;
//
//    //3
//   /* Uri Type;
//    Uri MoreInfo;*/
//    string Type;
//    string MoreInfo;
//
//    //4
//    Extensions Extensions;
//
//    //5
//    string InteractionType;
//    string CorrectResponsesPattern;
//
//    //6
//    string InteractionComponents;
//
//};