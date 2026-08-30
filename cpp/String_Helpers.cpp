// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
//#include "String_Helpers.h"
#include "pch.h"

namespace FebrisCppHelpers
{
	class String_Helpers
	{
	public:
		static bool IsNullOrEmpty(const std::string& str)
		{
			return str.empty() || str.find_first_not_of(' ') == std::string::npos;
		};

		/// <summary>
		/// Main will need to provide the variables and be activated like this -> main(int argc, char* argv[])
		/// </summary>
		/// <param name="argc">argument count</param>
		/// <param name="argv">argument vector</param>
		/// <returns></returns>
		/*static vector<string> GetCommandLineArgs(int argc, char* argv[]) {
			vector<string> output;
			try {
				std::cout << "Number of arguments: " << argc << std::endl;
				for (int i = 0; i < argc; i++) {
					output.push_back(argv[i]);
					std::cout << "Argument " << i << ": " << argv[i] << std::endl;
				}
			}
			catch (const std::exception& ex) {
				throw std::runtime_error("Error parsing command line arguments: " + std::string(ex.what()));
			}
			return output;
		};*/


		static string Replace(std::string str, const std::string& old_str, const std::string& new_str) {
			size_t pos = 0;
			while ((pos = str.find(old_str, pos)) != std::string::npos) {
				str.replace(pos, old_str.length(), new_str);
				pos += new_str.length();
			}
			return str;
		};

		static bool StartsWith(const std::string& str, const std::string& prefix) {
			return str.size() >= prefix.size() &&
				str.compare(0, prefix.size(), prefix) == 0;
		};

		//static void ChangePropertiesToLowerCase(json jsonObject)
		//{
		//	for (auto property : jsonObject.array())//jsonObject.Properties().ToList())
		//	{
		//		if (property.Value.Type == json::type::Object){
		//			// JTokenType.Object) {
		//			// replace property names in child object
		//			ChangePropertiesToLowerCase((json)property.Value);
		//		}

		//		if (property.Value.Type == JTokenType.Array)
		//		{
		//			auto arr = JArray.Parse(property.Value.ToString());
		//			for (auto pr : arr)
		//			{
		//				ChangePropertiesToLowerCase((json)pr);
		//			}

		//			property.Value = arr;
		//		}

		//		Replace(property, new JProperty(property.Name.ToLower(), property.Value));// properties are read-only, so we have to replace them
		//	}
		//};


		static void ChangePropertiesToLowerCase(json& jsonObject)
		{
			try {
				for (auto& [key, value] : jsonObject.items()) {
					if (value.is_object()) {
						ChangePropertiesToLowerCase(value);
					}
					else if (value.is_string()) {
						const std::string& str = value.get<std::string>();
						std::string lowerStr;
						std::transform(str.begin(), str.end(), std::back_inserter(lowerStr), ::tolower);
						value = lowerStr;
					}
					/*else if (value.is_string()) {
						std::string& str = value.get<std::string>();
						std::transform(str.begin(), str.end(), str.begin(), ::tolower);
					}*/
				}
			}
			catch (const std::exception& e) {
				std::cerr << "Error: " << e.what() << std::endl;
				throw;
			}
			//for (auto& [key, value] : jsonObject.items()) {
			//	// Convert the key to lowercase
			//	std::transform(key.begin(), key.end(), key.begin(), ::tolower);

			//	// Add a new property with the lowercase key and the original value
			//	jsonObject[key] = value;
			//	jsonObject.erase(key);
			//	jsonObject.emplace(key, value);
			//}
		};


		static json ChangeStringToObject(string input)
		{
			try {
				json obj = json::parse(input);
				return obj;
			}
			catch (const std::exception& ex) {
				throw std::runtime_error("Error parsing JSON string: " + std::string(ex.what()));
			}
		};

		//static vector<string> GetCommandLineArgs()
		//{
		//	vector<string> output;
		//	try
		//	{
		//		for (int i = 0; i < argc; i++)
		//		{

		//		}
		//		
		//	}
		//	catch (const std::runtime_error& e)
		//	{
		//		//Log.Logger.Error("Error converting string to Object :" + ex.Message);
		//	}
		//	return output;
		//}

		static vector<string> Split(string& input, char splitChar)
		{
			static std::vector<std::string> result; // the static vector of strings
			result.clear(); // clear the vector before reusing it

			std::string::size_type startPos = 0;
			std::string::size_type endPos = input.find(splitChar);

			while (endPos != std::string::npos) {
				std::string token = input.substr(startPos, endPos - startPos);
				result.push_back(token);

				startPos = endPos + 1;
				endPos = input.find(splitChar, startPos);
			}

			// add the last token (or the whole string if no splitChar is found)
			std::string token = input.substr(startPos);
			result.push_back(token);

			return result;			
		};


		/// <summary>
		/// Main will need to provide the variables and be activated like this -> main(int argc, char* argv[])
		/// </summary>
		/// <param name="argc">argument count</param>
		/// <param name="argv">argument vector</param>
		/// <returns>the statement argument</returns>
		string GetCommandLineArgs(char* argv[]) {
			try {
				int argCount = 0;
				std::vector<std::string> args_;
				// Copy command line arguments to vector
				//args_.reserve(argc);
				for (int i = 0; argv[i] != nullptr; ++i){
				//for (int i = 0; i < argc; i++) {
					argCount++;
					args_.emplace_back(argv[i]);
				}
			}
			catch (const std::exception& ex) {
				throw std::runtime_error("Error parsing command line arguments: " + std::string(ex.what()));
			}
			string output;
			output = GetArgumentValue(SharedDetails::StatementPreface, args_);
			return output;
		}
		///This is dependent on knowing the number of arguments. Obviously that is not what is happening. 
		//string GetCommandLineArgs(int argc, char* argv[]) {
		//	try {
		//		// Copy command line arguments to vector
		//		args_.reserve(argc);
		//		for (int i = 0; i < argc; i++) {
		//			args_.push_back(argv[i]);
		//		}
		//	}
		//	catch (const std::exception& ex) {
		//		throw std::runtime_error("Error parsing command line arguments: " + std::string(ex.what()));
		//	}
		//	string output;
		//	output = GetArgumentValue(SharedDetails::StatementPreface, args_);
		//	return output;
		//}
		/// Get the number of command line arguments
		int Size() const {
			return args_.size();
		}

		/// Check if an argument exists
		bool HasArgument(const std::string& arg) const {
			try {
				return std::find(args_.begin(), args_.end(), arg) != args_.end();
			}
			catch (const std::exception& ex) {
				throw std::runtime_error("Error checking for argument: " + std::string(ex.what()));
			}
		}

		/// Get the value of an argument
		string GetArgumentValue(const std::string& arg) const {
			try {
				auto it = std::find(args_.begin(), args_.end(), arg);
				if (it != args_.end() && ++it != args_.end()) {
					return *it;
				}
				return "";
			}
			catch (const std::exception& ex) {
				throw std::runtime_error("Error getting argument value: " + std::string(ex.what()));
			}
		}

		string GetArgumentValue(const std::string& argPrefix, const std::vector<std::string>& args) {
			try {
				for (size_t i = 0; i < args.size(); i++) {
					if (args[i].substr(0, argPrefix.length()) == argPrefix) {
						// found an argument with the prefix, return its value
						return args[i].substr(argPrefix.length() + 1);
					}
				}
			}
			catch (const std::exception& ex) {
				throw std::runtime_error("Error getting argument value: " + std::string(ex.what()));
			}
			// didn't find an argument with the prefix
			return "";
		}

	private:
		std::vector<std::string> args_;

	};
	//static json ChangeStringToObject(string input)
	//{
	//	// Parse the JSON string
	//	json j = json::parse(input);

	//	try
	//	{
	//		// Create a new json
	//		jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
	//		jmethodID jsonObjectConstructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
	//		jstring jsonStr = env->NewStringUTF(j.dump().c_str());
	//		jobject jsonObject = env->NewObject(jsonObjectClass, jsonObjectConstructor, jsonStr);

	//		return jsonObject;
	//	}
	//	catch (const std::runtime_error& e)
	//	{
	//		//Log.Logger.Error("Error converting string to Object :" + ex.Message);
	//	}
	//};
}