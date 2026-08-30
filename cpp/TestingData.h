// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef TESTINGDATA_H
#define TESTINGDATA_H
//#include "pch.h";
#endif 

class TESTINGDATA_H TestingData
{
public:

	string SelectTimeStamp(int input);
	string SelectActor(int input);
	string SelectObject(int input);
	string SelectVerb(int input);
	string SelectAttachment(int input);
	int RandomAttachment();
};