// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "pch.h"
#ifndef AUTHORITY_H
#define AUTHORITY_H
//#include ".h";


#endif 


//
//#ifdef FEBRISCPPINT_EXPORTS
//#define FEBRISCPPINT_API __declspec(dllexport)
//#else
//#define FEBRISCPPINT_API __declspec(dllimport)
//#endif


class AUTHORITY_H Authority {
public:
	Authority(long Id,
		GUID2 UUID,
		Actor Actor);
	Authority() : Id(0), UUID{} {}  // zero scalars: C# field-default parity
	//1
	long Id;
	GUID2 UUID;
	//2        
	Actor Actor;

};