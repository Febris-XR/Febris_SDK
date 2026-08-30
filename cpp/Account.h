// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef ACCOUNT_H
#define ACCOUNT_H

#endif 
//
//class ACCOUNT_H Account
//{
//public:
//	long Id;
//	GUID UUID;
//	//[Required]
//	/*Uri HomePage;*/
//	string HomePage;
//	//[Required]
//	string Name;
//};

class ACCOUNT_H Account
{
public:
	Account(long Id, GUID2 UUID, string HomePage, string Name);
	Account() : Id(0), UUID{} {}  // zero scalars: C# field-default parity
	long Id;
	GUID2 UUID;
	//[Required]
	/*Uri HomePage;*/
	string HomePage;
	//[Required]
	string Name;
};

