// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
//#include "Authority.h"
#include "pch.h"

namespace FebrisCppModelsxAPI
{
	//################################################################
	//  1) these are sql references
	//  2) Need an actor class
	//  
	//################################################################

	class Authority
	{
	public:
		Authority(long Id,
			GUID2 UUID,
			Actor Actor)
			:Id(Id), UUID(UUID), Actor(Actor)
		{}
		Authority() = default;
		//1
		long Id;
		GUID2 UUID;
		//2        
		Actor Actor;
	};
};
