// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef VERBENUMS_H
#define VERBENUMS_H
//#include "pch.h";
#endif 
enum class VERBENUMS_H VerbEnums
{
	Attempted,
	Completed,
	Initialized,
	Terminated,
	Pass,
	Not_Pass
};

#ifndef VERBIRIRESOLVER_H
#define VERBIRIRESOLVER_H
//#include "pch.h";
#endif 
class VERBIRIRESOLVER_H VerbIRIResolver
{
public:
	static string ResolveVerbIRI(VerbEnums iri);

	static VerbEnums GetVerbEnum(string currentVerb);
};