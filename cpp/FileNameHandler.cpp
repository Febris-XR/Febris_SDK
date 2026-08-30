// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
//#include "FileNameHandler.h"
#include "pch.h"

///No longer used after Android 11 refactor
namespace FebrisCppStatement
{
	
	
	
	/// <summary>
	/// This may not be needed. I can't figure out how it would work better than just writting the file and reading it.
	/// </summary>
	//class FileNameHandler
	//{
	//private:
	//	static MemoryMappedFile mmf;
	//public:
	//	//private static readonly ILogger //_log = Log.Logger;

	//	static void StoreFileName(string fileName)
	//	{
	//		try
	//		{

	//			mmf = MemoryMappedFile(fileName,fileName.size());


	//			/*std::string str = fileName;*/
	//			//std::fstream file(fileName, std::ios::out | std::ios::binary | std::ios::trunc);
	//			//file.seekp(str.size()); // Set the file size to the length of the string
	//			//file.write("", 1);      // Write a null terminator to the end of the file
	//			//file.seekp(0);  


	//			//std::ifstream mappedFile("statementFileName", std::ios::in | std::ios::binary);
	//			//mappedFile.seekg(0, std::ios::end); // Move the file pointer to the end of the file
	//			//size_t fileSize = mappedFile.tellg(); // Get the file size
	//			//mappedFile.seekg(0, std::ios::beg); // Move the file pointer back to the beginning of the file
	//			//std::string mappedStr(std::istreambuf_iterator<char>(mappedFile), std::istreambuf_iterator<char>());

	//			//// Copy the original string to the memory-mapped file
	//			//mappedStr.assign(str.c_str(), str.size());

	//			// Reset the file pointer to the beginning of the file

	//			//byte[] Buffer = ASCIIEncoding.ASCII.GetBytes(fileName);
	//			/*unsigned char Buffer = ASCIIEncoding.ASCII.GetBytes(fileName);
	//			MemoryMappedFile mmf = MemoryMappedFile.CreateOrOpen("statementFileName", 1000);
	//			MemoryMappedViewAccessor accessor = mmf.CreateViewAccessor();
	//			accessor.Write(54, (ushort)Buffer.Length);
	//			accessor.WriteArray(54 + 2, Buffer, 0, Buffer.Length);*/
	//		}
	//		catch (const std::runtime_error& e)
	//		{
	//			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);

	//		}
	//	};

	//	static string GetFileName()
	//	{
	//		try
	//		{
	//			char* data = mmf.GetData();

	//			string output;
	//			output.assign(data);
	//			return output;

	//			//MemoryMappedFile mmf = MemoryMappedFile::GetData("statementFileName");//::OpenExisting("statementFileName");
	//			//MemoryMappedViewAccessor accessor = mmf.CreateViewAccessor();
	//			//ushort Size = accessor.ReadUInt16(54);
	//			//byte[] Buffer = new byte[Size];
	//			/*unsigned char Buffer;
	//			accessor.ReadArray(54 + 2, Buffer, 0, Buffer.Length);*/
	//			//return ASCIIEncoding.ASCII.GetString(Buffer);
	//		}
	//		catch (const std::exception& ex)
	//		{
	//			vector<bfs::directory_entry> directory;
	//			directory = GetDirectoryInfo(FileSystem::FileSystem::StatementPath);

	//			bfs::directory_entry fileInfo = *std::max_element(directory.begin(), directory.end(),
	//				[](const bfs::directory_entry& a, const bfs::directory_entry& b) -> bool {
	//					return last_write_time(a) < last_write_time(b);
	//				});

	//			return fileInfo.path().filename().string();

	//			//= new DirectoryInfo(FileSystem.FileSystem.StatementPath);
	//			//FileInfo fileInfo = directory.GetFiles().OrderByDescending(f = > f.LastWriteTime).First();
	//			//StoreFileName(fileInfo.name);
	//			//return fileInfo.name;// .Name;
	//		}
	//	};

	//	static void CleanMMF()
	//	{
	//		try
	//		{
	//			//MemoryMappedFile mmf = 
	//			mmf.~MemoryMappedFile();
	//			//mmf.Dispose();
	//		}
	//		catch (const std::runtime_error& e)
	//		{
	//			//_log.Error(System.Reflection.MethodBase.GetCurrentMethod().Name + " Error: " + ex.Message);

	//		}
	//	};



	//	static vector<boost::filesystem::directory_entry> GetDirectoryInfo(const std::string& dirPath)
	//	{
	//		vector<boost::filesystem::directory_entry> entries;

	//		boost::filesystem::path path(dirPath);

	//		if (!boost::filesystem::is_directory(path))
	//		{
	//			// Throw an exception or return an empty vector to indicate an error
	//			return entries;
	//		}

	//		for (auto& entry : boost::filesystem::directory_iterator(path))
	//		{
	//			entries.push_back(entry);
	//		}

	//		return entries;
	//	}

	//	//std::vector<FileInfo> GetFileInfo(const std::string& dirPath)
	//	//{
	//	//    std::vector<FileInfo> fileInfo;

	//	//    boost::filesystem::path path(dirPath);

	//	//    if (!boost::filesystem::is_directory(path))
	//	//    {
	//	//        // Throw an exception or return an empty vector to indicate an error
	//	//        return fileInfo;
	//	//    }

	//	//    for (auto& entry : boost::filesystem::directory_iterator(path))
	//	//    {
	//	//        fileInfo.push_back(FileInfo(entry));
	//	//    }

	//	//    return fileInfo;
	//	//};

	//	//std::string name;
	//	//std::uintmax_t size;
	//	//bool isDirectory;
	//	//FileInfo(const boost::filesystem::directory_entry& entry)
	//	//{
	//	//    name = entry.path().filename().string();
	//	//    size = boost::filesystem::file_size(entry.path());
	//	//    isDirectory = boost::filesystem::is_directory(entry.path());
	//	//}
	//};
}
