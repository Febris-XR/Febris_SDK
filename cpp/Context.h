// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef CONTEXT_H
#define CONTEXT_H
//#include ".h";

//#include "ContextActivities.h"
//#include "Extensions.h"
//#include "StatementReference.h"
#endif 
//
//#ifndef CONTEXTACTIVITIES_H
//#define CONTEXTACTIVITIES_H
//#include "pch.h";
//
//#endif 

class CONTEXT_H Context {
public:
	Context(long Id,
		GUID2 UUID,
		GUID2 Registration,
		Actor Instructor,
		vector<Actor> Group,
		ContextActivities ContextActivities,
		string Revision,
		string Platform,
		string Language,
		StatementReference StatementReference,
		Extensions Extensions);
	Context() : Id(0), UUID{}, Registration{} {}  // zero scalars: C# field-default parity
	//1
	long Id;
	GUID2 UUID;
	//2
	GUID2 Registration;
	//3
	Actor Instructor;
	//4
	vector<Actor> Group;
	//5
	ContextActivities ContextActivities;
	//6
	string Revision;
	//7
	string Platform;
	//8 
	string Language;
	//9
	// PARITY (lifecycle harness): optional mirrors the C# nullable reference; see Member.Actors.
	std::optional<::StatementReference> StatementReference;
	//10
	Extensions Extensions;

};

//class CONTEXTACTIVITIES_H ContextActivities
//{
//public:
//	ContextActivities(long Id,
//		GUID UUID,
//		string Parent,
//		string Grouping,
//		string Category,
//		string Other);
//	long Id;
//	GUID UUID;
//	string Parent;
//	string Grouping;
//	string Category;
//	string Other;
//
//};