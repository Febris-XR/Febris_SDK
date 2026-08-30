// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "pch.h"
#ifndef FILEINFO_HELPERS_H
#define FILEINFO_HELPERS_H
//#include "pch.h";
#endif 

class FILEINFO_HELPERS_H FileInfo
{
public:
    string name;
    uintmax_t size;
    bool isDirectory;
    FileInfo(const std::filesystem::directory_entry& entry);
    
};
