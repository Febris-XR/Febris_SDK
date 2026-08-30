// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef MEMBER_H
//#include "pch.h";
//#include "Actor.h"
#define MEMBER_H
#endif 
//
////typedef struct Member
//class MEMBER_H Member
//{
//public:
//	long Id;
//	GUID UUID;
//	vector<Actor> Actors;
//};

class Actor;

class MEMBER_H Member
{
public:
	Member(long Id, GUID2 UUID, vector<Actor> Actors);
	Member() : Id(0), UUID{} {}  // zero scalars: C# field-default parity
	long Id;
	GUID2 UUID;
	// PARITY (harness): optional mirrors the C# nullable reference exactly. The
	// factoring produces two distinct wire shapes: SetupMember assigns the list
	// (empty included) -> "actors": [...] / [], while SetupActor's null-actor
	// branch leaves a bare Member -> "actors": null. A plain vector collapses
	// those into one state; the byte-parity harness caught the difference.
	std::optional<vector<Actor>> Actors;

	/*bool operator==(const Member& rhs) const {
		return Actors == rhs.Actors;
	}*/
};