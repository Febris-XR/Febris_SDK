// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#include "Member.h"
#ifndef ACTOR_H
#define ACTOR_H
//#include "Account.h"
#endif 

//#ifndef MEMBER_H
//#define MEMBER_H
//#include "pch.h";
//#endif 

//#ifndef ACCOUNT_H
//#define ACCOUNT_H
//#include "pch.h";
//#endif 

//No idea if this is correct here
//#include "Account.h"
//#include "Member.h"

//class Actor;
//class Member;
//class Account;

class ACTOR_H Actor
{
public:
	Actor(long Id, GUID2 UUID, string ObjectType, string Name, string Mbox, string Mbox_sha1sum, string OpenId, Account Account, Member Member);
		/*:Id(Id), UUID(UUID), ObjectType(ObjectType), Name(Name), Mbox(Mbox), Mbox_sha1sum(Mbox_sha1sum), OpenId(OpenId), Account(Account), Member(Member)
	{}*/
	Actor() : Id(0), UUID{} {}  // zero scalars: C# field-default parity
	//1
	long Id;
	GUID2 UUID;
	//2
	string ObjectType;
	string Name;
	//3 
	//Uri Mbox;
	string Mbox;
	string Mbox_sha1sum;
	//Uri OpenId;
	string OpenId;

	// PARITY (lifecycle harness): optionals mirror the C# nullable references.
	// SetupActor assigns both (its null branch included: wire objects), but the
	// BARE `new Actor()` in SetupContext's null branch (instructor) and
	// SetupAuthority's null branch leaves both references null (wire nulls).
	// A plain value member collapses those shapes; same reasoning as
	// Member.Actors and Result.Extensions.
	std::optional<::Account> Account;
	//ACCOUNT_H Account Account;
	//ACCOUNT_H Account Account;
	//4

	std::optional<::Member> Member;
	//MEMBER_H Member Member;
	//MEMBER_H Member Member;
};

//class ACCOUNT_H Account
//{
//public:
//	Account(long Id, GUID UUID, string HomePage, string Name);
//	long Id;
//	GUID UUID;
//	//[Required]
//	/*Uri HomePage;*/
//	string HomePage;
//	//[Required]
//	string Name;
//};

//class MEMBER_H Member
//{
//public:
//	Member(long Id, GUID UUID, vector<Actor> Actors);
//	long Id;
//	GUID UUID;
//	vector<Actor> Actors;
//};

//#ifndef MEMBER_H
//#define MEMBER_H
//#include "pch.h";
//#endif 
//
//class MEMBER_H Member
//{
//public:
//	long Id;
//	GUID UUID;
//	vector<Actor> Actors;
//};
//
//#ifndef ACCOUNT_H
//#define ACCOUNT_H
//#include "pch.h";
//#endif 
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
