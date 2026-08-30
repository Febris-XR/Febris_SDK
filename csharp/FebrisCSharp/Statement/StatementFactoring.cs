// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Febris.CsharpSimulationLibraryNetStandard.Models.xAPI;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
//using Serilog;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using System.Threading.Tasks;
// System.Xml deliberately not imported wholesale -- its Formatting type
// collides with Newtonsoft.Json.Formatting; XmlConvert is fully qualified.

namespace Febris.CsharpSimulationLibraryNetStandard.Statement
{
    internal class StatementFactoring
    {
        //private static readonly ILogger //_log = Log.Logger;

        //catch (Exception ex)
        //        {
        //            //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name+" Error: " + ex.Message);
        //
        //        }

        //#########################################################################################################################
        //May need to convert specific items in string to other strings - ie model names or uuid keys
        //#########################################################################################################################
        internal Models.xAPI.Statement FactorStatement(JObject input)
        {
            try
            {
                //Statement Variables     
                DateTime timeStamp;
                Actor actor = new Actor();
                Models.xAPI.Object xAPIObject = new Models.xAPI.Object();
                Verb verb = new Verb();
                Result result = new Result();
                Context context = new Context();
                Authority authority = new Authority();
                Models.xAPI.Version version = new Models.xAPI.Version();
                List<Models.xAPI.Attachment> attachments = new List<Models.xAPI.Attachment>();
                JToken statementToken = input;
                //***************************************************needs to go to lowercase
                //JObject converToLower = Object.from;
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

                Models.xAPI.Statement statement = new Models.xAPI.Statement()
                {
                    Timestamp = timeStamp,
                    Actor = actor,
                    Object = xAPIObject,
                    Verb = verb,
                    Result = result,
                    Context = context,
                    Authority = authority,
                    Version = version,
                    Attachments = attachments
                };

                return statement;
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
                return null;
            }
        }

        #region Actor
        //#########################################################################################################################
        //May need to convert specific items in string to other strings - ie model names or uuid keys
        //#########################################################################################################################
        private Actor SetupActor(JToken input)
        {
            try
            {
                //variables
                Actor actor = new Actor();
                Member member = new Member();
                Account account = new Account();
                //check if token is null
                if (input == null)
                {
                    actor = new Actor()
                    {
                        Member = member,
                        Account = account
                    };
                    return actor;
                }

                //get member info
                member = SetupMember(input["member"]);
                //get account info
                account = SetupAccount(input["account"]);

                actor = new Actor()
                {
                    Id = (long)input["id"],
                    UUID = (Guid)input["uuid"],
                    ObjectType = (string)input["objecttype"],
                    Name = (string)input["name"],
                    Mbox = (Uri)input["mbox"],
                    Mbox_sha1sum = (string)input["mbox_sha1sum"],
                    OpenId = (Uri)input["openid"],
                    Account = account,
                    Member = member
                };

                return (actor);
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
                return null;
            }
        }

            //#########################################################################################################################
            //get Member data    
            //#########################################################################################################################
            private Member SetupMember(JToken input)
        {
            try
            {
                Member member = new Member();
                List<Actor> actorList = new List<Actor>();
                //check if token is null
                if (input == null)
                {
                    member = new Member()
                    {
                        Actors = actorList
                    };

                    return member;
                }



                foreach (var item in input)//may need to add ["actor"]
                {
                    Actor actor = SetupActor(item);
                    //actorList.Append(actor);
                    actorList.Add(actor);
                }
                member = new Member()
                {
                    Actors = actorList
                };

                return (member);
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
                return null;
            }
        }

        //#########################################################################################################################
        //get Account data    
        //#########################################################################################################################
        private Account SetupAccount(JToken input)
        {
            try
            {
                Account account = new Account();
                //check if token is null
                if (input == null)
                {
                    return account;
                }

                account = new Account()
                {
                    HomePage = (Uri)input["homepage"],
                    Name = (string)input["name"],
                };

                return (account);
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
                return null;
            }
        }


        #endregion

        #region Object
        //#########################################################################################################################
        //May need to convert specific items in string to other strings - ie model names or uuid keys
        //#########################################################################################################################
        private Models.xAPI.Object SetupObject(JToken input)
        {
            try
            {
                //check if token is null
                if (input == null)
                {
                    return null;
                }
                //get definition
                Definition definition = new Definition();
                Models.xAPI.Object xAPIObject = new Models.xAPI.Object();

                definition = SetupObjectDefinition(input["definition"]);

                xAPIObject = new Models.xAPI.Object()
                {
                    Key = (long)input["key"],
                    UUID = (Guid)input["uuid"],
                    Id = (Uri)input["id"],
                    ObjectType = (string)input["objecttype"],
                    Definition = definition
                };


                return (xAPIObject);
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
                return null;
            }
        }
        //#########################################################################################################################
        //Setup object definition
        //#########################################################################################################################
        /// <summary>
        /// Builds the Activity Definition from the (lowercased) authored
        /// input. FIX (SDKV-1): <c>correctResponsesPattern</c> is parsed
        /// into a <see cref="List{T}"/> of strings (array input is taken
        /// as-is; a lone string is wrapped as a single-element array) so
        /// the wire carries a JSON array per xAPI 1.0.3 section 4.1.4.1.
        /// FIX (SDKV-13): <c>name</c>/<c>description</c> are parsed into
        /// Language Map dictionaries instead of being stringified.
        /// </summary>
        private Definition SetupObjectDefinition(JToken input)
        {
            try
            {
                Definition definition = new Definition();
                Models.xAPI.Extensions extensions = new Models.xAPI.Extensions();
                //check if token is null
                if (input == null)
                {
                    definition = new Definition()
                    {
                        Extensions = extensions
                    };
                    return definition;
                }

                //get name
                Dictionary<string, string> name = TokenToLanguageMap(input["name"]);
                Dictionary<string, string> description = TokenToLanguageMap(input["description"]);
                List<string> correctResponsesPattern = TokenToStringList(input["correctresponsespattern"]);
                string interactionComponents = TokenToString(input["interactioncomponents"]);
                #region [Historical - SDKV-1/SDKV-13] stringified reads
                //string name = TokenToString(input["name"]);
                //string description = TokenToString(input["description"]);
                //string correctResponsesPattern = TokenToString(input["correctresponsespattern"]);
                #endregion

                extensions = SetupObjectExtensions(input["extensions"]);

                definition = new Definition()
                {
                    //Name = (string)input["name"].ToString(),
                    //Description = (string)input["description"].ToString(),
                    Name = name,
                    Description = description,
                    Type = (Uri)input["type"],
                    MoreInfo = (Uri)input["moreinfo"],
                    Extensions = extensions,
                    InteractionType = (string)input["interactiontype"],
                    CorrectResponsesPattern = correctResponsesPattern,
                    //CorrectResponsesPattern = (string)input["correctresponsespattern"],
                    //InteractionComponents = (string)input["interactioncomponents"]
                    InteractionComponents = interactionComponents
                };


                return (definition);
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
                return null;
            }
        }
        //#########################################################################################################################
        //Setup object extensions
        //#########################################################################################################################
        private Models.xAPI.Extensions SetupObjectExtensions(JToken input)
        {
            try
            {
                Models.xAPI.Extensions extension = new Models.xAPI.Extensions();
                //check if token is null
                if (input == null)
                {
                    return extension;
                }

                //Models.xAPI.Extensions extensionList = new List<Models.xAPI.Extensions>();
                //foreach (var item in input)
                //{
                extension = new Models.xAPI.Extensions()
                {
                    Id = (long)input["id"],
                    UUID = (Guid)input["uuid"],
                    ExtensionMap = (string)input["extensionmap"]
                };

                //    extensionList.Add(extension);

                //}

                return (extension);
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
                return null;
            }
        }
        #endregion

        #region Verb
        //#########################################################################################################################
        //May need to convert specific items in string to other strings - ie model names or uuid keys
        //#########################################################################################################################
        /// <summary>
        /// Builds the Verb from the (lowercased) authored input.
        /// FIX (SDKV-13): <c>display</c> is parsed into a Language Map
        /// dictionary so the wire carries a JSON object per xAPI 1.0.3
        /// section 4.1.3 (previously it was stringified and relied on the
        /// Initializer's ChangeStringToObject hack to re-objectify it).
        /// </summary>
        private Verb SetupVerb(JToken input)
        {
            try
            {
                //check if token is null
                if (input == null)
                {
                    return null;
                }
                Verb verb = new Verb();
                bool verbFound = false;

                verb = new Verb()
                {
                    Key = (long)input["key"],
                    UUID = (Guid)input["uuid"],
                    Id = (Uri)input["id"],
                    Display = TokenToLanguageMap(input["display"])
                    //Display = (string)input["display"].ToString() // [Historical - SDKV-13] stringified language map
                };

                return (verb);
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
                return null;
            }
        }
        #endregion

        #region Result
        //#########################################################################################################################
        // 
        //#########################################################################################################################
        private Result SetupResult(JToken input)
        {
            try
            {
                //variables
                Models.xAPI.Extensions extensions = new Models.xAPI.Extensions();
                Score score = new Score();
                Result result = new Result();
                //check if token is null
                if (input == null)
                {
                    result = new Result()
                    {
                        Score = score,
                        Extensions = extensions
                    };
                    return result;
                }

                //variables
                //Models.xAPI.Extensions extensions = new Models.xAPI.Extensions();
                //Score score = new Score();

                //get score
                score = SetupScore(input["score"]);
                //get extensions
                extensions = SetupExtensions(input["extensions"]);


                //TimeSpan formattedDuration = XmlConvert.ToTimeSpan(input["duration"].Value<string>());

                result = new Result()
                {
                    Score = score,
                    // FIX (SDKV-3): tolerant nullable-bool reads -- a missing
                    // success/completion no longer throws the whole Result
                    // away; the typed bool members then serialize as genuine
                    // JSON Booleans.
                    Success = (bool?)input["success"] ?? false,
                    Completion = (bool?)input["completion"] ?? false,
                    Response = (string)input["response"],
                    // FIX (SDKV-5): parse an authored duration (ISO 8601 or
                    // legacy .NET format) so it round-trips; the model's
                    // Iso8601TimeSpanConverter re-emits it as ISO 8601.
                    Duration = TokenToDuration(input["duration"]),
                    //Duration = (TimeSpan?)input["duration"]??TimeSpan.Zero,
                    //Duration = TimeSpan.TryParseExact(input["duration"],IsoDateTimeConverter),
                    //Duration = formattedDuration,
                    //Extensions = extensions
                };
                return (result);
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
                return null;
            }
        }

        //#########################################################################################################################
        // 
        //#########################################################################################################################
        private Score SetupScore(JToken input)
        {
            try
            {
                //check if token is null
                if (input == null)
                {
                    return null;
                }

                Score score = new Score()
                {
                    Scaled = (float?)input["scaled"] ?? 0f,
                    Raw = (float?)input["raw"] ?? 0f,
                    Min = (float?)input["min"] ?? 0f,
                    Max = (float?)input["max"] ?? 0f
                };

                return (score);
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
                return null;
            }
        }
        //#########################################################################################################################
        // 
        //#########################################################################################################################
        private Models.xAPI.Extensions SetupExtensions(JToken input)
        {
            try
            {
                Models.xAPI.Extensions extensions = new Models.xAPI.Extensions();

                //check if token is null
                if (input == null)
                {
                    return extensions;
                }
                if ((long)input["id"] != 0) // This SHOULD ALWAYS be the case
                {
                    //extensions = _xAPIContext.Extensions.Find((long)input["id"]);
                }
                else
                {

                    extensions = new Models.xAPI.Extensions()
                    {
                        //ExtensionMap = (string)input["extensionmap"]
                    };
                }
                return (extensions);
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
                return null;
            }
        }


        #endregion

        #region Context
        //#########################################################################################################################
        // Context setup
        //#########################################################################################################################
        private Context SetupContext(JToken input)
        {
            try
            {
                Actor instructor = new Actor();
                List<Actor> group = new List<Actor>();
                ContextActivities contextActivities = new ContextActivities();
                StatementReference statementReference = new StatementReference();
                Models.xAPI.Extensions extensions = new Models.xAPI.Extensions();
                Context context = new Context();
                //check if token is null
                if (input == null)
                {
                    context = new Context()
                    {
                        Group = group,
                        ContextActivities = contextActivities,
                        StatementReference = statementReference,
                        Extensions = extensions,
                        Instructor = instructor
                    };
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

                context = new Context()
                {
                    Registration = (Guid)input["registration"],
                    Instructor = instructor,
                    Group = group,
                    ContextActivities = contextActivities,
                    Revision = (string)input["revision"],
                    Platform = (string)input["platform"],
                    Language = (string)input["language"],
                    StatementReference = statementReference,
                    Extensions = extensions
                };
                return (context);
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
                return null;
            }
        }


        //#########################################################################################################################
        //get group      
        //#########################################################################################################################
        private List<Actor> SetupActorGroup(JToken input)
        {
            try
            {
                List<Actor> actorList = new List<Actor>();
                //check if token is null
                if (input == null)
                {
                    return actorList;
                }

                //List<Actor> actorList = new List<Actor>();
                foreach (var item in input)//may need to add ["actor"]
                {
                    Actor actor = SetupActor(item);
                    //actorList.Append(actor);
                    actorList.Add(actor);
                }

                return (actorList);
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
                return null;
            }
        }

        //#########################################################################################################################
        //get context Activites        
        //#########################################################################################################################
        private ContextActivities SetupContextActivities(JToken input)
        {
            try
            {
                ContextActivities contextActivities = new ContextActivities();
                //check if token is null
                if (input == null)
                {
                    return contextActivities;
                }

                contextActivities = new ContextActivities()
                {
                    Parent = (string)input["parent"],
                    Grouping = (string)input["grouping"],
                    Category = (string)input["category"],
                    Other = (string)input["other"],
                };

                return (contextActivities);
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
                return null;
            }
        }
        //#########################################################################################################################        
        //get statement reference        
        //#########################################################################################################################
        private StatementReference SetupStatementReference(JToken input)
        {
            try
            {
                StatementReference statementReference = new StatementReference();
                //check if token is null
                if (input == null)
                {
                    return null;
                }

                statementReference = new StatementReference()
                {
                    ObjectType = (string)input["objecttype"],
                    Id = (Guid)input["id"]
                };
                return (statementReference);
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
                return null;
            }
        }
        //#########################################################################################################################        
        //get extensions
        //#########################################################################################################################

        #endregion

        //There has to be more to do here
        #region Authority
        //#########################################################################################################################
        // This is suppose to interact with OAuth but an actor seems to be all it needs. 
        //#########################################################################################################################
        private Authority SetupAuthority(JToken input)
        {
            try
            {
                Actor actor = new Actor();
                Authority authority = new Authority();
                //check if token is null
                if (input == null)
                {
                    authority = new Authority()
                    {
                        Actor = actor
                    };

                    return authority;
                }
                actor = SetupActor(input);

                authority = new Authority()
                {
                    Actor = actor
                };
                return (authority);
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
                return null;
            }
        }
        #endregion

        #region Version
        //#########################################################################################################################
        // Should be able to pull the version directly from db. They should not be able to really set the version
        //#########################################################################################################################
        private Models.xAPI.Version SetupVersion(JToken input)
        {
            try
            {
                //check if token is null
                if (input == null)
                {
                    return null;
                }

                //Check if the input exists
                // if not set to latest version avalable
                Models.xAPI.Version version = new Models.xAPI.Version();
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
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
                return null;
            }
        }
        #endregion

        #region Attachments
        //#########################################################################################################################
        // This one may be a little tricky - this is also where video should be input I think
        //#########################################################################################################################
        /// <summary>
        /// Builds the attachment list from the (lowercased) authored input.
        /// FIX (SDKV-2): <c>display</c>/<c>description</c> are parsed into
        /// Language Map dictionaries so the wire carries JSON objects per
        /// xAPI 1.0.3 section 4.1.11 -- previously both were stringified, which the
        /// node's typed /Submit binder rejected wholesale. String reads are
        /// also null-tolerant now (a single attachment missing an optional
        /// field no longer nulls out the whole list).
        /// </summary>
        private List<Models.xAPI.Attachment> SetupAttachments(JToken input)
        {
            try
            {
                List<Models.xAPI.Attachment> attachments = new List<Models.xAPI.Attachment>();

                //check if token is null
                if (input == null)
                {
                    return attachments;
                }

                //create list of attachments then go through and deserialize each one independently in a foreach and add it to the array.
                foreach (var item in input/*["attachments"]*/)
                {
                    Models.xAPI.Attachment attachment = new Models.xAPI.Attachment();

                    attachment.UsageType = (Uri)item["usagetype"];
                    attachment.Display = TokenToLanguageMap(item["display"]);
                    attachment.Description = TokenToLanguageMap(item["description"]);
                    #region [Historical - SDKV-2] stringified language maps
                    //attachment.Display = (string)item["display"].ToString().Replace("\r\n", string.Empty);
                    //attachment.Description = (string)item["description"].ToString().Replace("\r\n", string.Empty);
                    #endregion
                    attachment.ContentType = (string)item["contenttype"];
                    attachment.Length = (int?)item["length"] ?? 0;
                    attachment.Sha2 = (string)item["sha2"];
                    attachment.FileURL = (Uri)item["fileurl"];

                    attachments.Add(attachment);
                }

                return (attachments);
            }
            catch (Exception ex)
            {
                //_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);
                return null;
            }
        }

        #endregion

        #region timestamp
        private DateTime SetUpTimeStamp(JToken input)
        {
            DateTime output;
            
            try
            {
                if(input != null)
                {
                    output = (DateTime)input;
                }
                else
                {
                    output = DateTime.UtcNow;
                }                
            }
            catch
            {
                output = DateTime.UtcNow;
            }
            return output;
        }
        #endregion

        #region Miss
        private string TokenToString (JToken input)
        {
            string output = string.Empty;
            try
            {
                output = input.ToString(Formatting.None).Replace("\r\n", string.Empty);
            }
            catch(Exception ex)
            {
                //_log.Error("ArrayToString error: "+ex.Message);
                output = null;
            }

            return output;
        }

        /// <summary>
        /// FIX (SDKV-2/SDKV-13): converts an authored token into a Language
        /// Map dictionary (RFC 5646 tag -> string). Accepts a JSON object
        /// directly, a string containing serialized JSON-object text, or a
        /// plain string (wrapped as <c>{"en": value}</c>). Returns null for
        /// null/empty input so optional fields serialize as JSON null.
        /// </summary>
        internal static Dictionary<string, string> TokenToLanguageMap(JToken input)
        {
            try
            {
                if (input == null || input.Type == JTokenType.Null)
                {
                    return null;
                }
                if (input.Type == JTokenType.Object)
                {
                    return input.ToObject<Dictionary<string, string>>();
                }
                //string input: either serialized JSON-object text or a plain display string
                return StringToLanguageMap(input.ToString());
            }
            catch (Exception ex)
            {
                //_log.Error("TokenToLanguageMap error: " + ex.Message);
                return null;
            }
        }

        /// <summary>
        /// FIX (SDKV-2): wraps a plain string as a single-entry Language Map
        /// (<c>{"en": value}</c>); if the string itself is serialized
        /// JSON-object text it is parsed into the full map instead. Shared
        /// by the factoring above and <c>StatementHandler.AddAttachment</c>.
        /// </summary>
        internal static Dictionary<string, string> StringToLanguageMap(string input)
        {
            if (string.IsNullOrEmpty(input))
            {
                return null;
            }
            string trimmed = input.Trim();
            if (trimmed.StartsWith("{", StringComparison.Ordinal))
            {
                try
                {
                    return JsonConvert.DeserializeObject<Dictionary<string, string>>(trimmed);
                }
                catch (Exception ex)
                {
                    //fall through to the plain-string wrap below
                }
            }
            return new Dictionary<string, string>() { { "en", input } };
        }

        /// <summary>
        /// FIX (SDKV-1): converts an authored token into the array-of-strings
        /// shape xAPI 1.0.3 section 4.1.4.1 mandates for correctResponsesPattern.
        /// Accepts a JSON array directly, a string containing serialized
        /// JSON-array text, or a lone string (wrapped as a single-element
        /// array). Returns null for null input.
        /// </summary>
        internal static List<string> TokenToStringList(JToken input)
        {
            try
            {
                if (input == null || input.Type == JTokenType.Null)
                {
                    return null;
                }
                if (input.Type == JTokenType.Array)
                {
                    List<string> items = new List<string>();
                    foreach (JToken item in input)
                    {
                        if (item.Type == JTokenType.Undefined || item.Type == JTokenType.Null)
                        {
                            continue; // lenient-parser artifacts (e.g. "[,]") carry no data
                        }
                        items.Add(item.Type == JTokenType.String ? (string)item : item.ToString(Formatting.None));
                    }
                    return items;
                }
                string raw = input.ToString();
                string trimmed = raw.Trim();
                if (trimmed.StartsWith("[", StringComparison.Ordinal))
                {
                    try
                    {
                        JArray parsed = JArray.Parse(trimmed);
                        // Newtonsoft's parser is lenient: "[,]" parses to
                        // [undefined] instead of throwing. Undefined content
                        // means the text wasn't a real JSON array -- keep it
                        // as a lone entry instead.
                        if (!HasUndefinedToken(parsed))
                        {
                            return TokenToStringList(parsed);
                        }
                    }
                    catch (Exception ex)
                    {
                        //not valid JSON-array text -- wrap as a lone entry below
                    }
                }
                return new List<string>() { raw };
            }
            catch (Exception ex)
            {
                //_log.Error("TokenToStringList error: " + ex.Message);
                return null;
            }
        }

        /// <summary>
        /// Returns true when the token (or any nested child) is a JSON
        /// <c>undefined</c> -- Newtonsoft's lenient parser produces these for
        /// degenerate text like <c>[,]</c>, which therefore is NOT treated as
        /// a real JSON array by the helpers above.
        /// </summary>
        internal static bool HasUndefinedToken(JToken token)
        {
            if (token == null)
            {
                return false;
            }
            if (token.Type == JTokenType.Undefined)
            {
                return true;
            }
            foreach (JToken child in token.Children())
            {
                if (HasUndefinedToken(child))
                {
                    return true;
                }
            }
            return false;
        }

        /// <summary>
        /// FIX (SDKV-5): parses an authored duration tolerantly -- ISO 8601
        /// (<c>PT1M30S</c>) first, then the legacy .NET "c" format
        /// (<c>00:01:30</c>) -- falling back to <see cref="TimeSpan.Zero"/>.
        /// The model's Iso8601TimeSpanConverter re-emits whatever lands here
        /// as spec-valid ISO 8601.
        /// </summary>
        internal static TimeSpan TokenToDuration(JToken input)
        {
            try
            {
                if (input == null || input.Type == JTokenType.Null)
                {
                    return TimeSpan.Zero;
                }
                string raw = input.ToString();
                if (string.IsNullOrEmpty(raw))
                {
                    return TimeSpan.Zero;
                }
                try
                {
                    return System.Xml.XmlConvert.ToTimeSpan(raw);
                }
                catch (FormatException)
                {
                    TimeSpan fallback;
                    return TimeSpan.TryParse(raw, out fallback) ? fallback : TimeSpan.Zero;
                }
            }
            catch (Exception ex)
            {
                //_log.Error("TokenToDuration error: " + ex.Message);
                return TimeSpan.Zero;
            }
        }
        #endregion
    }
}
