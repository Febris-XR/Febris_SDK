// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
//#include "Attachment.h"
#include "pch.h"

namespace FebrisCppModelsxAPI
{
	//################################################################
	//  1) sql ids
	//  2) usageType... not really sure but it is the correct type
	//  3) language mapped strings
	//  4) This is an internet media type. It is the content type of the 
	//          -- is most likely "application/octet-stream"
	//  5) Length of the attachment data in octets
	//  6) hash of the attachment data
	//  7) an irl at which the attachment data can be retrieved, or from which it used to be retrievable
	//################################################################

	class Attachment
	{
	public:
		Attachment(long Id,
			GUID2 UUID,
			string UsageType,
			string Display,
			string Description,
			string ContentType,
			int Length,
			string Sha2,
			string FileURL)
			:Id(Id), UUID(UUID), UsageType(UsageType), Display(Display), Description(Description), ContentType(ContentType), Length(Length), Sha2(Sha2), FileURL(FileURL)
		{}
		Attachment() = default;
		//1
		long  Id;
		GUID2 UUID;
		//2        
		//Uri UsageType;
		string UsageType;

		//3               
		string Display;
		string Description;

		//4        
		string ContentType; //ie "application/octet-stream"
		//5         
		int Length;
		//6        
		string Sha2;
		//7
		//Uri FileURL;//user UUID to name video file
		string FileURL;
	};
};
