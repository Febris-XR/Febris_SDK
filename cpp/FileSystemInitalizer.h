// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef FILESYSTEMINITALIZER_H
#define FILESYSTEMINITALIZER_H
//#include "pch.h";
#endif 

class FILESYSTEMINITALIZER_H FileSystemInitalizer
{
public:
	static void FileInitalizer(string externalPath);
	static void FileInitalizer();
	static void CreateFileDirectory(string path, string name);
};
