// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "pch.h"
#ifndef FILENAMEHANDLER_H
#define FILENAMEHANDLER_H
//#include "pch.h";
#endif 

class FILENAMEHANDLER_H FileNameHandler
{
    //private static readonly ILogger //_log = Log.Logger;
public:
    static void StoreFileName(string fileName);
    static string GetFileName();
    static void CleanMMF();
};