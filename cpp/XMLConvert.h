// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef XMLCONVERT_H
#define XMLCONVERT_H
//#include "pch.h";
#endif 


class XMLCONVERT_H XMLConvert {
public: 
    static string millisecondsToIso8601(long long durationMs);
};