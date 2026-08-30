// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef CONTEXTACTIVITIES_H
#define CONTEXTACTIVITIES_H


#endif 


class CONTEXTACTIVITIES_H ContextActivities
{
public:
	ContextActivities(long Id,
		GUID2 UUID,
		string Parent,
		string Grouping,
		string Category,
		string Other);
	ContextActivities() : Id(0), UUID{} {}  // zero scalars: C# field-default parity
	long Id;
	GUID2 UUID;
	string Parent;
	string Grouping;
	string Category;
	string Other;

};