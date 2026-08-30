// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
// StatementFactoring, HEADER-ONLY as of the 2026-08-29 C++ port.
//
// The inherited layout declared a 34-line class here and DEFINED a second, unrelated
// class of the same name inside namespace FebrisCppStatement in the .cpp. Every
// cross-file caller bound the empty declaration, so no call to this class can ever
// have linked -- one more proof the C++ SDK never built. The real implementation now
// lives here at global scope (in-class static definitions are implicitly inline),
// and the .cpp is a stub.
#include "pch.h"


class StatementFactoring
{
public:
	// static readonly ILogger //_log = Log.Logger;

	//catch (const std::runtime_error& e)
	//        {
	//            //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name+" Error: " + ex.Message);
	//
	//        }

	//#########################################################################################################################
	//May need to convert specific items in string to other strings - ie model names or uuid keys
	//#########################################################################################################################
	static Statement FactorStatement(json input)
	{
		try
		{
			//Statement Variables     
			//time_t timeStamp;
			chrono::system_clock::time_point timeStamp;
			Actor actor;// = new Actor();
			Object xAPIObject;// = new Models.xAPI.Object();
			Verb verb;// = new Verb();
			Result result;// = new Result();
			Context context;// = new Context();
			Authority authority;// = new Authority();
			std::optional<Version> version;// = new Models.xAPI.Version();
			vector<Attachment> attachments;// = new vector<Attachment>();
			json::value_t statementToken;// = input;
			//***************************************************needs to go to lowercase
			//json converToLower = Object.from;
			//ChangePropertiesToLowerCase(input);                

			//timestamp
			timeStamp = SetUpTimeStamp(input["timestamp"]);
			//Get actor 
			actor = SetupActor(input["actor"]);
			//Get object
			xAPIObject = SetupObject(input["object"]);
			//Get verb
			verb = SetupVerb(input["verb"]);
			//Get result
			result = SetupResult(input["result"]);
			//Get context
			context = SetupContext(input["context"]);
			//Get authority
			authority = SetupAuthority(input["authority"]);
			//Get version
			version = SetupVersion(input["version"]);
			//Get attachments
			attachments = SetupAttachments(input["attachments"]);

			Statement statement = Statement();
			statement.Timestamp = timeStamp;
			statement.Actor = actor;
			statement.Object = xAPIObject;
			statement.Verb = verb;
			statement.Result = result;
			statement.Context = context;
			statement.Authority = authority;
			statement.Version = version;
			statement.Attachments = attachments;

			return statement;
		}
		catch (const std::runtime_error& e)
		{
			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
			throw;//return;// nullptr;
		}
	};

	//#region Actor
		//#########################################################################################################################
		//May need to convert specific items in string to other strings - ie model names or uuid keys
		//#########################################################################################################################
	static Actor SetupActor(json input)
	{
		try
		{
			//variables
			Actor actor;// = new Actor();
			Member member;// = new Member();
			Account account;// = new Account();
			//check if token is nullptr
			if (input == nullptr) //== json::empty)
			{
				actor.Account = account;
				actor.Member = member;
				/*actor = new Actor()
				{
					Member = member,
					Account = account
				};*/
				return actor;
			}

			//get member info
			member = SetupMember(input["member"]);
			//get account info
			account = SetupAccount(input["account"]);

			actor.Id = (long)input["id"];
			actor.UUID = TokenToGuid(input["uuid"]);
			actor.ObjectType = TokenToStringValue(input["objecttype"]);
			actor.Name = TokenToStringValue(input["name"]);
			actor.Mbox = TokenToStringValue(input["mbox"]);
			actor.Mbox_sha1sum = TokenToStringValue(input["mbox_sha1sum"]);
			actor.OpenId = TokenToStringValue(input["openid"]);
			actor.Account = account;
			actor.Member = member;
			/*actor = new Actor()
			{
				Id = (long)input["id"],
				UUID = TokenToGuid(input["uuid"]),
				ObjectType = (string)input["objecttype"],
				Name = (string)input["name"],
				Mbox = input["mbox"],
				Mbox_sha1sum = (string)input["mbox_sha1sum"],
				OpenId = input["openid"],
				Account = account,
				Member = member
			};*/

			return (actor);
		}
		catch (const std::runtime_error& e)
		{
			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
			throw;//return nullptr;
		}
	};

	//#########################################################################################################################
	//get Member data    
	//#########################################################################################################################
	static Member SetupMember(json input)
	{
		try
		{
			Member member;// = new Member();
			vector<Actor> actorList;// = new vector<Actor>();
			//check if token is nullptr
			if (input == nullptr)
			{
				// C# assigns the empty list here (wire "actors": []); only a wholly
				// absent ACTOR yields the bare Member whose actors emit null.
				member.Actors = actorList;
				/*member = new Member()
				{
					Actors = actorList
				};*/

				return member;
			}



			for (auto item : input)//may need to add ["actor"]
			{
				Actor actor = SetupActor(item);
				//actorList.Append(actor);
				actorList.push_back(actor);
			}
			member.Actors = actorList;
			/*member = new Member()
			{
				Actors = actorList
			};*/

			return (member);
		}
		catch (const std::runtime_error& e)
		{
			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
			throw;//return nullptr;
		}
	};

	//#########################################################################################################################
	//get Account data    
	//#########################################################################################################################
	static Account SetupAccount(json input)
	{
		try
		{
			Account account;// = new Account();
			//check if token is nullptr
			if (input == nullptr)
			{
				return account;
			}

			account.HomePage = TokenToStringValue(input["homepage"]);
			account.Name = TokenToStringValue(input["name"]);
			/*account = new Account()
			{
				HomePage = input["homepage"],
				Name = (string)input["name"],
			};*/

			return (account);
		}
		catch (const std::runtime_error& e)
		{
			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
			//return default;
			throw;
		}
	};


	//#endregion

	//#region Object
		//#########################################################################################################################
		//May need to convert specific items in string to other strings - ie model names or uuid keys
		//#########################################################################################################################
	static Object SetupObject(json input)
	{
		try
		{
			
			//get definition
			Definition definition;// = new Definition();
			Object xAPIObject;// = new Models.xAPI.Object();

			//check if token is nullptr
			if (input == nullptr)
			{
				return xAPIObject;
			}

			definition = SetupObjectDefinition(input["definition"]);

			xAPIObject.Key = (long)input["key"];
			xAPIObject.UUID = TokenToGuid(input["uuid"]);
			xAPIObject.Id = TokenToStringValue(input["id"]);
			xAPIObject.ObjectType = TokenToStringValue(input["objecttype"]);
			xAPIObject.Definition = definition;
			/*xAPIObject = new Models.xAPI.Object()
			{
				Key = (long)input["key"],
					UUID = TokenToGuid(input["uuid"]),
					Id = input["id"],
					ObjectType = (string)input["objecttype"],
					Definition = definition
			};*/


			return (xAPIObject);
		}
		catch (const std::runtime_error& e)
		{
			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
			throw;//return nullptr;
		}
	};
	//#########################################################################################################################
	//Setup object definition
	//#########################################################################################################################
	static Definition SetupObjectDefinition(json input)
	{
		try
		{
			Definition definition;// = new Definition();
			Extensions extensions;// = new Extensions();
			//check if token is nullptr
			if (input == nullptr)
			{
				definition.Extensions = extensions;
				/*= new Definition()
				{
					Extensions = extensions
				};*/
				return definition;
			}

			//get name
			// SDKV-13 / SDKV-1: language maps and the list shape, mirroring the C# side.
			map<string, string> name = TokenToLanguageMap(input["name"]);
			map<string, string> description = TokenToLanguageMap(input["description"]);
			vector<string> correctResponsesPattern = TokenToStringList(input["correctresponsespattern"]);
			string interactionComponents = TokenToString(input["interactioncomponents"]);

			extensions = SetupObjectExtensions(input["extensions"]);

			definition.Name = name;
			definition.Description = description;
			definition.Type = TokenToStringValue(input["type"]);
			definition.MoreInfo = TokenToStringValue(input["moreinfo"]);
			definition.Extensions = extensions;
			definition.InteractionType = TokenToStringValue(input["interactiontype"]);
			definition.CorrectResponsesPattern = correctResponsesPattern;
			definition.InteractionComponents = interactionComponents;
			//definition = new Definition()
			//{
			//	//Name = (string)input["name"].ToString(),
			//	//Description = (string)input["description"].ToString(),
			//	Name = name,
			//	Description = description,
			//	Type = input["type"],
			//	MoreInfo = input["moreinfo"],
			//	Extensions = extensions,
			//	InteractionType = (string)input["interactiontype"],
			//	CorrectResponsesPattern = correctResponsesPattern,
			//	//CorrectResponsesPattern = (string)input["correctresponsespattern"],
			//	//InteractionComponents = (string)input["interactioncomponents"]
			//	InteractionComponents = interactionComponents
			//};

			return (definition);
		}
		catch (const std::runtime_error& e)
		{
			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
			throw;//return nullptr;
		}
	}
	//#########################################################################################################################
	//Setup object extensions
	//#########################################################################################################################
	static Extensions SetupObjectExtensions(json input)
	{
		try
		{
			Extensions extension;// = new  Extensions();
			//check if token is nullptr
			if (input == nullptr)
			{
				return extension;
			}

			// Extensions extensionList = new vector< Extensions>();
			//foreach (autoitem in input)
			//{
			extension.Id = (long)input["id"];				
			extension.UUID = GuidGen((string)input["uuid"]);
			extension.ExtensionMap = TokenToStringValue(input["extensionmap"]);
			/*extension = new  Extensions()
			{
				Id = (long)input["id"],
					UUID = TokenToGuid(input["uuid"]),
					ExtensionMap = (string)input["extensionmap"]
			};*/

			//    extensionList.Add(extension);

			//}

			return (extension);
		}
		catch (const std::runtime_error& e)
		{
			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
			throw;//return nullptr;
		}
	}
	//#endregion

	//#region Verb
		//#########################################################################################################################
		//May need to convert specific items in string to other strings - ie model names or uuid keys
		//#########################################################################################################################
	static Verb SetupVerb(json input)
	{
		try
		{
			//check if token is nullptr
			// PARITY NOTE: C# returns null here (wire "verb": null). The C++ value
			// model cannot represent that; a default Verb is the closest shape. The
			// bare `throw;` this replaces was a std::terminate landmine (no active
			// exception to rethrow). Parity fixtures always author a verb.
			if (input == nullptr)
			{
				return Verb();
			}
			Verb verb;// = new Verb();
			bool verbFound = false;

			verb.Key = (long)input["key"];
			verb.UUID = TokenToGuid(input["uuid"]);
			verb.Id = input["id"];
			verb.Display = TokenToLanguageMap(input["display"]);  // SDKV-13
			/*verb = new Verb()
			{
				Key = (long)input["key"],
				UUID = TokenToGuid(input["uuid"]),
				Id = input["id"],
				Display = (string)input["display"].ToString()
			};*/

			return (verb);
		}
		catch (const std::runtime_error& e)
		{
			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
			throw;//return nullptr;
		}
	};
	//#endregion

	//#region Result
		//#########################################################################################################################
		// 
		//#########################################################################################################################
	static Result SetupResult(json input)
	{
		try
		{
			//variables
			Extensions extensions;// = new  Extensions();
			// Engaged default: the C# null branch assigns `score = new Score()`
			// (wire object); the present branch may replace it with nullopt when
			// the authored result carries no score (wire null).
			std::optional<Score> score = Score();// = new Score();
			Result result;// = new Result();
			//check if token is nullptr
			if (input == nullptr)
			{
				result.Score = score;
				result.Extensions = extensions;
				/*result = new Result()
				{
					Score = score,
					Extensions = extensions
				};*/
				return result;
			}

			//variables
			// Extensions extensions = new  Extensions();
			//Score score = new Score();

			//get score
			score = SetupScore(input["score"]);
			//get extensions
			extensions = SetupExtensions(input["extensions"]);


			//TimeSpan formattedDuration = XmlConvert.ToTimeSpan(input["duration"].Value<string>());

			result.Response = TokenToStringValue(input["response"]);
			result.Duration = TokenToDuration(input["duration"]);  // SDKV-5, was dropped entirely
			result.Completion = TokenToBool(input["completion"]);  // SDKV-3 tolerant read
			result.Success = TokenToBool(input["success"]);  // SDKV-3 tolerant read
			result.Score = score;
			//result = new Result()
			//{
			//	Score = score,
			//	Success = (bool)input["success"],
			//	Completion = (bool)input["completion"],
			//	Response = (string)input["response"],
			//	//Duration = (TimeSpan?)input["duration"]??TimeSpan.Zero,
			//	//Duration = TimeSpan.TryParseExact(input["duration"],IsoDateTimeConverter),
			//	//Duration = formattedDuration,
			//	//Extensions = extensions
			//};
			return (result);
		}
		catch (const std::runtime_error& e)
		{
			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
			throw;//return nullptr;
		}
	};

	//#########################################################################################################################
	// 
	//#########################################################################################################################
	static std::optional<Score> SetupScore(json input)
	{
		try
		{
			Score score;
			//check if token is nullptr
			// C# returns null here (wire "score": null when an authored result
			// carries no score) -- the optional represents it directly.
			if (input == nullptr)
			{
				return std::nullopt;
			}

			

			score.Scaled = TokenToFloat(input["scaled"]);
			score.Raw = TokenToFloat(input["raw"]);
			score.Min = TokenToFloat(input["min"]);
			score.Max = TokenToFloat(input["max"]);
			//Score score //= new Score()
			//{
			//	Scaled = (float)input["scaled"] ? 0,
			//	Raw = (float)input["raw"] ?  0,
			//	Min = (float)input["min"] ?  0,
			//	Max = (float)input["max"] ? 0
			//};

			return (score);
		}
		catch (const std::runtime_error& e)
		{
			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
			throw;//return nullptr;
		}
	};
	//#########################################################################################################################
	// 
	//#########################################################################################################################
	static Extensions SetupExtensions(json input)
	{
		try
		{
			Extensions extensions;// = new  Extensions();

			//check if token is nullptr
			if (input == nullptr)
			{
				return extensions;
			}
			if ((long)input["id"] != 0) // This SHOULD ALWAYS be the case
			{
				//extensions = _xAPIContext.Extensions.Find((long)input["id"]);
			}
			else
			{

				// PARITY (harness): the C# side never assigns ExtensionMap here -- the
				// assignment is commented out in its SetupExtensions -- so mirror that
				// and leave the default (wire "extensionmap": null).
				//extensions.ExtensionMap = TokenToStringValue(input["extensionmap"]);

				//extensions// = new Extensions()
				//{
				//	ExtensionMap = (string)input["extensionmap"]
				//};
			}
			return (extensions);
		}
		catch (const std::runtime_error& e)
		{
			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
			throw;//return nullptr;
		}
	};


	//#endregion

	//#region Context
		//#########################################################################################################################
		// Context setup
		//#########################################################################################################################
	static Context SetupContext(json input)
	{
		try
		{
			Actor instructor = Actor();// = new Actor();
			vector<Actor> group;// = new vector<Actor>();
			ContextActivities contextActivities;// = new ContextActivities();
			// Engaged default: the C# null branch assigns new StatementReference()
			// (wire object); the present branch may replace it with nullopt.
			std::optional<StatementReference> statementReference = StatementReference();// = new StatementReference();
			Extensions extensions;// = new Extensions();
			Context context;// = new Context();
			//check if token is nullptr
			if (input == nullptr)
			{
				context.Group = group;
				context.ContextActivities = contextActivities;
				context.StatementReference = statementReference;
				context.Extensions = extensions;
				context.Instructor = instructor;
				/*context = new Context()
				{
					Group = group,
					ContextActivities = contextActivities,
					StatementReference = statementReference,
					Extensions = extensions,
					Instructor = instructor
				};*/
				return context;
			}

			//get instructor
			instructor = SetupActor(input["instructor"]);
			//get group
			group = SetupActorGroup(input["group"]);
			//get context Activites
			contextActivities = SetupContextActivities(input["contextactivities"]);
			//get statement reference
			statementReference = SetupStatementReference(input["statementreference"]);
			//get extensions
			extensions = SetupExtensions(input["extensions"]);

			context.Registration = TokenToGuid(input["registration"]);
			context.Instructor = instructor;
			context.Group = group;
			context.ContextActivities = contextActivities;
			context.Revision = TokenToStringValue(input["revision"]);
			context.Platform = TokenToStringValue(input["platform"]);
			context.Language = TokenToStringValue(input["language"]);
			context.StatementReference = statementReference;
			context.Extensions = extensions;
			/*context = new Context()
			{
				Registration = TokenToGuid(input["registration"]),
				Instructor = instructor,
				Group = group,
				ContextActivities = contextActivities,
				Revision = (string)input["revision"],
				Platform = (string)input["platform"],
				Language = (string)input["language"],
				StatementReference = statementReference,
				Extensions = extensions
			};*/
			return (context);
		}
		catch (const std::runtime_error& e)
		{
			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
			throw;//return nullptr;
		}
	};


	//#########################################################################################################################
	//get group      
	//#########################################################################################################################
	static vector<Actor> SetupActorGroup(json input)
	{
		try
		{
			vector<Actor> actorList;// = new vector<Actor>();
			//check if token is nullptr
			if (input == nullptr)
			{
				return actorList;
			}

			//vector<Actor> actorList = new vector<Actor>();
			for (auto item : input)//may need to add ["actor"]
			{
				Actor actor = SetupActor(item);
				//actorList.Append(actor);
				actorList.push_back(actor);
			}

			return (actorList);
		}
		catch (const std::runtime_error& e)
		{
			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
			throw;//return nullptr;
		}
	};

	//#########################################################################################################################
	//get context Activites        
	//#########################################################################################################################
	static ContextActivities SetupContextActivities(json input)
	{
		try
		{
			ContextActivities contextActivities;// = new ContextActivities();
			//check if token is nullptr
			if (input == nullptr)
			{
				return contextActivities;
			}
			contextActivities.Parent = TokenToStringValue(input["parent"]);
			contextActivities.Grouping = TokenToStringValue(input["grouping"]);
			contextActivities.Category = TokenToStringValue(input["category"]);
			contextActivities.Other = TokenToStringValue(input["other"]);
			/*contextActivities = new ContextActivities()
			{
				Parent = (string)input["parent"],
				Grouping = (string)input["grouping"],
				Category = (string)input["category"],
				Other = (string)input["other"],
			};*/

			return (contextActivities);
		}
		catch (const std::runtime_error& e)
		{
			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
			throw;//return nullptr;
		}
	};
	//#########################################################################################################################        
	//get statement reference        
	//#########################################################################################################################
	static std::optional<StatementReference> SetupStatementReference(json input)
	{
		try
		{
			StatementReference statementReference;// = new StatementReference();
			//check if token is nullptr
			// C# returns null here (wire "statementreference": null) -- the
			// optional represents it directly. (The bare `throw;` this branch
			// once held was a std::terminate landmine.)
			if (input == nullptr)
			{
				return std::nullopt;
			}

			statementReference.ObjectType = TokenToStringValue(input["objecttype"]);
			statementReference.Id = TokenToGuid(input["id"]);

			/*statementReference = new StatementReference()
			{
				ObjectType = (string)input["objecttype"],
				Id = TokenToGuid(input["id"])
			};*/
			return (statementReference);
		}
		catch (const std::runtime_error& e)
		{
			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
			throw;//return nullptr;
		}
	};
	//#########################################################################################################################        
	//get extensions
	//#########################################################################################################################

	//#endregion

		//There has to be more to do here
		//#region Authority
		//#########################################################################################################################
		// This is suppose to interact with OAuth but an actor seems to be all it needs. 
		//#########################################################################################################################
	static Authority SetupAuthority(json input)
	{
		try
		{
			Actor actor;// = new Actor();
			Authority authority;// = new Authority();
			//check if token is nullptr
			if (input == nullptr)
			{
				authority.Actor = actor;
				/*authority = new Authority()
				{
					Actor = actor
				};*/

				return authority;
			}
			actor = SetupActor(input);

			authority.Actor = actor;
			/*authority = new Authority()
			{
				Actor = actor
			};*/
			return (authority);
		}
		catch (const std::runtime_error& e)
		{
			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
			throw;//return nullptr;
		}
	};
	//#endregion

	//#region Version
		//#########################################################################################################################
		// Should be able to pull the version directly from db. They should not be able to really set the version
		//#########################################################################################################################
	static std::optional<Version> SetupVersion(json input)
	{
		try
		{
			//check if token is nullptr
			// C# returns null here (wire "version": null) -- the optional
			// represents it directly. (The bare `throw;` this branch once held
			// was a std::terminate landmine.)
			if (input == nullptr)
			{
				return std::nullopt;
			}

			//Check if the input exists
			// if not set to latest version avalable
			Version version;// = new Models.xAPI.Version();
			//if (_xAPIContext.Version.Any(v => v.VersionNumber == (string)input["version"]))
			//{
			//    version = _xAPIContext.Version.Where(v => v.VersionNumber == (string)input["version"]).FirstOrDefault();
			//}
			//else
			//{
			//    version = _xAPIContext.Version.LastOrDefault();
			//}

			return (version);
		}
		catch (const std::runtime_error& e)
		{
			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
			throw;//return nullptr;
		}
	};
	//#endregion

	//#region Attachments
		//#########################################################################################################################
		// This one may be a little tricky - this is also where video should be input I think
		//#########################################################################################################################
	static vector<Attachment> SetupAttachments(json input)
	{
		try
		{
			//vector<Attachment> attachments = new vector<Models.xAPI.Attachment>();
			vector<Attachment> attachments;

			//check if token is nullptr
			if (input == nullptr)
			{
				return attachments;
			}

			//create list of attachments then go through and deserialize each one independently in a foreach and add it to the array. 
			// PARITY FIX (harness): `input` already IS the attachments token -- the
			// caller passes input["attachments"]. Indexing it again with a string
			// key throws on an array json, so no authored attachment ever parsed.
			// The C# side fixed the same bug (its historical ["attachments"] is
			// commented out in the foreach).
			for (auto item : input)
			{
				Attachment attachment = Attachment();// = new Models.xAPI.Attachment();

				attachment.UsageType = TokenToStringValue(item["usagetype"]);
				attachment.Display = TokenToLanguageMap(item["display"]);  // SDKV-13
				attachment.Description = TokenToLanguageMap(item["description"]);  // SDKV-13
				attachment.ContentType = TokenToStringValue(item["contenttype"]);
				attachment.Length = (item.contains("length") && !item["length"].is_null()) ? (int)item["length"] : 0;
				attachment.Sha2 = TokenToStringValue(item["sha2"]);
				attachment.FileURL = TokenToStringValue(item["fileurl"]);

				attachments.push_back(attachment);
			}

			return (attachments);
		}
		catch (const std::runtime_error& e)
		{
			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
			throw;//return nullptr;
		}
	};

	//#endregion

	//#region timestamp
	static chrono::system_clock::time_point SetUpTimeStamp(json input)
	{
		// PARITY (harness): mirror the C# side -- parse the authored timestamp,
		// falling back to UtcNow only when it is missing. The port previously
		// ignored the input and always answered "now", which both drifted from
		// the C# semantics and made byte-parity runs nondeterministic. The SDK
		// authors whole-second unzoned ISO 8601 ("2026-08-29T10:30:00"), which
		// is the shape parsed here; anything else falls through to UtcNow.
		if (input.is_string())
		{
			string raw = (string)input;
			int y = 0, mo = 0, d = 0, h = 0, mi = 0, s = 0;
			if (sscanf_s(raw.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d", &y, &mo, &d, &h, &mi, &s) == 6)
			{
				std::tm tm{};
				tm.tm_year = y - 1900;
				tm.tm_mon = mo - 1;
				tm.tm_mday = d;
				tm.tm_hour = h;
				tm.tm_min = mi;
				tm.tm_sec = s;
				tm.tm_isdst = 0;
				return chrono::system_clock::from_time_t(_mkgmtime(&tm));
			}
		}
		// second-truncated UtcNow, the C# fallback shape
		return chrono::system_clock::from_time_t(
			std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
	};

	//static chrono::system_clock::time_point SetUpTimeStamp(const json& input) {
	//	// Check that the input object is an object and contains the expected fields
	//	if (!input.is_object() ||
	//		!input.contains("year") ||
	//		!input.contains("month") ||
	//		!input.contains("day") ||
	//		!input.contains("hour") ||
	//		!input.contains("minute") ||
	//		!input.contains("second")) {
	//		throw std::invalid_argument("Invalid input object");
	//	}

	//	// Extract the year/month/day/hour/minute/second values from the input object
	//	int year = input["year"].get<int>();
	//	int month = input["month"].get<int>();
	//	int day = input["day"].get<int>();
	//	int hour = input["hour"].get<int>();
	//	int minute = input["minute"].get<int>();
	//	int second = input["second"].get<int>();

	//	// Create a tm structure representing the input time in the UTC timezone
	//	std::tm tm_utc{};
	//	tm_utc.tm_year = year - 1900;
	//	tm_utc.tm_mon = month - 1;
	//	tm_utc.tm_mday = day;
	//	tm_utc.tm_hour = hour;
	//	tm_utc.tm_min = minute;
	//	tm_utc.tm_sec = second;
	//	tm_utc.tm_isdst = 0; // assume no daylight saving time
	//	std::time_t time_utc = std::timegm(&tm_utc);
	//	//std::time_t time_utc = std::timegm(&tm_utc);

	//	// Convert the UTC time to a system_clock::time_point in the local timezone
	//	chrono::system_clock::time_point output = chrono::system_clock::from_time_t(time_utc);

	//	return output;
	//}


	/*static time_t SetUpTimeStamp(json input)
	{
		time_t output;

		try
		{
			if (input != nullptr)
			{
				output = (time_t)input;
			}
			else
			{
				output = time_t.UtcNow;
			}
		}
		catch
		{
			output = time_t.UtcNow;
		}
		return output;
	};*/

	//#endregion

	//#region Miss
	// ==== SDKV parse helpers (ported 2026-08-29, mirror the C# implementations) ====

	/// GUID2 from a json token: accepts a uuid string; zero-guid on null/bad input,
	/// matching the C# (Guid)cast-with-outer-try behaviour.
	/// PARITY (harness 2026-08-29): Newtonsoft's null-tolerant casts. The C# side
	/// reads optional leaves as (string)/(Uri)/(float?)??0f/(bool?)??false, which
	/// yield null/default for a missing or null token; nlohmann's raw casts throw
	/// instead. Empty string plays the C# null role (XApiJson emits it as null).
	/// Where C# THROWS on a missing token -- the (long) and (Guid) casts -- the raw
	/// casts below are kept on purpose.
	static string TokenToStringValue(json input)
	{
		if (input.is_null()) { return ""; }
		if (input.is_string()) { return (string)input; }
		return input.dump();  // number/bool token: C#'s (string) cast converts too
	};

	static float TokenToFloat(json input)
	{
		if (input.is_number()) { return (float)input; }
		if (input.is_string()) { try { return std::stof((string)input); } catch (...) { return 0.0f; } }
		return 0.0f;  // C#: (float?)token ?? 0f
	};

	static bool TokenToBool(json input)
	{
		if (input.is_boolean()) { return (bool)input; }
		if (input.is_string())
		{
			string raw = (string)input;
			return raw == "true" || raw == "True" || raw == "TRUE";
		}
		return false;  // C#: (bool?)token ?? false (SDKV-3)
	};

	static GUID2 TokenToGuid(json input)
	{
		try
		{
			if (input.is_null()) { return GUID2{}; }
			return GuidGen((string)input);
		}
		catch (const std::exception&) { return GUID2{}; }
	};

	/// FIX (SDKV-2): wraps a plain string as {"en": value}; serialized JSON-object
	/// text is parsed into the full map instead. Empty map plays the C# null role.
	static map<string, string> StringToLanguageMap(string input)
	{
		map<string, string> out;
		if (input.empty()) { return out; }
		string trimmed = input;
		trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
		if (!trimmed.empty() && trimmed[0] == '{')
		{
			try
			{
				json parsed = json::parse(trimmed);
				if (parsed.is_object())
				{
					for (auto& kv : parsed.items())
					{
						if (kv.value().is_string()) { out[kv.key()] = kv.value(); }
					}
					return out;
				}
			}
			catch (const std::exception&) { /* fall through to the plain wrap */ }
		}
		out["en"] = input;
		return out;
	};

	/// FIX (SDKV-2/SDKV-13): language map from a token -- a JSON object directly,
	/// serialized object text, or a plain string wrapped as {"en": value}.
	static map<string, string> TokenToLanguageMap(json input)
	{
		map<string, string> out;
		try
		{
			if (input.is_null()) { return out; }
			if (input.is_object())
			{
				for (auto& kv : input.items())
				{
					if (kv.value().is_string()) { out[kv.key()] = kv.value(); }
				}
				return out;
			}
			return StringToLanguageMap((string)input);
		}
		catch (const std::exception&) { return out; }
	};

	/// FIX (SDKV-1): the array-of-strings shape for correctResponsesPattern --
	/// a JSON array directly, serialized array text, or a lone string wrapped.
	static vector<string> TokenToStringList(json input)
	{
		vector<string> out;
		try
		{
			if (input.is_null()) { return out; }
			if (input.is_array())
			{
				for (auto& item : input)
				{
					if (item.is_null()) { continue; }
					out.push_back(item.is_string() ? (string)item : item.dump());
				}
				return out;
			}
			string raw = input.is_string() ? (string)input : input.dump();
			string trimmed = raw;
			trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
			if (!trimmed.empty() && trimmed[0] == '[')
			{
				try { return TokenToStringList(json::parse(trimmed)); }
				catch (const std::exception&) { /* not array text, wrap below */ }
			}
			out.push_back(raw);
			return out;
		}
		catch (const std::exception&) { return out; }
	};

	/// FIX (SDKV-5): milliseconds from an authored duration -- ISO 8601
	/// ("PT1M30.5S") or the legacy clock format ("00:01:30.5"); 0 otherwise.
	/// Mirrors the C# Iso8601TimeSpanConverter.ReadJson tolerance.
	static long TokenToDuration(json input)
	{
		try
		{
			if (input.is_null()) { return 0; }
			string raw = input.is_string() ? (string)input : input.dump();
			if (raw.empty()) { return 0; }
			double ms = 0;
			size_t i = 0;
			bool neg = false;
			if (raw[i] == '-') { neg = true; i++; }
			if (i < raw.size() && (raw[i] == 'P' || raw[i] == 'p'))
			{
				i++;
				bool inTime = false;
				while (i < raw.size())
				{
					if (raw[i] == 'T' || raw[i] == 't') { inTime = true; i++; continue; }
					size_t start = i;
					while (i < raw.size() && (isdigit((unsigned char)raw[i]) || raw[i] == '.')) { i++; }
					if (i >= raw.size() || start == i) { break; }
					double v = atof(raw.substr(start, i - start).c_str());
					char unit = (char)toupper((unsigned char)raw[i]); i++;
					if (unit == 'D') { ms += v * 86400000.0; }
					else if (unit == 'H') { ms += v * 3600000.0; }
					else if (unit == 'M') { ms += inTime ? v * 60000.0 : v * 2592000000.0; }
					else if (unit == 'S') { ms += v * 1000.0; }
					else if (unit == 'Y') { ms += v * 31536000000.0; }
					else if (unit == 'W') { ms += v * 604800000.0; }
				}
			}
			else
			{
				// legacy clock format [d.]hh:mm:ss[.fff]
				double parts[4] = { 0, 0, 0, 0 };
				int count = 0;
				size_t start = i;
				for (size_t k = i; k <= raw.size() && count < 4; k++)
				{
					if (k == raw.size() || raw[k] == ':' || raw[k] == '.')
					{
						if (k > start) { parts[count] = atof(raw.substr(start, k - start).c_str()); }
						count++;
						start = k + 1;
						if (k < raw.size() && raw[k] == '.' && count == 3)
						{
							parts[3] = atof(("0." + raw.substr(k + 1)).c_str()) * 1000.0;
							break;
						}
					}
				}
				if (count >= 3) { ms = parts[0] * 3600000.0 + parts[1] * 60000.0 + parts[2] * 1000.0 + parts[3]; }
			}
			return (long)(neg ? -ms : ms);
		}
		catch (const std::exception&) { return 0; }
	};

	static string TokenToString(json input)
	{
		string output;// = "";
		try
		{
			// PARITY (harness): C# is input.ToString(Formatting.None), i.e. the JSON
			// representation -- a string token keeps its quotes, an array/object
			// serializes compactly. dump() is the nlohmann equivalent. A missing
			// token answers empty (C# catches the NullReferenceException -> null).
			output = input.is_null() ? "" : String_Helpers::Replace(input.dump(), "\r\n", "");
		}
		catch (const std::exception& e)
		{
			//_log.Error("ArrayToString error: "+ex.Message);
			output = "";
		}

		return output;
	};
	//#endregion
};

