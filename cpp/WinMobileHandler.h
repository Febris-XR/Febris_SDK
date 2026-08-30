// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef WINMOBILEHANDLER_H
#define WINMOBILEHANDLER_H
//#include "pch.h";
#endif 


class WINMOBILEHANDLER_H WinMobileHandler : public IEnvironmentHandler
{
public:

	virtual HandlerResult CreateInitialPost(XApiJson::ojson& statementFromDataModel) override;

	virtual HandlerResult UpdatePost(XApiJson::ojson& statementFromDataModel) override;

	virtual HandlerResult ErrorPost(XApiJson::ojson& statementFromDataModel) override;


};
#pragma once
