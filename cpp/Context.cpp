// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
//#include "Context.h"


namespace FebrisCppModelsxAPI
{
	//################################################################
	//  1)sql ids
	//  2) Needs to be a UUID 
	//  3) If there is an instructor and they are also an Actor/Group it can be linked
	//  4) This is an array of actors for a group if not included in the Actor of the statement
	//  5) Valid context types : 
	//            "parent", "grouping", "category", "other"
	//  6) Reision of the learning activitiy (Testbase.version)
	//  7) Platform used (febris)
	//  8) Language based on Language map RFC 5646
	//  9) statement
	//  10) extensions
	//################################################################

	class Context
	{
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
			Extensions Extensions)
			:Id(Id), UUID(UUID), Registration(Registration), Instructor(Instructor), Group(Group), ContextActivities(ContextActivities), Revision(Revision),
			Platform(Platform), Language(Language), StatementReference(StatementReference), Extensions(Extensions)
		{}
		Context() = default;
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
		StatementReference StatementReference;
		//10
		Extensions Extensions;
	};
	//################################################################
	// There is more information to be found in spec page 36
	//################################################################

	class ContextActivities
	{
	public:
		ContextActivities(long Id,
			GUID2 UUID,
			string Parent,
			string Grouping,
			string Category,
			string Other)
			:Id(Id), UUID(UUID), Parent(Parent), Grouping(Grouping), Category(Category), Other(Other)
		{}
		ContextActivities()=default;

		long Id;
		GUID2 UUID;

		string Parent;
		string Grouping;
		string Category;
		string Other;

	};
};
