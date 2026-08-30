// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Text;

namespace Febris.CsharpSimulationLibraryNetStandard.Models.xAPI
{
    //################################################################
    //Todo: need to make the member an array so it can handle groups        
    //Will need to change Member over to an array[]
    // Need to add in Inverse Functional Identifiers in here. 
    // -- But members needs to be an array or maybe make member just an array of Actors?
    //  1) table Ids
    //  2) object types and names
    //  3) Inverse Function Identifier - there can be only one
    //      --  I am not sure how to handle this mbox or mbox_sha1sum or maybe openId
    //  4) Array of Actors
    //Note: if the object type is a "group" it must always be considered distinct if anonymous 
    //      - These is no identifier for this cluster. it is an "ad hoc team"
    //
    //################################################################
  
    /// <summary>
    /// xAPI Agent/Group actor node. FIX (SDKV-12): wire names are declared
    /// explicitly -- spec keys use exact xAPI 1.0.3 casing (notably
    /// <c>objectType</c>); <c>id</c>/<c>uuid</c> are Febris-dialect DB
    /// hints kept in their historical lowercase form.
    /// </summary>
    internal class Actor
    {
        //1
        [JsonProperty("id")] internal long Id { get; set; }
        [JsonProperty("uuid")] internal Guid UUID { get; set; }
        //2
        [JsonProperty("objectType")] internal string ObjectType { get; set; }
        [JsonProperty("name")] internal string Name { get; set; }
        //3
        [JsonProperty("mbox")] internal Uri Mbox { get; set; }
        [JsonProperty("mbox_sha1sum")] internal string Mbox_sha1sum { get; set; }
        [JsonProperty("openid")] internal Uri OpenId { get; set; }
        [JsonProperty("account")] internal Account Account { get; set; }
        //4
        [JsonProperty("member")] internal Member Member { get; set; }
    }
    //################################################################
    // Mbox needs to be of type mailtoIRI
    //I honestly cant tell if I need to change this. Or if I need to not have the actor the way I did before when creating them with users
    //I need this member set up to be able to use in the array - But would not be used if object type is an "Agent"
    //
    //################################################################
    
    /// <summary>
    /// Febris-dialect wrapper for a Group's member list (the spec shape is
    /// a bare array; the dialect wraps it with DB hints). All-lowercase
    /// wire names preserved.
    /// </summary>
    internal class Member
    {
        [JsonProperty("id")] internal long Id { get; set; }
        [JsonProperty("uuid")] internal Guid UUID { get; set; }
        [JsonProperty("actors")] internal List<Actor> Actors { get; set; }
    }
    //################################################################    
    //This is apparently made to use OAuth authentication. 
    //################################################################
    
    /// <summary>
    /// xAPI Account inverse-functional identifier. FIX (SDKV-12):
    /// <c>homePage</c> emits with exact spec casing -- the lowercased
    /// "homepage" form invalidated the IFI on conformant LRSs.
    /// </summary>
    internal class Account
    {
        [JsonProperty("id")] internal long Id { get; set; }
        [JsonProperty("uuid")] internal Guid UUID { get; set; }
        //[Required]
        [JsonProperty("homePage")] internal Uri HomePage { get; set; }
        //[Required]
        [JsonProperty("name")] internal string Name { get; set; }
    }
}
