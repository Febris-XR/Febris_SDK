// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
//#include <string>
//#include <list>
//#include <filesystem>
//#include <iostream>
//#include <guiddef.h>
//#include <chrono>
//#include "Extensions.cpp"
#include "pch.h"
//#include "Result.h"
//using namespace std;

namespace FebrisCppModelsxAPI
{
	//################################################################
	//################################################################

	class Result
	{
	public:
		Result(long Id,
			GUID2 UUID,
			Score Score,
			bool Success,
			bool Completion,
			string Response,
			long Duration,
			Extensions Extensions)
			:Id(Id), UUID(UUID), Score(Score), Success(Success), Completion(Completion), Response(Response), Duration(Duration), Extensions(Extensions)
		{}
		Result() = default;

		long Id;
		GUID2 UUID;
		Score Score;
		bool Success;
		bool Completion;
		string Response;
		//TimeSpan Duration;
		long Duration;
		Extensions Extensions;
	};
	//################################################################
	//################################################################

	class Score
	{
	public:
		Score(long Id,
			GUID2 UUID,
			float Scaled,
			float Raw,
			float Min,
			float Max)
			:Id(Id), UUID(UUID), Scaled(Scaled), Raw(Raw), Min(Min), Max(Max)
		{}

		long Id;
		GUID2 UUID;
		float Scaled;
		float Raw;
		float Min;
		float Max;
	};
	//this needs to be set up differently?
	//class Extensions
	//{

	//}
}
