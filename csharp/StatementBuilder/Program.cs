// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Febris.CsharpSimulationLibraryNetStandard.Enums;
using Febris.CsharpSimulationLibraryNetStandard.SharedDetails;
using Febris.CsharpSimulationLibraryNetStandard.Statement;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;

namespace StatementBuilder
{
    class Program
    {
        static void Main(string[] args)
        {
            int numberOfCycles = 1000;

            for (var i = 0; i < numberOfCycles; i++)
            {
                Stopwatch __stopwatch = new Stopwatch();
                string[] arguments = default;
                string statement = ArgumentBuilder();
                //arguments = ArgumentBuilder();
                string expectedArguments = AddWrapper(statement);
                bool initialized = false;
                string[,] returnedIntentTags = default;
                arguments = new string[] { expectedArguments };
                (initialized, returnedIntentTags) = Initializer.Initialize(arguments, ExpectedOperatingSystem.WindowsPC).Result;
                if (initialized)
                {
                    
                    ///start stop watch
                    __stopwatch.Start();
                    ///Set a couple of parameters
                    Febris.CsharpSimulationLibraryNetStandard.Statement.StatementHandler.UpdateStatement(XAPIProperties.Result, ResultOptions.ScoreMax, 100f);
                    Febris.CsharpSimulationLibraryNetStandard.Statement.StatementHandler.UpdateStatement(XAPIProperties.Result, ResultOptions.ScoreMin, 0f);
                    ///save file
                    bool ready = false;
                    string[,] outputArray = default;
                    (ready,outputArray)= Febris.CsharpSimulationLibraryNetStandard.Statement.StatementHandler.GetSendableUpdate().Result;

                    ///Add a restart counter
                    int restartCounter = RandomNumberGrabber(0, 10);

                    for (var j = 0; j < restartCounter; j++)
                    {
                        Febris.CsharpSimulationLibraryNetStandard.Statement.StatementHandler.StageRestart();
                    }
                    (ready, outputArray) = Febris.CsharpSimulationLibraryNetStandard.Statement.StatementHandler.GetSendableUpdate().Result;
                    ///Add a note
                    int noteCounter = RandomNumberGrabber(0, 10);

                    for (var j = 0; j < noteCounter; j++)
                    {
                        string note = "This is a very useful note: " + j.ToString();
                        Febris.CsharpSimulationLibraryNetStandard.Statement.StatementHandler.AddResultNote(note);
                    }
                    (ready, outputArray) = Febris.CsharpSimulationLibraryNetStandard.Statement.StatementHandler.GetSendableUpdate().Result;
                    ///SDKV-4: exercise a context-activity update -- previously
                    ///a silent no-op (typo key 'contextactivites'); the emitted
                    ///statement must now carry context.contextActivities.parent
                    Febris.CsharpSimulationLibraryNetStandard.Statement.StatementHandler.UpdateStatement(XAPIProperties.Context, ContextOptions.ContextActivites, ContextActivitesOptions.Parent, "https://febr.is/Curricula/parentActivity");
                    ///SDKV-2: exercise an attachment -- display/description must
                    ///emit as Language Map objects ({"en": ...}), keys usageType/
                    ///contentType/fileUrl in spec casing
                    Febris.CsharpSimulationLibraryNetStandard.Statement.StatementHandler.UpdateStatement(XAPIProperties.Attachments, "https://febr.is/Attachment/VideoReview", "Video Review", "Session capture", ContentType.video_mpeg, 2048, "sha2placeholder", "https://example.com/clip.mp4");
                    (ready, outputArray) = Febris.CsharpSimulationLibraryNetStandard.Statement.StatementHandler.GetSendableUpdate().Result;
                    ///add a scoring system
                    ///
                    int randomScore = RandomNumberGrabber(40, 100);
                    Febris.CsharpSimulationLibraryNetStandard.Statement.StatementHandler.UpdateStatement(XAPIProperties.Result, ResultOptions.ScoreRaw, randomScore);
                    (ready, outputArray) = Febris.CsharpSimulationLibraryNetStandard.Statement.StatementHandler.GetSendableUpdate().Result;
                    ///work with timing
                    ///
                    int randomTime = RandomNumberGrabber(600, 1800);
                    TimeSpan artificialTime = TimeSpan.FromSeconds(randomTime);
                    Febris.CsharpSimulationLibraryNetStandard.Statement.StatementHandler.DurationUpdate(artificialTime);// __stopwatch.Elapsed);

                    ///Set time stamp


                    ///end simulation
                    ///                    
                    if (randomScore > 70)
                    {                        
                        Febris.CsharpSimulationLibraryNetStandard.Statement.StatementHandler.SimulationPassed(true);
                        Febris.CsharpSimulationLibraryNetStandard.Statement.StatementHandler.VerbUpdate(VerbEnums.Pass);
                    }
                    else
                    {
                        Febris.CsharpSimulationLibraryNetStandard.Statement.StatementHandler.VerbUpdate(VerbEnums.Not_Pass);
                    }
                    
                    Febris.CsharpSimulationLibraryNetStandard.Statement.StatementHandler.SimulationComplete();
                    


                }
                else
                {
                    Console.WriteLine("error initializing arguments");
                }
            }

            ///Build out statement as if it were in a simulation
            //timer

            //starting contraints
            //float __rawScore = 0f;
            //float __minScore = 0f;
            //float __maxScore = 0f;
            //float __scaledScore = 0f;
            //float __period = 10f;
            //float __nextUpdate = 10f;

          
            




        }

        private static string AddWrapper(string arguments)
        {
            string output = default;
            try
            {
                //Add preface
                string preface = SharedDetails.StatementPreface +"{";
                string postface = "}";

                output = preface + arguments + postface;
            }
            catch (Exception ex)
            {

                throw;
            }
            return output;
        }

        ///Build out the starting statement
        //private static string[] ArgumentBuilder()
            private static string ArgumentBuilder()
        {
            //string[] output = new string[0];
            string output = default;
            try
            {
                string timeStampMod = GrabTimeStamp();

                //Add preface
                //string preface = SharedDetails.StatementPreface;
                //Actor
                string actor = GrabActor();
                //Object
                string xApiObject = GrabObject();
                //Verb
                string verb = GrabVerb();

                //List<string> stringList = new List<string>();
                ////stringList.Add(preface);
                //stringList.Add(actor);
                //stringList.Add(xApiObject);
                //stringList.Add(verb);

                //output = stringList.ToArray();
                output = timeStampMod+actor + xApiObject + verb;

            }
            catch (Exception ex)
            {

                throw;
            }
            return output;
        }

       

        #region operations

        static int RandomNumberGrabber(int start, int finish)
        {
            int output = default;
            try
            {
                Random random = new Random();
                output = random.Next(start, finish + 1);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.StackTrace);
                throw;
            }
            return output;
        }

        private static string GrabTimeStamp()
        {
            string output = default;
            try
            {
                int dateOffset = RandomNumberGrabber(0, 30);

                DateTime dateToUse = DateTime.UtcNow.AddDays(-dateOffset);

                output = "\"TimeStamp\":\""+dateToUse.ToString("o")+"\",";
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.StackTrace);
                throw;
            }
            return output;
        }
        static string GrabActor()
        {
            string output = default;
            try
            {
                List<string> list = GetActorList();

                int listSize = list.Count-1;

                if (listSize > 1)
                {
                    int selection = RandomNumberGrabber(0, listSize);
                    output = list.ElementAtOrDefault(selection);
                }
                else
                {
                    output = list.FirstOrDefault();
                }

            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.StackTrace);
                throw;
            }
            return output;
        }
        static string GrabObject()
        {
            string output = default;
            try
            {
                List<string> list = GetObjectList();

                int listSize = list.Count;

                if (listSize > 1)
                {
                    int selection = RandomNumberGrabber(0, listSize);
                    output = list.ElementAtOrDefault(selection);
                }
                else
                {
                    output = list.FirstOrDefault();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.StackTrace);
                throw;
            }
            return output;
        }
        static string GrabVerb()
        {
            string output = default;
            try
            {
                List<string> list = GetVerbList();

                int listSize = list.Count;

                if (listSize > 1)
                {
                    int selection = RandomNumberGrabber(0, listSize);
                    output = list.ElementAtOrDefault(selection);
                }
                else
                {
                    output = list.FirstOrDefault();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.StackTrace);
                throw;
            }
            return output;
        }
        #endregion



        #region Lists of options
        static List<string> GetActorList()
        {
            List<string> output = new List<string>();
            try
            {
                //string item0 = "\"actor\":{\"id\":3,\"uuid\":\"672896d1-d9f7-48d8-ac22-d4efa4e94902\",\"objecttype\":\"Agent\",\"name\":\"Kiera_Anthony27bc8cdb-8611-4f91-a8c2-2ae3618750ad\",\"mbox\":null,\"mbox_sha1sum\":\"4a544776e93b80615f77a462b0126c7976865fc6\",\"openid\":null,\"account\":{\"id\":0,\"uuid\":\"00000000-0000-0000-0000 - 000000000000\",\"homepage\":null,\"name\":null},\"member\":{\"id\":0,\"uuid\":\"00000000-0000-0000-0000-000000000000\",\"actors\":[]}},";
                string item0 = "\"Actor\":{ \"Id\":3,\"UUID\":\"672896d1-d9f7-48d8-ac22-d4efa4e94902\",\"ObjectType\":\"Agent\",\"Name\":\"Kiera_Anthony27bc8cdb-8611-4f91-a8c2-2ae3618750ad\",\"Mbox\":null,\"Mbox_sha1sum\":\"4a544776e93b80615f77a462b0126c7976865fc6\",\"OpenId\":null,\"Account\":null,\"Member\":null},";
                string item1 = "\"Actor\":{ \"Id\":1,\"UUID\":\"7be59fa8-d112-48ef-a218-e8ca89dc180c\",\"ObjectType\":\"Agent\",\"Name\":\"Antonio_Le07ebfcc5-5318-4df7-84fa-f7b442caf577\",\"Mbox\":null,\"Mbox_sha1sum\":\"009d2ed828d38440ab6251fe6e8ceeb0c2ad454c\",\"OpenId\":null,\"Account\":null,\"Member\":null},";
                output.Add(item0);
                output.Add(item1);

            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.StackTrace);
                throw;
            }
            return output;
        }

        static List<string> GetObjectList()
        {
            List<string> output = new List<string>();
            try
            {
                //string item0 = "\"object\":{\"key\":1,\"uuid\":\"99d9db56-48e8-4736-b92f-29e4d3361403\",\"id\":\"https://febr.is/Module/460a5ddf-02c3-4fb8-9d7c-0ef0da64925d\",\"objecttype\":\"Activity\",\"definition\":{\"id\":1,\"uuid\":\"be503c6c-f44b-4784-9077-88410f93b071\",\"name\":{\"en-US\":\"PC_-_Demo\"},\"description\":{\"en-US\":\"This_-_is_-_the_-_old_-_sterile_-_field_-_demo_-_with_-_integration_-_of_-_the_-_c#_-_library\"},\"type\":\"https://febr.is/Module/1\",\"moreinfo\":\"https://febr.is/Module/1\",\"extensions\":{\"id\":0,\"uuid\":\"00000000-0000-0000-0000-000000000000\",\"extensionmap\":null},\"interactiontype\":\"performance\",\"correctresponsespattern\":\"[,]\",\"interactioncomponents\":{\"step1\":\"Get Cloth\",\"step2\":\"Clean table\",\"step3\":\"Select Sterile pack\",\"step3.1\":\"Check Sterile pack for date and damage\",\"step4\":\"etc\"}}},";
                string item0 = "\"Object\":{ \"Key\":1,\"UUID\":\"99d9db56-48e8-4736-b92f-29e4d3361403\",\"Id\":\"https://febr.is/Module/460a5ddf-02c3-4fb8-9d7c-0ef0da64925d\",\"ObjectType\":\"Activity\",\"Definition\":{ \"Id\":1,\"UUID\":\"be503c6c-f44b-4784-9077-88410f93b071\",\"Name\":{ \"en-US\":\"PC_-_Demo\"},\"Description\":{ \"en-US\":\"This_-_is_-_the_-_old_-_sterile_-_field_-_demo_-_with_-_integration_-_of_-_the_-_c#_-_library\"},\"Type\":\"https://febr.is/Module/1\",\"MoreInfo\":\"https://febr.is/Module/1\",\"Extensions\":null,\"InteractionType\":\"performance\",\"CorrectResponsesPattern\":\"[,]\",\"InteractionComponents\":\"Step1:Clean_-_TablernStep1.1:Throw_-_away_-_clothrnStep2:Select_-_Sterile_-_SolutionrnStep3:Select_-_Sterile_-_package_-_that_-_is_-_undamagedrnStep4:_-_Unfold_-_Sterile_-_Field\"} },";
                output.Add(item0);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.StackTrace);
                throw;
            }
            return output;
        }

        static List<string> GetVerbList()
        {
            List<string> output = new List<string>();
            try
            {
                //string item0 = \"\"verb\":{\"key\":0,\"uuid\":\"00000000-0000-0000-0000-000000000000\",\"id\":\"https://febr.is/xAPI/VerbDetails/Initialized\",\"display\":null},";
                string item0 = "\"Verb\":{ \"Key\":0,\"UUID\":\"00000000-0000-0000-0000-000000000000\",\"Id\":\"https://febr.is/Verb/Details/Initialized\",\"Display\":null},";
                output.Add(item0);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.StackTrace);
                throw;
            }
            return output;
        }

        #endregion

    }





}
