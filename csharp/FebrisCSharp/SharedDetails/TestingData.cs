// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace Febris.CsharpSimulationLibraryNetStandard.SharedDetails
{
    /// <summary>
    /// used for testing
    /// </summary>
    public class TestingData
    {
        public static string SelectTimeStamp(int input)
        {
            string outputString = string.Empty;
            try
            {
                if (input < 10)
                {
                    outputString = "'timestamp':'2022-0" + input + "-08T23:02:46.4362204Z',";
                }
                else
                {
                    outputString = "'timestamp':'2022-" + input + "-08T23:02:46.4362204Z',";
                }
                //outputString = "'timestamp':'2022-" + input + "-8T23:02:46.4362204Z',";                
            }
            catch
            {

            }
            return outputString;

        }
        #region Actors
        public static string SelectActor(int input)
        {
            string outputString = string.Empty;
            try {
                switch (input)
                {
                    case 1:
                        outputString = "'actor':{ 'id':1,'uuid':'72446f6c-12a4-4276-bcd0-c53a4bce0ff0','objectType':'Agent','name':'Jaycee_Chenf1b4bc15-61be-4c37-91a3-432bdc56e756','mbox':null,'mbox_sha1sum':'d02e762fa2418293fc7812cdbba66830e68ee857','openId':null,'account':null,'member':null},";
                        break;
                    case 2:
                        outputString = "'actor':{ 'id':2,'uuid':'d42b4d40-438a-4370-b38e-18568fa26862','objectType':'Agent','name':'Briar_Maddenea1b6049-96eb-4753-8029-ea8da197c0a3','mbox':null,'mbox_sha1sum':'cc575dc06d1ae982e8489e316aa2024fe8fa4943','openId':null,'account':null,'member':null},";
                        break;
                    case 3:
                        outputString = "'actor':{ 'id':3,'uuid':'4ece0389-00a4-47ec-b27b-1f61ac28f217','objectType':'Agent','name':'Magnus_Doe11511293-7d01-47f4-bb22-0566457741d5','mbox':null,'mbox_sha1sum':'818c473b9a47779ff362bc17711f5ebdd90ee183','openId':null,'account':null,'member':null},";
                        break;
                    case 4:
                        outputString = "'actor':{ 'id':4,'uuid':'0aae164f-4af4-455d-bc9d-84a101ec8fe2','objectType':'Agent','name':'Blaze_Dyerc01b603d-ed82-44fa-9bb3-9389ecae694f','mbox':null,'mbox_sha1sum':'f3e146def5f882bc51e13cdc2fddeb3c549b7bf9','openId':null,'account':null,'member':null},";
                        break;
                    case 5:
                        outputString = "'actor':{ 'id':5,'uuid':'b683c809-c07f-4d66-8465-f39821d1d037','objectType':'Agent','name':'Londyn_Cochran24844061-0c0b-48be-8c33-199fca8c88b3','mbox':null,'mbox_sha1sum':'249f6fe162ae975bf77a88bc29f021bbafacc3a4','openId':null,'account':null,'member':null},";
                        break;
                    //case 6:
                    //    outputString = "'Timestamp':'2020-2-12T23:02:46.4362204Z','Actor': {'Id': 14,'UUID': '5e25e11f-92dd-4995-95b6-8b07bffc830a','ObjectType': 'Agent','Name': 'Camellia Canto','Mbox_sha1sum': '85e72e46a7927e97800be1457ddee1a9852266e1'},";
                    //    break;
                    //case 7:
                    //    outputString = "'Actor': {'Id': 52,'UUID': '362833b4-2f7d-474a-8dbd-bf21bf635ca6','ObjectType': 'Agent','Name': 'Lai Fullerton','Mbox_sha1sum': 'e442eb986cbc39c82c98d89be09b130a19b62e59'},";
                    //    break;
                    //case 8:
                    //    outputString = "'Actor': {'Id': 27,'UUID': '1bca26f0-64c1-4052-bbe8-eed52ee6ce34','ObjectType': 'Agent','Name': 'Delois Yoo','Mbox_sha1sum': '4105fd93a1bf9889fd5b7ed8142bbdada8874810'},";
                    //    break;
                    //case 9:
                    //    outputString = "'Timestamp':'2020-2-11T23:02:46.4362204Z','Actor': {'Id': 18,'UUID': '97cdd15f-f82c-4e52-98b1-f15c570228b0','ObjectType': 'Agent','Name': 'Chere Heindel','Mbox_sha1sum': 'f1d630ff1ec5c9a7a17508d5c46372943b222b55'},";
                    //    break;
                    //case 10:
                    //    outputString = "'Actor': {'Id': 18,'UUID': '97cdd15f-f82c-4e52-98b1-f15c570228b0','ObjectType': 'Agent','Name': 'Chere Heindel','Mbox_sha1sum': 'f1d630ff1ec5c9a7a17508d5c46372943b222b55'},";
                    //    break;
                }
                //switch (input)
                //{
                //    case 1:
                //        outputString = "'Actor': {'Id': 92,'UUID': '02ded85d-ae0a-42fb-a558-515ef4fe025d','ObjectType': 'Agent','Name': 'Vernell Mink','Mbox_sha1sum': 'f4bb29e17cddd7ba942c497b198d0a8a1710c203'},";
                //        break;
                //    case 2:
                //        outputString = "'Actor': {'Id': 39,'UUID': 'f4232c5e-6534-474d-b1d3-6995fb9bd68a','ObjectType': 'Agent','Name': 'Guy Wieck','Mbox_sha1sum': 'f5dc6ae3b7a58d89ca78e59002d3cc5fc5dae631'},";
                //        break;
                //    case 3:
                //        outputString = "'Timestamp':'2020-2-14T23:02:46.4362204Z','Actor': {'Id': 92,'UUID': '02ded85d-ae0a-42fb-a558-515ef4fe025d','ObjectType': 'Agent','Name': 'Vernell Mink','Mbox_sha1sum': 'f4bb29e17cddd7ba942c497b198d0a8a1710c203'},";
                //        break;
                //    case 4:
                //        outputString = "'Timestamp':'2020-2-13T23:02:46.4362204Z','Actor': {'Id': 39,'UUID': 'f4232c5e-6534-474d-b1d3-6995fb9bd68a','ObjectType': 'Agent','Name': 'Guy Wieck','Mbox_sha1sum': 'f5dc6ae3b7a58d89ca78e59002d3cc5fc5dae631'},";
                //        break;
                //    case 5:
                //        outputString = "'Actor': {'Id': 14,'UUID': '5e25e11f-92dd-4995-95b6-8b07bffc830a','ObjectType': 'Agent','Name': 'Camellia Canto','Mbox_sha1sum': '85e72e46a7927e97800be1457ddee1a9852266e1'},";
                //        break;
                //    case 6:
                //        outputString = "'Timestamp':'2020-2-12T23:02:46.4362204Z','Actor': {'Id': 14,'UUID': '5e25e11f-92dd-4995-95b6-8b07bffc830a','ObjectType': 'Agent','Name': 'Camellia Canto','Mbox_sha1sum': '85e72e46a7927e97800be1457ddee1a9852266e1'},";
                //        break;
                //    case 7:
                //        outputString = "'Actor': {'Id': 52,'UUID': '362833b4-2f7d-474a-8dbd-bf21bf635ca6','ObjectType': 'Agent','Name': 'Lai Fullerton','Mbox_sha1sum': 'e442eb986cbc39c82c98d89be09b130a19b62e59'},";
                //        break;
                //    case 8:
                //        outputString = "'Actor': {'Id': 27,'UUID': '1bca26f0-64c1-4052-bbe8-eed52ee6ce34','ObjectType': 'Agent','Name': 'Delois Yoo','Mbox_sha1sum': '4105fd93a1bf9889fd5b7ed8142bbdada8874810'},";
                //        break;
                //    case 9:
                //        outputString = "'Timestamp':'2020-2-11T23:02:46.4362204Z','Actor': {'Id': 18,'UUID': '97cdd15f-f82c-4e52-98b1-f15c570228b0','ObjectType': 'Agent','Name': 'Chere Heindel','Mbox_sha1sum': 'f1d630ff1ec5c9a7a17508d5c46372943b222b55'},";
                //        break;
                //    case 10:
                //        outputString = "'Actor': {'Id': 18,'UUID': '97cdd15f-f82c-4e52-98b1-f15c570228b0','ObjectType': 'Agent','Name': 'Chere Heindel','Mbox_sha1sum': 'f1d630ff1ec5c9a7a17508d5c46372943b222b55'},";
                //        break;                    
                //}
            } 
            catch 
            {
            
            }
            return outputString;

        }
        #endregion

        #region Object
        public static string SelectObject(int input)
        {
            string outputString = string.Empty;
            string objectString = string.Empty;
            string verbString = string.Empty;
            string attachmentString = string.Empty;
            try
            {
                switch (input)
                {
                    //case 1:
                    //    objectString = "'Object': {'Key': 1,'UUID': '3412dd90-5904-4038-b586-86ab484db4ec','Id': 'https://febr.is/ModuleBase/979be739-7b53-439b-a90a-621b3b01a816','ObjectType': 'Activity','Definition': {'Id': 1,'UUID': 'bfe8f561-71ce-4e10-b58d-645ee08775ca','Name': {'en': 'Sterile Field Preparation'},'Description': {'en': 'Steps for setting up a sterile field'},'Type': 'https://febr.is/ModuleBase/1','MoreInfo': 'https://febr.is/ModuleBase/1','InteractionType': 'performance','CorrectResponsesPattern': '[,]','InteractionComponents': {'Step1': ' Get Cloth','Step2': ' Clean table','Step3': ' Select Sterile pack','Step3.1': ' Check Sterile pack for date and damage','Step4': ' etc'}}}";
                    //    verbString = SelectVerb(1);
                    //    outputString = verbString + objectString + attachmentString;
                    //    break;
                    //case 2:
                    //    objectString = "'Object': {'Key': 2,'UUID': '24476af1-dbc2-48cf-856f-cec84a7d6d09','Id': 'https://febr.is/ModuleBase/842d1253-8523-4144-99db-cb75ec339df9','ObjectType': 'Activity','Definition': {'Id': 2,'UUID': '5def29ef-be3a-4351-b393-38405035b5e8','Name': {'en': 'Sterile Field Preparation Check off'},'Description': {'en': 'Sterile Field Preparation Check Off'},'Type': 'https://febr.is/ModuleBase/2','MoreInfo': 'https://febr.is/ModuleBase/2','InteractionType': 'performance','CorrectResponsesPattern': '[,]','InteractionComponents': {'Step1': ' Get Cloth','Step2': ' Clean table','Step3': ' Select Sterile pack','Step3.1': ' Check Sterile pack for date and damage','Step4': ' etc'}}}";
                    //    verbString = SelectVerb(2);
                    //    attachmentString = SelectAttachment(RandomAttachment());
                    //    outputString = verbString + objectString + attachmentString;
                    //    break;

                    case 1:
                        objectString = "'Object':{ 'Key':3,'UUID':'c066879b-abdb-47ef-b6b2-e6ffcc4c13e8','Id':'https://febr.is/Module/93235451-91ed-49ee-bab6-893110dededc','ObjectType':'Activity','Definition':{ 'Id':3,'UUID':'184ccd1d-5d72-4290-b983-d9f5b12d448a','Name':{'en':'APK Test Module'},'description':{'en':'APK Test Module. This is a nearly empy module and I honestly have no idea if it even runs. Used to test distribution and installations. '},'type':'https://febr.is/Module/2','moreInfo':'https://febr.is/Module/2','extensions':null,'interactionType':'performance','correctResponsesPattern':'[,]','interactionComponents':'label1:input1'} }";
                        verbString = SelectVerb(1);
                        outputString = verbString + objectString + attachmentString;
                        break;
                    case 2:
                        objectString = "'Object':{ 'Key':2,'UUID':'f82603a4-68e1-4c47-9a7e-e156303bbdb6','Id':'https://febr.is/Module/18f6d877-ee0a-4a5c-8e49-81332b07aab5','ObjectType':'Activity','Definition':{ 'Id':2,'UUID':'277efb51-fa12-4ee1-8f31-ddbeaf2df16b','Name':{'en':'Test Module 1'},'description':{'en':'Simple test sterile field preparation'},'type':'https://febr.is/Module/1','moreInfo':'https://febr.is/Module/1','extensions':null,'interactionType':'performance','correctResponsesPattern':'[,]','interactionComponents':'label1:input1,label2:input2'} }";
                        verbString = SelectVerb(1);
                        attachmentString = SelectAttachment(RandomAttachment());
                        outputString = verbString + objectString + attachmentString;
                        break;
                }
            }
            catch
            {

            }
            return outputString;

        }
        #endregion

        #region Verb
        public static string SelectVerb(int input)
        {
            string outputString = string.Empty;
            try
            {

                switch (input)
                {
                    //case 1:
                    //    outputString = "'Verb': {'Key': 1,'UUID': 'f72789c6-47ee-460f-8b68-05ca7d6f1cf9','Id': 'https://febr.is/xAPI/VerbDetails/Attempted','Display': {'en': 'Attempted'}},";
                    //    break;
                    //case 2:
                    //    outputString = "'Verb': {'Key': 3,'UUID': '24fb0f47-a85d-4e01-931b-ef2baeebe263','Id': 'https://febr.is/xAPI/VerbDetails/Initialized','Display': {'en': 'Initialized'}},";
                    //    break;
                    //    
                    case 1:
                        outputString = "'Verb':{ 'Key':1,'UUID':'784531c7-a7d4-4936-b9e1-2a0467a62f78','Id':'https://febr.is/Verb/Details/Initialized','Display':{'en':'Initialized'}},";
                        break;
                    case 2:
                        outputString = "'Verb':{ 'Key':2,'UUID':'2bfd879d-a90c-471c-9d5e-bf29725be4a8','Id':'https://febr.is/Verb/Details/Pass','Display':{'en':'Pass'}},";
                        break;
                    case 3:
                        outputString = "'Verb':{ 'Key':3,'UUID':'4a2db59d-8480-40e1-a76b-943e9c8e2a68','Id':'https://febr.is/Verb/Details/Not_Pass','Display':{'en':'Not_Pass'}},";
                        break;
                    case 4:
                        outputString = "'Verb':{ 'Key':4,'UUID':'dc6ec833-f6a8-410c-9466-8cd39e95fef1','Id':'https://febr.is/Verb/Details/Completed','Display':{'en':'Completed'}},";
                        break;
                    case 5:
                        outputString = "'Verb':{ 'Key':5,'UUID':'a8cde4a8-43b4-49a8-ae77-6d56c5b5d20a','Id':'https://febr.is/Verb/Details/Terminated_Early','Display':{'en':'Terminated_Early'}},";
                        break;


                    default:
                        outputString = "'Verb':{ 'Key':1,'UUID':'784531c7-a7d4-4936-b9e1-2a0467a62f78','Id':'https://febr.is/Verb/Details/Initialized','Display':{'en':'Initialized'}},";
                        break;
                    
                        
                }
            }
            catch
            {

            }
            return outputString;
        }
        #endregion

        #region Attachments
        public static string SelectAttachment(int input)
        {
            string outputString = string.Empty;
            try
            {
                switch (input)
                {
                    case 1:
                        outputString = string.Empty;
                        break;
                    case 2:
                        outputString = ",'Attachments': [{'UsageType': 'https://febr.is/xApi/attachments/video_review','Display': {'en': '2e9d392a-d07f-4498-a3ba-8e8360cd95f5'},'Description': {'en': 'Video of conducted education'},'ContentType': 'video/mp4','Sha2': '2c75c64a083e5cfa5e934573391878391fb8c9f407d35b276a864eaa071ccb05','FileURL': 'https://febr.is/widget/videoloader?videoName=2e9d392a-d07f-4498-a3ba-8e8360cd95f5'}]";
                        break;                   
                }
            }
            catch
            {
            }
            return outputString;
        }
        #endregion

        #region Statement assembly
        public static string StatementStarter = SharedDetails.StatementPreface + "{";
        public static string StatementEnder = "}";
        #endregion

        //fallback command arg input
        //public static string TestStatement = SharedDetails.StatementPreface+
        //    "{'Actor':{'Id': 333,'UUID': '26b0847e-3ca2-412f-8ec6-71bb593865a7','ObjectType': 'Agent','Name':'Nurse 35','Mbox': 'mailto:Nurse35@email.com'}," +
        //    "'Verb':{'Key': 3,'UUID': '1406f8cd-7985-44cc-a762-13eacc7bcfc3','Id':'https://febr.is/xAPI/VerbDetails/Initialized','Display': {'en': 'Initialized'}}," +
        //    "'Object': {'Key': 3,'UUID': '33f0608b-37e2-4d99-b877-7f147759470e','Id': 'https://febr.is/testbase/bc0ff9c5-0660-45bf-8bd4-2724c2fd41a7','ObjectType': 'Activity'}," +
        //    "'Attachments': [{'UsageType': 'https://febr.is/xApi/attachments/video_review','Display': {'en': '1d74d449-d520-421a-9131-9a9bafb67566'},'Description': {'en': 'Video of conducted test'},'ContentType': 'video/mp4','Sha2': 'f93af2deacf592cdcb6ab21d74ca40f2f8a0e936a3af28075b379734c62e9fdd','FileURL': 'https://febr.is/widget/videoloader?videoName=1d74d449-d520-421a-9131-9a9bafb67566'}]}";


        //public static string TestStatement = SharedDetails.StatementPreface +
        //    "{'Actor':{'Id': 333,'UUID': '26b0847e-3ca2-412f-8ec6-71bb593865a7','ObjectType': 'Agent','Name':'Nurse 35','Mbox': 'mailto:Nurse35@email.com'}," +
        //    "'Verb':{'Key': 3,'UUID': '1406f8cd-7985-44cc-a762-13eacc7bcfc3','Id':'https://febr.is/xAPI/VerbDetails/Initialized','Display': {'en': 'Initialized'}}," +
        //    "'Object': {'Key': 3,'UUID': '33f0608b-37e2-4d99-b877-7f147759470e','Id': 'https://febr.is/testbase/bc0ff9c5-0660-45bf-8bd4-2724c2fd41a7','ObjectType': 'Activity'}," +
        //    "'Attachments': [{'UsageType': 'https://febr.is/xApi/attachments/video_review','Display': {'en': '1d74d449-d520-421a-9131-9a9bafb67566'},'Description': {'en': 'Video of conducted test'},'ContentType': 'video/mp4','Sha2': 'f93af2deacf592cdcb6ab21d74ca40f2f8a0e936a3af28075b379734c62e9fdd','FileURL': 'https://febr.is/widget/videoloader?videoName=1d74d449-d520-421a-9131-9a9bafb67566'}]}";
        private static int RandomAttachment()
        {
            var random = new Random();
            Thread.Sleep(100);
            return random.Next(1, 3);
        }
    }
}
