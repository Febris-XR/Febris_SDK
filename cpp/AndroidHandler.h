// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef ANDROIDHANDLER_H
#define ANDROIDHANDLER_H
//#include "pch.h";
#endif 


class ANDROIDHANDLER_H AndroidHandler : public IEnvironmentHandler
{
public:

	/*not sure why but might need json&?*/
	
	virtual HandlerResult CreateInitialPost(XApiJson::ojson& statementFromDataModel) override;

	virtual HandlerResult UpdatePost(XApiJson::ojson& statementFromDataModel) override;

	virtual HandlerResult ErrorPost(XApiJson::ojson& statementFromDataModel) override;

	//virtual std::pair<bool, std::vector<std::vector<std::string>>>
	//	CreateInitialPost(json statementFromDataModel) override;

	//virtual std::pair<bool, std::vector<std::vector<std::string>>>
	//	UpdatePost(json statementFromDataModel) override;

	/*virtual std::pair<bool, std::vector<std::vector<std::string>>>
		ErrorPost(json statementFromDataModel) override;*/

};
#pragma once
