// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "pch.h"
#ifndef EXTENSIONIRIOPTIONS_H
#define EXTENSIONIRIOPTIONS_H
//#include "pch.h";
#endif 

enum class EXTENSIONIRIOPTIONS_H ExtensionIRIOptions
{
    RestartCounterIRI,
    NotesIRI,
};

#ifndef EXTENSIONIRIRESOLVER_H
#define EXTENSIONIRIRESOLVER_H
//#include "pch.h";
#endif 

class EXTENSIONIRIRESOLVER_H ExtensionIRIResolver
{
public:
    static string ResolveExtensionIRI(ExtensionIRIOptions iri);
    static ExtensionIRIOptions ResolveExtensionIRI(string currentExtension);
};