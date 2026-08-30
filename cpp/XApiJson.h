// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#pragma once
// SDKV emit port (2026-08-29). Object -> JSON serialization for the xAPI model classes,
// mirroring the C# surface's Newtonsoft emission byte-for-byte as far as the two
// libraries allow:
//
//   * Keys and their ORDER come from the C# [JsonProperty] declarations. Newtonsoft
//     emits properties in declaration order, and nlohmann's default json (std::map)
//     sorts keys alphabetically, so everything here builds nlohmann::ordered_json.
//     Emitting through plain `json` would silently reorder every object and break the
//     wire-parity rule (docs/OSS_SDK_DISTRIBUTION.md).
//   * C# reference members that were never assigned serialize as null. The C++ models
//     are value members, so the convention is: empty string / empty map / empty list
//     emits null. Statement.attachments is the one exception: the C# factoring always
//     constructs the list, so it emits [] when empty.
//   * Guid is a struct in C#: it always emits as a string, zero-guid included.
//   * Result.duration follows SDKV-5: ISO 8601 duration via the same rules as
//     XmlConvert.ToString(TimeSpan) -- zero components omitted, "PT0S" floor,
//     fractional seconds only when present. The C++ member stores MILLISECONDS.
//
// The parse-side counterparts (TokenToLanguageMap and friends) live in
// StatementFactoring.cpp. Keep the two in step.

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <charconv>
#include <cstdlib>

namespace XApiJson
{
    using ojson = nlohmann::ordered_json;

    inline std::string GuidString(const GUID2& g)
    {
        return boost::uuids::to_string(g);
    }

    // Empty-string-as-null, the C# unset-reference convention.
    inline ojson StringOrNull(const std::string& s)
    {
        return s.empty() ? ojson(nullptr) : ojson(s);
    }

    inline ojson MapOrNull(const std::map<std::string, std::string>& m)
    {
        if (m.empty())
        {
            return nullptr;
        }
        ojson o = ojson::object();
        for (const auto& kv : m)
        {
            o[kv.first] = kv.second;
        }
        return o;
    }

    inline ojson ListOrNull(const std::vector<std::string>& v)
    {
        if (v.empty())
        {
            return nullptr;
        }
        return ojson(v);
    }

    // ISO 8601 clock timestamp, seconds precision, no zone suffix -- Newtonsoft's shape
    // for an Unspecified-kind DateTime.
    inline std::string TimeIso8601(const std::chrono::system_clock::time_point& tp)
    {
        // A default-constructed time_point plays the C# default(DateTime) role:
        // the factoring never assigns Stored, and the C# wire shows the DateTime
        // default 0001-01-01T00:00:00. (Windows gmtime cannot represent year 1,
        // so the sentinel is also the only way to render it.)
        if (tp.time_since_epoch().count() == 0)
        {
            return "0001-01-01T00:00:00";
        }
        std::time_t t = std::chrono::system_clock::to_time_t(tp);
        std::tm tm{};
        gmtime_s(&tm, &t);
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d",
                      tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                      tm.tm_hour, tm.tm_min, tm.tm_sec);
        return buf;
    }

    // XmlConvert.ToString(TimeSpan) semantics from a millisecond count.
    inline std::string DurationIso8601(long long ms)
    {
        std::string out;
        if (ms < 0)
        {
            out += "-";
            ms = -ms;
        }
        long long days = ms / 86400000; ms %= 86400000;
        long long hours = ms / 3600000; ms %= 3600000;
        long long mins = ms / 60000;    ms %= 60000;
        long long secs = ms / 1000;     ms %= 1000;
        out += "P";
        if (days) { out += std::to_string(days) + "D"; }
        if (hours || mins || secs || ms || !days)
        {
            out += "T";
            if (hours) { out += std::to_string(hours) + "H"; }
            if (mins)  { out += std::to_string(mins) + "M"; }
            if (secs || ms || (!hours && !mins))
            {
                if (ms)
                {
                    char frac[16];
                    std::snprintf(frac, sizeof(frac), "%lld.%03lld", secs, ms);
                    std::string f = frac;
                    while (!f.empty() && f.back() == '0') { f.pop_back(); }
                    out += f + "S";
                }
                else
                {
                    out += std::to_string(secs) + "S";
                }
            }
        }
        return out;
    }

    // Newtonsoft emits a C# float via shortest-round-trip-for-FLOAT ("0.85"), but
    // nlohmann stores every number as double, and widening 0.85f to double gives
    // 0.8500000238418579. Format the float shortest first, then reparse as double
    // so the dump reproduces the C# digits. (Whole values still gain the ".0"
    // Newtonsoft appends: 100f -> "100.0" on both sides.)
    inline ojson FloatJson(float f)
    {
        char buf[64];
        auto res = std::to_chars(buf, buf + sizeof(buf) - 1, f);
        *res.ptr = '\0';
        return ojson(std::strtod(buf, nullptr));
    }
}

// to_json overloads. Free functions at global scope, same scope as the model classes,
// so nlohmann's ADL finds them. Key order below IS the C# declaration order -- do not
// alphabetise it.

inline void to_json(XApiJson::ojson& j, const Account& a)
{
    j = XApiJson::ojson{
        { "id", a.Id },
        { "uuid", XApiJson::GuidString(a.UUID) },
        { "homePage", XApiJson::StringOrNull(a.HomePage) },
        { "name", XApiJson::StringOrNull(a.Name) },
    };
}

inline void to_json(XApiJson::ojson& j, const Actor& a);

inline void to_json(XApiJson::ojson& j, const Member& m)
{
    j = XApiJson::ojson{
        { "id", m.Id },
        { "uuid", XApiJson::GuidString(m.UUID) },
    };
    // PARITY: mirror the two C# wire shapes exactly. SetupMember always assigns
    // the list (empty -> []); only the bare Member from SetupActor's null-actor
    // branch leaves the optional empty (-> null), matching C#'s untouched
    // new Member() whose Actors reference is null.
    if (!m.Actors.has_value())
    {
        j["actors"] = nullptr;
    }
    else
    {
        XApiJson::ojson arr = XApiJson::ojson::array();
        for (const auto& item : *m.Actors)
        {
            XApiJson::ojson o;
            to_json(o, item);
            arr.push_back(o);
        }
        j["actors"] = arr;
    }
}

inline void to_json(XApiJson::ojson& j, const Actor& a)
{
    // PARITY: null when the C# reference was never assigned (the bare
    // new Actor() shapes); objects whenever SetupActor ran (see Actor.h).
    XApiJson::ojson member = nullptr;
    if (a.Member.has_value())
    {
        to_json(member, *a.Member);
    }
    XApiJson::ojson account = nullptr;
    if (a.Account.has_value())
    {
        to_json(account, *a.Account);
    }
    j = XApiJson::ojson{
        { "id", a.Id },
        { "uuid", XApiJson::GuidString(a.UUID) },
        { "objectType", XApiJson::StringOrNull(a.ObjectType) },
        { "name", XApiJson::StringOrNull(a.Name) },
        { "mbox", XApiJson::StringOrNull(a.Mbox) },
        { "mbox_sha1sum", XApiJson::StringOrNull(a.Mbox_sha1sum) },
        { "openid", XApiJson::StringOrNull(a.OpenId) },
        { "account", account },
        { "member", member },
    };
}

inline void to_json(XApiJson::ojson& j, const Verb& v)
{
    j = XApiJson::ojson{
        { "key", v.Key },
        { "uuid", XApiJson::GuidString(v.UUID) },
        { "id", XApiJson::StringOrNull(v.Id) },
        { "display", XApiJson::MapOrNull(v.Display) },
    };
}

inline void to_json(XApiJson::ojson& j, const Extensions& e)
{
    j = XApiJson::ojson{
        { "id", e.Id },
        { "uuid", XApiJson::GuidString(e.UUID) },
        { "extensionmap", XApiJson::StringOrNull(e.ExtensionMap) },
    };
}

inline void to_json(XApiJson::ojson& j, const Definition& d)
{
    XApiJson::ojson ext;
    to_json(ext, d.Extensions);
    j = XApiJson::ojson{
        { "id", d.Id },
        { "uuid", XApiJson::GuidString(d.UUID) },
        { "name", XApiJson::MapOrNull(d.Name) },
        { "description", XApiJson::MapOrNull(d.Description) },
        { "type", XApiJson::StringOrNull(d.Type) },
        { "moreInfo", XApiJson::StringOrNull(d.MoreInfo) },
        { "extensions", ext },
        { "interactionType", XApiJson::StringOrNull(d.InteractionType) },
        { "correctResponsesPattern", XApiJson::ListOrNull(d.CorrectResponsesPattern) },
        { "interactioncomponents", XApiJson::StringOrNull(d.InteractionComponents) },
    };
}

inline void to_json(XApiJson::ojson& j, const Object& o)
{
    XApiJson::ojson def;
    to_json(def, o.Definition);
    j = XApiJson::ojson{
        { "key", o.Key },
        { "uuid", XApiJson::GuidString(o.UUID) },
        { "id", XApiJson::StringOrNull(o.Id) },
        { "objectType", XApiJson::StringOrNull(o.ObjectType) },
        { "definition", def },
    };
}

inline void to_json(XApiJson::ojson& j, const Score& s)
{
    j = XApiJson::ojson{
        { "id", s.Id },
        { "uuid", XApiJson::GuidString(s.UUID) },
        { "scaled", XApiJson::FloatJson(s.Scaled) },
        { "raw", XApiJson::FloatJson(s.Raw) },
        { "min", XApiJson::FloatJson(s.Min) },
        { "max", XApiJson::FloatJson(s.Max) },
    };
}

inline void to_json(XApiJson::ojson& j, const Result& r)
{
    // PARITY: null when an authored result carried no score (C# SetupScore
    // returns null); an object otherwise (the SetupResult null branch assigns
    // a default Score).
    XApiJson::ojson score = nullptr;
    if (r.Score.has_value())
    {
        to_json(score, *r.Score);
    }
    j = XApiJson::ojson{
        { "id", r.Id },
        { "uuid", XApiJson::GuidString(r.UUID) },
        { "score", score },
        { "success", r.Success },
        { "completion", r.Completion },
        { "response", XApiJson::StringOrNull(r.Response) },
        { "duration", XApiJson::DurationIso8601(r.Duration) },
        // PARITY (corrected by the lifecycle harness): the C# factoring assigns
        // Result.Extensions ONLY in SetupResult's null branch (no authored
        // "result"), where the wire shows the default Extensions object; the
        // input-present branch leaves the reference null (its assignment is
        // commented out) and the wire shows null. The optional member carries
        // that distinction.
        { "extensions", nullptr },
    };
    if (r.Extensions.has_value())
    {
        XApiJson::ojson ext;
        to_json(ext, *r.Extensions);
        j["extensions"] = ext;
    }
}

inline void to_json(XApiJson::ojson& j, const ContextActivities& c)
{
    j = XApiJson::ojson{
        { "id", c.Id },
        { "uuid", XApiJson::GuidString(c.UUID) },
        { "parent", XApiJson::StringOrNull(c.Parent) },
        { "grouping", XApiJson::StringOrNull(c.Grouping) },
        { "category", XApiJson::StringOrNull(c.Category) },
        { "other", XApiJson::StringOrNull(c.Other) },
    };
}

inline void to_json(XApiJson::ojson& j, const StatementReference& s)
{
    j = XApiJson::ojson{
        { "key", s.Key },
        { "uuid", XApiJson::GuidString(s.UUID) },
        { "id", XApiJson::GuidString(s.Id) },
        { "objectType", XApiJson::StringOrNull(s.ObjectType) },
    };
}

inline void to_json(XApiJson::ojson& j, const Context& c)
{
    XApiJson::ojson instructor;
    to_json(instructor, c.Instructor);
    XApiJson::ojson activities;
    to_json(activities, c.ContextActivities);
    // PARITY: null when the authored context carried no statementreference
    // (C# SetupStatementReference returns null); an object otherwise.
    XApiJson::ojson sref = nullptr;
    if (c.StatementReference.has_value())
    {
        to_json(sref, *c.StatementReference);
    }
    XApiJson::ojson ext;
    to_json(ext, c.Extensions);
    // PARITY: the C# factoring always constructs the group list (SetupContext's
    // null branch included), so context.group is never null on the wire -- an
    // empty list emits [].
    XApiJson::ojson group = XApiJson::ojson::array();
    for (const auto& item : c.Group)
    {
        XApiJson::ojson o;
        to_json(o, item);
        group.push_back(o);
    }
    j = XApiJson::ojson{
        { "id", c.Id },
        { "uuid", XApiJson::GuidString(c.UUID) },
        { "registration", XApiJson::GuidString(c.Registration) },
        { "instructor", instructor },
        { "group", group },
        { "contextActivities", activities },
        { "revision", XApiJson::StringOrNull(c.Revision) },
        { "platform", XApiJson::StringOrNull(c.Platform) },
        { "language", XApiJson::StringOrNull(c.Language) },
        { "statementreference", sref },
        { "extensions", ext },
    };
}

inline void to_json(XApiJson::ojson& j, const Authority& a)
{
    XApiJson::ojson actor;
    to_json(actor, a.Actor);
    j = XApiJson::ojson{
        { "id", a.Id },
        { "uuid", XApiJson::GuidString(a.UUID) },
        { "actor", actor },
    };
}

inline void to_json(XApiJson::ojson& j, const Version& v)
{
    j = XApiJson::ojson{
        { "id", v.Id },
        { "uuid", XApiJson::GuidString(v.UUID) },
        { "versionnumber", XApiJson::StringOrNull(v.VersionNumber) },
    };
}

inline void to_json(XApiJson::ojson& j, const Attachment& a)
{
    j = XApiJson::ojson{
        { "id", a.Id },
        { "uuid", XApiJson::GuidString(a.UUID) },
        { "usageType", XApiJson::StringOrNull(a.UsageType) },
        { "display", XApiJson::MapOrNull(a.Display) },
        { "description", XApiJson::MapOrNull(a.Description) },
        { "contentType", XApiJson::StringOrNull(a.ContentType) },
        { "length", a.Length },
        { "sha2", XApiJson::StringOrNull(a.Sha2) },
        { "fileUrl", XApiJson::StringOrNull(a.FileURL) },
    };
}

inline void to_json(XApiJson::ojson& j, const Statement& s)
{
    XApiJson::ojson actor;
    to_json(actor, s.Actor);
    XApiJson::ojson verb;
    to_json(verb, s.Verb);
    XApiJson::ojson object;
    to_json(object, s.Object);
    XApiJson::ojson result;
    to_json(result, s.Result);
    XApiJson::ojson context;
    to_json(context, s.Context);
    XApiJson::ojson authority;
    to_json(authority, s.Authority);
    // PARITY: null when the input carried no version (C# SetupVersion returns
    // null); the default Version object when it did (authored "version":{}).
    XApiJson::ojson version = nullptr;
    if (s.Version.has_value())
    {
        to_json(version, *s.Version);
    }
    // The one always-a-list member: the C# factoring constructs the list even when the
    // input carried no attachments, so [] is the parity shape, not null.
    XApiJson::ojson attachments = XApiJson::ojson::array();
    for (const auto& item : s.Attachments)
    {
        XApiJson::ojson o;
        to_json(o, item);
        attachments.push_back(o);
    }
    j = XApiJson::ojson{
        { "id", s.Id },
        { "uuid", XApiJson::GuidString(s.UUID) },
        { "timestamp", XApiJson::TimeIso8601(s.Timestamp) },
        { "stored", XApiJson::TimeIso8601(s.Stored) },
        { "actor", actor },
        { "verb", verb },
        { "object", object },
        { "result", result },
        { "context", context },
        { "authority", authority },
        { "version", version },
        { "attachments", attachments },
    };
}

namespace XApiJson
{
    // The wire string: ordered keys, compact separators -- Newtonsoft Formatting.None.
    inline std::string ToJsonString(const Statement& s)
    {
        ojson j;
        to_json(j, s);
        return j.dump();
    }
}
