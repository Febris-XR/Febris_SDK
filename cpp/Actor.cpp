// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
//#include "Actor.h";
//#include "Member.h";
//#include "Account.h";
#include "pch.h"
//using namespace std;

namespace FebrisCppModelsxAPI
{
	//################################################################
	//Todo: need to make the member an array so it can handle groups        
	//Will need to change Member over to an array[]
	// Need to add in Inverse Functional Identifiers in here. 
	// -- But members needs to be an array or maybe make member just an array of Actors?
	//  1) table Ids
	//  2) object types and names
	//  3) Inverse Function Identifier - there can be only one
	//      --  I am not sure how to handle this mbox or mbox_sha1sum or maybe openId
	//  4) Array of Actors
	//Note: if the object type is a "group" it must always be considered distinct if anonymous 
	//      - These is no identifier for this cluster. it is an "ad hoc team"
	//
	//################################################################

	class Actor
	{
	public:
		Actor(long Id, GUID2 UUID, string ObjectType, string Name, string Mbox, string Mbox_sha1sum, string OpenId, Account Account, Member Member) 
			:Id(Id), UUID(UUID), ObjectType(ObjectType), Name(Name), Mbox(Mbox), Mbox_sha1sum(Mbox_sha1sum), OpenId(OpenId), Account(Account), Member(Member) 
		{}
		//Person(std::string name, int age, std::string occupation = "", bool isEmployed = false, bool hasDriverLicense = false) : name(name), age(age), occupation(occupation), isEmployed(isEmployed), hasDriverLicense(hasDriverLicense) {}
		Actor() = default;
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
		Account Account;

		//4        
		Member Member;
	};
	//################################################################
	// Mbox needs to be of type mailtoIRI
	//I honestly cant tell if I need to change this. Or if I need to not have the actor the way I did before when creating them with users
	//I need this member set up to be able to use in the array - But would not be used if object type is an "Agent"
	//
	//################################################################

	class Member
	{
	public:
		Member(long Id, GUID2 UUID, vector<Actor> Actors)
			:Id(Id), UUID(UUID), Actors(Actors)
		{}
		Member() = default;
		long Id;
		GUID2 UUID;
		vector<Actor> Actors;		
	};
	//################################################################    
	//This is apparently made to use OAuth authentication. 
	//################################################################

	class Account
	{
	public:
		Account(long Id, GUID2 UUID, string HomePage, string Name)
			:Id(Id), UUID(UUID), HomePage(HomePage), Name(Name)
		{}
		Account() = default;
		long Id;
		GUID2 UUID;
		//[Required]
		/*Uri HomePage;*/
		string HomePage;
		//[Required]
		string Name;
	};
};
