// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef CONTEXTOPTIONS_H
#define CONTEXTOPTIONS_H
//#include "pch.h";
#endif 
enum class CONTEXTOPTIONS_H ContextOptions
{
	Registration,
	Instructor,
	Group,
	ContextActivites,
	Revision,
	Platform,
	Language,
	StatementReference,
	Extensions
};

#ifndef CONTEXTACTIVITESOPTIONS_H
#define CONTEXTACTIVITESOPTIONS_H
//#include "pch.h";
#endif 
enum class CONTEXTACTIVITESOPTIONS_H ContextActivitesOptions
{
	Parent,
	Grouping,
	Category,
	Other
};

#ifndef CONTEXTSTATEMENTREFERENCEOPTIONS_H
#define CONTEXTSTATEMENTREFERENCEOPTIONS_H
//#include "pch.h";
#endif 
enum class CONTEXTSTATEMENTREFERENCEOPTIONS_H ContextStatementReferenceOptions
{
	Id,
	ObjectType
};

#ifndef CONTEXTEXTENSIONOPTIONS_H
#define CONTEXTEXTENSIONOPTIONS_H
//#include "pch.h";
#endif 
enum class CONTEXTEXTENSIONOPTIONS_H ContextExtensionOptions
{
	Option1,
	Option2
};

#ifndef CONTEXTOPTIONRESOLVER_H
#define CONTEXTOPTIONRESOLVER_H
//#include "pch.h";
#endif 
class CONTEXTOPTIONRESOLVER_H ContextOptionResolver
{
public:
	static string ResolveExtensionIRI(ContextOptions option);

	static string ContextExtensionOptionResolver(ContextExtensionOptions option);

};