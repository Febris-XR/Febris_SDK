// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.



#ifndef PCH_H
#define PCH_H


// add headers that you want to pre-compile here
#include "framework.h"
#include <string>
#include <vector>
#include <list>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <type_traits>
#include <optional>
#include <memory>
#include <nlohmann/json.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/type_index.hpp>
// boost/filesystem retired 2026-08-29: it needs a compiled lib that packages.config never
// carried (one more never-linked proof). std::filesystem covers the 10 uses (path, directory_entry).
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <cstring>
#include <fstream>
#include <rapidxml/rapidxml.hpp>
#include <random>


using namespace std;
using namespace nlohmann::literals;
using namespace boost::interprocess;
//FILE SYSTEM
namespace fs = std::filesystem;
namespace bfs = std::filesystem;
using json = nlohmann::json;
using GUID2 = boost::uuids::uuid;
inline boost::uuids::string_generator GuidGen;  // inline: defined in every TU via pch, and the DLL now actually LINKS

//Enums
#include "XAPIProperties.h"
#include "ResultOptions.h"
#include "ContentType.h"
#include "ContextOptions.h"
#include "VerbEnums.h"
#include "ExtensionIRIOptions.h"
#include "ExpectedOperatingSystem.h"



//helpers
#include "String_Helpers.h"
#include "Time_Helpers.h"
#include "FileInfo_Helpers.h"
#include "MemoryMappedFile.h"
#include "XMLConvert.h"

//models
//no dependencies
#include "ContextActivities.h"
#include "StatementReference.h"
#include "Extensions.h"
#include "Score.h"
#include "Account.h"

//dependencies
#include "Actor.h"
#include "Member.h"

#include "Definition.h"
#include "Verb.h"
#include "Object.h"
#include "Result.h"
#include "Context.h"

#include "Authority.h"
#include "Attachment.h"
#include "Version.h"
#include "Statement.h"
#include "XApiJson.h"  // SDKV emit port: ordered-key serialization

//File System
#include "FileSystem.h"
#include "FileSystemInitalizer.h"

//handlers
#include "IEnvironmentHandler.h"
#include "AndroidHandler.h"
#include "iOSHandler.h"
#include "WinMobileHandler.h"
#include "WinPCHandler.h"

//Shared and StaticDetails
#include "SharedDetails.h"
#include "StaticDetails.h"

//core
#include "Initializer.h"
//#include "FileSystem.h"
#include "FileNameHandler.h"
//#include "FileSystemInitalizer.h"
#include "TestingData.h"
#include "JSONHandler.h"
#include "StatementHandler.h"
#include "StatementFactoring.h"



#endif //PCH_H
