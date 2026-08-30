// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef RESULTOPTIONS_H
#define RESULTOPTIONS_H
//#include "pch.h";
#endif 
enum class RESULTOPTIONS_H ResultOptions
{
    Success,
    Completion,
    Response,
    Duration,
    Extensions,
    ScoreMin,
    ScoreMax,
    ScoreScale,
    ScoreRaw
};

#ifndef RESULTEXTENSIONOPTIONS_H
#define RESULTEXTENSIONOPTIONS_H
//#include "pch.h";
#endif 
enum class RESULTEXTENSIONOPTIONS_H ResultExtensionOptions
{
    Notes,
    RestartCounter,
    //[Display(Name = "Restart Counter")] RestartCounter
};