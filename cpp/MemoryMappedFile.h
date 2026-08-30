// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef MEMORYMAPPEDFILE_H
#define MEMORYMAPPEDFILE_H
//#include "pch.h";
#endif 

class MemoryMappedFile {
public:
	MemoryMappedFile(const std::string& filePath, std::size_t size);
	char* GetData() const;
	//MemoryMappedFile();// : shm(open_or_create, "example", read_write), region(shm, read_write);
	//void writeData(const char* message);
	//void readData();
	~MemoryMappedFile();
private:
	boost::interprocess::mapped_region* m_region;
	/*shared_memory_object shm;
	mapped_region region;*/
};