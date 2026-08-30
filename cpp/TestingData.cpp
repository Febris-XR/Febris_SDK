// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"
//#include "TestingData.h"


namespace FebrisCppEnumsSharedDetails
{
	/// <summary>
	/// used for testing
	/// </summary>
	class TestingData
	{
	public:

		string SelectTimeStamp(int input)
		{
			string outputString;
			try
			{
				if (input < 10)
				{
					outputString = "'timestamp':'2022-0" + to_string(input) + "-08T23:02:46.4362204Z',";
				}
				else
				{
					outputString = "'timestamp':'2022-" + to_string(input) + "-08T23:02:46.4362204Z',";
				}
				//outputString = "'timestamp':'2022-" + input + "-8T23:02:46.4362204Z',";                
			}
			catch (const std::runtime_error& e)
			{
				//_log.Error(ex.Message);                
			}
			return outputString;

		};

		string SelectActor(int input)
		{
			string outputString;// = string.Empty;
			try {
				switch (input)
				{
				case 1:
					outputString = "'actor':{ 'id':1,'uuid':'72446f6c-12a4-4276-bcd0-c53a4bce0ff0','objectType':'Agent','name':'Jaycee_Chenf1b4bc15-61be-4c37-91a3-432bdc56e756','mbox':null,'mbox_sha1sum':'d02e762fa2418293fc7812cdbba66830e68ee857','openId':null,'account':null,'member':null},";
					break;
				case 2:
					outputString = "'actor':{ 'id':2,'uuid':'d42b4d40-438a-4370-b38e-18568fa26862','objectType':'Agent','name':'Briar_Maddenea1b6049-96eb-4753-8029-ea8da197c0a3','mbox':null,'mbox_sha1sum':'cc575dc06d1ae982e8489e316aa2024fe8fa4943','openId':null,'account':null,'member':null},";
					break;
				case 3:
					outputString = "'actor':{ 'id':3,'uuid':'4ece0389-00a4-47ec-b27b-1f61ac28f217','objectType':'Agent','name':'Magnus_Doe11511293-7d01-47f4-bb22-0566457741d5','mbox':null,'mbox_sha1sum':'818c473b9a47779ff362bc17711f5ebdd90ee183','openId':null,'account':null,'member':null},";
					break;
				case 4:
					outputString = "'actor':{ 'id':4,'uuid':'0aae164f-4af4-455d-bc9d-84a101ec8fe2','objectType':'Agent','name':'Blaze_Dyerc01b603d-ed82-44fa-9bb3-9389ecae694f','mbox':null,'mbox_sha1sum':'f3e146def5f882bc51e13cdc2fddeb3c549b7bf9','openId':null,'account':null,'member':null},";
					break;
				case 5:
					outputString = "'actor':{ 'id':5,'uuid':'b683c809-c07f-4d66-8465-f39821d1d037','objectType':'Agent','name':'Londyn_Cochran24844061-0c0b-48be-8c33-199fca8c88b3','mbox':null,'mbox_sha1sum':'249f6fe162ae975bf77a88bc29f021bbafacc3a4','openId':null,'account':null,'member':null},";
					break;

				}

			}
			catch (const std::runtime_error& e)
			{
				//_log.Error(ex.Message);                
			}
			return outputString;

		};

		string SelectObject(int input)
		{
			string outputString;
			string objectString;
			string verbString;
			string attachmentString;
			try
			{
				switch (input)
				{

				case 1:
					objectString = "'Object':{ 'Key':3,'UUID':'c066879b-abdb-47ef-b6b2-e6ffcc4c13e8','Id':'https://febr.is/Module/93235451-91ed-49ee-bab6-893110dededc','ObjectType':'Activity','Definition':{ 'Id':3,'UUID':'184ccd1d-5d72-4290-b983-d9f5b12d448a','Name':{'en':'APK Test Module'},'description':{'en':'APK Test Module. This is a nearly empy module and I honestly have no idea if it even runs. Used to test distribution and installations. '},'type':'https://febr.is/Module/2','moreInfo':'https://febr.is/Module/2','extensions':null,'interactionType':'performance','correctResponsesPattern':'[,]','interactionComponents':'label1:input1'} }";
					verbString = SelectVerb(1);
					outputString = verbString + objectString + attachmentString;
					break;
				case 2:
					objectString = "'Object':{ 'Key':2,'UUID':'f82603a4-68e1-4c47-9a7e-e156303bbdb6','Id':'https://febr.is/Module/18f6d877-ee0a-4a5c-8e49-81332b07aab5','ObjectType':'Activity','Definition':{ 'Id':2,'UUID':'277efb51-fa12-4ee1-8f31-ddbeaf2df16b','Name':{'en':'Test Module 1'},'description':{'en':'Simple test sterile field preparation'},'type':'https://febr.is/Module/1','moreInfo':'https://febr.is/Module/1','extensions':null,'interactionType':'performance','correctResponsesPattern':'[,]','interactionComponents':'label1:input1,label2:input2'} }";
					verbString = SelectVerb(1);
					attachmentString = SelectAttachment(RandomAttachment());
					outputString = verbString + objectString + attachmentString;
					break;
				}
			}
			catch (const std::runtime_error& e)
			{
				//_log.Error(ex.Message);                
			}
			return outputString;

		};

		string SelectVerb(int input)
		{
			string outputString;
			try
			{

				switch (input)
				{
					//case 1:
					//    outputString = "'Verb': {'Key': 1,'UUID': 'f72789c6-47ee-460f-8b68-05ca7d6f1cf9','Id': 'https://febr.is/xAPI/VerbDetails/Attempted','Display': {'en': 'Attempted'}},";
					//    break;
					//case 2:
					//    outputString = "'Verb': {'Key': 3,'UUID': '24fb0f47-a85d-4e01-931b-ef2baeebe263','Id': 'https://febr.is/xAPI/VerbDetails/Initialized','Display': {'en': 'Initialized'}},";
					//    break;
					//    
				case 1:
					outputString = "'Verb':{ 'Key':1,'UUID':'784531c7-a7d4-4936-b9e1-2a0467a62f78','Id':'https://febr.is/Verb/Details/Initialized','Display':{'en':'Initialized'}},";
					break;
				case 2:
					outputString = "'Verb':{ 'Key':2,'UUID':'2bfd879d-a90c-471c-9d5e-bf29725be4a8','Id':'https://febr.is/Verb/Details/Pass','Display':{'en':'Pass'}},";
					break;
				case 3:
					outputString = "'Verb':{ 'Key':3,'UUID':'4a2db59d-8480-40e1-a76b-943e9c8e2a68','Id':'https://febr.is/Verb/Details/Not_Pass','Display':{'en':'Not_Pass'}},";
					break;
				case 4:
					outputString = "'Verb':{ 'Key':4,'UUID':'dc6ec833-f6a8-410c-9466-8cd39e95fef1','Id':'https://febr.is/Verb/Details/Completed','Display':{'en':'Completed'}},";
					break;
				case 5:
					outputString = "'Verb':{ 'Key':5,'UUID':'a8cde4a8-43b4-49a8-ae77-6d56c5b5d20a','Id':'https://febr.is/Verb/Details/Terminated_Early','Display':{'en':'Terminated_Early'}},";
					break;
				default:
					outputString = "'Verb':{ 'Key':1,'UUID':'784531c7-a7d4-4936-b9e1-2a0467a62f78','Id':'https://febr.is/Verb/Details/Initialized','Display':{'en':'Initialized'}},";
					break;


				}
			}
			catch (const std::runtime_error& e)
			{
				//_log.Error(ex.Message);                
			}
			return outputString;
		};

		string SelectAttachment(int input)
		{
			string outputString;// = string.Empty;
			try
			{
				switch (input)
				{
				case 1:
					outputString;// = string.Empty;
					break;
				case 2:
					outputString = ",'Attachments': [{'UsageType': 'https://febrisportal.com/xApi/attachments/video_review','Display': {'en': '2e9d392a-d07f-4498-a3ba-8e8360cd95f5'},'Description': {'en': 'Video of conducted education'},'ContentType': 'video/mp4','Sha2': '2c75c64a083e5cfa5e934573391878391fb8c9f407d35b276a864eaa071ccb05','FileURL': 'https://FebrisPortal.com/widget/videoloader?videoName=2e9d392a-d07f-4498-a3ba-8e8360cd95f5'}]";
					break;
				}
			}
			catch (const std::runtime_error& e)
			{
				//_log.Error(ex.Message);                
			}
			return outputString;
		};

		SharedDetails _sharedDetails;
		string StatementStarter = _sharedDetails.StatementPreface + "{";
		string StatementEnder = "}";



		 int RandomAttachment()
		 {
			 /*int random = new Random();
			 Thread.Sleep(100);*/
			 return 1;// random.Next(1, 3);
		 }
	};
}
