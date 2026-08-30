// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Febris.CsharpSimulationLibraryNetStandard.Enums;
using Febris.CsharpSimulationLibraryNetStandard.Service;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Febris.CsharpSimulationLibraryNetStandard.Statement
{
    public class Initializer
    {


        /// <summary>
        /// This is the new default way of Initializing this library. It Now takes into account the Operating system and will route 
        /// files properly taking into account the changes made in Android 11+
        /// 
        /// This setup is specifically for android but if path is left as null or empty id defaults to internal document folders
        /// 
        /// This had to be update to work with intents and completely get rid of FileSystem
        /// </summary>
        /// <param name="input"></param>
        /// <param name="path"></param>
        /// <returns></returns>
        /// <summary>
        /// SIM-T13 G4 overload -- initialize with a host-provided
        /// <see cref="ISimulationLogger"/> so library failures route into the
        /// game's diagnostics surface instead of being swallowed by
        /// <see cref="Console.WriteLine"/>. Hosts that don't have a logger to
        /// supply can keep calling the original overload below; the library
        /// then uses <see cref="ConsoleSimulationLogger.Instance"/>.
        /// </summary>
        public static Task<(bool, string[,])> Initialize(string[] input, ExpectedOperatingSystem OS, ISimulationLogger logger)
        {
            StaticDetails.StaticDetails.Logger = logger;
            return Initialize(input, OS);
        }

        public static async Task<(bool, string[,])> Initialize(string[] input, ExpectedOperatingSystem OS)
        {
            //Console.WriteLine("Inside Initializer");
            bool isInitialized = false;
            string[,] outputArray = default;
            try
            {
                #region file system initalizer -- need this moved to platform specific areas

                //if (string.IsNullOrEmpty(path))
                //{
                //    FileSystem.FileSystemInitalizer.FileInitalizer();
                //}
                //else
                //{



                //    string _publicFileName = "com.febris.public_files";
                //    string _externalPath = path + "/" + _publicFileName;
                //    FileSystem.FileSystemInitalizer.FileInitalizer(_externalPath);
                //}



                #endregion

                #region command line args
                //get command line args
                string[] arguments = input;
                string statement = JSONHandler.ArgumentHandler(arguments);
                #endregion

                #region replace removed spaces from statement
                statement = statement.Replace("_-_", " ");
                #endregion

                #region Change to initialJobject
                JObject statementJObject = JSONHandler.ChangeToObject(statement);
                // INPUT normalization only (kept on purpose): authored statements
                // arrive with arbitrary key casing (the launcher/test harness
                // author PascalCase) and StatementFactoring reads all-lowercase
                // keys. Lowercasing the INPUT is a tolerant read. The matching
                // OUTPUT lowercasing below was the SDKV-12 bug and is retired.
                ChangePropertiesToLowerCase(statementJObject);
                #endregion

                #region Set up mapped logging

                #region Create log directory
                //string objectId = (string)statementJObject["object"]["uuid"];
                //InitializeLogging(objectId);
                #endregion
                Console.WriteLine("Finished creating log directory");

                #endregion

                #region convert to jobject following data model
                JObject statementFromDataModel = JSONHandler.CreateObjectFromDataModel(statementJObject);
                #region [Historical - SDKV-12] output lowercasing pass
                // Re-lowercasing the factored statement corrupted the
                // case-sensitive xAPI 1.0.3 keys (objectType, homePage,
                // moreInfo, interactionType, correctResponsesPattern,
                // usageType, contentType, fileUrl) on the wire -- a conformant
                // LRS rejects the Account IFI and loses objectType
                // discriminators. Superseded by explicit [JsonProperty] wire
                // names on the Models/xAPI classes, which now emit spec-exact
                // casing straight out of JObject.FromObject. The node reads
                // case-insensitively (parallel node change), so the dialect
                // keys' casing is declared there too.
                //ChangePropertiesToLowerCase(statementFromDataModel);
                #endregion
                #endregion


                #region Check needed parts of JObject                
                bool statmentIsProperlySetUp = StatementHandler.StatementCheck(statementFromDataModel);
                if (!statmentIsProperlySetUp)
                {
                    return (statmentIsProperlySetUp, outputArray);
                }
                #endregion

                #region set the new strings to jobjects
                // interactioncomponents is still stored as a string on the typed
                // model (its authored shape is free-form object/array text), so
                // it is the one remaining field that needs re-objectifying here.
                ChangeStringToObject(statementFromDataModel["object"]["definition"]["interactioncomponents"]);
                #region [Historical - SDKV-1/2/13] re-objectification of typed fields
                // These fields are now typed on the models (Verb.Display,
                // Definition.Name/Description as Dictionary<string,string>,
                // Definition.CorrectResponsesPattern as List<string>), so
                // JObject.FromObject already emits real objects/arrays and the
                // string-repair pass is superseded. Note the keys below are the
                // pre-SDKV-12 lowercase spellings. The attachment display/
                // description calls (further below) were already commented out
                // historically; the typed Attachment model supersedes them too.
                //ChangeStringToObject(statementFromDataModel["verb"]["display"]);
                //ChangeStringToObject(statementFromDataModel["object"]["definition"]["name"]);
                //ChangeStringToObject(statementFromDataModel["object"]["definition"]["description"]);
                //ChangeStringToObject(statementFromDataModel["object"]["definition"]["correctresponsespattern"]);
                #endregion



                //change attachment
                //ChangeStringToObject(statementFromDataModel["object"]["definition"]["correctresponsespattern"]);
                #endregion

                Console.WriteLine("Finished jobject factoring");

                switch (OS)
                {
                    case ExpectedOperatingSystem.WindowsPC:
                        {
                            StaticDetails.StaticDetails.Handler = new WinPCHandler();
                            //isInitialized = JSONHandler.CreateInitialDataFile(statementFromDataModel);
                            break;
                        }
                    case ExpectedOperatingSystem.Android:
                        {
                            StaticDetails.StaticDetails.Handler = new AndroidHandler();
                            //(isInitialized, outputArray) = await StaticDetails.StaticDetails.Handler.CreateInitialPost(statementFromDataModel);
                            break;
                        }
                    case ExpectedOperatingSystem.iOSvariant:
                        {
                            StaticDetails.StaticDetails.Handler = new iOSHandler();
                            //(isInitialized, outputArray) = await StaticDetails.StaticDetails.Handler.CreateInitialPost(statementFromDataModel);
                            break;
                        }
                    case ExpectedOperatingSystem.WinMobile:
                        {
                            StaticDetails.StaticDetails.Handler = new WinMobileHandler();

                            break;
                        }
                }

                (isInitialized, outputArray) = await StaticDetails.StaticDetails.Handler.CreateInitialPost(statementFromDataModel);

                Console.WriteLine("Finished writing data to file");

                switch (OS)
                {
                    //case ExpectedOperatingSystem.WindowsPC:
                    //    {


                    //        break;
                    //    }
                    case ExpectedOperatingSystem.Android:
                        {

                            break;
                        }
                    case ExpectedOperatingSystem.iOSvariant:
                        {

                            break;
                        }
                    case ExpectedOperatingSystem.WinMobile:
                        {

                            break;
                        }
                }


            }
            catch (Exception ex)
            {
                // FIX (SIM-B11): route initialization failure through the host logger instead of Console.WriteLine so it reaches the game diagnostics surface. See docs/MODERNIZATION/SIM_MODERNIZATION.md.
                //Console.WriteLine(ex.Message);
                StaticDetails.StaticDetails.Logger.Log(Service.SimulationLogLevel.Error, "Initializer: initialization failed.", ex);
                //Log.Logger.Fatal(ex.Message);
                //Log.Logger.Fatal(ex.Source);
                return (isInitialized, outputArray);
                // FIX (SIM-B11): removed unreachable throw after the return above (dead code). See docs/MODERNIZATION/SIM_MODERNIZATION.md.
                //throw;
            }
            return (isInitialized, outputArray);
        }

        //this will be used for alternative approach
        //private static FebrisLocalLibrary.Models.xAPI.Statement _statement;

        private void InitializeLogging(string idString)
        {
            bool isInitialized = false;
            try
            {

                #region set unmapped global logger
                //Log.Logger = CreateLogger(string.Empty);
                #endregion

                #region Create log directory                
                FileSystem.FileSystemInitalizer.CreateFileDirectory(FileSystem.FileSystem.SimulationLogBasePath, idString);
                #endregion

            }
            catch (Exception ex)
            {
                Console.WriteLine("Error creating log file: " + ex.Message);
                //Log.Logger.Fatal(ex.Message);
                //Log.Logger.Fatal(ex.Source);
                throw;
            }
        }

        #region Helpers
        #region break strings into objects
        /// <summary>
        /// Replaces a string-typed token whose text is serialized JSON with
        /// the parsed structure. FIX (SDKV-1): array-shaped text now parses
        /// via <see cref="JArray"/> as well as object-shaped text via
        /// <see cref="JObject"/> (previously only JObject was attempted, so
        /// array-shaped values stayed strings on the wire). Non-string
        /// tokens (already objects/arrays from the typed models) and
        /// unparseable text are left untouched. Internal for test access.
        /// </summary>
        internal static void ChangeStringToObject(JToken input)
        {
            //if (input == null|| input.Any()==false)
            //{ }
            //else if (input.Any())
            //{
            //    try
            //    {
            //        //something to do with attachments here I think.
            //    }
            //    catch (Exception ex)
            //    {
            //        Log.Logger.Error("Error converting string inside array to Object :" + ex.Message);
            //    }
            //}
            //else {
            try
            {
                if (input == null || input.Type != JTokenType.String)
                {
                    // already structured (typed models emit real objects/arrays)
                    // or nothing to repair
                    return;
                }
                string inputValue = input.ToString();
                string trimmed = inputValue.Trim();
                JToken output;
                if (trimmed.StartsWith("[", StringComparison.Ordinal))
                {
                    // FIX (SDKV-1): attempt JArray for array-shaped text.
                    // Newtonsoft parses degenerate text like "[,]" leniently
                    // (to [undefined]) instead of throwing -- treat that as
                    // unparseable and leave the string untouched.
                    output = JArray.Parse(trimmed);
                    if (StatementFactoring.HasUndefinedToken(output))
                    {
                        return;
                    }
                }
                else
                {
                    output = JsonConvert.DeserializeObject<JObject>(inputValue);
                }
                input.Replace(output);
            }
            catch (Exception ex)
            {
                //Log.Logger.Error("Error converting string to Object :" + ex.Message);
            }
            //}
        }

        #endregion

        #region change properties to lowercase

        /// <summary>
        /// Property names whose OBJECT values are xAPI Language Maps
        /// (RFC 5646 language tag -> string): verb.display,
        /// definition.name/description, attachments[].display/description,
        /// interactioncomponents[].description. Their child keys are
        /// language TAGS ("en-US"), not dialect property names -- the
        /// canonical RFC 5646 casing (lowercase language, UPPERCASE region)
        /// must survive the input-normalization pass because
        /// StatementFactoring.TokenToLanguageMap copies the keys verbatim
        /// onto the wire. The holder key itself IS still lowercased
        /// (factoring reads all-lowercase keys). In the authored dialect no
        /// non-language-map object ever sits under these names (actor.name /
        /// account.name are plain strings, which never recurse anyway).
        /// </summary>
        private static readonly string[] LanguageMapHolderKeys = { "display", "name", "description" };

        /// <summary>
        /// Recursively lowercases every property name. SDKV-12 note: this is
        /// now used ONLY to normalize the authored INPUT statement before
        /// factoring (StatementFactoring reads all-lowercase keys). It must
        /// never run on the factored output again -- emission casing comes
        /// from the explicit [JsonProperty] wire names on Models/xAPI.
        /// FIX (SDKV-23): arrays are walked per element by actual token type
        /// (see <see cref="ChangePropertiesToLowerCaseInArray"/>) instead of
        /// blind-casting every element to JObject, and language-map values
        /// keep their RFC 5646 tag keys untouched (see
        /// <see cref="LanguageMapHolderKeys"/>). Internal for test access
        /// (same MDM-T2 rationale as <see cref="ChangeStringToObject"/>).
        /// </summary>
        internal static void ChangePropertiesToLowerCase(JObject jsonObject)
        {
            foreach (var property in jsonObject.Properties().ToList())
            {
                if (property.Value.Type == JTokenType.Object)// replace property names in child object
                {
                    // FIX (SDKV-23, part 2): don't descend into language
                    // maps -- their keys are RFC 5646 tags ("en-US" stays
                    // "en-US"), not dialect property names.
                    if (!IsLanguageMapHolder(property.Name))
                    {
                        ChangePropertiesToLowerCase((JObject)property.Value);
                    }
                }

                if (property.Value.Type == JTokenType.Array)
                {
                    #region [Historical - SDKV-23] blind JObject cast per array element
                    // The original array walk round-tripped the array through
                    // text and cast EVERY element to JObject. A real JSON
                    // string array (spec-shaped correctresponsespattern:
                    // ["a","b"]) made the cast throw InvalidCastException,
                    // the Initialize-wide catch swallowed it, and the whole
                    // initialization returned false -- nothing emitted.
                    // Replaced by the type-checked per-element walk below.
                    //var arr = JArray.Parse(property.Value.ToString());
                    //foreach (var pr in arr)
                    //{
                    //    ChangePropertiesToLowerCase((JObject)pr);
                    //}

                    //property.Value = arr;
                    #endregion
                    ChangePropertiesToLowerCaseInArray((JArray)property.Value);
                }

                property.Replace(new JProperty(property.Name.ToLower(), property.Value));// properties are read-only, so we have to replace them
            }
        }

        /// <summary>
        /// FIX (SDKV-23): array companion to
        /// <see cref="ChangePropertiesToLowerCase(JObject)"/>. Object
        /// elements recurse into the normal key-lowercasing walk, nested
        /// arrays recurse here, and scalar elements (strings, numbers,
        /// booleans, nulls) carry no property names so they pass through
        /// untouched -- previously they were blind-cast to JObject, which
        /// threw and failed the whole Initialize.
        /// </summary>
        private static void ChangePropertiesToLowerCaseInArray(JArray array)
        {
            foreach (JToken element in array)
            {
                if (element.Type == JTokenType.Object)
                {
                    ChangePropertiesToLowerCase((JObject)element);
                }
                else if (element.Type == JTokenType.Array)
                {
                    ChangePropertiesToLowerCaseInArray((JArray)element);
                }
            }
        }

        /// <summary>
        /// FIX (SDKV-23, part 2): true when <paramref name="propertyName"/>
        /// is one of the <see cref="LanguageMapHolderKeys"/> (authored casing
        /// is arbitrary, so the comparison is case-insensitive).
        /// </summary>
        private static bool IsLanguageMapHolder(string propertyName)
        {
            return LanguageMapHolderKeys.Any(
                holder => string.Equals(propertyName, holder, StringComparison.OrdinalIgnoreCase));
        }
        #endregion

        #endregion

        // SIM-T13 G11: [Historical] block preserved under a relabeled region.
        // Five earlier shapes of Initialize() live below, all commented out. They
        // document the progression: synchronous-bool returns, Android-specific
        // pre-Android-11 path, Android post-11 path with explicit storage path,
        // device-type-routed variant, and the final "OS-routed via handler" shape
        // that became the live Initialize(input, ExpectedOperatingSystem OS). The
        // live signature is the one in the top half of this file; do NOT revive
        // anything from below without explicit reason -- they all have known
        // shortcomings the live shape addresses. Audit doc:
        // docs/SIMULATION_ROADMAP/TIER_13_AUDIT_MDM_COMPAT.md (gap G11).
        #region [Historical - SIM-T13] alternate Initialize overloads (5 earlier shapes)
        ///// <summary>
        ///// Initialize xAPI statement and populate needed items.
        ///// </summary>
        ///// <param name="input"></param>
        ///// <returns></returns>
        //public bool Initialize(string input)
        //{
        //    bool isInitialized = false;
        //    try
        //    {
        //        #region file system initalizer
        //        FileSystem.FileSystemInitalizer.FileInitalizer();
        //        #endregion

        //        #region set unmapped global logger
        //        //Log.Logger = CreateLogger(string.Empty);
        //        #endregion

        //        #region command line args
        //        //get command line args
        //        string[] arguments = Environment.GetCommandLineArgs();
        //        #region just for testing 
        //        if (arguments == null || arguments.Length < 2)
        //        {
        //            arguments = new string[] { input };
        //        }
        //        #endregion
        //        string statement = JSONHandler.ArgumentHandler(arguments);
        //        #endregion

        //        #region replace removed spaces from statement
        //        statement = statement.Replace("_-_", " ");
        //        #endregion

        //        #region Change to initalJobject
        //        JObject statementJObject = JSONHandler.ChangeToObject(statement);
        //        ChangePropertiesToLowerCase(statementJObject);
        //        #endregion

        //        #region Set up mapped logging

        //        #region Create log directory
        //        string objectId = (string)statementJObject["object"]["uuid"];
        //        FileSystem.FileSystemInitalizer.CreateFileDirectory(FileSystem.FileSystem.SimulationLogBasePath, objectId);
        //        #endregion

        //        #region set mapped global logger                
        //        //Log.Logger = CreateLogger(objectId);
        //        #endregion

        //        #endregion

        //        #region convert to jobject following data model
        //        JObject statementFromDataModel = JSONHandler.CreateObjectFromDataModel(statementJObject);
        //        ChangePropertiesToLowerCase(statementFromDataModel);
        //        #endregion



        //        #region Check needed parts of JObject                
        //        bool statmentIsProperlySetUp = StatementHandler.StatementCheck(statementFromDataModel);
        //        if (!statmentIsProperlySetUp)
        //        {
        //            return false;
        //        }
        //        #endregion

        //        #region set the new strings to jobjects
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["interactioncomponents"]);
        //        ChangeStringToObject(statementFromDataModel["verb"]["display"]);
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["name"]);
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["description"]);
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["correctresponsespattern"]);

        //        //change attachment
        //        //ChangeStringToObject(statementFromDataModel["attachments"][0?]["display"]);
        //        //ChangeStringToObject(statementFromDataModel["attachments"][0?]["description"]);
        //        #endregion

        //        #region write to data file
        //        //create and write object to a file            
        //        isInitialized = JSONHandler.CreateInitialDataFile(statementFromDataModel);
        //        #endregion
        //    }
        //    catch (Exception ex)
        //    {
        //        //Log.Logger.Fatal(ex.Message);
        //        //Log.Logger.Fatal(ex.Source);
        //        throw;
        //    }
        //    return isInitialized;
        //}


        //public bool Initialize(string input, string path)
        //{
        //    //Console.WriteLine("Inside Initializer");
        //    bool isInitialized = false;
        //    try
        //    {
        //        #region file system initalizer

        //        if (string.IsNullOrEmpty(path))
        //        {
        //            FileSystem.FileSystemInitalizer.FileInitalizer();
        //        }
        //        else
        //        {
        //            string _publicFileName = "com.febris.public_files";
        //            string _externalPath = path + "/" + _publicFileName;
        //            FileSystem.FileSystemInitalizer.FileInitalizer(_externalPath);
        //        }



        //        #endregion

        //        #region set unmapped global logger
        //        //Log.Logger = CreateLogger(string.Empty);
        //        #endregion

        //        #region command line args
        //        //get command line args
        //        string arguments = input;
        //        #region just for testing 
        //        //if (arguments == null || arguments.Length < 2)
        //        //{
        //        //    arguments = new string[] { input };
        //        //}
        //        #endregion                
        //        string statement = JSONHandler.ArgumentHandler(arguments);
        //        #endregion

        //        #region replace removed spaces from statement
        //        statement = statement.Replace("_-_", " ");
        //        #endregion

        //        #region Change to initalJobject
        //        JObject statementJObject = JSONHandler.ChangeToObject(statement);
        //        ChangePropertiesToLowerCase(statementJObject);
        //        #endregion

        //        #region Set up mapped logging

        //        #region Create log directory
        //        string objectId = (string)statementJObject["object"]["uuid"];
        //        FileSystem.FileSystemInitalizer.CreateFileDirectory(FileSystem.FileSystem.SimulationLogBasePath, objectId);
        //        #endregion

        //        #region set mapped global logger                
        //        //Log.Logger = CreateLogger(objectId);
        //        #endregion

        //        Console.WriteLine("Finished creating directory");

        //        #endregion

        //        #region convert to jobject following data model
        //        JObject statementFromDataModel = JSONHandler.CreateObjectFromDataModel(statementJObject);
        //        ChangePropertiesToLowerCase(statementFromDataModel);
        //        #endregion


        //        #region Check needed parts of JObject                
        //        bool statmentIsProperlySetUp = StatementHandler.StatementCheck(statementFromDataModel);
        //        if (!statmentIsProperlySetUp)
        //        {
        //            return false;
        //        }
        //        #endregion

        //        #region set the new strings to jobjects
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["interactioncomponents"]);
        //        ChangeStringToObject(statementFromDataModel["verb"]["display"]);
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["name"]);
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["description"]);
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["correctresponsespattern"]);

        //        //change attachment
        //        //ChangeStringToObject(statementFromDataModel["object"]["definition"]["correctresponsespattern"]);
        //        #endregion

        //        Console.WriteLine("Finished jobject factoring");

        //        #region write to data file
        //        //create and write object to a file            
        //        isInitialized = JSONHandler.CreateInitialDataFile(statementFromDataModel);
        //        #endregion

        //        Console.WriteLine("Finished writing data to file");
        //    }
        //    catch (Exception ex)
        //    {
        //        Console.WriteLine(ex.Message);
        //        //Log.Logger.Fatal(ex.Message);
        //        //Log.Logger.Fatal(ex.Source);
        //        return isInitialized;
        //        throw;
        //    }
        //    return isInitialized;
        //}

        ///// <summary>
        ///// Default for desktop and laptop deployments
        ///// </summary>
        ///// <param name="input"></param>
        ///// <returns></returns>
        //public bool Initialize(string[] input)
        //{
        //    //Console.WriteLine("Inside Initializer");
        //    bool isInitialized = false;
        //    try
        //    {
        //        #region file system initalizer

        //        //if (deviceType == DeviceType.Desktop)
        //        //{
        //        FileSystem.FileSystemInitalizer.FileInitalizer();
        //        //}
        //        //else if (deviceType == DeviceType.Handheld)
        //        //{
        //        //    string _publicFileName = "com.febris.public_files";
        //        //    Android.OS
        //        //    FileSystem.FileSystemInitalizer.FileInitalizer();
        //        //}



        //        #endregion

        //        #region set unmapped global logger
        //        //Log.Logger = CreateLogger(string.Empty);
        //        #endregion

        //        #region command line args
        //        //get command line args
        //        string[] arguments = input;
        //        #region just for testing 
        //        //if (arguments == null || arguments.Length < 2)
        //        //{
        //        //    arguments = new string[] { input };
        //        //}
        //        #endregion                
        //        string statement = JSONHandler.ArgumentHandler(arguments);
        //        #endregion

        //        #region replace removed spaces from statement
        //        statement = statement.Replace("_-_", " ");
        //        #endregion

        //        #region Change to initalJobject
        //        JObject statementJObject = JSONHandler.ChangeToObject(statement);
        //        ChangePropertiesToLowerCase(statementJObject);
        //        #endregion

        //        #region Set up mapped logging

        //        #region Create log directory
        //        string objectId = (string)statementJObject["object"]["uuid"];
        //        FileSystem.FileSystemInitalizer.CreateFileDirectory(FileSystem.FileSystem.SimulationLogBasePath, objectId);
        //        #endregion

        //        #region set mapped global logger                
        //        //Log.Logger = CreateLogger(objectId);
        //        #endregion

        //        Console.WriteLine("Finished creating directory");

        //        #endregion

        //        #region convert to jobject following data model
        //        JObject statementFromDataModel = JSONHandler.CreateObjectFromDataModel(statementJObject);
        //        ChangePropertiesToLowerCase(statementFromDataModel);
        //        #endregion


        //        #region Check needed parts of JObject                
        //        bool statmentIsProperlySetUp = StatementHandler.StatementCheck(statementFromDataModel);
        //        if (!statmentIsProperlySetUp)
        //        {
        //            return false;
        //        }
        //        #endregion

        //        #region set the new strings to jobjects
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["interactioncomponents"]);
        //        ChangeStringToObject(statementFromDataModel["verb"]["display"]);
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["name"]);
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["description"]);
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["correctresponsespattern"]);

        //        //change attachment
        //        //ChangeStringToObject(statementFromDataModel["object"]["definition"]["correctresponsespattern"]);
        //        #endregion

        //        Console.WriteLine("Finished jobject factoring");

        //        #region write to data file
        //        //create and write object to a file            
        //        isInitialized = JSONHandler.CreateInitialDataFile(statementFromDataModel);
        //        #endregion

        //        Console.WriteLine("Finished writing data to file");
        //    }
        //    catch (Exception ex)
        //    {
        //        Console.WriteLine(ex.Message);
        //        //Log.Logger.Fatal(ex.Message);
        //        //Log.Logger.Fatal(ex.Source);
        //        return isInitialized;
        //        throw;
        //    }
        //    return isInitialized;
        //}




        ///// <summary>
        ///// This setup is specifically for android but if path is left as null or empty id defaults to internal document folders
        ///// 
        ///// This had to be update to work with intents and completely get rid of FileSystem
        ///// </summary>
        ///// <param name="input"></param>
        ///// <param name="path"></param>
        ///// <returns></returns>
        //public bool Initialize(string[] input, string path)
        //{
        //    //Console.WriteLine("Inside Initializer");
        //    bool isInitialized = false;
        //    try
        //    {
        //        #region Post Android 11

        //        #region file system initalizer

        //        if (string.IsNullOrEmpty(path))
        //        {
        //            FileSystem.FileSystemInitalizer.FileInitalizer();
        //        }
        //        else
        //        {



        //            string _publicFileName = "com.febris.public_files";
        //            string _externalPath = path + "/" + _publicFileName;
        //            FileSystem.FileSystemInitalizer.FileInitalizer(_externalPath);
        //        }



        //        #endregion

        //        #region set unmapped global logger
        //        //Log.Logger = CreateLogger(string.Empty);
        //        #endregion

        //        #region command line args
        //        //get command line args
        //        string[] arguments = input;
        //        #region just for testing 
        //        //if (arguments == null || arguments.Length < 2)
        //        //{
        //        //    arguments = new string[] { input };
        //        //}
        //        #endregion                
        //        string statement = JSONHandler.ArgumentHandler(arguments);
        //        #endregion

        //        #region replace removed spaces from statement
        //        statement = statement.Replace("_-_", " ");
        //        #endregion

        //        #region Change to initalJobject
        //        JObject statementJObject = JSONHandler.ChangeToObject(statement);
        //        ChangePropertiesToLowerCase(statementJObject);
        //        #endregion

        //        #region Set up mapped logging

        //        #region Create log directory
        //        string objectId = (string)statementJObject["object"]["uuid"];
        //        FileSystem.FileSystemInitalizer.CreateFileDirectory(FileSystem.FileSystem.SimulationLogBasePath, objectId);
        //        #endregion

        //        Console.WriteLine("Finished creating directory");

        //        #endregion

        //        #region convert to jobject following data model
        //        JObject statementFromDataModel = JSONHandler.CreateObjectFromDataModel(statementJObject);
        //        ChangePropertiesToLowerCase(statementFromDataModel);
        //        #endregion


        //        #region Check needed parts of JObject                
        //        bool statmentIsProperlySetUp = StatementHandler.StatementCheck(statementFromDataModel);
        //        if (!statmentIsProperlySetUp)
        //        {
        //            return false;
        //        }
        //        #endregion

        //        #region set the new strings to jobjects
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["interactioncomponents"]);
        //        ChangeStringToObject(statementFromDataModel["verb"]["display"]);
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["name"]);
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["description"]);
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["correctresponsespattern"]);

        //        //change attachment
        //        //ChangeStringToObject(statementFromDataModel["object"]["definition"]["correctresponsespattern"]);
        //        #endregion

        //        Console.WriteLine("Finished jobject factoring");

        //        #region send statement to companion application

        //        isInitialized = StatementForwarding.CreateInitialPost(statementFromDataModel);


        //        //create and write object to a file            
        //        isInitialized = JSONHandler.CreateInitialDataFile(statementFromDataModel);
        //        #endregion

        //        Console.WriteLine("Finished writing data to file");

        //        #endregion


        //        #region Pre Android 11

        //        //#region file system initalizer

        //        //if (string.IsNullOrEmpty(path))
        //        //{
        //        //    FileSystem.FileSystemInitalizer.FileInitalizer();
        //        //}
        //        //else
        //        //{
        //        //    string _publicFileName = "com.febris.public_files";
        //        //    string _externalPath = path + "/" + _publicFileName;
        //        //    FileSystem.FileSystemInitalizer.FileInitalizer(_externalPath);
        //        //}



        //        //#endregion

        //        //#region set unmapped global logger
        //        ////Log.Logger = CreateLogger(string.Empty);
        //        //#endregion

        //        //#region command line args
        //        ////get command line args
        //        //string[] arguments = input;
        //        //#region just for testing 
        //        ////if (arguments == null || arguments.Length < 2)
        //        ////{
        //        ////    arguments = new string[] { input };
        //        ////}
        //        //#endregion                
        //        //string statement = JSONHandler.ArgumentHandler(arguments);
        //        //#endregion

        //        //#region replace removed spaces from statement
        //        //statement = statement.Replace("_-_", " ");
        //        //#endregion

        //        //#region Change to initalJobject
        //        //JObject statementJObject = JSONHandler.ChangeToObject(statement);
        //        //ChangePropertiesToLowerCase(statementJObject);
        //        //#endregion

        //        //#region Set up mapped logging

        //        //#region Create log directory
        //        //string objectId = (string)statementJObject["object"]["uuid"];
        //        //FileSystem.FileSystemInitalizer.CreateFileDirectory(FileSystem.FileSystem.SimulationLogBasePath, objectId);
        //        //#endregion

        //        //#region set mapped global logger                
        //        ////Log.Logger = CreateLogger(objectId);
        //        //#endregion

        //        //Console.WriteLine("Finished creating directory");

        //        //#endregion

        //        //#region convert to jobject following data model
        //        //JObject statementFromDataModel = JSONHandler.CreateObjectFromDataModel(statementJObject);
        //        //ChangePropertiesToLowerCase(statementFromDataModel);
        //        //#endregion


        //        //#region Check needed parts of JObject                
        //        //bool statmentIsProperlySetUp = StatementHandler.StatementCheck(statementFromDataModel);
        //        //if (!statmentIsProperlySetUp)
        //        //{
        //        //    return false;
        //        //}
        //        //#endregion

        //        //#region set the new strings to jobjects
        //        //ChangeStringToObject(statementFromDataModel["object"]["definition"]["interactioncomponents"]);
        //        //ChangeStringToObject(statementFromDataModel["verb"]["display"]);
        //        //ChangeStringToObject(statementFromDataModel["object"]["definition"]["name"]);
        //        //ChangeStringToObject(statementFromDataModel["object"]["definition"]["description"]);
        //        //ChangeStringToObject(statementFromDataModel["object"]["definition"]["correctresponsespattern"]);

        //        ////change attachment
        //        ////ChangeStringToObject(statementFromDataModel["object"]["definition"]["correctresponsespattern"]);
        //        //#endregion

        //        //Console.WriteLine("Finished jobject factoring");

        //        //#region write to data file
        //        ////create and write object to a file            
        //        //isInitialized = JSONHandler.CreateInitalDataFile(statementFromDataModel);
        //        //#endregion

        //        //Console.WriteLine("Finished writing data to file");

        //        #endregion
        //    }
        //    catch (Exception ex)
        //    {
        //        Console.WriteLine(ex.Message);
        //        //Log.Logger.Fatal(ex.Message);
        //        //Log.Logger.Fatal(ex.Source);
        //        return isInitialized;
        //        throw;
        //    }
        //    return isInitialized;
        //}

        //public bool Initialize()
        //{
        //    bool isInitialized = false;
        //    try
        //    {
        //        #region file system initalizer
        //        FileSystem.FileSystemInitalizer.FileInitalizer();
        //        #endregion

        //        #region set unmapped global logger
        //        //Log.Logger = CreateLogger(string.Empty);
        //        #endregion

        //        #region command line args
        //        //get command line args
        //        string[] arguments = Environment.GetCommandLineArgs();
        //        #region just for testing 
        //        //if (arguments == null || arguments.Length < 2)
        //        //{
        //        //    arguments = new string[] { input };
        //        //}
        //        #endregion
        //        string statement = JSONHandler.ArgumentHandler(arguments);
        //        #endregion

        //        #region replace removed spaces from statement
        //        statement = statement.Replace("_-_", " ");
        //        #endregion

        //        #region Change to initalJobject
        //        JObject statementJObject = JSONHandler.ChangeToObject(statement);
        //        ChangePropertiesToLowerCase(statementJObject);
        //        #endregion

        //        #region Set up mapped logging

        //        #region Create log directory
        //        string objectId = (string)statementJObject["object"]["uuid"];
        //        FileSystem.FileSystemInitalizer.CreateFileDirectory(FileSystem.FileSystem.SimulationLogBasePath, objectId);
        //        #endregion

        //        #region set mapped global logger                
        //        //Log.Logger = CreateLogger(objectId);
        //        #endregion

        //        #endregion

        //        #region convert to jobject following data model
        //        JObject statementFromDataModel = JSONHandler.CreateObjectFromDataModel(statementJObject);
        //        ChangePropertiesToLowerCase(statementFromDataModel);
        //        #endregion



        //        #region Check needed parts of JObject                
        //        bool statmentIsProperlySetUp = StatementHandler.StatementCheck(statementFromDataModel);
        //        if (!statmentIsProperlySetUp)
        //        {
        //            return false;
        //        }
        //        #endregion

        //        #region set the new strings to jobjects
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["interactioncomponents"]);
        //        ChangeStringToObject(statementFromDataModel["verb"]["display"]);
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["name"]);
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["description"]);
        //        ChangeStringToObject(statementFromDataModel["object"]["definition"]["correctresponsespattern"]);

        //        //change attachment
        //        //ChangeStringToObject(statementFromDataModel["object"]["definition"]["correctresponsespattern"]);
        //        #endregion

        //        #region write to data file
        //        //create and write object to a file            
        //        isInitialized = JSONHandler.CreateInitialDataFile(statementFromDataModel);
        //        #endregion
        //    }
        //    catch (Exception ex)
        //    {
        //        //Log.Logger.Fatal(ex.Message);
        //        //Log.Logger.Fatal(ex.Source);
        //        throw;
        //    }
        //    return isInitialized;
        //}

        #region serilog creation
        //private static ILogger CreateLogger()
        //{
        //    //var builder = new ConfigurationBuilder();
        //    //BuildConfig(builder);

        //    string path = FileSystem.FileSystem.SimulationLogBasePath+"ObjectId";

        //    var logger = new LoggerConfiguration()
        //        .WriteTo.File(Path.Combine(path, "log.json"), rollingInterval: RollingInterval.Day)
        //        .MinimumLevel.Debug()
        //        .CreateLogger();
        //    return logger;
        //}
        #endregion

        #region mapped serilog creation
        //private static ILogger CreateLogger(string input)
        //{
        //    //var builder = new ConfigurationBuilder();
        //    //BuildConfig(builder);
        //    string path = string.Empty;
        //    if (input == string.Empty)
        //    {
        //        path = FileSystem.FileSystem.SimulationLogBasePath;
        //    }
        //    else
        //    {
        //        path = Path.Combine(FileSystem.FileSystem.SimulationLogBasePath, input);
        //    }

        //    var logger = new LoggerConfiguration()
        //        .WriteTo.File(Path.Combine(path, "log.json"), rollingInterval: RollingInterval.Day)
        //        .MinimumLevel.Debug()
        //        .CreateLogger();
        //    return logger;
        //}
        #endregion

        #region config
        /// <summary>
        /// Logging setup with serialog
        /// </summary>
        /// <param name="builder"></param>
        //static void BuildConfig(IConfigurationBuilder builder)
        //{
        //    //builder.SetBasePath(Directory.GetCurrentDirectory()).AddJsonFile("appsettings.json", optional: false, reloadOnChange: true)
        //    //    .AddJsonFile($"appsettings.{Environment.GetEnvironmentVariable("ASPNETCORE_ENVIRONMENT") ?? "Production"}.json", optional: true)
        //    //    .AddEnvironmentVariables();
        //}
        #endregion

        #endregion
    }
}
