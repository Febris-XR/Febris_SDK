// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef IOSHANDLER_H
#define IOSHANDLER_H
//#include "pch.h";
#endif 


class IOSHANDLER_H iOSHandler : public IEnvironmentHandler
{
public:


	virtual HandlerResult CreateInitialPost(XApiJson::ojson& statementFromDataModel) override;

	virtual HandlerResult UpdatePost(XApiJson::ojson& statementFromDataModel) override;

	virtual HandlerResult ErrorPost(XApiJson::ojson& statementFromDataModel) override;


};
#pragma once
