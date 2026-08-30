// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"

#ifndef SCORE_H
#define SCORE_H
//#include ".h";

#endif

class SCORE_H Score
{
public:
    Score(long Id,
        GUID2 UUID,
        float Scaled,
        float Raw,
        float Min,
        float Max);
    Score() : Id(0), UUID{}, Scaled(0.0f), Raw(0.0f), Min(0.0f), Max(0.0f) {}  // zero scalars: C# field-default parity
    long Id;
    GUID2 UUID;
    float Scaled;
    float Raw;
    float Min;
    float Max;
};