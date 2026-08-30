// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "pch.h"
//#include "Score.h"
//#include "Extensions.h"
#ifndef RESULT_H

#define RESULT_H
//#include ".h";

#endif

//#ifndef SCORE_H
//#include "pch.h";
//#define SCORE_H
////#include ".h";
//
//#endif

class RESULT_H Result
{
public:
    Result(long Id,
        GUID2 UUID,
        Score Score,
        bool Success,
        bool Completion,
        string Response,
        long Duration,
        Extensions Extensions);
    Result() : Id(0), UUID{}, Success(false), Completion(false), Duration(0) {}  // zero scalars: C# field-default parity
    long Id;
    GUID2 UUID;
    // PARITY (lifecycle harness): optional mirrors the C# nullable reference; see Member.Actors.
    std::optional<::Score> Score;
    bool Success;
    bool Completion;
    string Response;
    //TimeSpan Duration;
    long Duration;
    // PARITY (lifecycle harness): optional mirrors the C# nullable reference.
    // SetupResult's NULL branch assigns Extensions = new Extensions() (wire
    // shape {id,uuid,"extensionmap":null}), while the input-present branch
    // leaves the C# reference null (wire null) -- two distinct shapes a plain
    // value member collapses. Same reasoning as Member.Actors.
    std::optional<::Extensions> Extensions;
};

//class SCORE_H Score
//{
//public:
//    Score(long Id,
//        GUID UUID,
//        float Scaled,
//        float Raw,
//        float Min,
//        float Max);
//    long Id;
//    GUID UUID;
//    float Scaled;
//    float Raw;
//    float Min;
//    float Max;
//};
